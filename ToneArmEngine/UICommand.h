/*
========================

Copyright (c) 2014 Vinyl Games Studio

	UICommand.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/10/2014 11:17:33 AM

========================
*/
#ifndef __UICOMMAND_H__
#define __UICOMMAND_H__

#include <list>

#include <SFML\System\Mutex.hpp>

#include <BitStream.h>

namespace vgs
{
	/*
	========================

		UICommand

			Commands render thread may send, which has to be processed on logic thread.
			Contains static queue with commands.

			Created by: Vladyslav Dolhopolov
			Created on: 7/10/2014

	========================
	*/
	class UICommand
	{
	public:
		typedef std::list<UICommand> CmdQueue;

		enum UICommandID
		{
			CMD_NET_CONNECT_TO,
			CMD_NET_SEND_JOIN_REQUEST,
			CMD_NET_PREGAME_SETUP_REQUEST,
			
			CMD_ENG_SET_SCENE,
			CMD_ENG_SHUTDOWN,
		};

									UICommand(UICommandID id, unsigned int len, unsigned char* data);
									UICommand(UICommandID id, RakNet::BitStream& stream);
									~UICommand();

		static void					AddNetCommand(const UICommand& cmd);
		static void					AddEngCommand(const UICommand& cmd);
		static void					SwapNet(CmdQueue& queue);
		static void					SwapEng(CmdQueue& queue);
		static int					SizeNet();
		static int					SizeEng();

		void						Free();

		UICommandID					GetId() const				{ return m_id; }
		unsigned char*				GetData() const				{ return m_data; }
		unsigned int				GetLength() const			{ return m_length; }
		RakNet::BitStream*			GetStream()					{ return m_stream; }

	private:
		UICommandID					m_id;

		unsigned char*				m_data;
		unsigned int				m_length;
		RakNet::BitStream*			m_stream;

		static sf::Mutex			s_mutex;
		static CmdQueue				s_netQueue;
		static CmdQueue				s_engQueue;
	};

} // namespace vgs

#endif __UICOMMAND_H__