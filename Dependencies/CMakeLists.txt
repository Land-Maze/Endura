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

find_package(Vulkan REQUIRED)

# Creating env Vulkan_INCLUDE_DIR using $VULKAN_SDK
if(NOT DEFINED Vulkan_INCLUDE_DIR)
	if(DEFINED ENV{VULKAN_SDK})
		set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
	else()
		message(FATAL_ERROR "Vulkan SDK not found. Please set the VULKAN_SDK environment variable.")
	endif()
endif()

add_library(Dependencies INTERFACE)
target_link_libraries(Dependencies INTERFACE glfw glm vulkan)