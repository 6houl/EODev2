#pragma once
#include "..\Utilities/Button.h"
class Map_UI_GameSettings
{
private:
	enum Setting
	{
		HearWhispers = 0,
		ShowBalloons,
		ShowShadows,
		SettingCount
	};

	Map_UI* m_MapUI;
	Game* m_game;
	Button* SettingsButtons[SettingCount];
	void ToggleSetting(Setting setting);
public:
	Button* UI_Element_GameSettingsButton;

	//Map_UI_Inventory();
	Map_UI_GameSettings(Map_UI* m_MapUI, Game* m_Game);
	void Update();
	void Render(float depth);
	~Map_UI_GameSettings();
};

