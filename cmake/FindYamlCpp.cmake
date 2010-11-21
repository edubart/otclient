# - Try to find yaml-cpp
# Once done, this will define
#
# YAMLCPP_LIBRARY, link these to use yaml-cpp
# YAMLCPP_FOUND - system has yaml-cpp
# YAMLCPP_INCLUDE_DIR, the yaml-cpp include directories

FIND_PATH(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h
HINTS
$ENV{PHYSFSDIR}
PATH_SUFFIXES include/yaml-cpp include
PATHS
~/Library/Frameworks
/Library/Frameworks
/usr/local
/usr
/sw # Fink
/opt/local # DarwinPorts
/opt/csw # Blastwave
/opt
)

FIND_LIBRARY(YAMLCPP_LIBRARY
NAMES yaml-cpp
HINTS
$ENV{PHYSFSDIR}
PATH_SUFFIXES lib64 lib
PATHS
~/Library/Frameworks
/Library/Frameworks
/usr/local
/usr
/sw
/opt/local
/opt/csw
/opt
)

INCLUDE("${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YamlCpp DEFAULT_MSG YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)