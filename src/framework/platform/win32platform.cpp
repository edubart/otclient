/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifdef WIN32

#include "platform.h"
#include <winsock2.h>
#include <windows.h>
#include <framework/stdext/stdext.h>
#include <boost/algorithm/string.hpp>
#include <tchar.h>

void Platform::processArgs(std::vector<std::string>& args)
{
    int nargs;
    wchar_t **wchar_argv = CommandLineToArgvW(GetCommandLineW(), &nargs);
    if(!wchar_argv)
        return;

    args.clear();
    if(wchar_argv) {
        for(int i=0;i<nargs;++i)
            args.push_back(stdext::utf16_to_utf8(wchar_argv[i]));
    }
}

bool Platform::spawnProcess(std::string process, const std::vector<std::string>& args)
{
    std::string commandLine;
    for(uint i = 0; i < args.size(); ++i)
        commandLine += stdext::format(" \"%s\"", args[i]);

    boost::replace_all(process, "/", "\\");
    if(!boost::ends_with(process, ".exe"))
        process += ".exe";

    std::wstring wfile = stdext::utf8_to_utf16(process);
    std::wstring wcommandLine = stdext::utf8_to_utf16(commandLine);

    if((size_t)ShellExecuteW(NULL, L"open", wfile.c_str(), wcommandLine.c_str(), NULL, SW_SHOWNORMAL) > 32)
        return true;
    return false;
}

int Platform::getProcessId()
{
    return GetCurrentProcessId();
}

bool Platform::isProcessRunning(const std::string& name)
{
    if(FindWindowA(name.c_str(), NULL) != NULL)
        return true;
    return false;
}

bool Platform::killProcess(const std::string& name)
{
    HWND window = FindWindowA(name.c_str(), NULL);
    if(window == NULL)
        return false;
    DWORD pid = GetProcessId(window);
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if(handle == NULL)
        return false;
    bool ok = TerminateProcess(handle, 1) != 0;
    CloseHandle(handle);
    return ok;
}

std::string Platform::getTempPath()
{
    std::string ret;
    wchar_t path[MAX_PATH];
    GetTempPathW(MAX_PATH, path);
    ret = stdext::utf16_to_utf8(path);
    boost::replace_all(ret, "\\", "/");
    return ret;
}

std::string Platform::getCurrentDir()
{
    std::string ret;
    wchar_t path[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, path);
    ret = stdext::utf16_to_utf8(path);
    boost::replace_all(ret, "\\", "/");
    ret += "/";
    return ret;
}

bool Platform::fileExists(std::string file)
{
    boost::replace_all(file, "/", "\\");
    std::wstring wfile = stdext::utf8_to_utf16(file);
    DWORD dwAttrib = GetFileAttributesW(wfile.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&  !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool Platform::copyFile(std::string from, std::string to)
{
    boost::replace_all(from, "/", "\\");
    boost::replace_all(to, "/", "\\");
    if(CopyFileW(stdext::utf8_to_utf16(from).c_str(), stdext::utf8_to_utf16(to).c_str(), FALSE) == 0)
        return false;
    return true;
}

bool Platform::removeFile(std::string file)
{
    boost::replace_all(file, "/", "\\");
    if(DeleteFileW(stdext::utf8_to_utf16(file).c_str()) == 0)
        return false;
    return true;
}

ticks_t Platform::getFileModificationTime(std::string file)
{
    boost::replace_all(file, "/", "\\");
    std::wstring wfile = stdext::utf8_to_utf16(file);
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData;
    memset(&fileAttrData, 0, sizeof(fileAttrData));
    GetFileAttributesExW(wfile.c_str(), GetFileExInfoStandard, &fileAttrData);
    ULARGE_INTEGER uli;
    uli.LowPart  = fileAttrData.ftLastWriteTime.dwLowDateTime;
    uli.HighPart = fileAttrData.ftLastWriteTime.dwHighDateTime;
    return uli.QuadPart;
}

void Platform::openUrl(std::string url)
{
    if(url.find("http://") == std::string::npos)
        url.insert(0, "http://");
    ShellExecuteW(NULL, L"open", stdext::utf8_to_utf16(url).c_str(), NULL, NULL, SW_SHOWNORMAL);
}

std::string Platform::getCPUName()
{
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    DWORD bufSize = sizeof(buf);
    HKEY hKey;
    if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return "";
    RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)buf, (LPDWORD)&bufSize);
    return buf;
}

