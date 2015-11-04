/*
========================

Copyright (c) 2014 Vinyl Games Studio

	LANMenu.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/23/2014

========================
*/
#ifndef __LANMENU_H__
#define __LANMENU_H__

#include "MessagePoolManager.h"
#include "RelativeGUILayer.h"
#include "ServerInfo.h"
#include "OpenGLGUIPass.h"

#include <unordered_map>

#include <RakNetTypes.h>

//#define INIT_POOLS_BEGIN \
//	virtual void InitPools() {
//
//#define CREATE_POOL(__class__, __msgType__, __poolName__, __size__)																					\
//	Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager()->GetMsgPoolMgr()->	\
//	AddResizeablePool<__msgType__, __class__>(__poolName__, __size__, 1, this, &__class__::Process##__msgType__##);									\
//
//#define INIT_POOLS_END \
//	}

namespace vgs
{
	/*
	========================

		AddServerMsg

			Message for adding entries to the table.

			Created by: Vladyslav Dolhopolov
			Created on: 5/23/2014 5:32:51 PM

	========================
	*/
	struct AddServerMsg
	{
		AddServerMsg()
		{
		}

		AddServerMsg(const RakNet::SystemAddress& a, unsigned char c)
			: address(a)
			, playerCapacity(c)
		{
		}

		RakNet::SystemAddress	address;
		unsigned char			playerCapacity;
	};

	/*
	========================

		UpdateServerMsg

			Message for updating entries in the table.

			Created by: Vladyslav Dolhopolov
			Created on: 5/23/2014 5:32:51 PM

	========================
	*/
	struct UpdateServerMsg
	{
		UpdateServerMsg()
		{
		}

		UpdateServerMsg(const RakNet::SystemAddress& a, unsigned char c, unsigned char n, unsigned short p)
			: address(a)
			, playerCapacity(c)
			, nowPlaying(n)
			, ping(p)
		{
		}

		RakNet::SystemAddress	address;
		unsigned char			playerCapacity;
		unsigned char			nowPlaying;
		unsigned short			ping;
	};

	struct SetSmartLabelMsg
	{
		enum SignalType
		{
			NONE,
			CONNECTION_FAILURE,
		};

		SetSmartLabelMsg()
		{
		}

		SetSmartLabelMsg(const std::string& t)
			: text(t)
			, signal(NONE)
		{
		}

		SetSmartLabelMsg(const std::string& t, SignalType sig)
			: text(t)
			, signal(sig)
		{
		}

		std::string text;
		SignalType	signal;
	};

	/*
	========================

		LANMenu

			Table widget for storing available server addresses.

			Created by: Vladyslav Dolhopolov
			Created on: 5/23/2014 5:32:51 PM

	========================
	*/
	class LANMenu : public RelativeGUILayer
	{
		struct EntryLine
		{
			sfg::Box::Ptr		box;
			sfg::Label::Ptr		address;
			sfg::Label::Ptr		capacity;
			sfg::Label::Ptr		ping;
			sfg::Button::Ptr	connectBtn;
		};

		typedef std::unordered_map<ServerInfo*, std::pair<EntryLine, float>> ServerLabelMap;

	public:
								LANMenu();	
		virtual					~LANMenu()													override;
		virtual SFGWinPtr		CreateElements(WidgetMap& widgetMap)						override;

		void					SendAddMsg(const AddServerMsg& msg);
		void					SendUpdateMsg(const UpdateServerMsg& msg);
		void					SendSetSmartLabelMsg(const SetSmartLabelMsg& msg);

		void					ProcessAddServerMsg(AddServerMsg* msgs, int amount);
		void					ProcessUpdateServerMsg(UpdateServerMsg* msgs, int amount);
		void					ProcessSetSmartLabelMsg(SetSmartLabelMsg* msgs, int amount);

		bool					IsAdded(const RakNet::SystemAddress& address) const;

		void					OnRowSelection(ServerInfo* info);

	protected:
		virtual void			RefreshLayer(float dt)										override;

		virtual void			ConnectSignals()											override;
		virtual void			InitPools()													override;

	private:
		void					OnCreateGameButtonClicked();
		void					OnJoinGameButtonClicked();
		void					OnReturnButtonClicked();
		
		void					AddServerToTheList(const RakNet::SystemAddress& address, unsigned char max);
		void					UpdateServerInfo(const RakNet::SystemAddress& address, unsigned char nowPlayers, unsigned short ping, unsigned char maxPlayers = 0);

	private:
		sfg::Box::Ptr			m_scrolledWindowBox;
		sfg::Button::Ptr		m_createBtn;
		sfg::Button::Ptr		m_joinBtn;
		sfg::Button::Ptr		m_returnBtn;

		ServerLabelMap			m_serverAddresses;

		ServerInfo*				m_connectionTarget;
		sfg::Label::Ptr			m_smartMessage;

		bool					m_joinPressed;

		float					m_winWidth;

	};

} // namespace vgs

#endif __LANMENU_H__