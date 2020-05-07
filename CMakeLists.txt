# This CMake file is basically only for ability to open whole
# directory in IDEs like VSCode, CLion, etc.

cmake_minimum_required(VERSION 3.13)
project(ExamplePlugin_plugin_project)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)   # Enable -fPIC for all targets

# Expose custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Set install directory to $JANA_HOME
#set(CMAKE_INSTALL_PREFIX $ENV{JANA_HOME} CACHE PATH "magic incantation" FORCE)
add_subdirectory(srcqe_analysis)


