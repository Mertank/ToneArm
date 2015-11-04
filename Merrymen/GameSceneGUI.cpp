/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	6/10/2014 3:32:11 PM
------------------------------------------------------------------------------------------
*/

#include "GameSceneGUI.h"
#include "../ToneArmEngine/OpenGLGUIPass.h"
#include "../ToneArmEngine/RunningInfo.h"

using namespace merrymen;
using namespace vgs;

GameSceneGUI::GameSceneGUI()
	: m_dirty(false)
	, m_newsTextChanged(false)
	, m_newsText("")
	, m_numbersFont(std::make_shared<sf::Font>())
	, m_textFont(std::make_shared<sf::Font>())
	, m_newsShowtime(7.f)
	, m_newsTimer(0)
	, m_fadingOut(false)
	, m_killingSpreeText("")
	, m_killingSpreeShowtime(5.f)
	, m_killingSpreeTimer(0)
	, m_killingSpreeFadingOut(false)
{
}

//
// destructor
//
GameSceneGUI::~GameSceneGUI() {

	// deallocate memory used by the widget map
	m_widgetMap->erase("ammo_lbl");
	m_widgetMap->erase("hp_lbl");
	m_widgetMap->erase("stamina_lbl");
	m_widgetMap->erase("armour_lbl");
}

