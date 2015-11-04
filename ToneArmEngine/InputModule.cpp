/*
========================

Copyright (c) 2014 Vinyl Games Studio

	InputModule.cpp

		Created by: Vladyslav Dolhopolov, Mikhail Kutuzov
		Created on: 5/30/2014 12:03:31 PM

========================
*/
#include "InputModule.h"
#include "NetworkMessageIDs.h"

#include <cmath>

using namespace vgs;

std::unordered_map<sf::Keyboard::Key, bool> InputModule::m_keyboardState;

InputModule::InputModule() {

	InitFromFile("");
}

InputModule::~InputModule()
{
}

InputModule::InputState::InputState()
	: m_changeThreshold(0.1f)
	, angle(0.f)
	, m_buttonStates(0)
{
}

bool InputModule::InputState::operator==(const InputState& other)
{
	if (this->angle != other.angle) {
		return false;
	}

	if (this->m_buttonStates.size() != other.m_buttonStates.size()) {
		return false;
	}

	for (auto btn : m_buttonStates) {

		if (btn.second != other.m_buttonStates.at(btn.first)) {
			return false;
		}
	}

	return true;
}

InputModule::InputState& InputModule::InputState::operator=(const InputState& other)
{
	for (auto btn : m_buttonStates) {

		btn.second = other.m_buttonStates.at(btn.first);
	}

	return *this;
}

void InputModule::InputState::Write(RakNet::BitStream& out)
{
	// movement (WASD?)
	if (m_buttonStates.at(ControlElement::Up) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}

	if (m_buttonStates.at(ControlElement::Down) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}

	if (m_buttonStates.at(ControlElement::Left) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}

	if (m_buttonStates.at(ControlElement::Right) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}

	// sprint
	if (m_buttonStates.at(ControlElement::Sprint) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}

	// mouse buttons
	if (m_buttonStates.at(ControlElement::Shoot) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}
	if (m_buttonStates.at(ControlElement::Aim) == BtnState::DOWN) {
		out.Write1();
	}
	else {
		out.Write0();
	}

	// angle character to mouse cursor
	out.WriteFloat16(angle, -360.f, 360.f);
}

void InputModule::InputState::Read(RakNet::BitStream& in)
{
	// movement (WASD?)
	if (in.ReadBit())
	{
		in.IgnoreBits(1);
		m_buttonStates[ControlElement::Up] = BtnState::DOWN;
	}
	else if (in.ReadBit())
	{
		m_buttonStates[ControlElement::Down] = BtnState::DOWN;
	}
	else
	{
		m_buttonStates[ControlElement::Up] = BtnState::UP;
		m_buttonStates[ControlElement::Down] = BtnState::UP;
	}

	if (in.ReadBit())
	{
		in.IgnoreBits(1);
		m_buttonStates[ControlElement::Left] = BtnState::DOWN;
	}
	else if (in.ReadBit())
	{
		m_buttonStates[ControlElement::Right] = BtnState::DOWN;
	}
	else
	{
		m_buttonStates[ControlElement::Left] = BtnState::UP;
		m_buttonStates[ControlElement::Right] = BtnState::UP;
	}

	// sprint
	if (in.ReadBit())
	{
		m_buttonStates[ControlElement::Sprint] = BtnState::DOWN;
	}
	else
	{
		m_buttonStates[ControlElement::Sprint] = BtnState::UP;
	}

	// mouse buttons
	m_buttonStates[ControlElement::Shoot] = in.ReadBit() ? BtnState::DOWN : BtnState::UP;
	m_buttonStates[ControlElement::Aim] = in.ReadBit() ? BtnState::DOWN : BtnState::UP;

	in.ReadFloat16(angle, -360.f, 360.f);
}

void InputModule::InputState::WriteDirection(glm::vec3& vec)
{
	vec.x = m_buttonStates[ControlElement::Left] == BtnState::DOWN ? -1.f	: m_buttonStates[ControlElement::Right] == BtnState::DOWN ? 1.f	: 0;
	vec.z = m_buttonStates[ControlElement::Down] == BtnState::DOWN ? 1.f	: m_buttonStates[ControlElement::Up] == BtnState::DOWN ? -1.f	: 0;
}

void InputModule::PackIntoBS(RakNet::BitStream& out)
{
	m_currentState.Write(out);
}

void InputModule::ReadFromBS(RakNet::BitStream& in)
{
	m_currentState.Read(in);
}

bool InputModule::IsDirty()
{
	bool res = !(m_currentState == m_prevState);
	m_prevState = m_currentState;
	return res;
}

//
// loads user-defined control scheme from a file
//
void InputModule::InitFromFile(const char* fileName) {

	// FIXME: Do actual loading
	m_keyMappings[ControlElement::Shoot]			= sf::Mouse::Button::Left;
	m_keyMappings[ControlElement::Aim]				= sf::Mouse::Button::Right;
	m_keyMappings[ControlElement::Up]				= sf::Keyboard::W;
	m_keyMappings[ControlElement::Down]				= sf::Keyboard::S;
	m_keyMappings[ControlElement::Right]			= sf::Keyboard::D;
	m_keyMappings[ControlElement::Left]				= sf::Keyboard::A;
	m_keyMappings[ControlElement::Reload]			= sf::Keyboard::R;
	m_keyMappings[ControlElement::Sprint]			= sf::Keyboard::LShift;
	m_keyMappings[ControlElement::PrimaryWeapon]	= sf::Keyboard::Num1;
	m_keyMappings[ControlElement::SecondaryWeapon]	= sf::Keyboard::Num2;
	m_keyMappings[ControlElement::ScoreMenu]		= sf::Keyboard::Tab;
	m_keyMappings[ControlElement::Pause]			= sf::Keyboard::Escape;
}

void InputModule::Startup(void)
{

}

void InputModule::Update(float dt)
{
	GetInput();
}

void InputModule::Shutdown(void)
{

}

//
// gets input from the user's input device and stores it as a current input state
//
void InputModule::GetInput() {

	// FIXME: make this work with other types of contollers (joystick?)
	for (auto element : m_keyMappings) {

		// FIXME: think of a way to make input universal (DEVICE_TYPE enum?)
		if (element.first == ControlElement::Shoot || element.first == ControlElement::Aim) {

			if (sf::Mouse::isButtonPressed((sf::Mouse::Button)(element.second))) {
				m_currentState.m_buttonStates[element.first] = BtnState::DOWN;
			}
			else {
				m_currentState.m_buttonStates[element.first] = BtnState::UP;
			}

			continue;
		}

		if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)element.second)) {
			m_currentState.m_buttonStates[element.first] = BtnState::DOWN;
		}
		else {
			m_currentState.m_buttonStates[element.first] = BtnState::UP;
		}
	}
}

bool InputModule::IsKeyPressed(ControlElement element) {

	return m_currentState.m_buttonStates[element] == BtnState::DOWN;
}

bool InputModule::KeyPressedDown( sf::Keyboard::Key keyEnum ) {
	std::unordered_map<sf::Keyboard::Key, bool>::iterator	key			= m_keyboardState.find( keyEnum );
	bool													keyPressed	= sf::Keyboard::isKeyPressed( keyEnum );

	if ( key == m_keyboardState.end() ) {
		m_keyboardState[keyEnum] = keyPressed;
	} else {
		bool isPressed = keyPressed;
		keyPressed  = sf::Keyboard::isKeyPressed( keyEnum ) && !key->second;
		key->second = isPressed;
	}

	return keyPressed;
}