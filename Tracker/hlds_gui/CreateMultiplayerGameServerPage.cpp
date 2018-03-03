//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <windows.h> // VXP
#include <stdio.h>

#include "CreateMultiplayerGameServerPage.h"

using namespace vgui;

#include <vgui_controls/Controls.h>
#include <KeyValues.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/MessageBox.h>

#include "FileSystem.h"
#include "vinternetdlg.h"


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::CCreateMultiplayerGameServerPage(vgui::Panel *parent, const char *name) : PropertyPage(parent, name)
{
	m_pMapList = new ListPanel(this, "MapList");

	m_pMODList = new CModCombo(this,"MODCombo",10,false);
	m_pExtraCmdLine = new TextEntry(this,"ExtraEdit");
	m_pGoButton = new Button(this,"GoButton","&Go");

	LoadControlSettings("Server/CreateMultiplayerGameServerPage.res", "PLATFORM");
	m_pMapList->SetEnabled(false);
	m_pGoButton->SetEnabled(false);

	m_pMapList->AddColumnHeader(0, "mapname", "Map", m_pMapList->GetWide(), true, RESIZABLE, RESIZABLE);
	LoadMODList();

	// load some defaults into the controls
	SetControlString("ServerNameEdit", "Player");
	SetControlString("MaxPlayersEdit", "4");

	m_szMapName[0] = 0;
	m_szHostName[0] = 0;
	m_szPassword[0] = 0;
	m_iMaxPlayers = 4;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::~CCreateMultiplayerGameServerPage()
{
}

//-----------------------------------------------------------------------------
// Purpose: called to get the info from the dialog
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::OnApplyChanges()
{
	strncpy(m_szHostName, GetControlString("ServerNameEdit", "listen server"), DATA_STR_LENGTH);
	strncpy(m_szPassword, GetControlString("PasswordEdit", ""), DATA_STR_LENGTH);
	m_iMaxPlayers = GetControlInt("MaxPlayersEdit", 4);

//	int selectedRow = m_pMapList->GetSelectedRow(0);
	int selectedRow = m_pMapList->GetSelectedItem(0);
	if (selectedRow >= 0)
	{
		KeyValues *kv = m_pMapList->GetItem(selectedRow);
		strncpy(m_szMapName, kv->GetString("mapname", ""), DATA_STR_LENGTH);
	}
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
/*
void CCreateMultiplayerGameServerPage::LoadMODList()
{

//	m_pMODList->RemoveAllItems();
	m_pMODList->DeleteAllItems();

	FileFindHandle_t findHandle = NULL;
	const char *filename = filesystem()->FindFirst("*", &findHandle);
	while (filename)
	{
		Msg( "LoadMODList: %s\n", filename );
		// add to the map list
		if( filesystem()->FindIsDirectory(findHandle)) 
		{
			char libname[1024];
			_snprintf(libname,1024,"%s\\liblist.gam",filename);
		//	_snprintf(libname,1024,"%s\\scripts\\liblist.gam", filename);
			if(filesystem()->FileExists(libname))
			{
				m_pMODList->AddItem( filename, NULL);
			}
		}
		
		filename = filesystem()->FindNext(findHandle);
	}
	filesystem()->FindClose(findHandle);

}
*/
void CCreateMultiplayerGameServerPage::LoadMODList()
{
	m_pMODList->DeleteAllItems();

	// look for third party games
	char szSearchPath[_MAX_PATH + 5];
	strcpy(szSearchPath, "*.*");

	// use local filesystem since it has to look outside path system, and will never be used under steam
	WIN32_FIND_DATA wfd;
	HANDLE hResult;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));
	
	hResult = FindFirstFile( szSearchPath, &wfd);
	if (hResult != INVALID_HANDLE_VALUE)
	{
		BOOL bMoreFiles;
		while (1)
		{
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (_strnicmp(wfd.cFileName, ".", 1)))
			{
				// Check for dlls\*.dll
				char szDllDirectory[MAX_PATH + 16];
			//	sprintf(szDllDirectory, "%s\\liblist.gam", wfd.cFileName);
				sprintf(szDllDirectory, "%s\\scripts\\liblist.gam", wfd.cFileName);

				FILE *f = fopen(szDllDirectory, "rb");
				if (f)
				{
					m_pMODList->AddItem( wfd.cFileName, NULL);
					fclose(f);
				}
			}
			bMoreFiles = FindNextFile(hResult, &wfd);
			if (!bMoreFiles)
				break;
		}
		
		FindClose(hResult);
	}
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();
	m_pMapList->SetEnabled(true);
	m_pGoButton->SetEnabled(true);

	m_pMODList->GetText(m_szMod,DATA_STR_LENGTH);
	char basedir[1024];
	//_snprintf(basedir,1024,"%s",m_szMod);
	vgui::filesystem()->AddSearchPath(m_szMod, "BASEDIR");

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	FileFindHandle_t findHandle = NULL;
	const char *filename = filesystem()->FindFirst("maps/*.bsp", &findHandle);
	while (filename)
	{
		// remove the text 'maps/' and '.bsp' from the file name to get the map name
		char mapname[256];
		char *str = (char *)strstr(filename, "maps");
		if (str)
		{

			strncpy(mapname, str + 4, sizeof(mapname) - 1);
		}
		else
		{
			strncpy(mapname, filename, sizeof(mapname) - 1);
		}
		str = strstr(mapname, ".bsp");
		if (str)
		{
			*str = 0;
		}

		//!! hack: strip out single player HL maps
		// this needs to be specified in a seperate file
		if ((mapname[0] == 'c' || mapname[0] == 't') && mapname[2] == 'a' && mapname[1] >= '0' && mapname[1] <= '5')
		{
			goto nextFile;
		}

		// add to the map list
		m_pMapList->AddItem(new KeyValues("data", "mapname", mapname), 0, false, false);

		// get the next file
	nextFile:
		filename = filesystem()->FindNext(findHandle);
	}
	filesystem()->FindClose(findHandle);

	// set the first item to be selected
	if (m_pMapList->GetItemCount() > 0)
	{
	//	m_pMapList->SetSelectedRows(0, 0);
		m_pMapList->SetSelectedCell(0, 0);
	}

	vgui::filesystem()->RemoveSearchPath(basedir);



}

