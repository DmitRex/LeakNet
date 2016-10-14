//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Entry point into app
//=============================================================================

#include <VGUI\MouseCode.h>
#include <VGUI\KeyCode.h>

#include "interface.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/IVGui.h>
#include "FileSystem.h"

#include "..\..\tracker\common\winlite.h"

#include "LocalizationDialog.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// Purpose: Entry point
//			loads interfaces and initializes dialog
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// load vgui
	CSysModule *vguiModule = Sys_LoadModule("vgui2.dll");
	if (!vguiModule)
	{
		vguiModule = Sys_LoadModule("../../bin/vgui2.dll");
	}
	CreateInterfaceFn vguiFactory = Sys_GetFactory(vguiModule);
	if (!vguiFactory)
	{
		printf("Fatal error: Could not load vgui2.dll\n");
		return 2;
	}

	CSysModule *filesystem = Sys_LoadModule("FileSystem_Stdio.dll");
	if (!filesystem)
	{
		filesystem = Sys_LoadModule("../../bin/FileSystem_Stdio.dll");
	}
	CreateInterfaceFn filesystemFactory = Sys_GetFactory(filesystem);
	if (!filesystem)
	{
		printf("Fatal error: Could not load bin/filesystem_stdio.dll\n");
		return 2;
	}	

//	CreateInterfaceFn factorylist[3];
//	factorylist[0] = Sys_GetFactoryThis();
//	factorylist[1] = Sys_GetFactory(vguiModule);
//	factorylist[2] = Sys_GetFactory(filesystem);
	CreateInterfaceFn factorylist[2];
	factorylist[0] = vguiFactory;
	factorylist[1] = filesystemFactory;

	// initialize interfaces
//	if (vgui::VGui_InitInterfacesList("vlocalize", factorylist, 3))
	if (!vgui::VGui_InitInterfacesList("vlocalize", factorylist, 2))
	{
		printf("Fatal error: Could not initalize interface factories\n");
		return 3;
	}

	vgui::filesystem()->AddSearchPath("../", "PLATFORM");

	vgui::surface()->Init(/*panel->GetVPanel()*/);

	// load the scheme
	if (!vgui::scheme()->LoadSchemeFromFile("Resource/TrackerScheme.res", "vlocalize"))
		return 1;

	// Init the surface
	vgui::Panel *panel = new vgui::Panel(NULL, "TopPanel");
//	vgui::surface()->Init(/*panel->GetVPanel()*/);
	vgui::surface()->SetEmbeddedPanel( panel->GetVPanel() );

	// Start vgui
	vgui::ivgui()->Start();

	// add our main window
	printf("Loading %s...\n", lpCmdLine);
//	vgui::Panel *main = new CLocalizationDialog(lpCmdLine);
	CLocalizationDialog *main = new CLocalizationDialog(lpCmdLine);
	main->Activate();

	// Run app frame loop
	while (vgui::ivgui()->IsRunning())
	{
		vgui::ivgui()->RunFrame();
	}

	// Shutdown
	vgui::surface()->Shutdown();

	delete main;

	Sys_UnloadModule(vguiModule);
	return 1;
}