double Platform::getTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}

#ifndef PRODUCT_PROFESSIONAL
#define PRODUCT_PROFESSIONAL    0x00000030
#define VER_SUITE_WH_SERVER     0x00008000
#define VER_PLATFORM_WIN32s             0
#define VER_PLATFORM_WIN32_WINDOWS      1
#define VER_PLATFORM_WIN32_NT           2
#define PRODUCT_UNDEFINED                           0x00000000
#define PRODUCT_ULTIMATE                            0x00000001
#define PRODUCT_HOME_BASIC                          0x00000002
#define PRODUCT_HOME_PREMIUM                        0x00000003
#define PRODUCT_ENTERPRISE                          0x00000004
#define PRODUCT_HOME_BASIC_N                        0x00000005
#define PRODUCT_BUSINESS                            0x00000006
#define PRODUCT_STANDARD_SERVER                     0x00000007
#define PRODUCT_DATACENTER_SERVER                   0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER                0x00000009
#define PRODUCT_ENTERPRISE_SERVER                   0x0000000A
#define PRODUCT_STARTER                             0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE              0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE                0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE              0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64              0x0000000F
#define PRODUCT_BUSINESS_N                          0x00000010
#define PRODUCT_WEB_SERVER                          0x00000011
#define PRODUCT_CLUSTER_SERVER                      0x00000012
#define PRODUCT_HOME_SERVER                         0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER              0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER             0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER            0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER           0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS            0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM        0x00000019
#define PRODUCT_HOME_PREMIUM_N                      0x0000001A
#define PRODUCT_ENTERPRISE_N                        0x0000001B
#define PRODUCT_ULTIMATE_N                          0x0000001C
#define PRODUCT_WEB_SERVER_CORE                     0x0000001D
#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT    0x0000001E
#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY      0x0000001F
#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING     0x00000020
#define PRODUCT_SERVER_FOUNDATION                   0x00000021
#define PRODUCT_HOME_PREMIUM_SERVER                 0x00000022
#define PRODUCT_SERVER_FOR_SMALLBUSINESS_V          0x00000023
#define PRODUCT_STANDARD_SERVER_V                   0x00000024
#define PRODUCT_DATACENTER_SERVER_V                 0x00000025
#define PRODUCT_ENTERPRISE_SERVER_V                 0x00000026
#define PRODUCT_DATACENTER_SERVER_CORE_V            0x00000027
#define PRODUCT_STANDARD_SERVER_CORE_V              0x00000028
#define PRODUCT_ENTERPRISE_SERVER_CORE_V            0x00000029
#define PRODUCT_HYPERV                              0x0000002A
#define PRODUCT_STORAGE_EXPRESS_SERVER_CORE         0x0000002B
#define PRODUCT_STORAGE_STANDARD_SERVER_CORE        0x0000002C
#define PRODUCT_STORAGE_WORKGROUP_SERVER_CORE       0x0000002D
#define PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE      0x0000002E
#define PRODUCT_STARTER_N                           0x0000002F
#define PRODUCT_PROFESSIONAL                        0x00000030
#define PRODUCT_PROFESSIONAL_N                      0x00000031
#define PRODUCT_SB_SOLUTION_SERVER                  0x00000032
#endif

