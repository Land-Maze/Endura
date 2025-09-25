project(EngineRenderer LANGUAGES CXX)

file(GLOB_RECURSE RENDERER_SOURCES CONFIGURE_DEPENDS
		*.cpp
		*.h
)

add_library(EngineRenderer STATIC ${RENDERER_SOURCES})
target_include_directories(EngineRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(EngineRenderer PUBLIC EngineCore Dependencies Vulkan::Vulkan)