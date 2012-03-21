/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "crashhandler.h"
#include <framework/global.h>
#include <framework/application.h>

#include <windows.h>
#include <process.h>
#include <imagehlp.h>

const char *getExceptionName(DWORD exceptionCode)
{
    switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:         return "Access violation";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Datatype misalignment";
        case EXCEPTION_BREAKPOINT:               return "Breakpoint";
        case EXCEPTION_SINGLE_STEP:              return "Single step";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array bounds exceeded";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Float denormal operand";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Float divide by zero";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "Float inexact result";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "Float invalid operation";
        case EXCEPTION_FLT_OVERFLOW:             return "Float overflow";
        case EXCEPTION_FLT_STACK_CHECK:          return "Float stack check";
        case EXCEPTION_FLT_UNDERFLOW:            return "Float underflow";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer divide by zero";
        case EXCEPTION_INT_OVERFLOW:             return "Integer overflow";
        case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged instruction";
        case EXCEPTION_IN_PAGE_ERROR:            return "In page error";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal instruction";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception";
        case EXCEPTION_STACK_OVERFLOW:           return "Stack overflow";
        case EXCEPTION_INVALID_DISPOSITION:      return "Invalid disposition";
        case EXCEPTION_GUARD_PAGE:               return "Guard page";
        case EXCEPTION_INVALID_HANDLE:           return "Invalid handle";
    }
    return "Unknown exception";
}

void Stacktrace(LPEXCEPTION_POINTERS e, std::stringstream& ss)
{
    PIMAGEHLP_SYMBOL pSym;
    STACKFRAME sf;
    HANDLE process, thread;
    DWORD dwModBase, Disp;
    BOOL more = FALSE;
    int count = 0;
    char modname[MAX_PATH];

    pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 16384);

    ZeroMemory(&sf, sizeof(sf));
    sf.AddrPC.Offset = e->ContextRecord->Eip;
    sf.AddrStack.Offset = e->ContextRecord->Esp;
    sf.AddrFrame.Offset = e->ContextRecord->Ebp;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;

    process = GetCurrentProcess();
    thread = GetCurrentThread();

    while(1) {
        more = StackWalk(IMAGE_FILE_MACHINE_I386,  process, thread, &sf, e->ContextRecord, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL);
        if(!more || sf.AddrFrame.Offset == 0)
            break;

        dwModBase = SymGetModuleBase(process, sf.AddrPC.Offset);

        if(dwModBase)
            GetModuleFileName((HINSTANCE)dwModBase, modname, MAX_PATH);
        else
            strcpy(modname, "Unknown");

        pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
        pSym->MaxNameLength = MAX_PATH;

        if(SymGetSymFromAddr(process, sf.AddrPC.Offset, &Disp, pSym)) {
            // this is the code path taken on VC if debugging syms are found.
            ss << Fw::formatString("    (%d) %s(%s+%#0lx) [0x%08lX]\n", count, modname, pSym->Name, Disp, sf.AddrPC.Offset);
        } else {
            // this is the code path taken on MinGW, and VC if no debugging syms are found.
            ss << Fw::formatString("    (%d) %s [0x%08lX]\n", count, modname, sf.AddrPC.Offset);
        }
        ++count;
    }
    GlobalFree(pSym);
}

LONG CALLBACK ExceptionHandler(LPEXCEPTION_POINTERS e)
{
    char date[32];
    time_t tnow;
    time(&tnow);
    tm *ts = localtime(&tnow);
    strftime(date, 32, "%b %d %Y %H:%M:%S", ts);

    // generate crash report
    SymInitialize(GetCurrentProcess(), 0, TRUE);
    std::stringstream ss;
    ss << "== application crashed\n";
    ss << Fw::formatString("app name: %s\n", g_app->getName().c_str());
    ss << Fw::formatString("app version: %s\n", g_app->getVersion().c_str());
    ss << Fw::formatString("app build date: %s\n", g_app->getBuildDate().c_str());
    ss << Fw::formatString("crash date: %s\n", date);
    ss << Fw::formatString("exception: %s (0x%08lx)\n", getExceptionName(e->ExceptionRecord->ExceptionCode), e->ExceptionRecord->ExceptionCode);
    ss << Fw::formatString("exception address: 0x%08lx\n", (long unsigned int)e->ExceptionRecord->ExceptionAddress);
    ss << Fw::formatString("backtrace:\n");
    Stacktrace(e, ss);
    ss << "\n";
    SymCleanup(GetCurrentProcess());

    // write stacktrace to crash.txt
    char dir[MAX_PATH];
    GetCurrentDirectory(sizeof(dir) - 1, dir);
    std::string fileName = Fw::formatString("%s\\crash_report.txt", dir);


    std::ofstream fout(fileName.c_str(), std::ios_base::out | std::ios_base::ate);
    fout << ss.str();
    fout.close();

    // print in stdout
    logInfo(ss.str());
    logInfo("Crash report saved to file ", fileName);

    // inform the user
    std::string msg = Fw::formatString("The application has crashed.\n\n"
                                       "A crash report has been written to:\n"
                                       "%s", fileName.c_str());
    MessageBox(NULL, msg.c_str(), "Application crashed", 0);

    // this seems to silently close the application
    return EXCEPTION_EXECUTE_HANDLER;

    // this triggers the microsoft "application has crashed" error dialog
    //return EXCEPTION_CONTINUE_SEARCH;
}

void installCrashHandler()
{
    SetUnhandledExceptionFilter(ExceptionHandler);
}
