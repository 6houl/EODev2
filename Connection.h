#pragma once
#include <Ptypes/pinet.h>
#include <Ptypes/ptypes.h>
#include <Ptypes/pstreams.h>
#include <Ptypes/ptime.h>
#include <Ptypes/pasync.h>
#include <Ptypes/pport.h>
#include "Packet_Handler/RequestGate.h"
class Connection  : public pt::thread
{
public:
	Game* V_Game;
	bool Initialize(pt::string _IPAddress, int _port);
	void execute();

	enum FileType
	{
		Map = 5,
		EIF = 6,
		ENF,
		ESF,
		PlayerList = 11,
		ECF = 12
	};
	struct FileContainer
	{
		FileType File_Type;
		int ID;
	};
	bool ConnectionAccepted = false;
	bool AccountCreatePending = false;
	DWORD AccountCreateStart = 0;
	std::string PendingAccountName;
	std::string PendingAccountPassword;
	std::string PendingAccountFullName;
	std::string PendingAccountLocation;
	std::string PendingAccountEmail;
	std::list<FileContainer> FileQueue;
	void QueueFile(const FileContainer& file);
	void CompleteFileRequest();
	void QueueNextPubFile(const FileContainer& completedFile);
	void ScheduleAccountCreate(std::string accountName, std::string password, std::string fullName, std::string location, std::string email);
	bool TryBeginLogin();
	bool TryBeginAccountRequest();
	void CompleteLogin();
	void CompleteAccountRequest();
	void ResetRequests();
	void RequestStop();
	//void ProcessFile(const char* m_Buffer, Connection::FileContainer m_filecontainer);
	Connection () : pt::thread(false), V_Game(NULL), ClientStream(NULL), StopRequested(false){}
	~Connection();
	pt::ipstream* ClientStream = nullptr;
	RequestGate LoginRequest;
	RequestGate AccountRequest;
	pt::string IPAddress;
	int Port = 0;

private:
	std::atomic<bool> StopRequested;
	void RequestNextFile();
};
