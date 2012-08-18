# Try to find the lua library
#  LUAJIT_FOUND - system has lua
#  LUAJIT_INCLUDE_DIR - the lua include directory
#  LUAJIT_LIBRARY - the lua library
#  LUAJIT_LIBRARIES - the lua library and it's dependencies

FIND_PATH(LUAJIT_INCLUDE_DIR NAMES lua.h PATH_SUFFIXES luajit-2.0)
SET(_LUAJIT_STATIC_LIBS libluajit51.a libluajit5.1.a libluajit-5.1.a libluajit.a)
SET(_LUAJIT_SHARED_LIBS libluajit51.dll.a libluajit5.1.dll.a libluajit-5.1.dll.a libluajit.dll.a luajit51 luajit5.1 luajit-5.1 luajit)
IF(USE_STATIC_LIBS)
    FIND_LIBRARY(LUAJIT_LIBRARY NAMES ${_LUAJIT_STATIC_LIBS} ${_LUAJIT_SHARED_LIBS})
ELSE()
    FIND_LIBRARY(LUAJIT_LIBRARY NAMES ${_LUAJIT_SHARED_LIBS} ${_LUAJIT_STATIC_LIBS})
ENDIF()
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LuaJIT DEFAULT_MSG LUAJIT_LIBRARY LUAJIT_INCLUDE_DIR)
MARK_AS_ADVANCED(LUAJIT_LIBRARY LUAJIT_INCLUDE_DIR)