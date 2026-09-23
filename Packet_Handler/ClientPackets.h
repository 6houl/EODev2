#pragma once

#include "../include/EOServ/packet.hpp"

#include <string>

namespace ClientPackets
{
	inline PacketBuilder WithSequence(PacketBuilder builder, unsigned int sequence)
	{
		const std::array<unsigned char, 4> encoded = PacketProcessor::ENumber(sequence);
		if (sequence >= PacketProcessor::MAX1)
			builder.Insertbyte(0, encoded[1]);
		builder.Insertbyte(0, encoded[0]);
		return builder;
	}

	inline PacketBuilder Init(unsigned int challenge, const std::string& hdid)
	{
		PacketBuilder builder;
		builder.AddThree(challenge);
		builder.AddChar(0);
		builder.AddChar(0);
		builder.AddChar(28);
		builder.AddChar(112);
		builder.AddChar(static_cast<unsigned char>(hdid.length()));
		builder.AddString(hdid);
		return builder;
	}

	inline PacketBuilder AccountRequest(const std::string& username)
	{
		PacketBuilder builder(PACKET_ACCOUNT, PACKET_REQUEST);
		builder.AddString(username);
		return builder;
	}

	inline PacketBuilder AccountCreate(unsigned short sessionId,
		const std::string& username,
		const std::string& password,
		const std::string& fullName,
		const std::string& location,
		const std::string& email,
		const std::string& computer,
		const std::string& hdid)
	{
		PacketBuilder builder(PACKET_ACCOUNT, PACKET_CREATE);
		builder.AddShort(sessionId);
		builder.Addbyte(255);
		builder.AddBreakString(username);
		builder.AddBreakString(password);
		builder.AddBreakString(fullName);
		builder.AddBreakString(location);
		builder.AddBreakString(email);
		builder.AddBreakString(computer);
		builder.AddBreakString(hdid);
		return builder;
	}

	inline PacketBuilder LoginRequest(const std::string& username, const std::string& password)
	{
		PacketBuilder builder(PACKET_LOGIN, PACKET_REQUEST);
		builder.AddBreakString(username);
		builder.AddBreakString(password);
		return builder;
	}

	inline PacketBuilder PlayerListRequest()
	{
		return PacketBuilder(PACKET_PLAYERS, PACKET_REQUEST);
	}

	inline PacketBuilder HearWhispers(bool enabled)
	{
		return PacketBuilder(PACKET_GLOBAL, enabled ? PACKET_REMOVE : PACKET_PLAYER);
	}

	inline PacketBuilder ConnectionAccept(unsigned short clientEncryptionMultiple,
		unsigned short serverEncryptionMultiple,
		unsigned short playerId)
	{
		PacketBuilder builder(PACKET_CONNECTION, PACKET_ACCEPT);
		builder.AddShort(clientEncryptionMultiple);
		builder.AddShort(serverEncryptionMultiple);
		builder.AddShort(playerId);
		return builder;
	}

	inline PacketBuilder WelcomeRequest(unsigned int characterId)
	{
		PacketBuilder builder(PACKET_WELCOME, PACKET_REQUEST);
		builder.AddInt(characterId);
		return builder;
	}

	inline PacketBuilder WelcomeMessage(unsigned int sessionId, unsigned int characterId)
	{
		PacketBuilder builder(PACKET_WELCOME, PACKET_MSG);
		builder.AddThree(sessionId);
		builder.AddInt(characterId);
		return builder;
	}

	inline PacketBuilder FileRequest(unsigned char fileType, unsigned short sessionId, unsigned short fileId)
	{
		PacketBuilder builder(PACKET_WELCOME, PACKET_AGREE);
		builder.AddChar(fileType);
		builder.AddShort(sessionId);

		if (fileType == 1)
			builder.AddShort(fileId);
		else
			builder.AddChar(static_cast<unsigned char>(fileId));

		return builder;
	}
}
