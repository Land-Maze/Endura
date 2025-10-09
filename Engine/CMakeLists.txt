project(Engine LANGUAGES CXX)

add_subdirectory(Core)
add_subdirectory(Renderer)
add_subdirectory(UI)

add_library(Engine INTERFACE)

target_link_libraries(Engine INTERFACE
        EngineCore
        EngineRenderer
        EngineUI
)
