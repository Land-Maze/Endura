project(EngineInput LANGUAGES CXX)

file(GLOB_RECURSE INPUT_SOURCES CONFIGURE_DEPENDS
		*.cpp
		*.h
)

add_library(EngineInput STATIC ${INPUT_SOURCES})
target_include_directories(EngineInput PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(EngineInput PUBLIC EngineCore Dependencies Vulkan::cppm)
