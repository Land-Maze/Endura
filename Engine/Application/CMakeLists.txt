project(EngineApplication LANGUAGES CXX)

file(GLOB_RECURSE APPLICATION_SOURCES CONFIGURE_DEPENDS
        *.cpp
        *.h
)

add_library(EngineApplication STATIC ${APPLICATION_SOURCES})
target_include_directories(EngineApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(EngineApplication
        PUBLIC Dependencies Vulkan::Vulkan
)
