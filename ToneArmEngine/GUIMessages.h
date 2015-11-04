/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GUIMessages.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/14/2014 1:13:07 PM

========================
*/
#ifndef __GUIMESSAGES_H__
#define __GUIMESSAGES_H__

namespace vgs
{
	class GUILayer;

	struct AddLayerMsg
	{
		AddLayerMsg()
		{
		}

		AddLayerMsg(GUILayer* l, const char* n)
			: layer(l)
			, name(n)
		{
		}

		GUILayer*	layer;
		const char* name;
	};

	struct RemoveLayerMsg
	{
		RemoveLayerMsg()
		{
		}

		RemoveLayerMsg(const char* n)
			: name(n)
		{
		}

		const char* name;
	};

	struct SetVisibilityMsg
	{
		SetVisibilityMsg()
		{
		}

		SetVisibilityMsg(const char* n, bool v)
			: name(n)
			, value(v)
		{
		}

		const char* name;
		bool		value;
	};

	struct SetEnableMsg
	{
		SetEnableMsg()
		{
		}

		SetEnableMsg(const char* n, bool v)
			: name(n)
			, value(v)
		{
		}

		const char* name;
		bool		value;
	};

} // namespace vgs

#endif __GUIMESSAGES_H__