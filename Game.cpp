#include "Game_Stage\Menu.h"
#include "Game_Stage\Map_UI\Map_UI_Cursor.h"
#include "Game_Stage\Map_UI\Map_UI.h"
#include "Game_Stage\Map.h"
#include "Utilities/ConfigFile.h"
#include "stdafx.h"
#include "Resource_Manager.h"
#include <algorithm>

sf::Sprite* sprite;
std::shared_ptr<sf::Texture> texture;
//Tempory font grabbing addon.
bool GetFontData(const HFONT fontHandle, std::vector<char>& data)
{
	bool result = false;
	HDC hdc = ::CreateCompatibleDC(NULL);
	if (hdc != NULL)
	{
		::SelectObject(hdc, fontHandle);
		const size_t size = ::GetFontData(hdc, 0, 0, NULL, 0);
		if (size > 0)
		{
			char* buffer = new char[size];
			if (::GetFontData(hdc, 0, 0, buffer, size) == size)
			{
				data.resize(size);
				memcpy(&data[0], buffer, size);
				result = true;
			}
			delete[] buffer;
		}
		::DeleteDC(hdc);
	}
	return result;
}

//This class handles all game content and chooses when to activate certain events (Menu stages etc).
void Game::Initialize(sf::RenderWindow*m_Device, World* _World)
{
		this->world = _World;
		this->MouseX,this->MouseY = 0;
		this->Device = m_Device;
		//D3DXCreateSprite(Device, &sprite);
		this->FPS = 60;
		this->LastFrameTime = std::chrono::steady_clock::now();
		this->RenderList.reserve(8192);

		this->DefaultFont = new sf::Font();
		HINSTANCE hResInstance = (HINSTANCE)GetModuleHandle(NULL);
		HRSRC res = FindResource(hResInstance, MAKEINTRESOURCE(IDR_FONT1), RT_FONT);
		HGLOBAL Handle = LoadResource(NULL, res);
		size_t sizeoffont = SizeofResource(NULL, res);
		this->DefaultFont->loadFromMemory(Handle, sizeoffont);
		
		sf::Text* sftxt = new sf::Text();
		this->rendertext = std::shared_ptr<sf::Text>(sftxt);
		this->rendertext->setFont(*this->DefaultFont);

		this->ResourceManager = new Resource_Manager();
		this->ResourceManager->Initialize(Device);

		menu = new Menu();
		map = new Map();

		this->BT_Message_OK = new Button(this, MessageX + 180, MessageY + 112, 0, 116, 91, 28, false, this->ResourceManager->GetResource(1, 15, true));
		this->BT_ExitGame = new Button(this, 640 - 53, -1, -1, -1, 50, 53, true, this->ResourceManager->GetResource(2, 39, true));
		this->BT_CharDeleteOK = new Button(this, MessageX + 180 - 92, MessageY + 112, 0, 116, 91, 28, false, this->ResourceManager->GetResource(1, 15, true));
		this->BT_CharDeleteCancel = new Button(this, MessageX + 180, MessageY + 112, 0, 29, 91, 28, false, this->ResourceManager->GetResource(1, 15, true));
		Map_UserInterface = new Map_UI();
		menu->Initialize(world, Device, this);
		map->Initialize(world, Device, this);
		Map_UserInterface->Initialize(world, Device, this);


		
		this->MessageDragging = false;
		this->MessageSelected = false;
		this->MessageX = 190;
		this->MessageY = 150;
		this->Closed = false;
		this->Stage = Game::PMenu;
		this->SubStage = 0;
		this->ActiveConfirmation = ConfirmationNone;
		this->ActiveConfirmationResult = ConfirmationPending;

		this->MapCursor = Map_UI_Cursor(this, this->map, Device);

		this->world->W_UI_Infobox = new UI_InformationBox(this);	
		#ifndef DEBUG
		this->world->W_UI_Infobox->NewBox();
		this->world->W_UI_Infobox->CreateMessage("EODEV Version 0.8", "Welcome to EODev; Gantic's hobby project!");
		this->world->W_UI_Infobox->AddMessage("Be prepared for bugs - It's held together by scotch tape and zip ties in here!");
		this->world->W_UI_Infobox->AddMessage("If you don't believe me you can browse through\n the source code and cry yourself to sleep at \ngithub/GanticsAntics/Endless-Online-Development. \nCredits go to the original Endless Online \ndevelopers, and to Sausage & Sordie. \n\nFinally I'd like to thank the remaining EOCommunity who have faithfully stuck to endless after all these years. :)");
		//this->world->W_UI_Infobox->AddMessage("Welcome to my shop, this is a generic client-sided debug block of text to demonstrate the ability for text to exist with a shop.");
		//this->world->W_UI_Infobox->AddShopBuyItem(3, 131, 1091,34);
		this->world->W_UI_Infobox->SetLocation(256, 90);
		#endif
}	

