# CMakeList.txt : CMake project for simple-event-aggreator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("simple-event-aggreator")

# Add source to this project's executable.
add_executable (simple-event-aggreator "main.cpp"  "simple_event_aggregator.h")

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

# TODO: Add tests and install targets if needed.
