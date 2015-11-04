/*
========================

Copyright (c) 2014 Vinyl Games Studio

	LANMenu.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/23/2014

========================
*/
#include "LANMenu.h"
#include "Engine.h"
#include "NetworkModule.h"
#include "RunningInfo.h"
#include "NetworkClient.h"
#include "Engine.h"
#include "RenderModule.h"
#include "GUIManager.h"
#include "OpenGLRenderModule.h"
#include "UICommand.h"

#include <NetConsts.h>

#include <sstream>

using namespace vgs;

LANMenu::LANMenu()
	: m_winWidth(333.3333333f)
	, m_connectionTarget(nullptr)
	, m_joinPressed(false)
{
}

LANMenu::~LANMenu()
{
	m_widgetMap->erase("createGame_btn");
	m_widgetMap->erase("connectGame_btn");

	for (auto& entry : m_serverAddresses)
	{
		delete entry.first;
	}
	m_serverAddresses.clear();

	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->GetMsgPoolMgr()->RemovePool("add_server");
		guiMgr->GetMsgPoolMgr()->RemovePool("update_server");
	}
}

SFGWinPtr LANMenu::CreateElements(WidgetMap& widgetMap)
{
	// save for better times
	m_widgetMap = &widgetMap;

	m_window = sfg::Window::Create( sfg::Window::Style::BACKGROUND );
	m_window->SetTitle("LAN Menu");

	//SetRelativePosition(sf::Vector2f(.5f, .5f));
	m_window->SetRequisition(sf::Vector2f(m_winWidth, 0.f));

	m_createBtn = sfg::Button::Create("Create new game");
	widgetMap["createGame_btn"] = std::shared_ptr<sfg::Button>(m_createBtn);
	m_joinBtn = sfg::Button::Create("Join game");
	m_returnBtn = sfg::Button::Create("Return");

	// Create a box for our ScrolledWindow. ScrolledWindows are bins
	// and can only contain 1 child widget.
	m_scrolledWindowBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

	// Create the ScrolledWindow.
	auto scrolledwindow = sfg::ScrolledWindow::Create();

	// Set the ScrolledWindow to always show the horizontal scrollbar
	// and only show the vertical scrollbar when needed.
	scrolledwindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_ALWAYS);

	// Add the ScrolledWindow box to the ScrolledWindow
	// and create a new viewport automatically.
	scrolledwindow->AddWithViewport(m_scrolledWindowBox);

	// Always remember to set the minimum size of a ScrolledWindow.
	scrolledwindow->SetRequisition(sf::Vector2f(m_winWidth, 100.f));

	// Create a vertical box layouter with 5 pixels spacing and add the label
	// and button to it.
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.f);

	auto buttonBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	buttonBox->Pack(m_createBtn);
	buttonBox->Pack(m_joinBtn);
	buttonBox->Pack(m_returnBtn);

	auto titleLbl = sfg::Label::Create("Found games");

	EntryLine entry;
	entry.box			= sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);
	entry.address		= sfg::Label::Create("IP:Port");
	entry.ping			= sfg::Label::Create("Ping");
	entry.capacity		= sfg::Label::Create("Players");
	//auto btnEmptySpace	= sfg::Label::Create(" ");

	entry.address->SetClass("inwindow_title");
	entry.ping->SetClass("inwindow_title");
	entry.capacity->SetClass("inwindow_title");
	//btnEmptySpace->SetClass("inwindow_title");

	entry.address->SetRequisition(sf::Vector2f(180.f, 0));
	entry.ping->SetRequisition(sf::Vector2f(50.f, 0));
	entry.capacity->SetRequisition(sf::Vector2f(75.f, 0));
	//btnEmptySpace->SetRequisition(sf::Vector2f(65.f, 0));

	entry.box->Pack(entry.address);
	entry.box->Pack(entry.capacity);
	entry.box->Pack(entry.ping);
	//entry.box->Pack(btnEmptySpace);
	
	auto separator = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);

	m_scrolledWindowBox->Pack(entry.box);
	m_scrolledWindowBox->Pack(separator);

	m_smartMessage = sfg::Label::Create(">");
	m_smartMessage->SetAlignment(sf::Vector2f(0.f, 0.f));

	box->Pack(titleLbl);
	box->Pack(scrolledwindow, false, true);
	box->Pack(buttonBox);
	box->Pack(m_smartMessage);
	
	m_window->Add(box);

	ConnectSignals();
	
	/*GUILayer::*/InitPools();
	SetVisible(false);

	m_deskPtr->SetProperty("Label.inwindow_title", "Color", sf::Color(110, 110, 255, 255));
	m_deskPtr->SetProperty("Label.selected_row", "Color", sf::Color(100, 255, 100, 255));
	m_deskPtr->SetProperty("Label.not_selected_row", "Color", sf::Color(200, 200, 200, 255));


	return m_window;
}