std::string Platform::getOSName()
{
    typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
    typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

    std::string ret;
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    PGNSI pGNSI;
    PGPI pGPI;
    BOOL bOsVersionInfoEx;
    DWORD dwType;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    bOsVersionInfoEx = VerifyVersionInfo(&osvi, 0, 0);

    if(!bOsVersionInfoEx)
        return std::string();

    pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
    if(NULL != pGNSI)
        pGNSI(&si);
    else
        GetSystemInfo(&si);

    if(VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4) {
        if(osvi.dwMajorVersion == 6) {
            if(osvi.dwMinorVersion == 0) {
                if(osvi.wProductType == VER_NT_WORKSTATION)
                    ret += "Windows Vista ";
                else ret += "Windows Server 2008 ";
            }

            if(osvi.dwMinorVersion == 1 || osvi.dwMinorVersion == 2)
            {
                if(osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 1)
                    ret += "Windows 7 ";
                 else
                    if(osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 2)
                        ret += "Windows 8 ";
                    else
                        ret += "Windows Server 2008 R2 ";
            }

            pGPI = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");

            pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

            switch(dwType)
            {
                case PRODUCT_ULTIMATE:
                    ret += "Ultimate Edition";
                    break;
                case PRODUCT_PROFESSIONAL:
                    ret += "Professional";
                    break;
                case PRODUCT_HOME_PREMIUM:
                    ret += "Home Premium Edition";
                    break;
                case PRODUCT_HOME_BASIC:
                    ret += "Home Basic Edition";
                    break;
                case PRODUCT_ENTERPRISE:
                    ret += "Enterprise Edition";
                    break;
                case PRODUCT_BUSINESS:
                    ret += "Business Edition";
                    break;
                case PRODUCT_STARTER:
                    ret += "Starter Edition";
                    break;
                case PRODUCT_CLUSTER_SERVER:
                    ret += "Cluster Server Edition";
                    break;
                case PRODUCT_DATACENTER_SERVER:
                    ret += "Datacenter Edition";
                    break;
                case PRODUCT_DATACENTER_SERVER_CORE:
                    ret += "Datacenter Edition (core installation)";
                    break;
                case PRODUCT_ENTERPRISE_SERVER:
                    ret += "Enterprise Edition";
                    break;
                case PRODUCT_ENTERPRISE_SERVER_CORE:
                    ret += "Enterprise Edition (core installation)";
                    break;
                case PRODUCT_ENTERPRISE_SERVER_IA64:
                    ret += "Enterprise Edition for Itanium-based Systems";
                    break;
                case PRODUCT_SMALLBUSINESS_SERVER:
                    ret += "Small Business Server";
                    break;
                case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                    ret += "Small Business Server Premium Edition";
                    break;
                case PRODUCT_STANDARD_SERVER:
                    ret += "Standard Edition";
                    break;
                case PRODUCT_STANDARD_SERVER_CORE:
                    ret += "Standard Edition (core installation)";
                    break;
                case PRODUCT_WEB_SERVER:
                    ret += "Web Server Edition";
                    break;
            }
        }

        if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
            if(GetSystemMetrics(SM_SERVERR2))
                ret +=  "Windows Server 2003 R2, ";
            else if(osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
                ret +=  "Windows Storage Server 2003";
            else if(osvi.wSuiteMask & VER_SUITE_WH_SERVER)
                ret +=  "Windows Home Server";
            else if(osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
                ret +=  "Windows XP Professional x64 Edition";
            else
                ret += "Windows Server 2003, ";

            if(osvi.wProductType != VER_NT_WORKSTATION) {
                if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64) {
                    if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
                        ret +=  "Datacenter Edition for Itanium-based Systems";
                    else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                        ret +=  "Enterprise Edition for Itanium-based Systems";
                }

                else if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) {
                    if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
                        ret +=  "Datacenter x64 Edition";
                    else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                        ret +=  "Enterprise x64 Edition";
                    else
                        ret +=  "Standard x64 Edition";
                } else {
                    if(osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
                        ret +=  "Compute Cluster Edition";
                    else if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
                        ret +=  "Datacenter Edition";
                    else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                        ret +=  "Enterprise Edition";
                    else if(osvi.wSuiteMask & VER_SUITE_BLADE)
                        ret +=  "Web Edition";
                    else ret +=  "Standard Edition";
                }
            }
        }

        if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
            ret += "Windows XP ";
            if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
                ret +=  "Home Edition";
            else ret +=  "Professional";
        }

        if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) {
            ret += "Windows 2000 ";
            if(osvi.wProductType == VER_NT_WORKSTATION) {
                ret +=  "Professional";
            } else {
                if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
                    ret +=  "Datacenter Server";
                else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                    ret +=  "Advanced Server";
                else ret +=  "Server";
            }
        }

        ret += stdext::format(" (build %d)", osvi.dwBuildNumber);

        if(osvi.dwMajorVersion >= 6 ) {
            if(si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
                ret +=  ", 64-bit";
            else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
                ret += ", 32-bit";
        }
    } else {
        ret = "Windows";
    }
    return ret;
}


std::string Platform::traceback(const std::string& where, int level, int maxDepth)
{
    std::stringstream ss;
    ss << "\nat:";
    ss << "\n\t[C++]: " << where;
    return ss.str();
}

#endif
