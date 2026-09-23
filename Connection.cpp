#include "stdafx.h"
#include "game.h"
#include "Packet_Handler\Send\SInit.h"
#include "Packet_Handler\Send\SAccount.h"
#include "Packet_Handler\Send\SWelcome.h"
#include "Packet_Handler\Receive\Handler.h"
#include "Packet_Handler\PacketFramer.h"
#include "Utilities\ResourceFile.h"
#include "Connection.h"
USING_PTYPES

Connection::~Connection()
{
	if (ClientStream)
	{
		ClientStream->close();
		delete ClientStream;
		ClientStream = NULL;
	}
}

bool Connection::Initialize(pt::string _IPAddress, int _port)
{
	IPAddress = _IPAddress;
	Port = _port;
	StopRequested = false;
	ResetRequests();
	return true;
}

void Connection::RequestStop()
{
	StopRequested = true;
	if (ClientStream)
		ClientStream->close();
	signal();
}

bool Connection::TryBeginLogin()
{
	return LoginRequest.Begin(GetTickCount());
}

bool Connection::TryBeginAccountRequest()
{
	return AccountRequest.Begin(GetTickCount());
}

void Connection::CompleteLogin()
{
	LoginRequest.Complete();
}

void Connection::CompleteAccountRequest()
{
	AccountRequest.Complete();
}

void Connection::ResetRequests()
{
	LoginRequest.Complete();
	AccountRequest.Complete();
	AccountCreatePending = false;
	FileQueue.clear();
}

void Connection::QueueFile(const FileContainer& file)
{
	const bool startTransfer = FileQueue.empty();
	FileQueue.push_back(file);
	if (startTransfer)
		RequestNextFile();
}

void Connection::CompleteFileRequest()
{
	if (!FileQueue.empty())
		FileQueue.pop_front();
	RequestNextFile();
}

void Connection::RequestNextFile()
{
	if (FileQueue.empty())
		return;

	char requestType = 0;
	switch (FileQueue.front().File_Type)
	{
	case FileType::Map: requestType = 1; break;
	case FileType::EIF: requestType = 2; break;
	case FileType::ENF: requestType = 3; break;
	case FileType::ESF: requestType = 4; break;
	case FileType::ECF: requestType = 5; break;
	default: throw std::runtime_error("Unknown file request type");
	}

	SWelcome::RequestFile(ClientStream, requestType, V_Game, FileQueue.front().ID);
}