void LANMenu::ConnectSignals()
{
	m_createBtn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&LANMenu::OnCreateGameButtonClicked, this));
	m_joinBtn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&LANMenu::OnJoinGameButtonClicked, this));
	m_returnBtn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&LANMenu::OnReturnButtonClicked, this));
}


void LANMenu::OnCreateGameButtonClicked()
{
	if (m_joinPressed)
	{
		return;
	}

	// watafaq! how this ever never crashed?
	// TODO: make UI command for starting server
	if (!Engine::GetInstance()->GetModuleByType<NetworkModule>(EngineModuleType::NETWORKER)->StartServer())
	{
		m_smartMessage->SetText("> Could not start server");
	}
	else
	{
		m_smartMessage->SetText("> Server started");
	}
}

void LANMenu::OnJoinGameButtonClicked()
{
	if (m_joinPressed)
	{
		return;
	}

	if (!m_connectionTarget)
	{
		m_smartMessage->SetText("> No server selected");
		return;
	}
	else
	{
		m_joinPressed = true;
		m_smartMessage->SetText("> Connecting ...");
	}

	unsigned short port = m_connectionTarget->address.GetPort();
	const char* host = m_connectionTarget->address.ToString(false);
	size_t hostLen = strlen(m_connectionTarget->address.ToString(false)) + 1;

	// len = port + len of ip str + ip str
	unsigned int	len		= sizeof(unsigned short) + sizeof(size_t) + hostLen + 1;
	unsigned char*	data	= new unsigned char[len]; // for \0
	unsigned int	offset	= 0;

	memcpy(data + offset, &port, sizeof(unsigned short));
	offset += sizeof(unsigned short);
	memcpy(data + offset, &hostLen, sizeof(size_t));
	offset += sizeof(size_t);
	memcpy(data + offset, host, hostLen);

	data[len - 1] = '\0';

	UICommand::AddNetCommand(UICommand(UICommand::CMD_NET_CONNECT_TO, len, data));
	UICommand::AddNetCommand(UICommand(UICommand::CMD_NET_SEND_JOIN_REQUEST, 0, 0));
}

void LANMenu::OnReturnButtonClicked()
{
	if (m_joinPressed)
	{
		return;
	}

	SetVisible(false);
	SetEnabled(false);
	
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->SendVisMsg(SetVisibilityMsg("main_menu", true));
		guiMgr->SendEnableMsg(SetEnableMsg("main_menu", true));
	}
}

void LANMenu::AddServerToTheList(const RakNet::SystemAddress& address, unsigned char max)
{
	EntryLine entry;
	entry.box			= sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);
	entry.address		= sfg::Label::Create(address.ToString());
	entry.ping			= sfg::Label::Create("0");
	entry.capacity		= sfg::Label::Create("0/0");
	//entry.connectBtn	= sfg::Button::Create("Connect");

	entry.address->SetRequisition(sf::Vector2f(180.f, 0));
	entry.ping->SetRequisition(sf::Vector2f(50.f, 0));
	entry.capacity->SetRequisition(sf::Vector2f(75.f, 0));
	//entry.connectBtn->SetRequisition(sf::Vector2f(65.f, 0));

	entry.address->SetClass("not_selected_row");
	entry.ping->SetClass("not_selected_row");
	entry.capacity->SetClass("not_selected_row");

	entry.box->Pack(entry.address);
	entry.box->Pack(entry.capacity);
	entry.box->Pack(entry.ping);
	//entry.box->Pack(entry.connectBtn);
	
	m_scrolledWindowBox->Pack(entry.box);

	ServerInfo* info				= new ServerInfo();
	info->address					= address;
	info->capacity					= max;
	m_serverAddresses[info].first	= entry;
	m_serverAddresses[info].second	= 0.0f;

	entry.box->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&ServerInfo::Selected, info));
	//entry.connectBtn->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&ServerInfo::Connect, info));

	GUILayer::RefreshLayer(0);
}

bool LANMenu::IsAdded(const RakNet::SystemAddress& address) const
{
	for (const auto& entry : m_serverAddresses)
	{
		if (RakNet::SystemAddress::ToInteger(entry.first->address) == RakNet::SystemAddress::ToInteger(address))
		{
			return true;
		}
	}
	return false;
}

void LANMenu::UpdateServerInfo(const RakNet::SystemAddress& address, unsigned char nowPlayers, unsigned short ping, unsigned char maxPlayers)
{
	for (auto& entry : m_serverAddresses)
	{
		if (entry.first->address == address)
		{
			entry.first->nowPlaying = nowPlayers;
			entry.first->ping		= ping;
			
			if (maxPlayers > 0)
			{
				entry.first->capacity = maxPlayers;
			}

			std::stringstream ss;
			ss << (unsigned)entry.first->nowPlaying << "/" << (unsigned)entry.first->capacity;

			entry.second.first.capacity->SetText(ss.str().c_str());

			ss.clear();
			ss.str(std::string());
			ss << ping;

			entry.second.first.ping->SetText(ss.str().c_str());

			// reset counter
			entry.second.second = 0;

			GUILayer::RefreshLayer(0);
			break;
		}
	}
}

