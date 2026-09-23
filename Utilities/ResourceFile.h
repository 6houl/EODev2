#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace ResourceFile
{
	inline bool Write(const std::filesystem::path& path, const std::string& data)
	{
		std::ofstream output(path, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!output)
			return false;

		output.write(data.data(), static_cast<std::streamsize>(data.size()));
		return output.good();
	}
}
