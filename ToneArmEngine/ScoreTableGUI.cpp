/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ScoreTableGUI.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 6/17/2014 1:07:58 PM

========================
*/
#include "ScoreTableGUI.h"
#include "Engine.h"
#include "RunningInfo.h"
#include "NetworkNode.h"
#include "OpenGLGUIPass.h"
#include "MessagePoolManager.h"
#include "RenderModule.h"
#include "GUIManager.h"
#include "InputModule.h"
#include "OpenGLRenderModule.h"

#include <sstream>

using namespace merrymen;
using namespace vgs;

ScoreTableGUI::ScoreTableGUI()
	: m_enabled(false)
	, m_postgame(false)
{

}

ScoreTableGUI::~ScoreTableGUI()
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->GetMsgPoolMgr()->RemovePool("add_score_row");
		guiMgr->GetMsgPoolMgr()->RemovePool("remove_score_row");
		guiMgr->GetMsgPoolMgr()->RemovePool("update_score_row");
	}
}

vgs::SFGWinPtr ScoreTableGUI::CreateElements(vgs::WidgetMap& widgetMap)
{
	m_window = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
	m_window->SetId("score_window");

	SetRelativePosition(sf::Vector2f(.5f, .4f));
	m_window->SetRequisition(sf::Vector2f(700.f, 0.f));

	sfg::Box::Ptr winContainer = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);

	sfg::Box::Ptr titleBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);

	auto l1 = sfg::Label::Create( "Player" );
	l1->SetRequisition(sf::Vector2f(400, 0.f));
	l1->SetAlignment(sf::Vector2f(0.f, 0.5f));
	l1->SetClass("title_label");
	
	auto l2 = sfg::Label::Create( "Kills" );
	l2->SetRequisition(sf::Vector2f(100, 0.f));
	l2->SetAlignment(sf::Vector2f(0.f, 0.5f));
	l2->SetClass("title_label");
	
	auto l3 = sfg::Label::Create( "Deaths" );
	l3->SetRequisition(sf::Vector2f(100, 0.f));
	l3->SetAlignment(sf::Vector2f(0.f, 0.5f));
	l3->SetClass("title_label");
	
	auto l4 = sfg::Label::Create( "Ping" );
	l4->SetRequisition(sf::Vector2f(100, 0.f));
	l4->SetAlignment(sf::Vector2f(0.f, 0.5f));
	l4->SetClass("title_label");

	titleBox->Pack(l1);
	titleBox->Pack(l2);
	titleBox->Pack(l3);
	titleBox->Pack(l4);

	auto separator = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
	auto separator2 = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);

	m_team1Box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, .0f);
	auto team1Title = sfg::Label::Create("Team #1");
	team1Title->SetAlignment(sf::Vector2f(0.f, 0.5f));
	team1Title->SetId("team1_title");

	m_team2Box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, .0f);
	auto team2Title = sfg::Label::Create("Team #2");
	team2Title->SetAlignment(sf::Vector2f(0.f, 0.5f));
	team2Title->SetId("team2_title");

	winContainer->Pack(titleBox);
	winContainer->Pack(separator);
	winContainer->Pack(team1Title);
	winContainer->Pack(m_team1Box);
	winContainer->Pack(separator2);
	winContainer->Pack(team2Title);
	winContainer->Pack(m_team2Box);

	m_window->Add(winContainer);

	m_deskPtr->SetProperty("Window#score_window > Box > Box > Label.title_label", "Color", sf::Color(110, 110, 255, 255));

	m_deskPtr->SetProperty("Window#score_window > Box > Label#team1_title", "Color", sf::Color(110, 110, 255, 255));
	m_deskPtr->SetProperty("Window#score_window > Box > Label#team2_title", "Color", sf::Color(255, 110, 110, 255));

	m_deskPtr->SetProperty("Window#score_window > Box > Label", "FontSize", 14.f);
	m_deskPtr->SetProperty("Window#score_window > Box > Box > Label", "FontSize", 18.f);
	m_deskPtr->SetProperty("Window#score_window > Box > Box > Box > Label", "FontSize", 14.f);

	m_deskPtr->SetProperty("Window#score_window", "BackgroundColor", sf::Color(0, 0, 0, 127));

	InitPools();
	SetVisible(false);

	return m_window;
}

void ScoreTableGUI::RefreshLayer(float dt)
{
	m_postMutex.lock();
	bool checkInput = !m_postgame;
	m_postMutex.unlock();

	if (checkInput)
	{
		SetVisible(sf::Keyboard::isKeyPressed(sf::Keyboard::Tab));
	}

	RelativeGUILayer::RefreshLayer(dt);
}

