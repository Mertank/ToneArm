/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MessagePoolManager.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/25/2014 3:23:59 PM

========================
*/
#ifndef __MESSAGEPOOLMANAGER_H__
#define __MESSAGEPOOLMANAGER_H__

#include <set>
#include <unordered_map>

#include "ResizableMessagePool.h"

namespace vgs
{
	/*
	========================

		MessagePoolManager

			Stores render message pools. Capable of processing each one.

			Created by: Vladyslav Dolhopolov
			Created on: 5/25/2014 3:23:59 PM

	========================
	*/
	class MessagePoolManager
	{
	public:
		MessagePoolManager()
		{
		}

		~MessagePoolManager()
		{
			for (auto pool : m_pools)
			{
				delete pool;
			}
			m_pools.clear();
		}

		template <typename T, typename C>
		void AddResizeablePool(const char* name, unsigned int size, int priority,
							   C* obj, void (C::*procFn)(T*, int), bool resizeble = false)
		{
			ResizableMessagePool<T, C>* pool = new ResizableMessagePool<T, C>(size);
		
			pool->SetCallOnObj(obj);
			pool->SetProcessFunc(procFn);
			pool->SetPriotity(priority);
			pool->SetType(resizeble);

			m_pools.insert(pool);
			m_poolNameMap[name] = pool;
		}

		void RemovePool(const char* name)
		{
			if (m_poolNameMap.count(name))
			{
				std::multiset<MessagePool*, MessagePool>::const_iterator iter = std::find(m_pools.begin(), m_pools.end(), m_poolNameMap[name]);
				if (iter != m_pools.end())
				{
					m_pools.erase(iter);
					delete m_poolNameMap[name];
					m_poolNameMap.erase(name);
				}
			}
		}

		template <typename T, typename C>
		T* GetMessageByName(const char* name)
		{
			if (m_poolNameMap.count(name))
			{
				return static_cast<ResizableMessagePool<T, C>*>(m_poolNameMap[name])->GetMsg();
			}
			else
			{
				return nullptr;
			}
		}

		void Process()
		{
			for (auto pool : m_pools)
			{
				pool->Process();
			}
		}

		template <typename T, typename C>
		void LockPool(const char* name)
		{
			if (m_poolNameMap.count(name))
			{
				static_cast<ResizableMessagePool<T, C>*>(m_poolNameMap[name])->Lock();
			}
		}

		template <typename T, typename C>
		void UnlockPool(const char* name)
		{
			if (m_poolNameMap.count(name))
			{
				static_cast<ResizableMessagePool<T, C>*>(m_poolNameMap[name])->Unlock();
			}
		}

	private:
		std::multiset<MessagePool*, MessagePool>		m_pools;
		std::unordered_map<const char*, MessagePool*>	m_poolNameMap;

	};
}

#endif __MESSAGEPOOLMANAGER_H__