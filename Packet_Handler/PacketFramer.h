#pragma once

#include "../include/EOServ/packet.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

class PacketFramer
{
public:
	void Append(const char* data, std::size_t length)
	{
		data_.append(data, length);
	}

	bool Pop(std::string& packet)
	{
		if (data_.size() < 2)
			return false;

		const unsigned int length = PacketProcessor::Number(
			static_cast<unsigned char>(data_[0]), static_cast<unsigned char>(data_[1]));

		if (length < 2 || length > 64008)
			throw std::runtime_error("Invalid packet length");

		if (data_.size() < length + 2)
			return false;

		packet.assign(data_, 2, length);
		data_.erase(0, length + 2);
		return true;
	}

	void Reset()
	{
		data_.clear();
	}

private:
	std::string data_;
};
