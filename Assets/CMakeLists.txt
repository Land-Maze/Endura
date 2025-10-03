project(Assets LANGUAGES CXX)

file(GLOB ASSET_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(Assets ${ASSET_SOURCES})
target_include_directories(Assets
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)



set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BUILD_DIR ${CMAKE_BINARY_DIR}/Client/Shaders)
file(MAKE_DIRECTORY ${SHADER_BUILD_DIR})

file(GLOB SHADER_FILES CONFIGURE_DEPENDS "${SHADER_SRC_DIR}/*.slang")

set(SPIRV_OUTPUTS "")
foreach(SRC ${SHADER_FILES})
    get_filename_component(NAME_WE ${SRC} NAME_WE)
    set(OUTPUT ${SHADER_BUILD_DIR}/${NAME_WE}.spv)
    list(APPEND SPIRV_OUTPUTS ${OUTPUT})

    add_custom_command(
            OUTPUT ${OUTPUT}
            COMMAND slangc ${SRC}
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            -o ${OUTPUT}
            DEPENDS ${SRC}
            COMMENT "Compiling shader ${NAME_WE}"
            VERBATIM
    )
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})




set(CLIENT_BINARY_DIR ${CMAKE_BINARY_DIR}/Client)
set(ASSET_FOLDERS Textures Meshes)

set(ASSET_FILES "")
foreach(FOLDER ${ASSET_FOLDERS})
    file(GLOB_RECURSE FOLDER_FILES CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER}/*)
    list(APPEND ASSET_FILES ${FOLDER_FILES})
endforeach()

list(APPEND ASSET_FILES ${SPIRV_OUTPUTS})

add_custom_target(CopyAssets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Textures ${CLIENT_BINARY_DIR}/Textures
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Meshes ${CLIENT_BINARY_DIR}/Meshes
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SHADER_BUILD_DIR} ${CLIENT_BINARY_DIR}/Shaders
        DEPENDS ${ASSET_FILES}
        COMMENT "Copying assets to Client runtime directory"
)



add_dependencies(Assets CompileShaders CopyAssets)
