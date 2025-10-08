cmake_minimum_required(VERSION 3.15)
project(read_warpx_particles LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Geant4
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# OpenPMD
find_package(openPMD REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cc
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

# Executable
add_executable(read_warpx_particles ${SOURCES})

target_link_libraries(read_warpx_particles
    PRIVATE
      ${Geant4_LIBRARIES}
      openPMD::openPMD
)

# Copy Geant4 macro files to build directory
file(GLOB MACROS
    "${PROJECT_SOURCE_DIR}/*.mac"
)
file(COPY ${MACROS} DESTINATION ${PROJECT_BINARY_DIR})

# Enable testing and add alias target
enable_testing()
add_custom_target(sim DEPENDS read_warpx_particles)

# Installation rules (optional)
install(TARGETS read_warpx_particles DESTINATION bin)
install(FILES ${MACROS} DESTINATION bin)