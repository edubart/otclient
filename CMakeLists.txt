CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(otclient)

INCLUDE(src/framework/CMakeLists.txt)
INCLUDE(src/otclient/CMakeLists.txt)

OPTION(USE_PCH "Use precompiled header (speed up compile)" ON)

SET(EXECUTABLE_SOURCES src/main.cpp)

# add executable icon for win32 platforms
IF(WIN32)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
                       COMMAND ${CMAKE_RC_COMPILER}
                       -I${CMAKE_CURRENT_SOURCE_DIR}/src
                       -i${CMAKE_CURRENT_SOURCE_DIR}/src/otcicon.rc
                       -o ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o)
    SET(${EXECUTABLE_SOURCES} ${EXECUTABLE_SOURCES} otcicon.o)
ENDIF(WIN32)

# add otclient executable
ADD_EXECUTABLE(otclient ${FRAMEWORK_SOURCES} ${OTCLIENT_SOURCES} ${EXECUTABLE_SOURCES})

# target link libraries
TARGET_LINK_LIBRARIES(otclient ${FRAMEWORK_LIBRARIES})

IF(USE_PCH)
    FIND_PACKAGE(PCHSupport REQUIRED)
    ADD_PRECOMPILED_HEADER(otclient ${CMAKE_CURRENT_SOURCE_DIR}/src/framework/pch.h)
    MESSAGE(STATUS "Use precompiled header: ON")
ELSEIF(USE_PCH)
    MESSAGE(STATUS "Use precompiled header: OFF")
ENDIF(USE_PCH)

# installation
SET(DATA_INSTALL_DIR share/otclient)
INSTALL(TARGETS otclient RUNTIME DESTINATION bin)
INSTALL(DIRECTORY modules DESTINATION ${DATA_INSTALL_DIR}
        PATTERN ".git" EXCLUDE)