const char *CCreateMultiplayerGameServerPage::GetMapName()
{
//	if( m_pMapList->GetNumSelectedRows())
	if( m_pMapList->GetSelectedItemsCount())
	{
	//	return m_pMapList->GetItem(m_pMapList->GetSelectedRow(0))->GetString("mapname");
		return m_pMapList->GetItem(m_pMapList->GetSelectedItem(0))->GetString("mapname");
	}
	return NULL;
}


void CCreateMultiplayerGameServerPage::OnCommand(const char *text)
{
	if(!_stricmp(text,"go"))
	{

		// create the command to execute
		char cvars[1024];
		char cmdline[1024];

		OnApplyChanges(); // update our member variables

		_snprintf(cmdline,1024,"-game %s -maxplayers %i +map %s ",
						m_szMod,m_iMaxPlayers,GetMapName());

		m_pExtraCmdLine->GetText(cvars,1024);
		strncat(cmdline,cvars,1024);		

		_snprintf(cvars,1024, "sv_lan 0\nsetmaster enable\nsv_password \"%s\"\nhostname \"%s\"\n",
				m_szPassword,m_szHostName);

		VInternetDlg::GetInstance()->StartServer(cmdline,cvars);

	}

	BaseClass::OnCommand(text);
}

void CCreateMultiplayerGameServerPage::CModCombo::OnHideMenu(vgui::Menu *menu)
{
	m_pParent->LoadMapList();
}	
