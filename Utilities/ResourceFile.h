#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <windows.h>

namespace ResourceFile
{
	inline bool Write(const std::filesystem::path& path, const std::string& data)
	{
		const std::filesystem::path temporaryPath = path.wstring() + L".download";
		std::ofstream output(temporaryPath, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!output)
			return false;

		output.write(data.data(), static_cast<std::streamsize>(data.size()));
		output.flush();
		const bool written = output.good();
		output.close();

		std::error_code error;
		if (!written || std::filesystem::file_size(temporaryPath, error) != data.size() || error)
		{
			std::filesystem::remove(temporaryPath, error);
			return false;
		}

		if (!MoveFileExW(temporaryPath.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			std::filesystem::remove(temporaryPath, error);
			return false;
		}

		return true;
	}
}
