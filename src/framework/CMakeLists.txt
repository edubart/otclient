# CMAKE_CURRENT_LIST_DIR cmake 2.6 compatiblity
IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6)
    GET_FILENAME_COMPONENT(CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6)

# add framework cmake modules
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")

OPTION(CRASH_HANDLER "Generate crash reports" ON)
OPTION(LUAJIT "Use lua jit" OFF)
SET(OPENGLES "OFF" CACHE "Use OpenGL ES 1.0 or 2.0 (for mobiles devices)" STRING)
SET(BUILD_REVISION "custom" CACHE "Git revision string (intended for releases)" STRING)

# set debug as default build type
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF()

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH_FLAGS "-m64 -march=x86-64 -mtune=generic")
ELSE()
    SET(ARCH_FLAGS "-m32 -march=i686 -mtune=generic")
ENDIF()
SET(WARNS_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable")
SET(CMAKE_CXX_FLAGS                   "${CMAKE_CXX_FLAGS} ${CXX_WARNS} ${ARCH_FLAGS} -std=gnu++0x -pipe")
SET(CMAKE_CXX_FLAGS_DEBUG             "-O0 -ggdb")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO    "-O1 -ggdb -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS_RELEASE           "-O2")

SET(CMAKE_CXX_LINK_FLAGS              "${CMAKE_CXX_LINK_FLAGS} -static-libgcc -static-libstdc++ -Wl,--as-needed")

IF(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_CXX_LINK_FLAGS              "${CMAKE_CXX_LINK_FLAGS} -Wl,-s")
ENDIF()

MESSAGE(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
ADD_DEFINITIONS(-D"BUILD_TYPE=\\\"${CMAKE_BUILD_TYPE}\\\"")

MESSAGE(STATUS "Build revision: ${BUILD_REVISION}")
ADD_DEFINITIONS(-D"BUILD_REVISION=\\\"${BUILD_REVISION}\\\"")

# find needed libraries
IF(WIN32)
    SET(Boost_THREADAPI win32)
ENDIF()
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

IF(OPENGLES STREQUAL "2.0")
    FIND_PACKAGE(OpenGLES2 REQUIRED)
    FIND_PACKAGE(EGL REQUIRED)
    ADD_DEFINITIONS(-DOPENGL_ES=2)
    SET(OPENGL_INCLUDE_DIR ${EGL_INCLUDE_DIR} ${OPENGLES2_INCLUDE_DIR})
    SET(OPENGL_LIBRARIES ${EGL_LIBRARY} ${OPENGLES2_LIBRARY})

    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl2.cpp)
ELSEIF(OPENGLES STREQUAL "1.0")
    FIND_PACKAGE(OpenGLES1 REQUIRED)
    FIND_PACKAGE(EGL REQUIRED)
    ADD_DEFINITIONS(-DOPENGL_ES=1)
    SET(OPENGL_INCLUDE_DIR ${EGL_INCLUDE_DIR} ${OPENGLES1_INCLUDE_DIR})
    SET(OPENGL_LIBRARIES ${EGL_LIBRARY} ${OPENGLES1_LIBRARY})

    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl1.cpp)
ELSE()
    FIND_PACKAGE(OpenGL REQUIRED)
    FIND_PACKAGE(GLEW REQUIRED)
    SET(OPENGL_LIBRARIES ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})

    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl1.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl2.cpp)
ENDIF()
MESSAGE(STATUS "OpenGL ES: " ${OPENGLES})

IF(LUAJIT)
    FIND_PACKAGE(LuaJIT REQUIRED)
    SET(LUA_INCLUDE_DIR ${LUAJIT_INCLUDE_DIR})
    SET(LUA_LIBRARIES ${LUAJIT_LIBRARIES})
ELSE()
    FIND_PACKAGE(Lua REQUIRED)
ENDIF()
MESSAGE(STATUS "LuaJIT: " ${LUAJIT})

FIND_PACKAGE(PhysFS REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(VorbisFile REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    MESSAGE(STATUS "Debug information: ON")
    ADD_DEFINITIONS(-DDEBUG)
ELSE()
    MESSAGE(STATUS "Debug information: OFF")

    # NDEBUG disable asserts
    ADD_DEFINITIONS(-DNDEBUG)

    # strip all debug information
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -s")
ENDIF()

IF(CRASH_HANDLER)
    ADD_DEFINITIONS(-DCRASH_HANDLER)
    MESSAGE(STATUS "Crash handler: ON")
ELSE()
    MESSAGE(STATUS "Crash handler: OFF")
ENDIF()

IF(WIN32)
    OPTION(WINDOWS_CONSOLE "Enables console window on Windows platform" OFF)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthreads")
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
    SET(ADDITIONAL_LIBRARIES ws2_32 mswsock imagehlp winmm)

    # fix boost thread linkage
    ADD_DEFINITIONS(-DBOOST_THREAD_USE_LIB)

    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/platform/win32window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/win32crashhandler.cpp)

    IF(WINDOWS_CONSOLE)
        MESSAGE(STATUS "Windows console: ON")
    ELSE()
        SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
        MESSAGE(STATUS "Windows console: OFF")
    ENDIF()
ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -rdynamic")

    SET(ADDITIONAL_LIBRARIES X11 dl)
    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/platform/x11window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/unixcrashhandler.cpp)
