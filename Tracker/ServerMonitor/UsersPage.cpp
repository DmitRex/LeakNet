//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "UsersPage.h" 
#include "ServerList.h"

#include <vgui_controls/Controls.h>
#include <VGUI/IScheme.h>
//#include <vgui_controls/TextEntry.h>
#include <vgui_controls/RichText.h>

#include <stdio.h>

using namespace vgui;

// server list
extern CServerList *g_pServerList;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CUsersPage::CUsersPage()
	: PropertyPage(NULL, "UsersPage")
{
//	m_pText = new TextEntry(this, "UsersText");
	m_pText = new RichText(this, "UsersText");
//	m_pText->SetMultiline(true);
//	m_pText->SetRichEdit(true);
//	m_pText->SetEditable(false);
	m_pText->SetVerticalScrollbar(true);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CUsersPage::~CUsersPage()
{
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the new text
//-----------------------------------------------------------------------------
void CUsersPage::PerformLayout()
{
	BaseClass::PerformLayout();

	// layout controls
	int x = 10, y = 10, wide, tall;
	GetSize(wide, tall);
	tall -= (y * 2);
	wide -= (x * 2);

	m_pText->SetBounds(x, y, wide, tall);
	m_pText->SetText("");

	// get server details
	int userCount = 0, maxCount = 0;
	int underLoad = 0, activeServers = 0;
	int bandwidthRecv = 0, bandwidthSent = 0;
	for (int i = 0; i < g_pServerList->ServerCount(); i++)
	{
		server_t &server = g_pServerList->GetServer(i);

		userCount += server.users;
		maxCount += server.maxUsers;
		bandwidthRecv += server.bytesReceivedPerSecond;
		bandwidthSent += server.bytesSentPerSecond;

		if (server.underHeavyLoad || server.state == SERVER_NOTRESPONDING)
		{
			underLoad += 1;
		}

		if (server.state == SERVER_ACTIVE)
		{
			activeServers += 1;
		}
	}
	float load = 0.0f;
	if (g_pServerList->ServerCount() > 0)
	{
		load = (float)underLoad / (float)g_pServerList->ServerCount();
	}

	Color col1 = GetSchemeColor("DimBaseText", scheme()->GetIScheme(scheme()->GetDefaultScheme()));
	Color col2 = GetSchemeColor("BaseText", scheme()->GetIScheme(scheme()->GetDefaultScheme()));
	Color col3 = GetSchemeColor("BrightBaseText", scheme()->GetIScheme(scheme()->GetDefaultScheme()));

	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Active Users: ");
	m_pText->InsertColorChange(col2);
	char buf[64];
	sprintf(buf, "%d / %d", userCount, maxCount);
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");

	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Active Servers: ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d / %d", activeServers, g_pServerList->ServerCount());
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");

	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Server Load: ");

	if (load < 0.2f)
	{
		m_pText->InsertColorChange(col2);
		m_pText->InsertString("Light");
	}
	else if (load < 0.5f)
	{
		m_pText->InsertColorChange(col2);
		m_pText->InsertString("Medium");
	}
	else if (load < 0.8f)
	{
		m_pText->InsertColorChange(col3);
		m_pText->InsertString("High");
	}
	else
	{
		m_pText->InsertColorChange(col3);
		m_pText->InsertString("Very High");
	}

	m_pText->InsertString("\n");

	float flRecvKB = bandwidthRecv / 1024.0f;
	float flSentKB = bandwidthSent / 1024.0f;

	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Data receive rate: ");
	m_pText->InsertColorChange(col2);
	if (flRecvKB < 0.01f)
	{
		sprintf(buf, "%d kb/s", 0);
	}
	else if (flRecvKB < 1.0f)
	{
		sprintf(buf, "%.2f kb/s", flRecvKB);
	}
	else if (flRecvKB < 10.0f)
	{
		sprintf(buf, "%.1f kb/s", flRecvKB);
	}
	else
	{
		sprintf(buf, "%d kb/s", (int)flRecvKB);
	}
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");

	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Data send rate: ");
	m_pText->InsertColorChange(col2);
	if (flSentKB < 0.01f)
	{
		sprintf(buf, "%d kb/s", 0);
	}
	else if (flSentKB < 1.0f)
	{
		sprintf(buf, "%.2f kb/s", flSentKB);
	}
	else if (flSentKB < 10.0f)
	{
		sprintf(buf, "%.1f kb/s", flSentKB);
	}
	else
	{
		sprintf(buf, "%d kb/s", (int)flSentKB);
	}
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");
}
