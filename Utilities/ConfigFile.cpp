#include "..\stdafx.h"
#include "ConfigFile.h"
#include <iostream>
#include <Windows.h>

CIniReader::CIniReader(char* szFileName)
{
 memset(m_szFileName, 0x00, 255);
 memcpy(m_szFileName, szFileName, strlen(szFileName));
}
int CIniReader::ReadInteger(char* szSection, char* szKey, int iDefaultValue)
{
 int iResult = GetPrivateProfileIntA(szSection,  szKey, iDefaultValue, m_szFileName); 
 return iResult;
}
float CIniReader::ReadFloat(char* szSection, char* szKey, float fltDefaultValue)
{
 char szResult[255];
 char szDefault[255];
 float fltResult;
 sprintf(szDefault, "%f",fltDefaultValue);
 GetPrivateProfileStringA(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
 fltResult =  atof(szResult);
 return fltResult;
}
bool CIniReader::ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue)
{
 char szResult[255];
 char szDefault[255];
 sprintf(szDefault, "%s", bolDefaultValue? "on" : "off");
 GetPrivateProfileStringA(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
 return _stricmp(szResult, "true") == 0 || _stricmp(szResult, "on") == 0 || strcmp(szResult, "1") == 0;
}
char* CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue)
{
 char* szResult = new char[255];
 memset(szResult, 0x00, 255);
 GetPrivateProfileStringA(szSection,  szKey, 
		szDefaultValue, szResult, 255, m_szFileName); 
 return szResult;
}

CIniWriter::CIniWriter(char* szFileName)
{
 memset(m_szFileName, 0x00, 255);
 memcpy(m_szFileName, szFileName, strlen(szFileName));
}
void CIniWriter::WriteInteger(char* szSection, char* szKey, int iValue)
{
 char szValue[255];
 sprintf(szValue, "%d", iValue);
 WritePrivateProfileStringA(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteFloat(char* szSection, char* szKey, float fltValue)
{
 char szValue[255];
 sprintf(szValue, "%f", fltValue);
 WritePrivateProfileStringA(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteBoolean(char* szSection, char* szKey, bool bolValue)
{
 char szValue[255];
 sprintf(szValue, "%s", bolValue ? "on" : "off");
 WritePrivateProfileStringA(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteString(char* szSection, char* szKey, char* szValue)
{
 WritePrivateProfileStringA(szSection,  szKey, szValue, m_szFileName);
}
std::string IniConfiguration::Host = "";
int IniConfiguration::Port = 0;
bool IniConfiguration::FullScreen = false;
bool IniConfiguration::Sizeable = true;
bool IniConfiguration::StayOnTop = false;
bool IniConfiguration::Music = false;
bool IniConfiguration::Sound = true;
bool IniConfiguration::Shadows = true;
bool IniConfiguration::ChatBalloons = true;
bool IniConfiguration::HearWhispers = true;
bool IniConfiguration::Filter = true;
bool IniConfiguration::FilterAll = false;

IniConfiguration::IniConfiguration()
	: reader(NULL), writer(NULL)
{
}

void IniConfiguration::Init()
{
	DWORD cchCurDir = MAX_PATH;
	char szCurDir[MAX_PATH];
		
	int cnt = GetCurrentDirectoryA(cchCurDir, szCurDir);
	std::string str = "\\config\\setup.ini";
	for(int i = 0; i <str.length();i++)
	{
		szCurDir[cnt + i] = str[i];
	}
	szCurDir[cnt + str.length()] = '\0';
	reader = new CIniReader(szCurDir);
	writer = new CIniWriter(szCurDir);
	char* host = reader->ReadString("CONNECTION","Host","game.endless-online.com");
	this->Host = host;
	delete[] host;
	this->Port = reader->ReadInteger("CONNECTION","Port",8078);
	this->FullScreen = reader->ReadBoolean("CONFIGURATION", "Fullscreen", false);
	this->Sizeable = reader->ReadBoolean("CONFIGURATION", "Sizeable", true);
	this->StayOnTop = reader->ReadBoolean("CONFIGURATION", "StayOnTop", false);
	this->Music = reader->ReadBoolean("SETTINGS", "Music", false);
	this->Sound = reader->ReadBoolean("SETTINGS", "Sound", true);
	this->Shadows = reader->ReadBoolean("SETTINGS", "ShowShadows", true);
	this->ChatBalloons = reader->ReadBoolean("SETTINGS", "ShowBaloons", true);
	this->HearWhispers = reader->ReadBoolean("CHAT", "HearWhisper", true);
	this->Filter = reader->ReadBoolean("CHAT", "Filter", true);
	this->FilterAll = reader->ReadBoolean("CHAT", "FilterAll", false);
}

void IniConfiguration::SaveSettings()
{
	if (!writer)
		return;

	writer->WriteBoolean("SETTINGS", "ShowShadows", Shadows);
	writer->WriteBoolean("SETTINGS", "ShowBaloons", ChatBalloons);
	writer->WriteBoolean("CHAT", "HearWhisper", HearWhispers);
}