void ScoreTableGUI::AddRow(unsigned char kills, unsigned char deaths, char team, std::string name, unsigned char uid, bool local)
{
	Row rowEntry;
	std::stringstream ss;
	
	rowEntry.box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);

	ss << name.c_str() << (local ? " (you)" : "");

	rowEntry.name = sfg::Label::Create(ss.str().c_str());
	rowEntry.name->SetRequisition(sf::Vector2f(400, 0.f));
	rowEntry.name->SetAlignment(sf::Vector2f(0.f, 0.5f));
	
	ss.clear();
	ss.str(std::string());
	ss << (unsigned)kills;
	rowEntry.kills = sfg::Label::Create(ss.str().c_str());
	rowEntry.kills->SetRequisition(sf::Vector2f(100, 0.f));
	rowEntry.kills->SetAlignment(sf::Vector2f(0.f, 0.5f));
	
	ss.clear();
	ss.str(std::string());
	ss << (unsigned)deaths;
	rowEntry.deaths = sfg::Label::Create(ss.str().c_str());
	rowEntry.deaths->SetRequisition(sf::Vector2f(100, 0.f));
	rowEntry.deaths->SetAlignment(sf::Vector2f(0.f, 0.5f));
	
	rowEntry.ping = sfg::Label::Create("unnoticeable");
	rowEntry.ping->SetRequisition(sf::Vector2f(100, 0.f));
	rowEntry.ping->SetAlignment(sf::Vector2f(0.f, 0.5f));

	rowEntry.box->Pack(rowEntry.name);
	rowEntry.box->Pack(rowEntry.kills);
	rowEntry.box->Pack(rowEntry.deaths);
	rowEntry.box->Pack(rowEntry.ping);

	if (team == (char)TeamTag::TEAM_A)
	{
		m_team1Box->PackEnd(rowEntry.box);
	}
	else if (team == (char)TeamTag::TEAM_B)
	{
		m_team2Box->PackEnd(rowEntry.box);
	}

	m_table[uid] = rowEntry;

	RefreshLayer(0);
}

void ScoreTableGUI::RemoveRow(unsigned char uid, char team)
{
	for (auto row : m_table)
	{
		if (row.first == uid)
		{
			/* LEGENDARY */
			row.second.box->Remove(row.second.name);
			row.second.name->Show(false);
			row.second.box->Remove(row.second.kills);
			row.second.kills->Show(false);
			row.second.box->Remove(row.second.deaths);
			row.second.deaths->Show(false);
			row.second.box->Remove(row.second.ping);
			row.second.ping->Show(false);
			row.second.box->RemoveAll();
			row.second.box->Show(false);
			if (team == (char)TeamTag::TEAM_A)
			{
				m_team1Box->Remove(row.second.box);
			}
			else if (team == (char)TeamTag::TEAM_B)
			{
				m_team2Box->Remove(row.second.box);
			}

			break;
		}
	}

	RefreshLayer(0);
}

void ScoreTableGUI::UpdateRow(unsigned char uid, unsigned char kills, unsigned char deaths)
{
	for (auto row : m_table)
	{
		if (row.first == uid)
		{
			std::stringstream ss;
	
			ss << (unsigned)kills;
			row.second.kills->SetText(ss.str().c_str());
			
			ss.clear();
			ss.str(std::string());
			ss << (unsigned)deaths;
			row.second.deaths->SetText(ss.str().c_str());
			
			//row.second.ping->SetText("unnoticeable");
			break;
		}
	}

	RefreshLayer(0);
}

void ScoreTableGUI::InitPools()
{
	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->
		AddResizeablePool<AddRowMsg, ScoreTableGUI>("add_score_row", 10, 10, this, &ScoreTableGUI::ProcessAddRowMsg);
	
	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->
		AddResizeablePool<RemoveRowMsg, ScoreTableGUI>("remove_score_row", 10, 10, this, &ScoreTableGUI::ProcessRemoveRowMsg);

	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->
		AddResizeablePool<UpdateRowMsg, ScoreTableGUI>("update_score_row", 10, 10, this, &ScoreTableGUI::ProcessUpdateRowMsg);
}

void ScoreTableGUI::SendAddMsg(const AddRowMsg& msg)
{
	MessagePoolManager* mgr = Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr();

	mgr->LockPool<AddRowMsg, ScoreTableGUI>("add_score_row");
	AddRowMsg* m = mgr->GetMessageByName<AddRowMsg, ScoreTableGUI>("add_score_row");

	if (m)
	{
		*m = msg;
	}

	mgr->UnlockPool<AddRowMsg, ScoreTableGUI>("add_score_row");
}
	
void ScoreTableGUI::SendRemoveMsg(const RemoveRowMsg& msg)
{
	MessagePoolManager* mgr = Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr();

	mgr->LockPool<RemoveRowMsg, ScoreTableGUI>("remove_score_row");
	RemoveRowMsg* m = mgr->GetMessageByName<RemoveRowMsg, ScoreTableGUI>("remove_score_row");

	if (m)
	{
		*m = msg;
	}

	mgr->UnlockPool<RemoveRowMsg, ScoreTableGUI>("remove_score_row");
}

void ScoreTableGUI::SendUpdateMsg(const UpdateRowMsg& msg)
{
	MessagePoolManager* mgr = Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr();

	mgr->LockPool<UpdateRowMsg, ScoreTableGUI>("update_score_row");
	UpdateRowMsg* m = mgr->GetMessageByName<UpdateRowMsg, ScoreTableGUI>("update_score_row");

	if (m)
	{
		*m = msg;
	}

	mgr->UnlockPool<UpdateRowMsg, ScoreTableGUI>("update_score_row");
}

void ScoreTableGUI::ProcessAddRowMsg(AddRowMsg* msgs, int amount)
{
	for (AddRowMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		AddRow(ptr->kills, ptr->deaths, ptr->team, ptr->name, ptr->uid, ptr->local);
	}
}

void ScoreTableGUI::ProcessRemoveRowMsg(RemoveRowMsg* msgs, int amount)
{
	for (RemoveRowMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		RemoveRow(ptr->uid, ptr->team);
	}
}

void ScoreTableGUI::ProcessUpdateRowMsg(UpdateRowMsg* msgs, int amount)
{
	for (UpdateRowMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		UpdateRow(ptr->uid, ptr->kills, ptr->deaths);
	}
}

