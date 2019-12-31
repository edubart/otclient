/*
* Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "declarations.h"
#include "animator.h"

#include <framework/core/clock.h>
#include <framework/core/filestream.h>

Animator::Animator()
{
    m_animationPhases = 0;
    m_startPhase = 0;
    m_loopCount = 0;
    m_async = false;
    m_currentDuration = 0;
    m_currentDirection = AnimDirForward;
    m_currentLoop = 0;
    m_lastPhaseTicks = 0;
    m_isComplete = false;
    m_phase = 0;
}

void Animator::unserialize(int animationPhases, const FileStreamPtr& fin)
{
    m_animationPhases = animationPhases;
    m_async = fin->getU8() == 0;
    m_loopCount = fin->get32();
    m_startPhase = fin->get8();

    for(int i = 0; i < m_animationPhases; ++i) {
        int minimum = fin->getU32();
        int maximum = fin->getU32();
        m_phaseDurations.emplace_back(minimum, maximum);
    }

    m_phase = getStartPhase();

    assert(m_animationPhases == (int)m_phaseDurations.size());
    assert(m_startPhase >= -1 && m_startPhase < m_animationPhases);
}

void Animator::serialize(const FileStreamPtr& fin)
{
    fin->addU8(m_async ? 0 : 1);
    fin->add32(m_loopCount);
    fin->add8(m_startPhase);

    for(std::tuple<int, int> phase : m_phaseDurations) {
        fin->addU32(std::get<0>(phase));
        fin->addU32(std::get<1>(phase));
    }
}

void Animator::setPhase(int phase)
{
    if(m_phase == phase) return;

    if(m_async) {
        if(phase == AnimPhaseAsync)
            m_phase = 0;
        else if(phase == AnimPhaseRandom)
            m_phase = (int)stdext::random_range(0, (long)m_animationPhases);
        else if(phase >= 0 && phase < m_animationPhases)
            m_phase = phase;
        else
            m_phase = getStartPhase();

        m_isComplete = false;
        m_lastPhaseTicks = g_clock.millis();
        m_currentDuration = getPhaseDuration(phase);
        m_currentLoop = 0;
    } else
        calculateSynchronous();
}

int Animator::getPhase()
{
    ticks_t ticks = g_clock.millis();
    if(ticks != m_lastPhaseTicks && !m_isComplete) {
        int elapsedTicks = (int)(ticks - m_lastPhaseTicks);
        if(elapsedTicks >= m_currentDuration) {
            int phase = 0;
            if(m_loopCount < 0)
                phase = getPingPongPhase();
            else
                phase = getLoopPhase();

            if(m_phase != phase) {
                int duration = getPhaseDuration(phase) - (elapsedTicks - m_currentDuration);
                if(duration < 0 && !m_async) {
                    calculateSynchronous();
                } else {
                    m_phase = phase;
                    m_currentDuration = std::max<int>(0, duration);
                }
            } else
                m_isComplete = true;
        } else
            m_currentDuration -= elapsedTicks;

        m_lastPhaseTicks = ticks;
    }
    return m_phase;
}

int Animator::getPhaseAt(ticks_t time)
{
    int index = 0;
    ticks_t total = 0;

    for(const auto &pair: m_phaseDurations) {
        total += std::get<1>(pair);

        if (time < total) {
            return index;
        }

        ++index;
    }

    return std::min<int>(index, m_animationPhases - 1);
}

int Animator::getStartPhase()
{
    if(m_startPhase > -1)
        return m_startPhase;
    return (int)stdext::random_range(0, (long)m_animationPhases);
}

void Animator::resetAnimation()
{
    m_isComplete = false;
    m_currentDirection = AnimDirForward;
    m_currentLoop = 0;
    setPhase(AnimPhaseAutomatic);
}

int Animator::getPingPongPhase()
{
    int count = m_currentDirection == AnimDirForward ? 1 : -1;
    int nextPhase = m_phase + count;
    if(nextPhase < 0 || nextPhase >= m_animationPhases) {
        m_currentDirection = m_currentDirection == AnimDirForward ? AnimDirBackward : AnimDirForward;
        count *= -1;
    }
    return m_phase + count;
}

int Animator::getLoopPhase()
{
    int nextPhase = m_phase + 1;
    if(nextPhase < m_animationPhases)
        return nextPhase;

    if(m_loopCount == 0)
        return 0;

    if(m_currentLoop < (m_loopCount - 1)) {
        m_currentLoop++;
        return 0;
    }

    return m_phase;
}

int Animator::getPhaseDuration(int phase)
{
    assert(phase < (int)m_phaseDurations.size());

    std::tuple<int, int> data = m_phaseDurations.at(phase);
    int min = std::get<0>(data);
    int max = std::get<1>(data);
    if(min == max) return min;
    return (int)stdext::random_range((long)min, (long)max);
}

void Animator::calculateSynchronous()
{
    int totalDuration = 0;
    for(int i = 0; i < m_animationPhases; i++)
        totalDuration += getPhaseDuration(i);

    ticks_t ticks = g_clock.millis();
    int elapsedTicks = (int)(ticks % totalDuration);
    int totalTime = 0;
    for(int i = 0; i < m_animationPhases; i++) {
        int duration = getPhaseDuration(i);
        if(elapsedTicks >= totalTime && elapsedTicks < totalTime + duration) {
            m_phase = i;
            m_currentDuration = duration - (elapsedTicks - totalTime);
            break;
        }
        totalTime += duration;
    }
    m_lastPhaseTicks = ticks;
}

ticks_t Animator::getTotalDuration()
{
    ticks_t time = 0;
    for (const auto &pair: m_phaseDurations) {
        time += std::get<1>(pair);
    }

    return time;
}
