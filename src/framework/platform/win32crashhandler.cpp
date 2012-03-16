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
#include <windows.h>
#include <dbghelp.h>
#include <framework/application.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
  HANDLE hProcess,
  DWORD ProcessId,
  HANDLE hFile,
  MINIDUMP_TYPE DumpType,
  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
  PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LONG WINAPI crashHandler(EXCEPTION_POINTERS* exceptionPointers)
{
    logError("Application crashed");
    HMODULE hDbgHelp = LoadLibraryA("DBGHELP.DLL");
    char fileName[128];

    if(hDbgHelp) {
        MINIDUMPWRITEDUMP minuDumpWriteDump = (MINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

        SYSTEMTIME systemTime;
        GetSystemTime(&systemTime);
        snprintf(fileName, 128, "%s_%02u-%02u-%04u_%02u-%02u-%02u.mdmp", g_app->getName().c_str(),
                systemTime.wDay, systemTime.wMonth, systemTime.wYear,
                systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

        HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile) {
            MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
            exceptionInformation.ClientPointers = FALSE;
            exceptionInformation.ExceptionPointers = exceptionPointers;
            exceptionInformation.ThreadId = GetCurrentThreadId();

            HANDLE hProcess = GetCurrentProcess();
            DWORD ProcessId = GetProcessId(hProcess);
            MINIDUMP_TYPE flags = (MINIDUMP_TYPE)(MiniDumpNormal);

            BOOL dumpResult = minuDumpWriteDump(hProcess, ProcessId, hFile, flags, &exceptionInformation, NULL, NULL);
            if(!dumpResult){
                logError("Cannot generate minidump: ", GetLastError());
                CloseHandle(hFile);
                DeleteFileA(fileName);
                return EXCEPTION_CONTINUE_SEARCH;
            } else {
                logInfo("Crash minidump genarated on file ", fileName);
            }
        } else {
            logError("Cannot create dump file: ", GetLastError());
        }
    } else {
        logError("Cannot create dump file: dbghlp.dll not found");
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void installCrashHandler()
{
    SetUnhandledExceptionFilter(crashHandler);
}
