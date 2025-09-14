cmake_minimum_required(VERSION 3.20)
project(Dependencies LANGUAGES CXX)

# ------------------------------
# GLFW
# ------------------------------
find_package(glfw3 3.3 REQUIRED)

# ------------------------------
# GLM
# ------------------------------
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

if(NOT GLM_INCLUDE_DIR)
	message(FATAL_ERROR "GLM not found!")
endif()

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})

add_library(Dependencies INTERFACE)
target_link_libraries(Dependencies INTERFACE glfw glm)