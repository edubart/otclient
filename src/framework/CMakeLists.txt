# CMAKE_CURRENT_LIST_DIR cmake 2.6 compatiblity
IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6)
    GET_FILENAME_COMPONENT(CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
ENDIF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 6)

# add framework cmake modules
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")

# framework options
OPTION(NO_CONSOLE "Disables console window on Windows platform" OFF)
OPTION(CRASH_HANDLER "Generate crash reports" OFF)
OPTION(USE_OPENGL_ES2 "Use OpenGL ES 2.0 (for mobiles devices)" OFF)

# set debug as default build type
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)

# find needed libraries
SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

IF(USE_OPENGL_ES2)
    FIND_PACKAGE(OpenGLES2 REQUIRED)
    FIND_PACKAGE(EGL REQUIRED)
    ADD_DEFINITIONS(-DOPENGL_ES2)
ELSE(USE_OPENGL_ES2)
    FIND_PACKAGE(OpenGL REQUIRED)
    FIND_PACKAGE(GLEW REQUIRED)
ENDIF(USE_OPENGL_ES2)

FIND_PACKAGE(Lua REQUIRED)
FIND_PACKAGE(PhysFS REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

# setup compiler options
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CXX_WARNS "-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable")
    SET(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${CXX_WARNS} -std=gnu++0x -pipe")
    SET(CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} ${CXX_WARNS} -pipe")
    SET(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -ggdb")
    SET(CMAKE_C_FLAGS_DEBUG            "-O0 -g -ggdb")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O1 -g -ggdb")
    SET(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O1 -g -ggdb")
    SET(CMAKE_CXX_FLAGS_RELEASE        "-O2")
    SET(CMAKE_C_FLAGS_RELEASE          "-O2")
    #SET(CMAKE_CXX_FLAGS_PERFORMANCE    "-Ofast -fomit-frame-pointer")
    #SET(CMAKE_C_FLAGS_PERFORMANCE      "-Ofast -fomit-frame-pointer")
    SET(CMAKE_CXX_LINK_FLAGS           "${CMAKE_CXX_LINK_FLAGS} -static-libgcc -static-libstdc++ -Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

MESSAGE(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
IF(USE_OPENGL_ES2)
    MESSAGE(STATUS "Renderer: OpenGL ES 2.0")
ELSE(USE_OPENGL_ES2)
    MESSAGE(STATUS "Renderer: OpenGL")
ENDIF(USE_OPENGL_ES2)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    ADD_DEFINITIONS(-DDEBUG)
    MESSAGE(STATUS "Debug information: ON")
ELSE()
    MESSAGE(STATUS "Debug information: OFF")
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL "Release")
    # NDEBUG disable asserts
    ADD_DEFINITIONS(-DNDEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")

IF(CRASH_HANDLER)
    ADD_DEFINITIONS(-DCRASH_HANDLER)
    MESSAGE(STATUS "Crash handler: ON")
ELSE(CRASH_HANDLER)
    MESSAGE(STATUS "Crash handler: OFF")
ENDIF(CRASH_HANDLER)

IF(WIN32)
    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/platform/win32window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/win32crashhandler.cpp)
    SET(ADDITIONAL_LIBRARIES ws2_32 mswsock)

    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthreads")

    IF(NO_CONSOLE)
        IF(CMAKE_COMPILER_IS_GNUCXX)
            SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
        ENDIF(CMAKE_COMPILER_IS_GNUCXX)
        MESSAGE(STATUS "Disable windows console: ON")
    ELSE(NO_CONSOLE)
        MESSAGE(STATUS "Disable windows console: OFF")
    ENDIF(NO_CONSOLE)
ELSE(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -rdynamic")
    SET(ADDITIONAL_LIBRARIES X11 dl)
    SET(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/platform/x11window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/unixcrashhandler.cpp)
ENDIF(WIN32)


INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${EGL_INCLUDE_DIR}
    ${OPENGLES_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    "${CMAKE_CURRENT_LIST_DIR}/.."
)

SET(framework_LIBRARIES
    ${Boost_LIBRARIES}
    ${GLEW_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${OPENGLES_LIBRARY}
    ${EGL_LIBRARY}
    ${LUA_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${GMP_LIBRARY}
    ${ZLIB_LIBRARY}
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
    ${CMAKE_CURRENT_LIST_DIR}/graphics/coordsbuffer.cpp

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
    ${CMAKE_CURRENT_LIST_DIR}/ui/uilineedit.cpp
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
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/lbitlib-5.2.0-backport4.c
)
