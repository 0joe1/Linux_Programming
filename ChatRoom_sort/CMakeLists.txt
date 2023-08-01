cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

set(CMAKE_CXX_STANDARD 14)

# You should add -pthread if you are using <thread> in your code.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

include_directories(include)
include_directories(include/client)
include_directories(include/common)
include_directories(include/server)

# Add sub-directories
add_subdirectory(src)

# Uncomment this if you have tests in your project.
# enable_testing()
# add_subdirectory(tests)
