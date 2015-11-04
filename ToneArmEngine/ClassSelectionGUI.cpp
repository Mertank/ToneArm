/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ClassSelectionGUI.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 6/19/2014 3:21:22 PM

========================
*/
#include "ClassSelectionGUI.h"
#include "Engine.h"
#include "RunningInfo.h"
#include "RenderModule.h"
#include "GUIManager.h"
#include "OpenGLGUIPass.h"

using namespace merrymen;
using namespace vgs;

ClassSelectionGUI::ClassSelectionGUI(const std::function<void()>& func)
	: m_spinStarted(false)
	, m_callback(func)
	, m_donePressed(false)
{
}

vgs::SFGWinPtr ClassSelectionGUI::CreateElements(vgs::WidgetMap& widgetMap)
{
	m_window = sfg::Window::Create(sfg::Window::Style::BACKGROUND & sfg::Window::Style::TOPLEVEL);
	m_window->SetId("class_select_window");

	m_window->SetRequisition(sf::Vector2f(200.f, 0.f));

	sfg::Box::Ptr winContainer = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 1.0f);

	auto nameLbl = sfg::Label::Create("Your name");
	nameLbl->SetClass("title_label");
	auto teamLbl = sfg::Label::Create("Your team");
	teamLbl->SetClass("title_label");
	auto classLbl = sfg::Label::Create("Your class");
	classLbl->SetClass("title_label");
	m_nameEntry = sfg::Entry::Create("player");
	m_doneBtn = sfg::Button::Create("Continue");

	auto radioBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	auto radioBox2 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);

	m_team1Radio = sfg::RadioButton::Create("Team #1");
	//m_team1Radio->SetActive(true);
	m_team1Radio->SetState(sfg::Widget::State::INSENSITIVE);
	m_team2Radio = sfg::RadioButton::Create("Team #2", m_team1Radio->GetGroup());
	m_team2Radio->SetState(sfg::Widget::State::INSENSITIVE);

	m_class1Radio = sfg::RadioButton::Create("Tank");
	m_class2Radio = sfg::RadioButton::Create("Sniper", m_class1Radio->GetGroup());

	m_class1Radio->SetActive(true);

	radioBox->Pack( m_team1Radio );
	radioBox->Pack( m_team2Radio );

	radioBox2->Pack(m_class1Radio);
	radioBox2->Pack(m_class2Radio);

	auto sep1 = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
	auto sep2 = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
	auto sep3 = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);

	m_spinnerBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.f);
	
	m_spinner = sfg::Spinner::Create();
	m_spinner->SetRequisition( sf::Vector2f( 40.f, 40.f ) );
	
	auto connectLbl = sfg::Label::Create("A minute of silence...");

	m_spinnerBox->Pack(sep3);
	m_spinnerBox->Pack(connectLbl);
	//m_spinnerBox->Pack(m_spinner);
	m_spinnerBox->Show(false);

	winContainer->Pack(nameLbl);
	winContainer->Pack(m_nameEntry);
	winContainer->Pack(sep1);
	//winContainer->Pack(teamLbl);
	//winContainer->Pack(radioBox);
	//winContainer->Pack(sep2);
	winContainer->Pack(classLbl);
	winContainer->Pack(radioBox2);
	winContainer->Pack(m_doneBtn);
	winContainer->Pack(m_spinnerBox);

	m_window->Add(winContainer);

	m_deskPtr->SetProperty("Window#class_select_window > Box > Label.title_label", "FontSize", 14.f);
	m_deskPtr->SetProperty("Window#class_select_window > Box > Label.title_label", "Color", sf::Color(110, 110, 255, 255));

	InitPools();
	m_doneBtn->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&ClassSelectionGUI::OnDonePressed, this));

	return m_window;
}

std::string ClassSelectionGUI::GetName() const
{
	if (m_nameEntry->GetText().getSize() <= 30) {
		return m_nameEntry->GetText().toAnsiString();
	}
	else {
		char name[31];
		memcpy(name, &m_nameEntry->GetText().toAnsiString()[0], 30 * sizeof(char));
		name[30] = '\0';
		return std::string(name);
	}
}

const char ClassSelectionGUI::GetCharacterClass() const
{
	if (m_class1Radio->IsActive()) {
		return (char)CharacterClass::TANK;
	}
	else {
		return (char)CharacterClass::SNIPER;
	}
}

const char ClassSelectionGUI::GetTeam() const
{
	return m_team1Radio->IsActive() ? (char)TeamTag::TEAM_A : (char)TeamTag::TEAM_B;
}

void ClassSelectionGUI::StartSpinner()
{
	m_spinnerBox->Show();
	//m_spinner->Start();

	//m_spinStarted = true;

	//GUILayer::RefreshLayer(0);
}

void ClassSelectionGUI::StopSpinner()
{
	m_spinnerBox->Show(false);
	m_spinner->Stop();
}

void ClassSelectionGUI::OnDonePressed()
{
	if (m_donePressed)
	{
		return;
	}

	m_donePressed = true;
	m_callback();
}

void ClassSelectionGUI::RefreshLayer(float dt)
{
	RelativeGUILayer::RefreshLayer(dt);
}