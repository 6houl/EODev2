#include "../Packet_Handler/ClientPackets.h"
#include "../Packet_Handler/PacketFramer.h"
#include "../Packet_Handler/RequestGate.h"
#include "../Utilities/ResourceFile.h"

#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <string>

namespace
{
	int failures = 0;

	std::string Bytes(std::initializer_list<unsigned int> values)
	{
		std::string result;
		result.reserve(values.size());
		for (unsigned int value : values)
			result.push_back(static_cast<char>(value));
		return result;
	}

	void ExpectPacket(const char* name, const PacketBuilder& packet, const std::string& expected)
	{
		const std::string actual = packet.Get();
		if (actual == expected)
		{
			std::cout << "PASS " << name << '\n';
			return;
		}

		std::cerr << "FAIL " << name << ": expected " << expected.size()
			<< " bytes, received " << actual.size() << '\n';
		++failures;
	}

	void TestInit()
	{
		std::string expected = Bytes({15, 254, 255, 255, 246, 235, 2, 1, 1, 29, 113, 5});
		expected += "1234";
		ExpectPacket("init", ClientPackets::Init(123456, "1234"), expected);
	}

	void TestAccountRequest()
	{
		std::string expected = Bytes({8, 254, 1, 2});
		expected += "alice";
		ExpectPacket("account request", ClientPackets::AccountRequest("alice"), expected);
	}

	void TestAccountCreate()
	{
		std::string expected = Bytes({39, 254, 6, 2, 69, 2, 255});
		expected += "alice" + Bytes({255});
		expected += "secret" + Bytes({255});
		expected += "Alex" + Bytes({255});
		expected += "CA" + Bytes({255});
		expected += "a@b" + Bytes({255});
		expected += "PC" + Bytes({255});
		expected += "1234" + Bytes({255});

		ExpectPacket("account create", ClientPackets::AccountCreate(321, "alice", "secret",
			"Alex", "CA", "a@b", "PC", "1234"), expected);
	}

	void TestLogin()
	{
		std::string expected = Bytes({16, 254, 1, 4});
		expected += "alice" + Bytes({255});
		expected += "secret" + Bytes({255});
		ExpectPacket("login", ClientPackets::LoginRequest("alice", "secret"), expected);
	}

	void TestWelcome()
	{
		ExpectPacket("welcome request", ClientPackets::WelcomeRequest(1234567),
			Bytes({7, 254, 1, 5, 181, 73, 20, 254}));

		ExpectPacket("welcome message", ClientPackets::WelcomeMessage(321, 1234567),
			Bytes({10, 254, 15, 5, 69, 2, 254, 181, 73, 20, 254}));
	}

	void TestFileRequests()
	{
		ExpectPacket("map request", ClientPackets::FileRequest(1, 321, 42),
			Bytes({8, 254, 5, 5, 2, 69, 2, 43, 254}));

		ExpectPacket("pub request", ClientPackets::FileRequest(2, 321, 1),
			Bytes({7, 254, 5, 5, 3, 69, 2, 2}));
	}

	void Expect(bool condition, const char* name)
	{
		if (condition)
		{
			std::cout << "PASS " << name << '\n';
			return;
		}

		std::cerr << "FAIL " << name << '\n';
		++failures;
	}

	void TestPartialReads()
	{
		const std::string first = ClientPackets::LoginRequest("alice", "secret").Get();
		const std::string second = ClientPackets::AccountRequest("bob").Get();
		PacketFramer framer;
		std::string packet;

		framer.Append(first.data(), 1);
		Expect(!framer.Pop(packet), "partial length");
		framer.Append(first.data() + 1, 4);
		Expect(!framer.Pop(packet), "partial body");
		framer.Append(first.data() + 5, first.size() - 5);
		Expect(framer.Pop(packet) && packet == first.substr(2), "completed partial packet");

		const std::string combined = first + second;
		framer.Append(combined.data(), combined.size());
		Expect(framer.Pop(packet) && packet == first.substr(2), "first combined packet");
		Expect(framer.Pop(packet) && packet == second.substr(2), "second combined packet");
		Expect(!framer.Pop(packet), "combined buffer drained");

		framer.Append(first.data(), 5);
		framer.Reset();
		framer.Append(second.data(), second.size());
		Expect(framer.Pop(packet) && packet == second.substr(2), "framing reset on reconnect");
	}

