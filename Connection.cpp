#include "stdafx.h"
#include "game.h"
#include "Packet_Handler\Send\SInit.h"
#include "Packet_Handler\Send\SAccount.h"
#include "Packet_Handler\Receive\Handler.h"
#include "Packet_Handler\PacketFramer.h"
#include "Connection.h"
USING_PTYPES
bool Connection::ConnectionDropped;
bool Connection::Initialize(pt::string _IPAddress, int _port)
{
	IPAddress = _IPAddress;
	Port = _port;
	ConnectionDropped = false;
	ResetRequests();
	return true;
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
void ProcessFile(const char* m_Buffer, Connection::FileContainer m_filecontainer)
{
	switch (m_filecontainer.File_Type)
	{
	case(Connection::FileType::Map):
		{
			std::ofstream output;

			wchar_t* dir_buffer = new wchar_t[1024];
			GetCurrentDirectory(1024, dir_buffer);
			std::wstring Path = L"";
			Path += dir_buffer;
			Path += L"\\Maps\\";
			wstring NumberPath = std::to_wstring(m_filecontainer.ID);
			ConnectionTextPadTo(NumberPath, 5, '0');
			Path += NumberPath += L".emf";

			output.open(Path, ios::out | ios::binary);
			output.write(m_Buffer, strlen(m_Buffer));
			std::string strMapID = "Map File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			delete [] dir_buffer;
			output.close();

			break;
		}
	case(Connection::FileType::EIF):
		{
			std::ofstream output;

			wchar_t* dir_buffer = new wchar_t[1024];
			GetCurrentDirectory(1024, dir_buffer);
			std::wstring Path = L"";
			Path += dir_buffer;
			Path += L"\\pub\\dat";
			wstring NumberPath = std::to_wstring(m_filecontainer.ID);
			ConnectionTextPadTo(NumberPath, 3, '0');
			Path += NumberPath += L".eif";

			output.open(Path, ios::out | ios::binary);
			output.write(m_Buffer, strlen(m_Buffer));

			std::string strMapID = "Item File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			delete [] dir_buffer;
			output.close();
			std::string str(Path.begin(), Path.end());
			World::EIF_File = new EIF("\pub\\dat001.eif");
			break;
		}
		case(Connection::FileType::ENF):
		{
			std::ofstream output;

			wchar_t* dir_buffer = new wchar_t[1024];
			GetCurrentDirectory(1024, dir_buffer);
			std::wstring Path = L"";
			Path += dir_buffer;
			Path += L"\\pub\\dtn";
			wstring NumberPath = std::to_wstring(m_filecontainer.ID);
			ConnectionTextPadTo(NumberPath, 3, '0');
			Path += NumberPath += L".enf";

			output.open(Path, ios::out | ios::binary);
			output.write(m_Buffer, strlen(m_Buffer));
			
			std::string strMapID = "NPC File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			delete [] dir_buffer;
			output.close();
			std::string str(Path.begin(), Path.end());
			World::ENF_File = new ENF("\pub\\dtn001.enf");
			break;
		}
		case(Connection::FileType::ESF):
		{
			std::ofstream output;

			wchar_t* dir_buffer = new wchar_t[1024];
			GetCurrentDirectory(1024, dir_buffer);
			std::wstring Path = L"";
			Path += dir_buffer;
			Path += L"\\pub\\dsl";
			wstring NumberPath = std::to_wstring(m_filecontainer.ID);
			ConnectionTextPadTo(NumberPath, 3, '0');
			Path += NumberPath += L".esf";

			output.open(Path, ios::out | ios::binary);
			output.write(m_Buffer, strlen(m_Buffer));
			std::string strMapID = "Spell File " + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			delete[] dir_buffer;
			output.close();
			std::string str(Path.begin(), Path.end());
			World::ESF_File = new ESF("\pub\\dsl001.esf");
			break;
		}
		case(Connection::FileType::ECF):
		{
			std::ofstream output;

			wchar_t* dir_buffer = new wchar_t[1024];
			GetCurrentDirectory(1024, dir_buffer);
			std::wstring Path = L"";
			Path += dir_buffer;
			Path += L"\\pub\\dat";
			wstring NumberPath = std::to_wstring(m_filecontainer.ID);
			ConnectionTextPadTo(NumberPath, 3, '0');
			Path += NumberPath += L".ecf";

			output.open(Path, ios::out | ios::binary);
			output.write(m_Buffer, strlen(m_Buffer));

			std::string strMapID = "Class File" + std::to_string(m_filecontainer.ID) + " Recieved!";
			World::DebugPrint(strMapID.c_str());
			delete[] dir_buffer;
			output.close();
			std::string str(Path.begin(), Path.end());
			World::ECF_File = new ECF("\pub\\dat001.ecf");;
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

	} catch(...)
	{ 
		World::DebugPrint("Couldn't connect to the login server.");
		World::ThrowMessage("Could not find server","The game server could not be found,\nplease try again at a later time."); 
		World::Connected = false; 
		World::Connecting = false;
		
		return;
	}
	while(true)
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
						this->ConnectionDropped = false;
						World::ThrowMessage("Could not find server","The game server could not be found,\nplease try again at a later time.");
						World::Connected = false;
						World::Connecting = false;
						return;
					}
				}
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
							if (ID <= FileType::Map)
							{
								Connection::FileContainer filecont;
								filecont.File_Type = FileType::Map;
	
								Game* gme =  V_Game;
								filecont.ID = gme->map->MapID;
								std::string str = reader->GetEndString();
								ProcessFile(str.c_str(), filecont);
								gme->map->LoadMap(filecont.ID);
							}
							else if (ID == 9)
							{
								//FriendsList
								
								int numberofplayers = reader->GetShort();
								reader->Getbyte();
								World::OnlinePlayers.clear();
								std::vector<World::OnlinePlayerContainer> Sortedcontainer;
								for (int i = 0; i < numberofplayers; i++)
								{
									World::OnlinePlayerContainer _player;
									_player._Name = reader->GetBreakString();
									_player._Name[0] = toupper(_player._Name[0]);
									_player._Title = reader->GetBreakString();
									reader->Getbyte();
									_player._Icon = reader->GetChar();
									_player._ClassID = reader->GetChar();
									_player._GuildTag = reader->GetBreakString();
									
									int insertindex = 0;
									for (int j = 0; j < Sortedcontainer.size(); j++)
									{
										if ((int)Sortedcontainer[j]._Name[0] <= (int)_player._Name[0])
										{
											insertindex = j + 1;
										}
										else { break; }
									}
									Sortedcontainer.insert(Sortedcontainer.begin() + insertindex, _player);
								}
								World::OnlinePlayers = Sortedcontainer;
								World::DebugPrint("Player list recieved!");
							}
							else
							{
								Connection::FileContainer filecont;
								filecont.File_Type = (FileType)ID;
								filecont.ID = reader->GetChar();
								std::string str = reader->GetEndString();
								ProcessFile(str.c_str(), filecont);
							}
							
							if (ID != 9 && ID != 11)
							{
								if (!FileQueue.empty())
									FileQueue.pop_front();
							}

						}
						else
						{
							std::string reportstr = "";
							reportstr += "[" + PacketProcessor::GetFamilyName(reader->Family()) + "]";
							reportstr += "[" + PacketProcessor::GetActionName(reader->Action()) + "]";

							for (int i = 0; i < reader->Length(); i++)
							{
								char* st = new char[8];
								_itoa(newbuf[i], st, 10);
								//reportstr += "[";
								//reportstr += st;
								//reportstr += "]";
								delete st;
							}
							World::DebugPrint(reportstr.c_str());
							Game* game = (Game*)V_Game;
							//game->map->ThreadLock.lock();
							packetHandler.HandlePacket(*reader, game, ClientStream);
							//game->map->ThreadLock.unlock();
						}

						delete reader;
					}
				}
				else
				{
					pt::psleep(1);
				}
			}
			catch(...)
			{
				if(!World::Connected && !this->ConnectionDropped)
				{
					World::DebugPrint("connection lost\n");
					this->ConnectionDropped = false;
					World::ThrowMessage("Could not find server","The game server could not be found,\nplease try again at a later time.");
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
