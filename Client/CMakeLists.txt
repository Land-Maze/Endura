project(Client)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS
		*.cpp
)

add_executable(Client ${CLIENT_SOURCES})

target_link_libraries(Client PRIVATE Dependencies)

target_compile_definitions(Client PUBLIC
		VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
		VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_link_libraries(Client
		PRIVATE EngineCore EngineRenderer EngineUI
)
