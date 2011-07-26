#include "packages.h"

#include <core/resources.h>
#include <script/luainterface.h>

Packages g_packages;

void Packages::loadPackages()
{
    std::list<std::string> packages = g_resources.listDirectoryFiles("modules");
    foreach(const std::string& package, packages) {
        std::string dir = make_string("modules/", package);
        g_resources.pushCurrentPath(dir);

        std::list<std::string> packagesFiles = g_resources.listDirectoryFiles();
        foreach(const std::string& packageFile, packagesFiles) {
            if(boost::ends_with(packageFile, ".lua")) {
                g_lua.runScript(packageFile);
            }
        }

        g_resources.popCurrentPath();
    }
}

void Packages::terminate()
{

}
