project(Engine LANGUAGES CXX)

add_subdirectory(Core)
add_subdirectory(Graphics)
#add_subdirectory(UI)
add_subdirectory(Application)

add_library(Engine INTERFACE)

target_link_libraries(Engine INTERFACE
        EngineCore
        EngineRenderer
#        EngineUI
        EngineApplication
)
