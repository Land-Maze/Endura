project(EngineGraphics LANGUAGES CXX)

file(GLOB_RECURSE GRAPHICS_SOURCES CONFIGURE_DEPENDS
		*.cpp
		*.h
)

add_library(EngineGraphics STATIC ${GRAPHICS_SOURCES})
target_include_directories(EngineGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(EngineGraphics PUBLIC Dependencies Vulkan::Vulkan Assets)