void World::ThrowMessage(std::string Title, std::string Message)
{
	World::MBTitle = Title;
	World::MBMessage = Message;
	World::MBHidden = false;
}

void World::DebugPrint(pt::string Message)
{
#ifdef DEBUG
	std::printf("%s\n", Message.c_str());
#endif
	
}
void Game::ShowConfirmation(const std::string& title, const std::string& message, ConfirmationOwner owner)
{
	World::MBTitle = title;
	World::MBMessage = message;
	World::MBHidden = false;
	this->ActiveConfirmation = owner;
	this->ActiveConfirmationResult = ConfirmationPending;
}

Game::ConfirmationResult Game::ConsumeConfirmation(ConfirmationOwner owner)
{
	if (this->ActiveConfirmation != owner || this->ActiveConfirmationResult == ConfirmationPending)
		return ConfirmationPending;

	ConfirmationResult result = this->ActiveConfirmationResult;
	this->ActiveConfirmation = ConfirmationNone;
	this->ActiveConfirmationResult = ConfirmationPending;
	return result;
}
void Game::Update()
{
	const auto frameTime = std::chrono::steady_clock::now();
	double elapsed = std::chrono::duration<double>(frameTime - this->LastFrameTime).count();
	this->LastFrameTime = frameTime;
	if (elapsed <= 0.0)
	{
		elapsed = 1.0 / 120.0;
	}
	this->DeltaSeconds = (std::min)(elapsed, 0.1);
	this->ElapsedMilliseconds += static_cast<std::uint64_t>(this->DeltaSeconds * 1000.0);
	const double currentFPS = 1.0 / elapsed;
	const double blend = (std::min)(1.0, this->DeltaSeconds * 4.0);
	this->FPS += (currentFPS - this->FPS) * blend;

	std::unique_lock<std::recursive_mutex> stateLock(this->StateLock);
	if (this->ConsumeConfirmation(ConfirmationReturnToMenu) == ConfirmationAccepted)
	{
		this->Stage = Game::PMenu;
		stateLock.unlock();
		world->DropConnection();
		stateLock.lock();
	}

	if(!world->Connected && Stage != Game::PViewCredits)
	{
		Stage = Game::GameStage::PMenu;
	}
	if(World::MBHidden)
	{
		if (this->Stage < 5)
		{
			menu->Update();
			this->world->W_UI_Infobox->Update();
		}
		else
		{
			this->MapCursor.Update();
			//if (World::UIBox_Hidden)
			{
				map->Update();
			}
			this->world->W_UI_Infobox->Update();
			Map_UserInterface->Update();
		}

		if(this->Stage > 1)
		{ this->BT_ExitGame->Update(this->MouseX,this->MouseY,this->MousePressed); }
		
		if(this->BT_ExitGame->MouseClickedOnElement())
		{ 
			if(Stage == this->PViewCredits)
			{
				stateLock.unlock();
				world->DropConnection();
				stateLock.lock();
				this->Stage = Game::PMenu;
				this->SubStage = 0;
				this->BT_ExitGame->MouseClickProccessed();
			}
			else
			{
				this->ShowConfirmation("Return to menu", "Are you sure you want to return to the \nmain menu?", ConfirmationReturnToMenu);
				this->BT_ExitGame->MouseClickProccessed();
			}

		}
	}
	else
	{
		menu->BT_CC_Delete1->SetFrameID(0);
		menu->BT_CC_Delete2->SetFrameID(0);
		menu->BT_CC_Delete3->SetFrameID(0);
		menu->BT_CC_Login1->SetFrameID(0);
		menu->BT_CC_Login2->SetFrameID(0);
		menu->BT_CC_Login3->SetFrameID(0);
		menu->BT_ChangePassCancel->SetFrameID(0);
		menu->BT_ChangePassOK->SetFrameID(0);
		menu->BT_CreateAccount->SetFrameID(0);
		menu->BT_CreateCharCancel->SetFrameID(0);
		menu->BT_CreateCharOK->SetFrameID(0);
		menu->BT_ExitGame->SetFrameID(0);
		menu->BT_LGCancel->SetFrameID(0);
		menu->BT_LGChangePass->SetFrameID(0);
		menu->BT_LGCreateChar->SetFrameID(0);
		menu->BT_LGPlayGame->SetFrameID(0);
		menu->BT_PlayGame->SetFrameID(0);
		menu->BT_ViewCredits->SetFrameID(0);
		if(this->ActiveConfirmation == ConfirmationNone)
		{
			if (this->BT_Message_OK->GetFrameID() == 0 && this->MouseX > this->MessageX && this->MouseX < this->MessageX + 290 && this->MouseY > this->MessageY  && this->MouseY < this->MessageY  + 157)
			   {
				 if (this->RAWMousePressed)
					{
					   this->MessageSelected = true;
					}
			   }
		}
		else
		{
			if (this->BT_CharDeleteOK->GetFrameID() == 0  && this->BT_CharDeleteCancel->GetFrameID() == 0 && this->MouseX > this->MessageX && this->MouseX < this->MessageX + 290 && this->MouseY > this->MessageY  && this->MouseY < this->MessageY  + 157)
			   {
				 if (this->RAWMousePressed)
					{
					   this->MessageSelected = true;
					}
			   }
		}
        if (this->MessageSelected == true)
           {
             if (this->RAWMousePressed)
                {
                  if (this->MessageDragging == false)
                     {
                      DragX = this->MouseX - this->MessageX;
                      DragY = this->MouseY - this->MessageY ;
                      this->MessageDragging = true;
                     }
                  this->MessageX = this->MouseX - DragX;
                  this->MessageY  = this->MouseY - DragY;
                }
                else
                {
                 this->MessageDragging = false;
                 this->MessageSelected = false;
                }
           }
			this->BT_Message_OK->SetPosition(std::pair<int,int>(MessageX + 181,MessageY+ 113));
			this->BT_CharDeleteOK->SetPosition(std::pair<int, int>(MessageX + 180 - 92,MessageY+ 112));
			this->BT_CharDeleteCancel->SetPosition(std::pair<int, int>(MessageX + 181,MessageY+ 113));
			if(!this->MessageDragging)
			{
				if(this->ActiveConfirmation == ConfirmationNone)
				{
					this->BT_Message_OK->Update(MouseX,MouseY,MousePressed);
					if(this->BT_Message_OK->MouseClickedOnElement())
					{
						World::MBHidden = true;
						this->BT_Message_OK->MouseClickProccessed();
					}
				}
				else
				{
					this->BT_CharDeleteOK->Update(MouseX,MouseY,MousePressed);
					this->BT_CharDeleteCancel->Update(MouseX,MouseY,MousePressed);
					if(this->BT_CharDeleteOK->MouseClickedOnElement())
					{
						this->ActiveConfirmationResult = ConfirmationAccepted;
						World::MBHidden = true;
						this->BT_CharDeleteOK->MouseClickProccessed();
					}
					if(this->BT_CharDeleteCancel->MouseClickedOnElement())
					{
						this->ActiveConfirmationResult = ConfirmationCancelled;
						World::MBHidden = true;
						this->BT_CharDeleteCancel->MouseClickProccessed();
					}

				}

			}
	}
	this->MouseWheelVal = 0;
}
std::string fpsstring;
int Game_FPSCounter = 0;
void Game::Render()
{
	std::lock_guard<std::recursive_mutex> stateLock(this->StateLock);
	this->map->FinalizeMapState();
	this->RenderList.clear();
	Game_FPSCounter++;
	Device->clear(sf::Color::Black);

	if (this->Stage <= 4)
	{
		this->menu->Render();
	}
	else
	{
		this->map->Render();
		Map_UserInterface->Render();
	
		//this->map->ThreadLock.lock();
		RECT rct;

		rct.left = 20;
		rct.right = 300;
		rct.top = 32;
		rct.bottom = 300;

		//this->map->Sprite->Begin(D3DXSPRITE_ALPHABLEND);
		//this->DefaultFont->DrawTextW(this->map->Sprite, this->MapCursor.cur_istring.c_str(), -1, &rct, DT_LEFT, sf::Color::Color(255, 255, 255, 255));
		//this->map->Sprite->End();
		//this->map->ThreadLock.unlock();
	}

	if (this->Stage > 1)
	{
		this->BT_ExitGame->Draw();
	}
	world->W_UI_Infobox->Draw(0.001f);
	if (!World::MBHidden)
	{
		this->Draw(this->ResourceManager->GetResource(1, 18, false), this->MessageX - 1, this->MessageY - 1, sf::Color::Color(255, 255, 255, 255), 0,0,-1,-1, sf::Vector2f(1,1),0);
		if (this->ActiveConfirmation == ConfirmationNone)
		{
			this->BT_Message_OK->Draw();
		}
		else
		{
			this->BT_CharDeleteOK->Draw();
			this->BT_CharDeleteCancel->Draw();
		}
		RECT rct;
		rct.left = this->MessageX + 60;
		rct.right = rct.left + 270;
		rct.top = this->MessageY + 25;
		rct.bottom = rct.top + 30;
		this->DrawTextW(World::MBTitle.c_str(), rct.left, rct.top, sf::Color::Color(240, 240, 199, 255),  11, false, 0);
		rct.left = this->MessageX + 20;
		rct.right = rct.left + 270;
		rct.top = this->MessageY + 60;
		rct.bottom = rct.top + 130;
		this->DrawTextW(World::MBMessage.c_str(), rct.left, rct.top, sf::Color::Color(240, 240, 199,255), 11, false, 0);
	}


	this->FpsDisplayElapsed += this->DeltaSeconds;
	if (this->FpsDisplayElapsed >= 0.1)
	{
		fpsstring = to_string((int)FPS);
		fpsstring += " fps";
		this->FpsDisplayElapsed = 0.0;
	}

	this->DrawTextW(fpsstring.c_str(), 33, 14, sf::Color(240, 240, 199, 255), 16, false,0,1);
	this->FinalizeRender();
	//this->menu->Render();
	Device->display();


	//Device->Present( NULL, NULL, NULL, NULL );
	//World::DebugPrint(std::to_string(FPS).c_str());
}
void Game::Unload()
{
	//if(this->DefaultFont){this->DefaultFont->Release();}
	//this->DefaultFont = NULL;
	if(this->menu){this->menu->Release();}
	if(this->ResourceManager){this->ResourceManager->Release();}
	//this->ResourceManager = NULL;

	//if(sprite){sprite->Release();}
	//sprite = NULL;
	if(texture){texture.reset();}

}
void Game::ResetDevice()
{
		//D3DXCreateSprite(Device,&sprite);
}
void Game::Draw(Resource_Manager::TextureData* dat, float x, float y, sf::Color Color, int imgx, int imgy, int imgw, int  imgh, sf::Vector2f scale, float _Depth)
{
	RenderInfo newinfo = RenderInfo(dat, x, y, Color, imgx, imgy, imgw, imgh, scale);
	newinfo.depth = _Depth;
	newinfo.order = this->RenderList.size();
	this->RenderList.push_back(std::move(newinfo));
}

