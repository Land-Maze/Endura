#include "Mesh.h"

namespace Renderer
{
	Mesh::Mesh(std::string name, std::vector<glm::vec3> vertices, std::vector<uint32_t> indices)
	{
		m_name = std::move(name);
		m_vertices = std::move(vertices);
		m_indices = std::move(indices);
	}

	void Mesh::upload(MeshBufferAllocator& allocator)
	{
		if(m_uploaded)
			return;

		m_allocation = allocator.uploadMesh(m_vertices, m_indices);
		m_uploaded = true;
	}
}
