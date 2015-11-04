/*
========================

Copyright (c) 2014 Vinyl Games Studio

	InputModule.h

		Created by: Vladyslav Dolhopolov, Mikhail Kutuzov
		Created on: 5/30/2014 12:03:31 PM

========================
*/
#ifndef __INPUTMODULE_H__
#define __INPUTMODULE_H__

#include "EngineModule.h"

#include <SFML/Window.hpp>
#include <BitStream.h>
#include <glm\glm.hpp>
#include <unordered_map>

namespace vgs
{
	/*
	========================

		InputModule

			Capable of storing current input state.
			Forms and reads the bitstream of input packet.

			Created by: Vladyslav Dolhopolov
			Created on: 5/30/2014 12:03:31 PM

	========================
	*/
	class InputModule : public EngineModule
	{

	public:
		enum class ControlElement : unsigned char {
			Up,
			Down,
			Right,
			Left,
			Shoot,
			Aim,
			Reload,
			Sprint,
			PrimaryWeapon,
			SecondaryWeapon,
			Button11,
			Button12,
			Button13,
			Button14,
			Button15,
			Button16,
			Button17,
			ScoreMenu,
			Pause,
		};

		enum class BtnState : unsigned char
		{
			UP,
			DOWN,
		};

		struct InputState
		{
																	InputState();
			void 													Write(RakNet::BitStream& out);
			void													Read(RakNet::BitStream& in);

			void													WriteDirection(glm::vec3& vec);

			std::unordered_map<ControlElement, BtnState>			m_buttonStates;
			float													angle;

			bool													operator==(const InputState& other);
			InputState&												operator=(const InputState& other);


		private:
			const float												m_changeThreshold;
		};

																	InputModule();
																	~InputModule();
		static InputModule*											Create()								{ return new InputModule(); }

		virtual void												Startup(void)							override;
		virtual void												Update(float dt)						override;
		virtual void												Shutdown(void)							override;

		void														InitFromFile(const char* fileName);
		void														GetInput();
		bool														IsKeyPressed(ControlElement element);

		InputState&													GetState()								{ return m_currentState; }
		bool														IsDirty();

		void 														PackIntoBS(RakNet::BitStream& out);
		void														ReadFromBS(RakNet::BitStream& in);

		static bool													KeyPressedDown( sf::Keyboard::Key keyEnum );
	private:

		std::unordered_map<ControlElement, unsigned char>			m_keyMappings;

		InputState													m_currentState;
		InputState													m_prevState;

		static std::unordered_map<sf::Keyboard::Key, bool>			m_keyboardState;
	};

} // namespace vgs

#endif __INPUTMODULE_H__