project(Client)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS
		*.cpp
)

add_executable(Client ${CLIENT_SOURCES})

target_link_libraries(Client PRIVATE Dependencies)

setup_vulkan_cpp_module(Client)
add_slang_shader_target(MyShaders SOURCES shader.slang)
add_dependencies(Client MyShaders)

#target_link_libraries(Client
#		PRIVATE EngineRenderer
#)