//
// create neccessary gui elements
//
vgs::SFGWinPtr GameSceneGUI::CreateElements(vgs::WidgetMap& widgetMap) {

	m_numbersFont->loadFromFile("Data/college_halo.ttf");
	m_deskPtr->GetEngine().GetResourceManager().AddFont("stat_numbers_font", m_numbersFont);

	m_textFont->loadFromFile("Data/blistered.ttf");
	m_deskPtr->GetEngine().GetResourceManager().AddFont("stat_text_font", m_textFont);

	RunningInfo* args = Engine::GetInstance()->GetRunningInfo();

	m_widgetMap = &widgetMap;
	
	// create labels and add them to the widget map with the specified names (id's?)
	m_window = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
	m_window->SetState(sfg::Widget::State::INSENSITIVE);
	m_window->SetId("gs_win");

	m_statBoxHeight = 40.f;
	m_titleLblWidth = 65.f;
	m_valueLblWidth = 100.f;

	auto ammoPairBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);
	auto hpPairBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);
	hpPairBox->SetRequisition(sf::Vector2f(0, m_statBoxHeight));
	auto stPairBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);
	stPairBox->SetRequisition(sf::Vector2f(0, m_statBoxHeight));
	auto arPairBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.0f);
	arPairBox->SetRequisition(sf::Vector2f(0, m_statBoxHeight));

	auto ammoTitle = sfg::Label::Create("ammo");
	ammoTitle->SetRequisition(sf::Vector2f(50.f, .0f));
	ammoTitle->SetClass("gamescene_stat_name");
	ammoTitle->SetAlignment(sf::Vector2f(0, 1.f));
	auto hpTitle = sfg::Label::Create("health");
	hpTitle->SetClass("gamescene_stat_name");
	hpTitle->SetRequisition(sf::Vector2f(m_titleLblWidth, .0f));
	hpTitle->SetAlignment(sf::Vector2f(0, 1.f));
	auto stTitle = sfg::Label::Create("stamina");
	stTitle->SetClass("gamescene_stat_name");
	stTitle->SetRequisition(sf::Vector2f(m_titleLblWidth, .0f));
	stTitle->SetAlignment(sf::Vector2f(0, 1.f));
	auto arTitle = sfg::Label::Create("armour");
	arTitle->SetClass("gamescene_stat_name");
	arTitle->SetRequisition(sf::Vector2f(m_titleLblWidth, .0f));
	arTitle->SetAlignment(sf::Vector2f(0, 1.f));

	m_ammoLabel = sfg::Label::Create("");
	m_ammoLabel->SetAlignment(sf::Vector2f(0.f, .5f));
	m_ammoLabel->SetClass("gamescene_stat_value");
	widgetMap["ammo_lbl"] = std::shared_ptr<sfg::Label>(m_ammoLabel);
	ammoPairBox->Pack(ammoTitle);
	ammoPairBox->Pack(m_ammoLabel);

	m_hpLabel = sfg::Label::Create("");
	m_hpLabel->SetAlignment(sf::Vector2f(0.f, .0f));
	m_hpLabel->SetClass("gamescene_stat_value");
	widgetMap["hp_lbl"] = std::shared_ptr<sfg::Label>(m_hpLabel);
	hpPairBox->Pack(hpTitle);
	hpPairBox->Pack(m_hpLabel);

	m_staminaLabel = sfg::Label::Create("");
	m_staminaLabel->SetAlignment(sf::Vector2f(0.f, .0f));
	m_staminaLabel->SetClass("gamescene_stat_value");
	widgetMap["stamina_lbl"] = std::shared_ptr<sfg::Label>(m_staminaLabel);
	stPairBox->Pack(stTitle);
	stPairBox->Pack(m_staminaLabel);

	m_armourLabel = sfg::Label::Create("");
	m_armourLabel->SetAlignment(sf::Vector2f(0.f, .0f));
	m_armourLabel->SetClass("gamescene_stat_value");
	widgetMap["armour_lbl"] = std::shared_ptr<sfg::Label>(m_armourLabel);
	arPairBox->Pack(arTitle);
	arPairBox->Pack(m_armourLabel);

	// pack the labels into the box container
	auto ammoBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 0.0f);
	auto hpBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 0.0f);
	
	ammoBox->Pack(ammoPairBox);
	hpBox->Pack(hpPairBox);
	hpBox->Pack(stPairBox);
	hpBox->Pack(arPairBox);

	m_newsLabel = sfg::Label::Create(m_newsText.c_str());
	m_newsLabel->SetClass("gamescene_chat");

	m_killingSpreeLabel = sfg::Label::Create(m_killingSpreeText.c_str());
	m_killingSpreeLabel->SetId("gamescene_killing_spree_msg");

	m_aligner = sfg::Fixed::Create();

	m_aligner->Put(ammoBox, sf::Vector2f(35.f, args->contextHeight - m_statBoxHeight - 75.f));
	m_aligner->Put(hpBox, sf::Vector2f(args->contextWidth - 220.f, args->contextHeight - m_statBoxHeight * 3 - 75.f));
	m_aligner->Put(m_newsLabel, sf::Vector2f(args->contextWidth / 2.f - 20.f, args->contextHeight - 250.f));
	m_aligner->Put(m_killingSpreeLabel, sf::Vector2f(args->contextWidth / 2.f - 50.f, 150.f));

	m_prevWidth = args->contextWidth;
	m_prevHeight = args->contextHeight;

	// add the box to the window
	m_window->Add(m_aligner);

	//m_deskPtr->SetProperty("Label.gamescene_stat_name", "FontSize", 16.f);
	//m_deskPtr->SetProperty("Label.gamescene_chat", "FontSize", 16.f);
	////m_deskPtr->SetProperty("Label.gamescene_stat_value", "FontSize", 36.f);
	////m_deskPtr->SetProperty("Label.gamescene_stat_value", "Color", sf::Color(200, 150, 25, 255));
	////m_deskPtr->SetProperty("Label.gamescene_stat_value", "FontName", sf::Color(200, 150, 25, 255));

	m_deskPtr->SetProperties(
		"Label.gamescene_stat_value"
		"{"
		"	FontName: stat_numbers_font;"
		"	FontSize: 52;"
		"	Color: #fefefeff;"
		//"	Color: #C89619ff;"
		"}"
	);

	m_deskPtr->SetProperties(
		"Label.gamescene_stat_name"
		"{"
		"	FontName: stat_text_font;"
		"	FontSize: 24;"
		"	Color: #fefefeff;"
		"}"
	);

	m_deskPtr->SetProperties(
		"Label.gamescene_chat"
		"{"
		"	FontName: stat_text_font;"
		"	FontSize: 30;"
		"	Color: #fefefeff;"
		"}"
	);

	m_deskPtr->SetProperties(
		"Label#gamescene_killing_spree_msg"
		"{"
		"	FontName: stat_text_font;"
		"	FontSize: 36;"
		"	Color: #fefefeff;"
		"}"
	);

	InitPools();
	
	return m_window;
}

void GameSceneGUI::SetUpdateLabelsMsg(const UpdateLabelsMsg& msg)
{
	m_updateMutex.lock();
	m_valuesToUpdate	= msg;
	m_dirty				= true;
	m_updateMutex.unlock();
}