void Game::Draw(DWORD ModuleID, int GFXID, bool BlackIsTransparent, float x, float y, sf::Color Color, int imgx, int imgy, int imgw, int  imgh, sf::Vector2f scale, float _Depth)
{
	Resource_Manager::TextureData* _dat = this->ResourceManager->GetResource(ModuleID, GFXID, BlackIsTransparent);
	RenderInfo newinfo = RenderInfo(_dat, x, y, Color, imgx, imgy, imgw, imgh, scale);
	newinfo.depth = _Depth;
	newinfo.order = this->RenderList.size();
	this->RenderList.push_back(std::move(newinfo));
}
void Game::Draw(std::shared_ptr<sf::Sprite>* _RenderSprite, std::shared_ptr<sf::RenderTexture>*  Rendertex, float x, float y, sf::Color Color, int imgx, int imgy, int imgw, int  imgh, sf::Vector2f Scale, float Depth )
{
	RenderInfo newinfo = RenderInfo(_RenderSprite, Rendertex, x, y, Color, imgx, imgy, imgw, imgh, Scale);
	newinfo.depth = Depth;
	newinfo.order = this->RenderList.size();
	this->RenderList.push_back(std::move(newinfo));
}
void Game::DrawText(std::string str, float x, float y, sf::Color Color, int height, bool centered, float _Depth, int outlinethickness, int bottomx, int bottomy)
{
	RenderInfo newinfo = RenderInfo(str, x, y, Color, bottomx, bottomy, NULL, height, sf::Vector2f(1,1),centered, outlinethickness);
	newinfo.depth = _Depth;
	newinfo.order = this->RenderList.size();
	this->RenderList.push_back(std::move(newinfo));

}
sf::Vector2f Game::GetFontSize(std::string _Message, int fontsize)
{
	RenderTextLock.lock();
	this->rendertext->setString(_Message);
	this->rendertext->setScale(0.5, 0.5);
	this->rendertext->setCharacterSize(fontsize * 2);
	sf::Vector2f returnval = sf::Vector2f(this->rendertext->getGlobalBounds().width, this->rendertext->getGlobalBounds().height);
	RenderTextLock.unlock();
	return returnval;
}
void Game::FinalizeRender()
{
	std::sort(this->RenderList.begin(), this->RenderList.end(), [](const RenderInfo& left, const RenderInfo& right)
	{
		if (left.depth != right.depth)
			return left.depth > right.depth;
		return left.order < right.order;
	});
	for (auto const& entry : this->RenderList)
	{
		switch (entry.ResourceType)
		{
			case(RenderInfo::DrawType::Texture):
			{
				if (entry._TextureData != NULL)
				{
					int imgw = entry.imgw - entry.imgx;
					int imgh = entry.imgh - entry.imgy;
					entry._TextureData->_Sprite->setScale(entry.Scale);
					entry._TextureData->_Sprite->setPosition(entry.x + (entry.Scale.x == -1 ? ((imgw + imgh < 0) ? entry._TextureData->_width : imgw) : 0), entry.y + (entry.Scale.y == -1 ? ((imgw + imgh < 0) ? entry._TextureData->_height : imgh) : 0));
					if (imgw + imgh < 0)
					{
						entry._TextureData->_Sprite->setTextureRect(sf::IntRect(0, 0, entry._TextureData->_width, entry._TextureData->_height));
					}
					else
					{
						entry._TextureData->_Sprite->setTextureRect(sf::IntRect(entry.imgx, entry.imgy, imgw, imgh));
					}
					entry._TextureData->_Sprite->setColor(entry.Color);

					Device->draw(*entry._TextureData->_Sprite);
				}
				break;
			}
			case(RenderInfo::DrawType::Text):
			{
				this->RenderTextLock.lock();
				rendertext->setColor(entry.Color);
				rendertext->setString(entry._Message);
				rendertext->setOutlineThickness(entry.borderthickness);
				rendertext->setOutlineColor(sf::Color::Black);
				rendertext->setScale(0.5, 0.5);
				rendertext->setCharacterSize(entry.imgh * 2);
				if (entry.textcentered)
				{
					sf::FloatRect textRect = rendertext->getGlobalBounds();
					rendertext->setPosition(entry.x - (textRect.width / 2), entry.y);
				}
				else
				{
					rendertext->setPosition(entry.x, entry.y);
				}
				if (entry.imgx > 0 || entry.imgy > 0)
				{
					//rendertext->
				}
				Device->draw(*rendertext.get());
				this->RenderTextLock.unlock();
				break;
			}
			case(RenderInfo::DrawType::MergedTexture):
			{	
				sf::RenderTexture* txturhandle = entry.RenderTextureTarget->get();
				sf::Sprite* sptehndle = entry.RenderTargetSprite->get();
				
				if (txturhandle != nullptr)
				{
					int imgw = entry.imgw - entry.imgx;
					int imgh = entry.imgh - entry.imgy;
					int txturew = txturhandle->getSize().x;
					int txtureh = txturhandle->getSize().y;
					entry.RenderTargetSprite->get()->setScale(entry.Scale);
					entry.RenderTargetSprite->get()->setPosition(entry.x + (entry.Scale.x == -1 ? ((imgw + imgh < 0) ? txturew : imgw) : 0), entry.y);
					if (imgw + imgh < 0)
					{
						entry.RenderTargetSprite->get()->setTextureRect(sf::IntRect(0, 0, txturew, txtureh));
					}
					else
					{
						entry.RenderTargetSprite->get()->setTextureRect(sf::IntRect(entry.imgx, entry.imgy, imgw, imgh));
					}
					entry.RenderTargetSprite->get()->setColor(entry.Color);

					Device->draw(*entry.RenderTargetSprite->get());
				}
				break;
			}
		}
	}
}
void Game::SetStage(GameStage _Stage)
{
	this->Stage = _Stage;
}

Game::~Game()
{
	this->Map_UserInterface->~Map_UI();
}

void Game::Close()
{
	Device->close();
}
