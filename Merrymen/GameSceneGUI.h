/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	6/10/2014 3:32:11 PM
------------------------------------------------------------------------------------------
*/

#ifndef __GAME_SCENE_GUI_H__
#define __GAME_SCENE_GUI_H__

#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/GUILayer.h"

#include <SFML\System\Mutex.hpp>

#include <sstream>

namespace merrymen
{

	enum class KillingSpreeValue : unsigned char
	{
		DoubleKill = 2,
		TripleKill,
		QuadroKill,
		PentaKill,
		HexaKill,
	};

	struct UpdateLabelsMsg
	{
		UpdateLabelsMsg()
		{
		}

		UpdateLabelsMsg(unsigned short curAmmo, unsigned short totalAmmo, float hp, float st, float ar)
		{
			std::stringstream str;
			str << curAmmo << " / " << totalAmmo;
			this->ammo = str.str();

			str.str("");
			str << (unsigned)hp;
			this->hp = str.str();

			str.str("");
			str << (unsigned)st;
			this->stamina = str.str();

			str.str("");
			str << (unsigned)ar;
			this->armour = str.str();
		}

		std::string ammo;
		std::string hp;
		std::string stamina;
		std::string armour;
	};

	/*
	------------------------------------------------------------------------------------------
		GameSceneGUI

		Class for control over GUI elements on the main Game screen. 
	------------------------------------------------------------------------------------------
	*/
	class GameSceneGUI : public vgs::GUILayer {

		typedef std::shared_ptr<sf::Font> Font;

	public:
								GameSceneGUI();
		virtual					~GameSceneGUI();

		virtual vgs::SFGWinPtr	CreateElements(vgs::WidgetMap& widgetMap);

		void					SetUpdateLabelsMsg(const UpdateLabelsMsg& msg);
		void					SetNewsLabel(const char* text);
		void					SetKillingSpreeLabel(const char* name, KillingSpreeValue count);

	protected:
		virtual void			RefreshLayer(float dt)						override;

		virtual void			ConnectSignals()	{}
		virtual void			InitPools()			{}

	private:
		void					SetAmmoValue(const char* value)				{ m_ammoLabel->SetText(value); }
		void					SetHPValue(const char* value)				{ m_hpLabel->SetText(value); }
		void					SetStaminaValue(const char* value)			{ m_staminaLabel->SetText(value); }
		void					SetArmourValue(const char* value)			{ m_armourLabel->SetText(value); }

	private:
		sfg::Label::Ptr			m_ammoLabel;
		sfg::Label::Ptr			m_hpLabel;
		sfg::Label::Ptr			m_staminaLabel;
		sfg::Label::Ptr			m_armourLabel;

		sfg::Label::Ptr			m_newsLabel;
		std::string				m_newsText;
		bool					m_newsTextChanged;

		sfg::Label::Ptr			m_killingSpreeLabel;
		std::string				m_killingSpreeText;
		bool					m_killingSpreeValChanged;
	
		float					m_newsShowtime;
		float					m_newsTimer;
		bool					m_fadingOut;

		float					m_killingSpreeShowtime;
		float					m_killingSpreeTimer;
		bool					m_killingSpreeFadingOut;

		float					m_statBoxHeight;
		float					m_titleLblWidth;
		float					m_valueLblWidth;

		sfg::Fixed::Ptr			m_aligner;
		unsigned int			m_prevWidth;
		unsigned int			m_prevHeight;

		UpdateLabelsMsg			m_valuesToUpdate;
		sf::Mutex				m_updateMutex;

		bool					m_dirty;

		Font					m_numbersFont;
		Font					m_textFont;
	};

} // namespace merrymen

#endif __GAME_SCENE_GUI_H__