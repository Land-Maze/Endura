cmake_minimum_required(VERSION 3.20)
project(Dependencies LANGUAGES CXX)

include(FetchContent)

# ------------------------------
# GLFW
# ------------------------------
find_package(glfw3 3.3 QUIET)
if (NOT glfw3_FOUND)
	FetchContent_Declare(
			glfw
			GIT_REPOSITORY https://github.com/glfw/glfw.git
			GIT_TAG 3.3.10
	)
	FetchContent_MakeAvailable(glfw)
endif()

# ------------------------------
# GLM
# ------------------------------
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

if(NOT GLM_INCLUDE_DIR)
	FetchContent_Declare(
			glm
			GIT_REPOSITORY https://github.com/g-truc/glm.git
			GIT_TAG 1.0.1
	)
	FetchContent_MakeAvailable(glm)
endif()

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})

# ------------------------------
# Vulkan Memory Allocator
# ------------------------------
FetchContent_Declare(
		vma
		GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
		GIT_TAG v3.3.0
)
FetchContent_MakeAvailable(vma)
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE ${vma_SOURCE_DIR}/include)

# ------------------------------
# STB Image
# ------------------------------
FetchContent_Declare(
		stb
		GIT_REPOSITORY https://github.com/nothings/stb.git
		GIT_TAG master
)
FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# ------------------------------
# tinyobjloader
# ------------------------------
FetchContent_Declare(
		tinyobjloader
		GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
		GIT_TAG release
)
FetchContent_MakeAvailable(tinyobjloader)

# ------------------------------
# Vulkan
# ------------------------------

find_package(Vulkan REQUIRED)
# Creating env Vulkan_INCLUDE_DIR using $VULKAN_SDK
if(NOT DEFINED Vulkan_INCLUDE_DIR)
	if(DEFINED ENV{VULKAN_SDK})
		set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
	else()
		message(FATAL_ERROR "Vulkan SDK not found. Please set the VULKAN_SDK environment variable.")
	endif()
endif()

# ------------------------------
# ------------------------------
# ------------------------------
add_library(Dependencies INTERFACE)
target_link_libraries(Dependencies INTERFACE
		glfw
		glm
		vma
		tinyobjloader
		stb
)