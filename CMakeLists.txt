CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(otclient)

ADD_DEFINITIONS(-DFW_WINDOW)

INCLUDE(src/framework/CMakeLists.txt)
INCLUDE(src/otclient/CMakeLists.txt)

# functions map for reading backtraces
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-Map=otclient.map")

OPTION(USE_PCH "Use precompiled header (speed up compile)" OFF)

SET(executable_SOURCES
    src/main.cpp
)

# add executable icon for win32 platforms
IF(WIN32)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
                       COMMAND ${CMAKE_RC_COMPILER}
                       -I${CMAKE_CURRENT_SOURCE_DIR}/src
                       -i${CMAKE_CURRENT_SOURCE_DIR}/src/otcicon.rc
                       -o ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o)
    SET(executable_SOURCES ${executable_SOURCES} otcicon.o)
ENDIF(WIN32)

# add otclient executable
ADD_EXECUTABLE(otclient ${framework_SOURCES} ${otclient_SOURCES} ${executable_SOURCES})

# target link libraries
TARGET_LINK_LIBRARIES(otclient ${framework_LIBRARIES})

IF(USE_PCH)
    include(cotire)
    cotire(otclient)
    MESSAGE(STATUS "Use precompiled header: ON")
ELSE()
    MESSAGE(STATUS "Use precompiled header: OFF")
ENDIF()

# installation
SET(DATA_INSTALL_DIR share/otclient)
INSTALL(TARGETS otclient RUNTIME DESTINATION bin)
INSTALL(DIRECTORY modules DESTINATION ${DATA_INSTALL_DIR}
        PATTERN ".git" EXCLUDE)
