#pragma once
#include <glm/glm.hpp>
#include <Graphics/MeshBufferAllocator.h>
#include <Graphics/VulkanBuffer.h>

namespace Renderer
{
	class Mesh
	{
	public:
		Mesh(std::string name, std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);

		void upload(MeshBufferAllocator& allocator);

		const MeshAllocation& getAllocation() const { return m_allocation; }
		const std::string& getName() const { return m_name; }
		const std::vector<glm::vec3>& getVertices() const { return m_vertices; }
		const std::vector<uint32_t>& getIndices() const { return m_indices; }

	private:
		std::string m_name;
		std::vector<glm::vec3> m_vertices;
		std::vector<uint32_t> m_indices;

		MeshAllocation m_allocation{};
		bool m_uploaded = false;
	};
}