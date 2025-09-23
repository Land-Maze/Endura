project(EngineCore LANGUAGES CXX)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
        *.cpp
        *.h
)

add_library(EngineCore STATIC ${CORE_SOURCES})
target_include_directories(EngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(EngineCore
        PUBLIC Dependencies Vulkan::Vulkan
)
