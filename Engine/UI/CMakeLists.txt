project(EngineUI LANGUAGES CXX)

file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS
		*.cpp
		*.h
)

add_library(EngineUI STATIC ${UI_SOURCES})
target_include_directories(EngineUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(EngineUI PUBLIC EngineCore Dependencies)