ENDIF()


INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR} ${VORBISFILE_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    "${CMAKE_CURRENT_LIST_DIR}/.."
)

SET(framework_LIBRARIES
    ${Boost_LIBRARIES}
    ${LUA_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${GMP_LIBRARY}
    ${ZLIB_LIBRARY}
    ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${ADDITIONAL_LIBRARIES}
)

SET(framework_SOURCES ${framework_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/application.cpp
    ${CMAKE_CURRENT_LIST_DIR}/application.h
    ${CMAKE_CURRENT_LIST_DIR}/const.h
    ${CMAKE_CURRENT_LIST_DIR}/global.h
    ${CMAKE_CURRENT_LIST_DIR}/pch.h
    ${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp

    # core
    ${CMAKE_CURRENT_LIST_DIR}/core/adaptativeframecounter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/adaptativeframecounter.h
    ${CMAKE_CURRENT_LIST_DIR}/core/clock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/clock.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/configmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/core/eventdispatcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/eventdispatcher.h
    ${CMAKE_CURRENT_LIST_DIR}/core/filestream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/filestream.h
    ${CMAKE_CURRENT_LIST_DIR}/core/inputevent.h
    ${CMAKE_CURRENT_LIST_DIR}/core/logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/logger.h
    ${CMAKE_CURRENT_LIST_DIR}/core/module.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/module.h
    ${CMAKE_CURRENT_LIST_DIR}/core/modulemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/modulemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/resourcemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/resourcemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/timer.h

    # graphics
    ${CMAKE_CURRENT_LIST_DIR}/graphics/animatedtexture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/animatedtexture.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/cachedtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/cachedtext.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/coordsbuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/coordsbuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/font.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/font.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/fontmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/fontmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/glutil.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/graphics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/graphics.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/hardwarebuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/image.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painter.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl1.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl1.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl2.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painterogl2_shadersources.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/paintershaderprogram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/paintershaderprogram.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particleaffector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particleaffector.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particleemitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particleemitter.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particle.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particlemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particlemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particlesystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particlesystem.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/shader.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/shaderprogram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/shaderprogram.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/texture.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/texturemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/texturemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/graphics/vertexarray.h

    # luascript
    ${CMAKE_CURRENT_LIST_DIR}/luascript/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luabinder.h
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luaexception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luaexception.h
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luainterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luainterface.h
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luaobject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luaobject.h
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luavaluecasts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luavaluecasts.h

    # net
    ${CMAKE_CURRENT_LIST_DIR}/net/connection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/connection.h
    ${CMAKE_CURRENT_LIST_DIR}/net/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/net/inputmessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/inputmessage.h
    ${CMAKE_CURRENT_LIST_DIR}/net/outputmessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/outputmessage.h
    ${CMAKE_CURRENT_LIST_DIR}/net/protocol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocol.h
    ${CMAKE_CURRENT_LIST_DIR}/net/server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/server.h

    # otml
    ${CMAKE_CURRENT_LIST_DIR}/otml/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmldocument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmldocument.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlemitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlemitter.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlexception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlexception.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otml.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlnode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlnode.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlparser.h

    # platform
    ${CMAKE_CURRENT_LIST_DIR}/platform/crashhandler.h
    ${CMAKE_CURRENT_LIST_DIR}/platform/platformwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/platformwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/platform/unixcrashhandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/win32crashhandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/win32window.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/win32window.h
    ${CMAKE_CURRENT_LIST_DIR}/platform/x11window.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/x11window.h

    # sound
    ${CMAKE_CURRENT_LIST_DIR}/sound/combinedsoundsource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/combinedsoundsource.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/oggsoundfile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/oggsoundfile.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundbuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundbuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundfile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundfile.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundsource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundsource.h
    ${CMAKE_CURRENT_LIST_DIR}/sound/streamsoundsource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/streamsoundsource.h

    # stdext
    ${CMAKE_CURRENT_LIST_DIR}/stdext/cast.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/compiler.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/demangle.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/dumper.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/exception.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/math.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/stdext.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/string.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/time.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/types.h

    # third party
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/apngloader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/apngloader.h
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/lbitlib-5.2.0-backport4.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/lbitlib-5.2.0-backport4.h

    # ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uianchorlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uianchorlayout.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiboxlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiboxlayout.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uigridlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uigridlayout.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/ui.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uihorizontallayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uihorizontallayout.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uilayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uilayout.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimanager.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uirichtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uirichtext.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uitextedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uitextedit.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uitranslator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uitranslator.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiverticallayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiverticallayout.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgetbasestyle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidget.h
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgetimage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgettext.cpp

    # util
    ${CMAKE_CURRENT_LIST_DIR}/util/boolean.h
    ${CMAKE_CURRENT_LIST_DIR}/util/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/color.h
    ${CMAKE_CURRENT_LIST_DIR}/util/crypt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/crypt.h
    ${CMAKE_CURRENT_LIST_DIR}/util/databuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/util/matrix.h
    ${CMAKE_CURRENT_LIST_DIR}/util/point.h
    ${CMAKE_CURRENT_LIST_DIR}/util/rect.h
    ${CMAKE_CURRENT_LIST_DIR}/util/rsa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/rsa.h
    ${CMAKE_CURRENT_LIST_DIR}/util/size.h
)
