cmake_minimum_required(VERSION 3.22)
project(raspberrypi_video_network)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/raspberrypi_video_network/include)
include_directories(${CMAKE_SOURCE_DIR}/raspberrypi_libs/leptonSDKEmb32PUB)

# Source files
# file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp") #TODO: Remove

# Define the executable
add_executable(raspberrypi_video raspberrypi_video_network/src/main.cpp)

# Ensure the library path is correct
link_directories(${CMAKE_SOURCE_DIR}/raspberrypi_libs/leptonSDKEmb32PUB/Debug)

# Link the libraries
target_link_libraries(raspberrypi_video LEPTON_SDK pthread)

# Custom commands to make sure the SDK is up to date
add_custom_target(sdk
    COMMAND make -C ${CMAKE_SOURCE_DIR}/raspberrypi_libs/leptonSDKEmb32PUB
    COMMENT "Building the SDK"
)

add_custom_target(sdkclean
    COMMAND make -C ${CMAKE_SOURCE_DIR}/raspberrypi_libs/leptonSDKEmb32PUB clean
    COMMENT "Cleaning the SDK"
)

# Ensure the SDK is built before the main project
add_dependencies(raspberrypi_video sdk)

# Set the output directories
set_target_properties(raspberrypi_video PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.
    OBJECT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gen_objs
)
