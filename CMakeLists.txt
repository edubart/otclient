CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(otclient)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# find needed packages
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost COMPONENTS system regex REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Lua51 REQUIRED)
FIND_PACKAGE(YamlCpp REQUIRED)
FIND_PACKAGE(PhysFS REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(GMP REQUIRED)

# choose a default build type if not specified
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)
MESSAGE(STATUS "BUILD TYPE: " ${CMAKE_BUILD_TYPE})

# setup compiler options
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -std=c++0x")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -ggdb -fno-inline")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wl,-s")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${YAMLCPP_INCLUDE_DIRS}
    ${PHYSFS_INCLUDE_DIRS}
    ${GMP_INCLUDE_DIR}
    ${PNG_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/framework")

LINK_DIRECTORIES(
    ${Boost_LIBRARY_DIRS}
    ${LUA_LIBRARY_DIRS}
    ${PHYSFS_LIBRARY_DIRS}
    ${YAMLCPP_LIBRARY_DIRS}
    ${PNG_LIBRARY_DIRS})

# setup definitions
ADD_DEFINITIONS(-D_REENTRANT)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(SOURCES
# game sources
    src/main.cpp
    src/menustate.cpp
    src/teststate.cpp

# game net
    src/net/protocoltibia87.cpp

# framework core
    src/framework/core/dispatcher.cpp
    src/framework/core/configs.cpp
    src/framework/core/resources.cpp
    src/framework/core/engine.cpp

# framework utilities
    src/framework/util/color.cpp
    src/framework/util/util.cpp
    src/framework/util/logger.cpp
    src/framework/util/rsa.cpp

# framework graphics
    src/framework/graphics/image.cpp
    src/framework/graphics/borderedimage.cpp
    src/framework/graphics/framebuffer.cpp
    src/framework/graphics/font.cpp
    src/framework/graphics/fonts.cpp
    src/framework/graphics/fonttext.cpp
    src/framework/graphics/textureloader.cpp
    src/framework/graphics/texture.cpp
    src/framework/graphics/textures.cpp
    src/framework/graphics/graphics.cpp

# framework ui
    src/framework/ui/uilayout.cpp
    src/framework/ui/uielement.cpp
    src/framework/ui/uielementskin.cpp
    src/framework/ui/uibuttonskin.cpp
    src/framework/ui/uicontainer.cpp
    src/framework/ui/uiskins.cpp
    src/framework/ui/uiloader.cpp
    src/framework/ui/uipanel.cpp
    src/framework/ui/uibutton.cpp
    src/framework/ui/uilabel.cpp
    src/framework/ui/uiwindow.cpp
    src/framework/ui/uiwindowskin.cpp
    src/framework/ui/uitextedit.cpp
    src/framework/ui/uitexteditskin.cpp

# framework net
    src/framework/net/connection.cpp
    src/framework/net/connections.cpp
    src/framework/net/protocol.cpp
    src/framework/net/networkmessage.cpp
)

IF(WIN32)
    SET(SOURCES ${SOURCES} src/framework/platform/win32platform.cpp)
    SET(ADDITIONAL_LIBRARIES ws2_32)
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
ELSE(WIN32)
    SET(SOURCES ${SOURCES} src/framework/platform/x11platform.cpp)
    SET(ADDITIONAL_LIBRARIES pthread GLU)
ENDIF(WIN32)

# target executable
ADD_EXECUTABLE(otclient ${SOURCES})

# target link libraries
TARGET_LINK_LIBRARIES(otclient
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${LUA51_LIBRARY}
    ${YAMLCPP_LIBRARY}
    ${PHYSFS_LIBRARY}
    ${PNG_LIBRARY}
    ${GMP_LIBRARIES}
    ${ADDITIONAL_LIBRARIES})