void GameSceneGUI::SetNewsLabel(const char* text)
{
	m_updateMutex.lock();
	m_newsText			= text;
	m_newsTextChanged	= true;
	m_updateMutex.unlock();
}

void GameSceneGUI::RefreshLayer(float dt)
{
	bool needsRefresh = false;

	m_updateMutex.lock();

	if (m_dirty)
	{
		SetAmmoValue(m_valuesToUpdate.ammo.c_str());
		SetHPValue(m_valuesToUpdate.hp.c_str());
		SetStaminaValue(m_valuesToUpdate.stamina.c_str());
		SetArmourValue(m_valuesToUpdate.armour.c_str());

		m_dirty = false;
		needsRefresh = true;
	}

	if (m_newsTextChanged)
	{
		m_newsLabel->SetText(m_newsText.c_str());
		m_newsTextChanged = false;
		needsRefresh = true;
		
		m_newsTimer = 0;
		m_fadingOut = true;
		m_deskPtr->SetProperty("Label.gamescene_chat", "Color", sf::Color(254, 254, 254, 255));
	}

	if (m_killingSpreeValChanged)
	{
		m_killingSpreeLabel->SetText(m_killingSpreeText.c_str());
		m_killingSpreeValChanged = false;
		needsRefresh = true;
		
		m_killingSpreeTimer = 0;
		m_killingSpreeFadingOut = true;
		m_deskPtr->SetProperty("Label#gamescene_killing_spree_msg", "Color", sf::Color(254, 254, 254, 255));
	}

	m_updateMutex.unlock();

	m_newsTimer += dt;
	m_killingSpreeTimer += dt;

	if (m_fadingOut)
	{
		if (m_newsTimer > m_newsShowtime)
		{
			m_fadingOut = false;
			m_newsTimer = 0;
			m_deskPtr->SetProperty("Label.gamescene_chat", "Color", sf::Color(254, 254, 254, 0));
			needsRefresh = true;
		}
	}

	if (m_killingSpreeFadingOut)
	{
		if (m_killingSpreeTimer > m_killingSpreeShowtime)
		{
			m_killingSpreeFadingOut = false;
			m_killingSpreeTimer = 0;
			m_deskPtr->SetProperty("Label#gamescene_killing_spree_msg", "Color", sf::Color(254, 254, 254, 0));
			needsRefresh = true;
		}
	}

	RunningInfo* args = Engine::GetInstance()->GetRunningInfo();

	if (m_prevWidth != args->contextWidth || m_prevHeight != args->contextHeight)
	{
		m_prevWidth = args->contextWidth;
		m_prevHeight = args->contextHeight;

		m_aligner->GetChildren()[0]->SetPosition(sf::Vector2f(35.f, args->contextHeight - m_statBoxHeight - 75.f));
		m_aligner->GetChildren()[1]->SetPosition(sf::Vector2f(args->contextWidth - 220.f, args->contextHeight - m_statBoxHeight * 3 - 75.f));
		m_aligner->GetChildren()[2]->SetPosition(sf::Vector2f(args->contextWidth / 2.f - 20.f, args->contextHeight - 250.f));
		m_aligner->GetChildren()[3]->SetPosition(sf::Vector2f(args->contextWidth / 2.f - 50.f, 150.f));

		needsRefresh = true;
	}

	if (needsRefresh)
	{
		GUILayer::RefreshLayer(dt);
	}
}

void GameSceneGUI::SetKillingSpreeLabel(const char* name, KillingSpreeValue count)
{
	if ((char)count < 2)
	{
		return;
	}
	
	std::stringstream ss;

	switch (count)
	{
	case merrymen::KillingSpreeValue::DoubleKill:
		ss << name << " is running amok";
		break;

	case merrymen::KillingSpreeValue::TripleKill:
		ss << name << " is wreaking havok";
		break;

	case merrymen::KillingSpreeValue::QuadroKill:
		ss << name << " is running rampant";
		break;

	case merrymen::KillingSpreeValue::PentaKill:
		ss << name << " is going postal";
		break;

	case merrymen::KillingSpreeValue::HexaKill:
	default:
		ss << name << " is commiting KILLTROCITY!";
		break;
	}

	m_updateMutex.lock();
	m_killingSpreeText = ss.str().c_str();
	m_killingSpreeValChanged = true;
	m_updateMutex.unlock();
}