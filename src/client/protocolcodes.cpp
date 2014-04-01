/*
 * Copyright (c) 2010-2014 OTClient <https://github.com/edubart/otclient>
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

#include "protocolcodes.h"

namespace Proto {

std::map<uint8, uint8> messageModesMap;

void buildMessageModesMap(int version) {
    messageModesMap.clear();
    if(version >= 1036) {
            for(int i = Otc::MessageNone; i <= Otc::MessageBeyondLast; ++i) {
                if(i >= Otc::MessageNpcTo)
                    messageModesMap[i] = i + 1;
                else
                    messageModesMap[i] = i;
            }
    } else if(version >= 900) {
        for(int i = Otc::MessageNone; i <= Otc::MessageBeyondLast; ++i)
            messageModesMap[i] = i;
    } else if(version >= 861) {
        messageModesMap[Otc::MessageNone]                    = 0;
        messageModesMap[Otc::MessageSay]                     = 1;
        messageModesMap[Otc::MessageWhisper]                 = 2;
        messageModesMap[Otc::MessageYell]                    = 3;
        messageModesMap[Otc::MessageNpcTo]                   = 4;
        messageModesMap[Otc::MessageNpcFrom]                 = 5;
        messageModesMap[Otc::MessagePrivateFrom]             = 6;
        messageModesMap[Otc::MessagePrivateTo]               = 6;
        messageModesMap[Otc::MessageChannel]                 = 7;
        messageModesMap[Otc::MessageChannelManagement]       = 8;
        messageModesMap[Otc::MessageGamemasterBroadcast]     = 9;
        messageModesMap[Otc::MessageGamemasterChannel]       = 10;
        messageModesMap[Otc::MessageGamemasterPrivateFrom]   = 11;
        messageModesMap[Otc::MessageGamemasterPrivateTo]     = 11;
        messageModesMap[Otc::MessageChannelHighlight]        = 12;
        messageModesMap[Otc::MessageMonsterSay]              = 13;
        messageModesMap[Otc::MessageMonsterYell]             = 14;
        messageModesMap[Otc::MessageWarning]                 = 15;
        messageModesMap[Otc::MessageGame]                    = 16;
        messageModesMap[Otc::MessageLogin]                   = 17;
        messageModesMap[Otc::MessageStatus]                  = 18;
        messageModesMap[Otc::MessageLook]                    = 19;
        messageModesMap[Otc::MessageFailure]                 = 20;
        messageModesMap[Otc::MessageBlue]                    = 21;
        messageModesMap[Otc::MessageRed]                     = 22;
    } else if(version >= 840) {
        messageModesMap[Otc::MessageNone]                    = 0;
        messageModesMap[Otc::MessageSay]                     = 1;
        messageModesMap[Otc::MessageWhisper]                 = 2;
        messageModesMap[Otc::MessageYell]                    = 3;
        messageModesMap[Otc::MessageNpcTo]                   = 4;
        messageModesMap[Otc::MessageNpcFrom]                 = 5;
        messageModesMap[Otc::MessagePrivateFrom]             = 6;
        messageModesMap[Otc::MessagePrivateTo]               = 6;
        messageModesMap[Otc::MessageChannel]                 = 7;
        messageModesMap[Otc::MessageChannelManagement]       = 8;
        messageModesMap[Otc::MessageRVRChannel]              = 9;
        messageModesMap[Otc::MessageRVRAnswer]               = 10;
        messageModesMap[Otc::MessageRVRContinue]             = 11;
        messageModesMap[Otc::MessageGamemasterBroadcast]     = 12;
        messageModesMap[Otc::MessageGamemasterChannel]       = 13;
        messageModesMap[Otc::MessageGamemasterPrivateFrom]   = 14;
        messageModesMap[Otc::MessageGamemasterPrivateTo]     = 14;
        messageModesMap[Otc::MessageChannelHighlight]        = 15;
        // 16, 17, 18 ??
        messageModesMap[Otc::MessageRed]                     = 18;
        messageModesMap[Otc::MessageMonsterSay]              = 19;
        messageModesMap[Otc::MessageMonsterYell]             = 20;
        messageModesMap[Otc::MessageWarning]                 = 21;
        messageModesMap[Otc::MessageGame]                    = 22;
        messageModesMap[Otc::MessageLogin]                   = 23;
        messageModesMap[Otc::MessageStatus]                  = 24;
        messageModesMap[Otc::MessageLook]                    = 25;
        messageModesMap[Otc::MessageFailure]                 = 26;
        messageModesMap[Otc::MessageBlue]                    = 27;
    } else if(version >= 760) {
        messageModesMap[Otc::MessageNone]                    = 0;
        messageModesMap[Otc::MessageSay]                     = 1;
        messageModesMap[Otc::MessageWhisper]                 = 2;
        messageModesMap[Otc::MessageYell]                    = 3;
        messageModesMap[Otc::MessagePrivateFrom]             = 4;
        messageModesMap[Otc::MessagePrivateTo]               = 4;
        messageModesMap[Otc::MessageChannel]                 = 5;
        messageModesMap[Otc::MessageRVRChannel]              = 6;
        messageModesMap[Otc::MessageRVRAnswer]               = 7;
        messageModesMap[Otc::MessageRVRContinue]             = 8;
        messageModesMap[Otc::MessageGamemasterBroadcast]     = 9;
        messageModesMap[Otc::MessageGamemasterChannel]       = 10;
        messageModesMap[Otc::MessageGamemasterPrivateFrom]   = 11;
        messageModesMap[Otc::MessageGamemasterPrivateTo]     = 11;
        messageModesMap[Otc::MessageChannelHighlight]        = 12;
        // 13, 14, 15 ??
        messageModesMap[Otc::MessageMonsterSay]              = 16;
        messageModesMap[Otc::MessageMonsterYell]             = 17;
        messageModesMap[Otc::MessageWarning]                 = 18;
        messageModesMap[Otc::MessageGame]                    = 19;
        messageModesMap[Otc::MessageLogin]                   = 20;
        messageModesMap[Otc::MessageStatus]                  = 21;
        messageModesMap[Otc::MessageLook]                    = 22;
        messageModesMap[Otc::MessageFailure]                 = 23;
        messageModesMap[Otc::MessageBlue]                    = 24;
        messageModesMap[Otc::MessageRed]                     = 25;
    }
}

Otc::MessageMode translateMessageModeFromServer(uint8 mode)
{
    auto it = std::find_if(messageModesMap.begin(), messageModesMap.end(), [=] (const std::pair<uint8, uint8>& p) { return p.second == mode; });
    if(it != messageModesMap.end())
        return (Otc::MessageMode)it->first;
    return Otc::MessageInvalid;
}

uint8 translateMessageModeToServer(Otc::MessageMode mode)
{
    if(mode < 0 || mode >= Otc::LastMessage)
        return Otc::MessageInvalid;
    auto it = messageModesMap.find(mode);
    if(it != messageModesMap.end())
        return it->second;
    return Otc::MessageInvalid;
}

}
