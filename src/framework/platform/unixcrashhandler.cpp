/*
 * Copyright (c) 2010-2016 OTClient <https://github.com/edubart/otclient>
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

#if !defined(WIN32) && defined(CRASH_HANDLER)

#include "crashhandler.h"
#include <framework/global.h>
#include <framework/core/application.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <csignal>
#include <execinfo.h>
#include <ucontext.h>

#define MAX_BACKTRACE_DEPTH 128
#define DEMANGLE_BACKTRACE_SYMBOLS

void crashHandler(int signum, siginfo_t* info, void* secret)
{
    g_logger.error("Application crashed");

    std::stringstream ss;
    ss << stdext::format("app name: %s\n", g_app.getName());
    ss << stdext::format("app version: %s\n", g_app.getVersion());
    ss << stdext::format("build compiler: %s\n", BUILD_COMPILER);
    ss << stdext::format("build date: %s\n", __DATE__);
    ss << stdext::format("build type: %s\n", BUILD_TYPE);
    ss << stdext::format("build revision: %s (%s)\n", BUILD_REVISION, BUILD_COMMIT);
    ss << stdext::format("crash date: %s\n", stdext::date_time_string());
    ss.flags(std::ios::hex | std::ios::showbase);

    ucontext_t context = *(ucontext_t*)secret;
#if __WORDSIZE == 64
    ss << "  at rip = " << context.uc_mcontext.gregs[REG_RIP] << std::endl;
    ss << "     rax = " << context.uc_mcontext.gregs[REG_RAX] << std::endl;
    ss << "     rbx = " << context.uc_mcontext.gregs[REG_RBX] << std::endl;
    ss << "     rcx = " << context.uc_mcontext.gregs[REG_RCX] << std::endl;
    ss << "     rdx = " << context.uc_mcontext.gregs[REG_RDX] << std::endl;
    ss << "     rsi = " << context.uc_mcontext.gregs[REG_RSI] << std::endl;
    ss << "     rdi = " << context.uc_mcontext.gregs[REG_RDI] << std::endl;
    ss << "     rbp = " << context.uc_mcontext.gregs[REG_RBP] << std::endl;
    ss << "     rsp = " << context.uc_mcontext.gregs[REG_RSP] << std::endl;
    ss << "     efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
    ss << std::endl;
#elif defined(REG_EIP)
    ss << "  at eip = " << context.uc_mcontext.gregs[REG_EIP] << std::endl;
    ss << "     eax = " << context.uc_mcontext.gregs[REG_EAX] << std::endl;
    ss << "     ebx = " << context.uc_mcontext.gregs[REG_EBX] << std::endl;
    ss << "     ecx = " << context.uc_mcontext.gregs[REG_ECX] << std::endl;
    ss << "     edx = " << context.uc_mcontext.gregs[REG_EDX] << std::endl;
    ss << "     esi = " << context.uc_mcontext.gregs[REG_ESI] << std::endl;
    ss << "     edi = " << context.uc_mcontext.gregs[REG_EDI] << std::endl;
    ss << "     ebp = " << context.uc_mcontext.gregs[REG_EBP] << std::endl;
    ss << "     esp = " << context.uc_mcontext.gregs[REG_ESP] << std::endl;
    ss << "     efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
    ss << std::endl;
#endif

    ss.flags(std::ios::dec);
    ss << "  backtrace:" << std::endl;

    void* buffer[MAX_BACKTRACE_DEPTH];
    int numLevels = backtrace(buffer, MAX_BACKTRACE_DEPTH);
    char **tracebackBuffer = backtrace_symbols(buffer, numLevels);
    if(tracebackBuffer) {
        for(int i = 2; i < numLevels; i++) {
            std::string line = tracebackBuffer[i];
            if(line.find("__libc_start_main") != std::string::npos)
                break;
#ifdef DEMANGLE_BACKTRACE_SYMBOLS
            std::size_t demanglePos = line.find("(_Z");
            if(demanglePos != std::string::npos) {
                demanglePos++;
                int len = std::min(line.find_first_of("+", demanglePos), line.find_first_of(")", demanglePos)) - demanglePos;
                std::string funcName = line.substr(demanglePos, len);
                line.replace(demanglePos, len, stdext::demangle_name(funcName.c_str()));
            }
#endif
            ss << "    " << i-1 << ": " << line << std::endl;
        }
        free(tracebackBuffer);
    }

    g_logger.info(ss.str());

    std::string fileName = "crash_report.log";
    std::ofstream fout(fileName.c_str(), std::ios::out | std::ios::app);
    if(fout.is_open() && fout.good()) {
        fout << "== application crashed\n";
        fout << ss.str();
        fout << "\n";
        fout.close();
        g_logger.info(stdext::format("Crash report saved to file %s", fileName));
    } else
        g_logger.error("Failed to save crash report!");

    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

void installCrashHandler()
{
    struct sigaction sa;
    sa.sa_sigaction = &crashHandler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    sigaction(SIGILL, &sa, nullptr);   // illegal instruction
    sigaction(SIGSEGV, &sa, nullptr);  // segmentation fault
    sigaction(SIGFPE, &sa, nullptr);   // floating-point exception
    sigaction(SIGABRT, &sa, nullptr);  // process aborted (asserts)
}

#endif
