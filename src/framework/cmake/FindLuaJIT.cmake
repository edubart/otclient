# Try to find the lua librairy
#  LUAJIT_FOUND - system has lua
#  LUAJIT_INCLUDE_DIR - the lua include directory
#  LUAJIT_LIBRARY - the lua library
#  LUAJIT_LIBRARIES - the lua library and it's dependencies

FIND_PATH(LUAJIT_INCLUDE_DIR NAMES lua.h PATH_SUFFIXES luajit-2.0)
FIND_LIBRARY(LUAJIT_LIBRARY NAMES libluajit51.a libluajit5.1.a libluajit-5.1.a libluajit.a luajit51 luajit5.1 luajit-5.1 luajit)
SET(LUAJIT_LIBRARIES "${LUAJIT_LIBRARY}")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LuaJIT DEFAULT_MSG LUAJIT_LIBRARIES LUAJIT_INCLUDE_DIR)
MARK_AS_ADVANCED(LUAJIT_INCLUDE_DIR LUAJIT_LIBRARIES LUAJIT_LIBRARY)

