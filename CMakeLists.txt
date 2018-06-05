cmake_minimum_required(VERSION 3.0)
project(voice_recognition)

set(CMAKE_BUILD_TYPE  Debug)
set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(
	vrecog
	src/main.cpp
	src/audiodecoder.cpp
	src/signaltransformer.cpp
)

target_link_libraries(
	vrecog avformat avcodec avutil
)