void Connection::ScheduleAccountCreate(std::string accountName, std::string password, std::string fullName, std::string location, std::string email)
{
	PendingAccountName = accountName;
	PendingAccountPassword = password;
	PendingAccountFullName = fullName;
	PendingAccountLocation = location;
	PendingAccountEmail = email;
	AccountCreateStart = GetTickCount();
	AccountCreatePending = true;
}
void ConnectionTextPadTo(std::wstring& str, const size_t num, const char paddingChar = ' ')
{
	if (num > str.size())
		str.insert(0, num - str.size(), paddingChar);
}
void ProcessFile(const std::string& data, Connection::FileContainer m_filecontainer)
{
	wchar_t directory[MAX_PATH + 1] = {0};
	if (!GetCurrentDirectoryW(MAX_PATH, directory))
		throw std::runtime_error("Unable to find the client directory");

	std::wstring path(directory);
	std::wstring numberPath = std::to_wstring(m_filecontainer.ID);

	switch (m_filecontainer.File_Type)
	{
	case(Connection::FileType::Map):
		{
			ConnectionTextPadTo(numberPath, 5, '0');
			path += L"\\Maps\\" + numberPath + L".emf";

			if (!ResourceFile::Write(path, data))
				throw std::runtime_error("Unable to save map file");
			std::string strMapID = "Map File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			break;
		}
	case(Connection::FileType::EIF):
		{
			ConnectionTextPadTo(numberPath, 3, '0');
			path += L"\\pub\\dat" + numberPath + L".eif";

			if (!ResourceFile::Write(path, data))
				throw std::runtime_error("Unable to save item file");

			std::string strMapID = "Item File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			std::string filePath(path.begin(), path.end());
			EIF* replacement = new EIF(filePath.c_str());
			delete World::EIF_File;
			World::EIF_File = replacement;
			break;
		}
		case(Connection::FileType::ENF):
		{
			ConnectionTextPadTo(numberPath, 3, '0');
			path += L"\\pub\\dtn" + numberPath + L".enf";

			if (!ResourceFile::Write(path, data))
				throw std::runtime_error("Unable to save NPC file");
			
			std::string strMapID = "NPC File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			std::string filePath(path.begin(), path.end());
			ENF* replacement = new ENF(filePath.c_str());
			delete World::ENF_File;
			World::ENF_File = replacement;
			break;
		}
		case(Connection::FileType::ESF):
		{
			ConnectionTextPadTo(numberPath, 3, '0');
			path += L"\\pub\\dsl" + numberPath + L".esf";

			if (!ResourceFile::Write(path, data))
				throw std::runtime_error("Unable to save spell file");
			std::string strMapID = "Spell File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			std::string filePath(path.begin(), path.end());
			ESF* replacement = new ESF(filePath.c_str());
			delete World::ESF_File;
			World::ESF_File = replacement;
			break;
		}
		case(Connection::FileType::ECF):
		{
			ConnectionTextPadTo(numberPath, 3, '0');
			path += L"\\pub\\dat" + numberPath + L".ecf";

			if (!ResourceFile::Write(path, data))
				throw std::runtime_error("Unable to save class file");

			std::string strMapID = "Class File" + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			std::string filePath(path.begin(), path.end());
			ECF* replacement = new ECF(filePath.c_str());
			delete World::ECF_File;
			World::ECF_File = replacement;
			break;
		}
	}
}
void Connection::execute()
{
	DWORD connectStart = GetTickCount();
	PacketFramer framer;
	PacketHandler packetHandler;
	try
	{
	World::DebugPrint("Connecting to " + IPAddress);
	this->ClientStream = new ipstream(IPAddress, Port);
	this->ClientStream->open();
	SInit::SendInit(this->ClientStream);
	ConnectionAccepted = false;

	} catch(const std::exception& error)
	{ 
		World::DebugPrint(error.what());
		World::ThrowMessage("Could not find server","The game server could not be found,\nplease try again at a later time."); 
		World::Connected = false; 
		World::Connecting = false;
		
		return;
	}
	while(!StopRequested)
	{
		DWORD now = GetTickCount();
		if (LoginRequest.Expired(now, 5000))
		{
			CompleteLogin();
			World::ThrowMessage("Login timed out", "The server did not answer the login request.");
		}
		if (AccountRequest.Expired(now, 5000))
		{
			CompleteAccountRequest();
			World::ThrowMessage("Request timed out", "The server did not answer the account request.");
		}

		if (AccountCreatePending && GetTickCount() - AccountCreateStart >= 2000)
		{
			AccountCreatePending = false;
			SAccount::CreateAccount(ClientStream, PendingAccountName, PendingAccountPassword, PendingAccountFullName, PendingAccountLocation, PendingAccountEmail, V_Game);
		}

			try
			{
				if(World::Connecting)
				{
					if(GetTickCount() - connectStart > 5000)
					{
						World::DebugPrint("Couldn't connect to the login server.");
						World::ThrowMessage("Could not find server","The game server could not be found,\nplease try again at a later time.");
						World::Connected = false;
						World::Connecting = false;
						return;
					}
				}
				if (!ClientStream->waitfor(50))
					continue;

				int val = ClientStream->get_dataavail();

				if(val > 0)
				{
					std::string incoming(val, '\0');
					int received = ClientStream->read(&incoming[0], val);
					if (received <= 0)
						throw std::runtime_error("Connection closed");

					framer.Append(incoming.data(), received);
					std::string newbuf;
					while (framer.Pop(newbuf))
					{
						newbuf = World::Receive(this->V_Game, newbuf);
						PacketReader* reader = new PacketReader(newbuf);

						int familyID = reader->Family();
						int ActionID = reader->Action();
						if (familyID == 255 && ActionID == 255 && ConnectionAccepted)
						{
							int ID = reader->Getbyte();
							if (ID == 4 || ID == 5 || ID == FileType::EIF || ID == FileType::ENF || ID == FileType::ESF || ID == FileType::ECF)
							{
								if (FileQueue.empty())
									throw std::runtime_error("Unexpected file response");

								const FileContainer filecont = FileQueue.front();
								const FileType responseType = (ID == 4 || ID == 5) ? FileType::Map : static_cast<FileType>(ID);
								if (responseType != filecont.File_Type)
									throw std::runtime_error("Unexpected file response type");

								if (responseType != FileType::Map)
								{
									const int responseFileId = reader->GetChar();
									if (responseFileId != filecont.ID)
										throw std::runtime_error("Unexpected pub file ID");
								}

								std::string str = reader->GetEndString();
								ProcessFile(str, filecont);
								if (responseType == FileType::Map)
									V_Game->map->LoadMap(filecont.ID);
								CompleteFileRequest();
							}
							else if (ID == 9)
							{
								//FriendsList
								
								int numberofplayers = reader->GetShort();
								reader->Getbyte();
								std::vector<World::OnlinePlayerContainer> Sortedcontainer;
								for (int i = 0; i < numberofplayers; i++)
								{
									World::OnlinePlayerContainer _player;
									_player._Name = reader->GetBreakString();
									if (!_player._Name.empty())
										_player._Name[0] = toupper(static_cast<unsigned char>(_player._Name[0]));
									_player._Title = reader->GetBreakString();
									reader->Getbyte();
									_player._Icon = reader->GetChar();
									_player._ClassID = reader->GetChar();
									_player._GuildTag = reader->GetBreakString();
									
									int insertindex = 0;
									for (int j = 0; j < Sortedcontainer.size(); j++)
									{
										if (Sortedcontainer[j]._Name <= _player._Name)
										{
											insertindex = j + 1;
										}
										else { break; }
									}
									Sortedcontainer.insert(Sortedcontainer.begin() + insertindex, _player);
								}
								World::SetOnlinePlayers(Sortedcontainer);
								World::DebugPrint("Player list recieved!");
							}
							else if (ID != 11)
							{
								throw std::runtime_error("Unknown init response");
							}

						}
						else
						{
							std::string reportstr = "";
							reportstr += "[" + PacketProcessor::GetFamilyName(reader->Family()) + "]";
							reportstr += "[" + PacketProcessor::GetActionName(reader->Action()) + "]";

							World::DebugPrint(reportstr.c_str());
							Game* game = (Game*)V_Game;
							//game->map->ThreadLock.lock();
							packetHandler.HandlePacket(*reader, game, ClientStream);
							//game->map->ThreadLock.unlock();
						}

						delete reader;
					}
				}
			}
			catch(const std::exception& error)
			{
				if (!StopRequested)
				{
					World::DebugPrint(error.what());
					if (ConnectionAccepted)
					{
						const bool resourceFailure = !FileQueue.empty();
						World::ThrowMessage(resourceFailure ? "Resource update failed" : "Connection error", error.what());
					}
					else
					{
						World::ThrowMessage("Could not find server", "The game server could not be found,\nplease try again at a later time.");
					}
				}
				ConnectionAccepted = false;
				ResetRequests();
				World::Connected = false;
				World::Connecting = false;
				Game* gme = (Game*)V_Game;
				World::WorldCharacterID = -1;				
				return;
			}
	}
	World::DebugPrint("connection cut\n");
	World::Connected = false;
	World::Connecting = false;
}
