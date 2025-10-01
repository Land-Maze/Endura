#pragma once
#include <memory>
#include <vector>

namespace Assets
{
	namespace AssetType
	{
		struct Shader
		{
			std::vector<uint32_t> spirV;
		};

		struct Generic
		{
			std::vector<char> bytes;
		};
	}

	class AssetManager
	{
	public:
		template<typename T>
		[[nodiscard]] static std::shared_ptr<T> load(const std::string& filename);

	private:
		static std::ifstream openFile(const std::string& filename);
	};
}