	void TestRequestGate()
	{
		RequestGate gate;
		Expect(gate.Begin(100), "request begins");
		Expect(!gate.Begin(101), "duplicate request blocked");
		Expect(!gate.Expired(5099, 5000), "request waits for timeout");
		Expect(gate.Expired(5100, 5000), "request timeout");
		gate.Complete();
		Expect(!gate.Pending(), "request completion");
		Expect(gate.Begin(5200), "request allowed after reset");
	}

	void TestBinaryResourceWrite()
	{
		const std::string path = "tests/build/binary-resource-fixture.dat";
		const std::string expected = Bytes({69, 77, 70, 0, 255, 1, 0, 2});
		Expect(ResourceFile::Write(path, expected), "binary resource write");

		std::ifstream input(path, std::ios::in | std::ios::binary);
		const std::string actual((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		Expect(actual == expected, "binary resource preserves zero bytes");
		std::remove(path.c_str());
	}

	void TestSequenceEncoding()
	{
		PacketBuilder oneByte = ClientPackets::WithSequence(ClientPackets::LoginRequest("a", "b"), 252);
		ExpectPacket("sequence 252", oneByte,
			Bytes({8, 254, 1, 4, 253, 'a', 255, 'b', 255}));

		PacketBuilder twoByte = ClientPackets::WithSequence(ClientPackets::LoginRequest("a", "b"), 253);
		ExpectPacket("sequence 253", twoByte,
			Bytes({9, 254, 1, 4, 1, 2, 'a', 255, 'b', 255}));
	}

	void TestEncryptionRoundTrip()
	{
		PacketProcessor processor;
		processor.SetEMulti(6, 6);
		const std::string raw = ClientPackets::WithSequence(
			ClientPackets::LoginRequest("alice", "secret"), 123).Get();
		const std::string encoded = processor.Encode(raw);
		Expect(encoded != raw, "encrypted packet differs");
		Expect(processor.Decode(encoded.substr(2)) == raw.substr(2), "encryption round trip");

		const std::string init = ClientPackets::Init(123456, "1234").Get();
		Expect(processor.Encode(init) == init, "init packet remains raw");
	}

	void TestLengthValidation()
	{
		PacketFramer framer;
		std::string packet;
		const std::string invalid = Bytes({1, 1});
		framer.Append(invalid.data(), invalid.size());
		bool malformedRejected = false;
		try
		{
			framer.Pop(packet);
		}
		catch (const std::runtime_error&)
		{
			malformedRejected = true;
		}
		Expect(malformedRejected, "invalid frame length rejected");

		PacketBuilder oversized(PACKET_LOGIN, PACKET_REQUEST);
		oversized.AddString(std::string(64007, 'x'));
		bool oversizedRejected = false;
		try
		{
			oversized.Get();
		}
		catch (const std::length_error&)
		{
			oversizedRejected = true;
		}
		Expect(oversizedRejected, "oversized packet rejected");

		PacketReader shortReader(Bytes({1, 4, 2}));
		Expect(shortReader.GetInt() == 1 && shortReader.Remaining() == 0,
			"truncated number stays bounded");

		PacketReader stringReader(Bytes({1, 4, 'o', 'k'}));
		Expect(stringReader.GetBreakString() == "ok" && stringReader.Remaining() == 0,
			"missing string break stays bounded");
	}
}

int main()
{
	TestInit();
	TestAccountRequest();
	TestAccountCreate();
	TestLogin();
	TestWelcome();
	TestFileRequests();
	TestPartialReads();
	TestRequestGate();
	TestBinaryResourceWrite();
	TestSequenceEncoding();
	TestEncryptionRoundTrip();
	TestLengthValidation();

	if (failures != 0)
	{
		std::cerr << failures << " packet fixture(s) failed\n";
		return 1;
	}

	std::cout << "All packet fixtures passed\n";
	return 0;
}
