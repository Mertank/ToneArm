/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ServerInfo.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/26/2014 5:53:51 PM

========================
*/
#ifndef __UIDMANAGER_H__
#define __UIDMANAGER_H__

#include <RakNetTypes.h>

namespace vgs
{
	/*
	========================

		ServerInfo

			Stores server address, ping and player capacity.
			Used in LANGUI menu.

			Created by: Vladyslav Dolhopolov
			Created on: 5/26/2014 5:53:51 PM

	========================
	*/
	struct ServerInfo
	{
								ServerInfo();
		void					Connect();
		void					Reconnect();
		void					Selected();

		bool					operator ==(const ServerInfo& other);
		bool					operator ==(const RakNet::SystemAddress& other);

		RakNet::SystemAddress	address;
		unsigned char			capacity;
		unsigned char			nowPlaying;
		short					ping;

		bool					clicked;
	};

} // vgs

#endif