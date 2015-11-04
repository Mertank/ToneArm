/*
========================

Copyright (c) 2014 Vinyl Games Studio

	UIDManager.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/26/2014 5:53:51 PM

========================
*/
#ifndef __UIDMANAGER_H__
#define __UIDMANAGER_H__

namespace vgs
{
	/*
	========================

		UIDManager

			Gives away unique ids.

			Created by: Vladyslav Dolhopolov
			Created on: 5/26/2014 5:53:51 PM

	========================
	*/
	class UIDManager
	{
	public:
						UIDManager() : uid(1) {} // 0 means its not relative to any player
		unsigned char	GenerateUID() { return uid++; } // TODO: come up with something smarter

	private:
		unsigned char	uid;
	};

} // namespace vgs

#endif __UIDMANAGER_H__