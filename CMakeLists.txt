CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(otclient)

# setup custom cmake modules path
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

OPTION(USE_PCH "Use precompiled header" ON)
OPTION(NO_CONSOLE "Disable console window on Windows" OFF)
OPTION(HANDLE_EXCEPTIONS "Generate crash reports" OFF)
OPTION(FORBIDDEN_FUNCTIONS "Enable forbidden lua functions" ON)
OPTION(USE_GLES "Use OpenGLES (for mobiles devices)" OFF)

# find needed packages
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED OFF)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

IF(USE_GLES)
    FIND_PACKAGE(OpenGLES REQUIRED)
    FIND_PACKAGE(EGL REQUIRED)
    SET(OPENGL_INCLUDE_DIR ${OPENGLES_INCLUDE_DIR} ${EGL_INCLUDE_DIR})
    SET(OPENGL_LIBRARIES ${OPENGLES_LIBRARY} ${EGL_LIBRARY})
    ADD_DEFINITIONS(-DGLES -DEGL)
ELSE(USE_GLES)
    FIND_PACKAGE(OpenGL REQUIRED)
ENDIF(USE_GLES)

FIND_PACKAGE(Lua REQUIRED)
FIND_PACKAGE(PhysFS REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(PCHSupport REQUIRED)

# choose a default build type if not specified
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ReleaseWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)
MESSAGE(STATUS "BUILD TYPE: " ${CMAKE_BUILD_TYPE})

# setup compiler options
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CXX_WARNS "-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-variable -Wno-switch")
    SET(CMAKE_CXX_FLAGS "-std=gnu++0x -pipe ${CXX_WARNS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb3 -fno-inline")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O1 -g -fno-inline")
    SET(CMAKE_CXX_LINK_FLAGS "-rdynamic -static-libgcc -static-libstdc++ -Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(SOURCES
    # main
    src/main.cpp

    # otclient
    src/otclient/otclient.cpp

    # otclient luascript
    src/otclient/luascript/luafunctions.cpp
    src/otclient/luascript/luavaluecasts.cpp

    # otclient core
    src/otclient/core/game.cpp
    src/otclient/core/map.cpp
    src/otclient/core/thingstype.cpp
    src/otclient/core/spritemanager.cpp
    src/otclient/core/item.cpp
    src/otclient/core/tile.cpp
    src/otclient/core/thing.cpp
    src/otclient/core/creature.cpp
    src/otclient/core/effect.cpp
    src/otclient/core/missile.cpp
    src/otclient/core/localplayer.cpp
    src/otclient/core/outfit.cpp

    # otclient ui
    src/otclient/ui/uiitem.cpp
    src/otclient/ui/uicreature.cpp
    src/otclient/ui/uimap.cpp
    src/otclient/ui/uigame.cpp

    # otclient net
    src/otclient/net/protocollogin.cpp
    src/otclient/net/protocolgame.cpp
    src/otclient/net/protocolgamesend.cpp
    src/otclient/net/protocolgameparse.cpp

    # framework third party
    src/framework/thirdparty/apngloader.cpp

    # framework net
    src/framework/net/connection.cpp
    src/framework/net/inputmessage.cpp
    src/framework/net/outputmessage.cpp
    src/framework/net/protocol.cpp
    src/framework/net/rsa.cpp
    src/framework/net/server.cpp

    # framework core
    src/framework/core/logger.cpp
    src/framework/core/configs.cpp
    src/framework/core/resourcemanager.cpp
    src/framework/core/eventdispatcher.cpp
    src/framework/core/modulemanager.cpp
    src/framework/core/module.cpp

    # framework graphics
    src/framework/graphics/font.cpp
    src/framework/graphics/fontmanager.cpp
    src/framework/graphics/graphics.cpp
    src/framework/graphics/texture.cpp
    src/framework/graphics/framebuffer.cpp
    src/framework/graphics/animatedtexture.cpp
    src/framework/graphics/framebuffer.cpp
    src/framework/graphics/texturemanager.cpp
    src/framework/graphics/borderimage.cpp
    src/framework/graphics/image.cpp

    # framework otml
    src/framework/otml/otmldocument.cpp
    src/framework/otml/otmlemitter.cpp
    src/framework/otml/otmlnode.cpp
    src/framework/otml/otmlparser.cpp
    src/framework/otml/otmlexception.cpp

    # framework luascript
    src/framework/luascript/luainterface.cpp
    src/framework/luascript/luaobject.cpp
    src/framework/luascript/luaexception.cpp
    src/framework/luascript/luafunctions.cpp
    src/framework/luascript/luavaluecasts.cpp

    # framework ui
    src/framework/ui/uimanager.cpp
    src/framework/ui/uiwidget.cpp
    src/framework/ui/uilabel.cpp
    src/framework/ui/uibutton.cpp
    src/framework/ui/uilineedit.cpp
    src/framework/ui/uiwindow.cpp
    src/framework/ui/uianchorlayout.cpp
    src/framework/ui/uiverticallayout.cpp
    src/framework/ui/uilayout.cpp
    src/framework/ui/uiprogressbar.cpp
    src/framework/ui/uicheckbox.cpp
    src/framework/ui/uiframecounter.cpp
    src/framework/ui/uitranslator.cpp
)

IF(HANDLE_EXCEPTIONS)
    ADD_DEFINITIONS(-DHANDLE_EXCEPTIONS)
ENDIF(HANDLE_EXCEPTIONS)

IF(FORBIDDEN_FUNCTIONS)
    ADD_DEFINITIONS(-DFORBIDDEN_FUNCTIONS)
ENDIF(FORBIDDEN_FUNCTIONS)

IF(WIN32)
    SET(SOURCES ${SOURCES} src/framework/platform/win32platform.cpp)
    SET(ADDITIONAL_LIBRARIES ws2_32 mswsock)

    IF(CMAKE_COMPILER_IS_GNUCXX)
        ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)

    IF(NO_CONSOLE)
        ADD_DEFINITIONS(-DWIN32_NO_CONSOLE)
        IF(CMAKE_COMPILER_IS_GNUCXX)
            SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
        ENDIF(CMAKE_COMPILER_IS_GNUCXX)
    ENDIF(NO_CONSOLE)

    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icon.o
                       COMMAND ${CMAKE_RC_COMPILER}
                       -I${CMAKE_CURRENT_SOURCE_DIR}/src/otclient/win32icon
                       -i${CMAKE_CURRENT_SOURCE_DIR}/src/otclient/win32icon/icon.rc
                       -o ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
    SET(SOURCES ${SOURCES} icon.o)
ELSE(WIN32)
    SET(ADDITIONAL_LIBRARIES -lpthread)
    SET(SOURCES ${SOURCES} src/framework/platform/x11platform.cpp)
ENDIF(WIN32)

# target executable
ADD_EXECUTABLE(otclient ${SOURCES})

# target link libraries
TARGET_LINK_LIBRARIES(
    otclient
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${LUA_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${GMP_LIBRARY}
    ${ZLIB_LIBRARY}
    ${ADDITIONAL_LIBRARIES}
)

IF(USE_PCH)
    ADD_PRECOMPILED_HEADER(otclient ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)
ENDIF(USE_PCH)

# installation
SET(DATA_INSTALL_DIR share/otclient)
INSTALL(TARGETS otclient RUNTIME DESTINATION bin)
INSTALL(DIRECTORY modules DESTINATION ${DATA_INSTALL_DIR}
        PATTERN ".git" EXCLUDE)