void LANMenu::RefreshLayer(float dt)
{
	if (!m_joinPressed)
	{
		ServerLabelMap::iterator iter = m_serverAddresses.begin();
		for (; iter != m_serverAddresses.end();)
		{
			float& counter = iter->second.second;
			counter += dt;

			if (counter > g_clientNetInfo.timeoutTime * 0.001)
			{
				m_scrolledWindowBox->Remove(iter->second.first.box);
				delete iter->first;
				iter = m_serverAddresses.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	RelativeGUILayer::RefreshLayer(dt);
}

void LANMenu::InitPools()
{
	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->
		AddResizeablePool<AddServerMsg, LANMenu>("add_server", 10, 10, this, &LANMenu::ProcessAddServerMsg);
	
	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->
		AddResizeablePool<UpdateServerMsg, LANMenu>("update_server", 10, 10, this, &LANMenu::ProcessUpdateServerMsg);

	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->
		AddResizeablePool<SetSmartLabelMsg, LANMenu>("set_smart_label", 10, 10, this, &LANMenu::ProcessSetSmartLabelMsg);
}

void LANMenu::SendAddMsg(const AddServerMsg& msg)
{
	MessagePoolManager* mgr = Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr();

	mgr->LockPool<AddServerMsg, LANMenu>("add_server");
	AddServerMsg* m = mgr->GetMessageByName<AddServerMsg, LANMenu>("add_server");

	if (m)
	{
		m->address			= msg.address;
		m->playerCapacity	= msg.playerCapacity;
	}

	mgr->UnlockPool<AddServerMsg, LANMenu>("add_server");
}

void LANMenu::SendUpdateMsg(const UpdateServerMsg& msg)
{
	MessagePoolManager* mgr = Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr();

	mgr->LockPool<UpdateServerMsg, LANMenu>("update_server");
	UpdateServerMsg* m = mgr->GetMessageByName<UpdateServerMsg, LANMenu>("update_server");

	if (m)
	{
		m->address			= msg.address;
		m->nowPlaying		= msg.nowPlaying;
		m->ping				= msg.ping;
		m->playerCapacity	= msg.playerCapacity;
	}

	mgr->UnlockPool<UpdateServerMsg, LANMenu>("update_server");
}

void LANMenu::SendSetSmartLabelMsg(const SetSmartLabelMsg& msg)
{
	MessagePoolManager* mgr = Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr();

	mgr->LockPool<SetSmartLabelMsg, LANMenu>("set_smart_label");
	SetSmartLabelMsg* m = mgr->GetMessageByName<SetSmartLabelMsg, LANMenu>("set_smart_label");

	if (m)
	{
		*m = msg;
	}

	mgr->UnlockPool<SetSmartLabelMsg, LANMenu>("set_smart_label");
}

void LANMenu::ProcessSetSmartLabelMsg(SetSmartLabelMsg* msgs, int amount)
{
	for (SetSmartLabelMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		m_smartMessage->SetText(ptr->text);

		switch (ptr->signal)
		{
		case SetSmartLabelMsg::SignalType::CONNECTION_FAILURE:
			m_joinPressed = false;
			break;

		default:
			break;
		}
	}
}

void LANMenu::ProcessAddServerMsg(AddServerMsg* msgs, int amount)
{
	for (AddServerMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		if (!IsAdded(ptr->address))
		{
			AddServerToTheList(ptr->address, ptr->playerCapacity);
		}
	}
}

void LANMenu::ProcessUpdateServerMsg(UpdateServerMsg* msgs, int amount)
{
	for (UpdateServerMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		if (IsAdded(ptr->address))
		{
			UpdateServerInfo(ptr->address, ptr->nowPlaying, ptr->ping, ptr->playerCapacity);
		}
		else
		{
			AddServerToTheList(ptr->address, ptr->playerCapacity);
		}
	}
}

void LANMenu::OnRowSelection(ServerInfo* info)
{
	if (!info)
	{
		return;
	}

	ServerLabelMap::const_iterator iter = m_serverAddresses.find(info);
	
	if (iter != m_serverAddresses.end())
	{
		for (auto& row : m_serverAddresses)
		{
			row.second.first.address->SetClass("not_selected_row");
			row.second.first.ping->SetClass("not_selected_row");
			row.second.first.capacity->SetClass("not_selected_row");
		}

		iter->second.first.address->SetClass("selected_row");
		iter->second.first.ping->SetClass("selected_row");
		iter->second.first.capacity->SetClass("selected_row");
	}

	m_connectionTarget = iter->first;
}