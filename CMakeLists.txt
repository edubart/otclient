cmake_minimum_required(VERSION 2.6)
project(otclient)

set(FRAMEWORK_SOUND ON)
set(FRAMEWORK_GRAPHICS ON)
set(FRAMEWORK_XML ON)
set(FRAMEWORK_NET ON)
include(src/framework/CMakeLists.txt)
include(src/otclient/CMakeLists.txt)

# functions map for reading backtraces
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-Map=otclient.map")

option(USE_PCH "Use precompiled header (speed up compile)" OFF)

set(executable_SOURCES
    src/main.cpp
)

# add executable icon for win32 platforms
if(WIN32)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
                       COMMAND ${CMAKE_RC_COMPILER}
                       -I${CMAKE_CURRENT_SOURCE_DIR}/src
                       -i${CMAKE_CURRENT_SOURCE_DIR}/src/otcicon.rc
                       -o ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o)
    SET(executable_SOURCES ${executable_SOURCES} otcicon.o)
endif()

# add otclient executable
add_executable(otclient ${framework_SOURCES} ${otclient_SOURCES} ${executable_SOURCES})

# target link libraries
target_link_libraries(otclient ${framework_LIBRARIES})

if(USE_PCH)
    include(cotire)
    cotire(otclient)
    message(STATUS "Use precompiled header: ON")
else()
    message(STATUS "Use precompiled header: OFF")
endif()

# installation
set(DATA_INSTALL_DIR share/otclient)
install(TARGETS otclient RUNTIME DESTINATION bin)
install(FILES README.md BUGS LICENSE AUTHORS init.lua DESTINATION ${DATA_INSTALL_DIR})
install(DIRECTORY modules DESTINATION ${DATA_INSTALL_DIR}
        PATTERN ".git" EXCLUDE)
