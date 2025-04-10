cmake_minimum_required(VERSION 3.10)

project(VANETSimulation)

set(CMAKE_CXX_STANDARD 14)

# Include directories
include_directories(src)
include_directories(src/routing)
include_directories(src/utils)

# Source files
set(SOURCES
    src/main.cc
    src/vanet-application.cc
    src/routing/aodv-implementation.cc
    src/routing/dsdv-implementation.cc
    src/routing/dsr-implementation.cc
    src/routing/gpsr-implementation.cc
    src/utils/mobility-helper.cc
)

# Create the executable
add_executable(vanet-simulation ${SOURCES})

# Link against NS3 libraries
find_package(NS3 REQUIRED)
target_link_libraries(vanet-simulation ${NS3_LIBRARIES})