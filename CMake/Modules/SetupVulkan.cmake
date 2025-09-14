# This function sets up Vulkan and creates the Vulkan C++20 module target.
function(setup_vulkan_cpp_module TARGET)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

	find_package(Vulkan REQUIRED)

	add_library(VulkanCppModule)
	add_library(Vulkan::cppm ALIAS VulkanCppModule)

	target_compile_definitions(VulkanCppModule PUBLIC
			VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
			VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
	)

	# Creating env Vulkan_INCLUDE_DIR using $VULKAN_SDK
	if(NOT DEFINED Vulkan_INCLUDE_DIR)
		if(DEFINED ENV{VULKAN_SDK})
			set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
		else()
			message(FATAL_ERROR "Vulkan SDK not found. Please set the VULKAN_SDK environment variable.")
		endif()
	endif()

	target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")
	target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

	target_sources(VulkanCppModule
			PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
			BASE_DIRS "${Vulkan_INCLUDE_DIR}"
			FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
	)

	if(TARGET ${TARGET})
		target_link_libraries(${TARGET} PUBLIC Vulkan::cppm)
	endif()
endfunction()
