function (add_slang_shader_target TARGET)
	cmake_parse_arguments(SHADER "" "" "SOURCES" ${ARGN})

	set(SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/Assets/Shaders)
	file(MAKE_DIRECTORY ${SHADERS_DIR})

	set(ABS_SHADER_SOURCES "")
	foreach(SRC ${SHADER_SOURCES})
		list(APPEND ABS_SHADER_SOURCES ${CMAKE_SOURCE_DIR}/Assets/Shaders/${SRC})
	endforeach()

	set(ENTRY_POINTS -entry vertMain -entry fragMain)
	set(SLANGC_EXECUTABLE slangc)

	add_custom_command(
			OUTPUT ${SHADERS_DIR}/slang.spv
			COMMAND ${SLANGC_EXECUTABLE} ${ABS_SHADER_SOURCES}
			-target spirv -profile spirv_1_4 -emit-spirv-directly
			-fvk-use-entrypoint-name ${ENTRY_POINTS}
			-o slang.spv
			WORKING_DIRECTORY ${SHADERS_DIR}
			DEPENDS ${ABS_SHADER_SOURCES}
			COMMENT "Compiling Slang Shaders"
			VERBATIM
	)

	add_custom_target(${TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
endfunction()
