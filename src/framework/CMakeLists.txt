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

# setup compiler options
SET(CXX_WARNS "-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable")
SET(CMAKE_CXX_FLAGS                   "${CMAKE_CXX_FLAGS} ${CXX_WARNS} -std=gnu++0x -pipe")
SET(CMAKE_C_FLAGS                     "${CMAKE_C_FLAGS} ${CXX_WARNS} -pipe")
SET(CMAKE_CXX_FLAGS_DEBUG             "-O0 -ggdb")
SET(CMAKE_C_FLAGS_DEBUG               "-O0 -ggdb")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO    "-O1 -ggdb")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO      "-O1 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE           "-Os -ffast-math")
SET(CMAKE_C_FLAGS_RELEASE             "-Os -ffast-math")
SET(CMAKE_CXX_FLAGS_RELFORPERFORMANCE "-Ofast -mmmx -msse -msse2")
SET(CMAKE_C_FLAGS_RELFORPERFORMANCE   "-Ofast -mmmx -msse -msse2")
SET(CMAKE_CXX_LINK_FLAGS              "${CMAKE_CXX_LINK_FLAGS} -static-libgcc -static-libstdc++ -Wl,--as-needed")

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
    # framework
    ${CMAKE_CURRENT_LIST_DIR}/application.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp

    # framework util
    ${CMAKE_CURRENT_LIST_DIR}/util/utf8.cpp
    ${CMAKE_CURRENT_LIST_DIR}/math/color.cpp

    # framework core
    ${CMAKE_CURRENT_LIST_DIR}/core/logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/configmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/resourcemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/eventdispatcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/modulemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/module.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/clock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/filestream.cpp

    # framework net
    ${CMAKE_CURRENT_LIST_DIR}/net/connection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/inputmessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/outputmessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/protocol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/rsa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/server.cpp

    # framework platform
    ${CMAKE_CURRENT_LIST_DIR}/platform/platformwindow.cpp

    # framework graphics
    ${CMAKE_CURRENT_LIST_DIR}/graphics/font.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/fontmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/graphics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/painter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/animatedtexture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/texturemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particlemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particlesystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particleemitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particleaffector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/particle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/shaderprogram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/paintershaderprogram.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/paintershadermanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/coordsbuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/graphics/image.cpp

    # framework sound
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundsource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundbuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundfile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/soundmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/oggsoundfile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/streamsoundsource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sound/combinedsoundsource.cpp

    # framework otml
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmldocument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlemitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlnode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlexception.cpp

    # framework luascript
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luainterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luaobject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luaexception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luascript/luavaluecasts.cpp

    # framework ui
    ${CMAKE_CURRENT_LIST_DIR}/ui/uimanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgetimage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgettext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgetbasestyle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uitextedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiboxlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uihorizontallayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiverticallayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uigridlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uianchorlayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uilayout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uiframecounter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ui/uitranslator.cpp

    # framework third party
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/apngloader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/lbitlib-5.2.0-backport4.cpp
)
