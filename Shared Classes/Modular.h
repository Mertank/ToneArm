/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Modular.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/30/2014 3:10:51 PM

========================
*/
#ifndef __MODULAR_H__
#define __MODULAR_H__

#include "../ToneArmEngine/EngineModule.h"

#include <unordered_map>

namespace vgs
{
	enum class EngineModuleType : char
	{
		RENDERER = 0,
		RESOURCELOADER,
		DEBUGCONSOLE,
		NETWORKER,
		SOUNDER,
		INPUTER,
		PHYSICS,
	};

	typedef std::unordered_map<EngineModuleType, EngineModule*> ModuleMap;

	/*
	========================

		Modular

			...

			Created by: Vladyslav Dolhopolov
			Created on: 9/30/2014 3:10:51 PM

	========================
	*/
	class Modular
	{
	public:
		~Modular()
		{
			ModuleMap::iterator iter = m_modules.begin();
			while (iter != m_modules.end())
			{
				delete iter->second;
				++iter;
			}
		}

		template <class T>
		inline T* GetModuleByType(const EngineModuleType type)
		{
			if (m_modules.count(type))
			{
				return static_cast<T*>(m_modules[type]);
			}
			else
			{
				return nullptr;
			}
		}

	protected:
		ModuleMap m_modules;

	};

} // namespace vgs

#endif __MODULAR_H__