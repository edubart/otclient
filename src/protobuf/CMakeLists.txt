# *****************************************************************************
# Project protobuf
# *****************************************************************************
project(protobuf)

if(APPLE)
	set(CMAKE_CXX_STANDARD 11)
endif(APPLE)

find_package(Protobuf REQUIRED)

if(EXISTS ${PROTOBUF_PROTOC_EXECUTABLE})
	message(STATUS "Found Protobuf Compiler: ${PROTOBUF_PROTOC_EXECUTABLE}")
else()
	message(FATAL_ERROR "Could not find Protobuf Compiler")
endif()

if(PROTOBUF_FOUND)
	include_directories(${PROTOBUF_INCLUDE_DIRS})
	file(GLOB ProtoFiles
		"${CMAKE_CURRENT_SOURCE_DIR}/appearances.proto"
	)
	PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})
	add_library(${PROJECT_NAME} STATIC ${ProtoSources} ${ProtoHeaders})
	target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARY})

	add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
		# Copy files "appearances.pb.cc" to the "src/protobuf" folder
		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_BINARY_DIR}/appearances.pb.cc
			${CMAKE_CURRENT_SOURCE_DIR}/appearances.pb.cc

		# Copy files "appearances.pb.h" to the "src/protobuf" folder
		COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_BINARY_DIR}/appearances.pb.h
			${CMAKE_CURRENT_SOURCE_DIR}/appearances.pb.h
	)
endif(PROTOBUF_FOUND)
