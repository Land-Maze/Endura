project(Assets LANGUAGES CXX)


file(GLOB ASSET_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(Assets ${ASSET_SOURCES})

target_include_directories(Assets
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)


set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
file(GLOB SHADER_FILES CONFIGURE_DEPENDS "${SHADER_SRC_DIR}/*.*")

file(MAKE_DIRECTORY ${SHADER_BUILD_DIR})

set(SPIRV_OUTPUTS "")
foreach(SRC ${SHADER_FILES})
    get_filename_component(NAME_WE ${SRC} NAME_WE)
    set(OUTPUT ${SHADER_BUILD_DIR}/${NAME_WE}.spv)
    list(APPEND SPIRV_OUTPUTS ${OUTPUT})

    add_custom_command(
            OUTPUT ${OUTPUT}
            COMMAND slangc ${SRC} -target spirv -profile spirv_1_4 -emit-spirv-directly
            -fvk-use-entrypoint-name -o ${OUTPUT}
            DEPENDS ${SRC}
            COMMENT "Compiling shader ${NAME_WE}"
            VERBATIM
    )
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})
add_dependencies(Assets CompileShaders)


set(ASSET_FOLDERS Textures Meshes Shaders)

foreach(FOLDER ${ASSET_FOLDERS})
    add_custom_command(
            TARGET Assets POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER}
            ${CMAKE_CURRENT_BINARY_DIR}/${FOLDER}
            COMMENT "Copying ${FOLDER} folder to build directory"
    )
endforeach()
