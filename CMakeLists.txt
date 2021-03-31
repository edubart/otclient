cmake_minimum_required(VERSION 3.1)
project(otclient)

set(VERSION "1.0.0")

option(FRAMEWORK_SOUND "Use SOUND " ON)
option(FRAMEWORK_GRAPHICS "Use GRAPHICS " ON)
option(FRAMEWORK_XML "Use XML " ON)
option(FRAMEWORK_NET "Use NET " ON)
option(FRAMEWORK_SQL "Use SQL" OFF)

include(src/framework/CMakeLists.txt)
include(src/client/CMakeLists.txt)

# functions map for reading backtraces
if(NOT APPLE)
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-Map=${PROJECT_NAME}.map")
endif()

option(USE_PCH "Use precompiled header (speed up compile)" OFF)

set(executable_SOURCES
    src/main.cpp
)

if(WIN32)
    if(MSVC)
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
                           COMMAND ${CMAKE_RC_COMPILER}
                           -I${CMAKE_CURRENT_SOURCE_DIR}/src
                           -fo${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
                           ${CMAKE_CURRENT_SOURCE_DIR}/src/otcicon.rc
        )
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    else()
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
                           COMMAND ${CMAKE_RC_COMPILER}
                           -I${CMAKE_CURRENT_SOURCE_DIR}/src
                           -i${CMAKE_CURRENT_SOURCE_DIR}/src/otcicon.rc
                           -o ${CMAKE_CURRENT_BINARY_DIR}/otcicon.o
        )
    endif()

    set(executable_SOURCES ${executable_SOURCES} otcicon.o)
endif()

add_definitions(-D"VERSION=\\"${VERSION}\\"")

# add client executable
add_executable(${PROJECT_NAME} ${framework_SOURCES} ${client_SOURCES} ${executable_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME} ${framework_LIBRARIES})

if(APPLE AND USE_STATIC_LIBS)
    target_link_libraries(${PROJECT_NAME} "-framework Foundation" "-framework IOKit")
endif()

if(USE_PCH)
    include(cotire)
    cotire(${PROJECT_NAME})
    message(STATUS "Use precompiled header: ON")
else()
    message(STATUS "Use precompiled header: OFF")
endif()

# installation
set(DATA_INSTALL_DIR share/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(FILES README.md BUGS LICENSE AUTHORS init.lua ${PROJECT_NAME}rc.lua DESTINATION ${DATA_INSTALL_DIR})
install(DIRECTORY data modules DESTINATION ${DATA_INSTALL_DIR} PATTERN ".git" EXCLUDE)

# add "make run"
add_custom_target(run COMMAND ${PROJECT_NAME} DEPENDS ${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
