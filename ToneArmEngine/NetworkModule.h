/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkModule.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/14/2014 5:38:57 PM

========================
*/
#ifndef __NETWORKMODULE_H__
#define __NETWORKMODULE_H__

#include "EngineModule.h"
#include "NetworkClient.h"
#include "DebugCommand.h"

namespace vgs
{
	class MessageHandler;
	class LANMenu;

	/*
	========================

		NetworkModule

			A class to manage and handle client side of networking on engine level.

			Created by: Vladyslav Dolhopolov
			Created on: 5/14/2014 5:38:57 PM

	========================
	*/
	class NetworkModule : public EngineModule
	{
	public:
									NetworkModule();
									~NetworkModule();
		
		static NetworkModule*		Create();

		void						Startup(void)													override;
		void						Update(float dt)												override;
		void						Shutdown(void)													override;

		void						SetMsgHandler(MessageHandler* mh);
		MessageHandler*				GetMsgHandler()	{ return m_msgHandler; }

		void						Send(char* data, int len);
		void						Send(const RakNet::BitStream& bs);
		
		bool						StartupNetwork();
		void						ShutdownNetwork();
		
		bool						StartServer();
		void						ShutdownServer();
		bool						ConnectToServer(unsigned int port, const char* ip);
		void						Disconnect();

		void						OnServerConnectionLost(RakNet::SystemAddress* address);

		static bool					GetEnableInterpolation()										{ return s_enableInterpolation; }
		static float				GetClientDelay()												{ return s_clientDelay; }
		static void					SetEnableInterpolation(bool value)								{ s_enableInterpolation = value; }
		static void					SetClientDelay(float value)										{ s_clientDelay = value; }

		DECLARE_DEBUG_COMMAND_CALLBACK(vgs::NetworkModule, network)

	private:
		unsigned char				GetPacketIdentifier(RakNet::Packet *p);
		void						ProcessUICommands();
		
	private:
		NetworkClient				m_interface;
		MessageHandler*				m_msgHandler;
		
		bool						m_serverStarted;
		unsigned int				m_serverPort;
		std::string					m_serverAddress;

		static bool					s_enableInterpolation;
		static float				s_clientDelay;

		float						m_pingRate;
		float						m_pingCounter;

		LANMenu*					m_lanMenu;

		enum class ClientState { HOST, REMOTE, SINGLE } m_state;

	};

} // namespace vgs

#endif __NETWORKMODULE_H__