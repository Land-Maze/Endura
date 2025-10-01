#include "AssetManager.h"

#include <cstring>
#include <fstream>

namespace Assets
{
	template <>
	std::shared_ptr<AssetType::Shader> AssetManager::load<AssetType::Shader>(const std::string& filename)
	{
		auto shader = std::make_shared<AssetType::Shader>();
		auto file = openFile("Shaders/" + filename + ".spv");
		auto fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0, std::ios::beg);

		file.read((buffer.data()),
		          buffer.size());

		shader->spirV.resize(fileSize / sizeof(uint32_t));

		std::memcpy(shader->spirV.data(), buffer.data(), fileSize);

		file.close();

		return shader;
	}

	std::ifstream AssetManager::openFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		return file;
	}

	template <typename T>
	std::shared_ptr<T> AssetManager::load(const std::string& filename)
	{
		static_assert(sizeof(T) == 0, "Unsupported asset type in AssetManager::load");
		return nullptr;
	}
}
