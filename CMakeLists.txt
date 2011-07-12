CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(otclient)

# setup custom cmake modules path
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# find needed packages
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED OFF)
FIND_PACKAGE(Boost COMPONENTS system signals REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Lua REQUIRED)
FIND_PACKAGE(PhysFS REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

# choose a default build type if not specified
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)
MESSAGE(STATUS "BUILD TYPE: " ${CMAKE_BUILD_TYPE})

# setup compiler options
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-result -std=gnu++0x")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -ggdb -fno-inline")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wl,-s")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/framework")

# setup definitions
ADD_DEFINITIONS(-D_REENTRANT)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(SOURCES
# game sources
    src/main.cpp
    src/allocator.cpp

# game net
    src/protocollogin.cpp

# framework core
    src/framework/core/dispatcher.cpp
    src/framework/core/configs.cpp
    src/framework/core/resources.cpp
    src/framework/core/engine.cpp
    src/framework/core/modules.cpp

# framework script
    src/framework/script/scriptable.cpp
    src/framework/script/luascript.cpp
    src/framework/script/luafunctions.cpp

# framework utilities
    src/framework/util/color.cpp
    src/framework/util/util.cpp
    src/framework/util/logger.cpp
    src/framework/util/rsa.cpp
    src/framework/util/apngloader.cpp
    src/framework/util/fml.cpp

# framework graphics
    src/framework/graphics/image.cpp
    src/framework/graphics/borderedimage.cpp
    src/framework/graphics/framebuffer.cpp
    src/framework/graphics/font.cpp
    src/framework/graphics/fonts.cpp
    src/framework/graphics/textureloader.cpp
    src/framework/graphics/texture.cpp
    src/framework/graphics/animatedtexture.cpp
    src/framework/graphics/textures.cpp
    src/framework/graphics/graphics.cpp
    src/framework/graphics/textarea.cpp

# framework ui
    src/framework/ui/uianchorlayout.cpp
    src/framework/ui/uielement.cpp
    src/framework/ui/uielementskin.cpp
    src/framework/ui/uibuttonskin.cpp
    src/framework/ui/uilabelskin.cpp
    src/framework/ui/uicontainer.cpp
    src/framework/ui/uiskins.cpp
    src/framework/ui/uiloader.cpp
    src/framework/ui/uibutton.cpp
    src/framework/ui/uilabel.cpp
    src/framework/ui/uiwindow.cpp
    src/framework/ui/uiwindowskin.cpp
    src/framework/ui/uitextedit.cpp
    src/framework/ui/uitexteditskin.cpp
    src/framework/ui/uicheckboxskin.cpp
    src/framework/ui/uicheckbox.cpp

# framework net
    src/framework/net/connection.cpp
    src/framework/net/protocol.cpp
    src/framework/net/inputmessage.cpp
    src/framework/net/outputmessage.cpp
)

IF(WIN32)
    SET(SOURCES ${SOURCES} src/framework/platform/win32platform.cpp)
    SET(ADDITIONAL_LIBRARIES ws2_32)
    IF(CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
        ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-DWIN32_NO_CONSOLE)
ELSE(WIN32)
    SET(SOURCES ${SOURCES} src/framework/platform/x11platform.cpp)
    SET(ADDITIONAL_LIBRARIES pthread GLU)
ENDIF(WIN32)

# target executable
ADD_EXECUTABLE(otclient ${SOURCES})

# target link libraries
TARGET_LINK_LIBRARIES(otclient
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${LUA_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${GMP_LIBRARY}
    ${ZLIB_LIBRARY}
    ${ADDITIONAL_LIBRARIES})

# installation
SET(DATA_INSTALL_DIR share/otclient)
INSTALL(TARGETS otclient RUNTIME DESTINATION bin)
INSTALL(DIRECTORY data DESTINATION ${DATA_INSTALL_DIR})
