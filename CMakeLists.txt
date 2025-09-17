cmake_minimum_required(VERSION 4.0)
project(Endura LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/Modules")
include(SetupVulkan OPTIONAL)
include(FindVulkan OPTIONAL)

add_subdirectory(Dependencies)

#add_subdirectory(Engine)
#add_subdirectory(Game)

add_subdirectory(Client)