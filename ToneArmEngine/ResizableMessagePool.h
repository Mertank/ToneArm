/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ResizableMessagePool.h

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 4:01:56 PM

========================
*/
#ifndef __RESIZABLEMESSAGEPOOL_H__
#define __RESIZABLEMESSAGEPOOL_H__

#include <vector>
#include <iterator>
#include <functional>

#include "RenderModule.h"
#include "VGSAssert.h"

#include <SFML\System\Mutex.hpp>

//#define PROCESS_FN_PTR(__msgType__, __ptrName__, __callOn__) \
//void (__callOn__::*(__ptrName__))((__msgType__)*, int)

#define CALL_PROC_FN(__objectPtr__, __ptrToMember__) \
((__objectPtr__)->*(__ptrToMember__))

namespace vgs
{
	/*
	========================

		MessagePool

			Base class for message pools, contains pool priority.

			Created by: Vladyslav Dolhopolov
			Created on: 5/7/2014 3:23:59 PM

	========================
	*/
	class MessagePool
	{
	public:
		virtual			~MessagePool() {}

		int				GetPriority()							{ return m_priority; }
		void			SetPriotity(int value)					{ m_priority = value; }

		virtual void	Process() {}

		bool operator() (MessagePool* lhs, MessagePool* rhs) const
		{
			return lhs->m_priority > rhs->m_priority;
		}

	protected:
		int				m_priority;
		
	};

	/*
	========================

		ResizableMessagePool

			General class for managing a ResizableMessagePool of objects.

			Created by: Vladyslav Dolhopolov
			Created on: 5/7/2014 3:23:59 PM

	========================
	*/
	template <class T, class C>
	class ResizableMessagePool : public MessagePool
	{
	public:
		typedef	void	(C::*ProcFnPtr)(T*, int);

						ResizableMessagePool();
						ResizableMessagePool(int capacity);
						ResizableMessagePool(int capacity, char incThreshold, char decThreshold);
		virtual			~ResizableMessagePool() override;
		
		T*				GetMsg();
		T*				Swap(bool& resized, int& used);
		void			FreePool(T* in);
		void			Resize();

		virtual void	Process() override;

		void			Lock();
		void			Unlock();

		int				GetUsedAmount() const			{ return m_usedAmount; }
		int				GetCapacity() const				{ return m_capacity; } 

		void			SetType(bool resizeble)			{ m_type = resizeble ? Type::INCREASABLE : Type::STATIC; }
		void			SetProcessFunc(ProcFnPtr fnPtr)	{ m_procFn = fnPtr; }

		//C*			GetCallOnObj()					{ return m_callOnObj; }
		void			SetCallOnObj(C* value)			{ m_callOnObj = value; }

	private:
		void			Init(int capacity, char incThreshold, char decThreshold);
		void			Allocate();
		void			RecalcThresholds();

	private:
		int				m_usedAmount;
		int				m_capacity;
		T*				m_objects;
		
		T*				m_firstToWrite;
		T*				m_firstToRead;
		T*				m_currentObj;

		char			m_relativeIncThreshold; // in %
		char			m_relativeDecThreshold; // in %
		int				m_currentIncThreshold;
		int				m_currentDecThreshold;
		char			m_resizeRate;			// in %

		enum State		{ NONE, REQUIRESINCRESIZE, REQUIRESDECRESIZE }	m_state;
		enum Type		{ INCREASABLE, DECREASABLE, BOTH, STATIC }		m_type;

		ProcFnPtr		m_procFn;
		C*				m_callOnObj;

		sf::Mutex		m_mutex;

	};

	template <class T, class C>
	ResizableMessagePool<T, C>::ResizableMessagePool()
	{
		ResizableMessagePool::Init(100, 80, 50);
	}

	template <class T, class C>
	ResizableMessagePool<T, C>::ResizableMessagePool(int capacity)
	{
		ResizableMessagePool::Init(capacity, 80, 50);
	}

	template <class T, class C>
	ResizableMessagePool<T, C>::ResizableMessagePool(int capacity, char incThreshold, char decThreshold)
	{
		ResizableMessagePool::Init(capacity, incThreshold, decThreshold);
	}

	template <class T, class C>
	ResizableMessagePool<T, C>::~ResizableMessagePool()
	{
		FreePool(m_objects);
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::Init(int capacity, char incThreshold, char decThreshold)
	{
		m_usedAmount			= 0;
		m_capacity				= capacity;
		m_objects				= nullptr;
		m_relativeIncThreshold	= incThreshold;
		m_relativeDecThreshold	= decThreshold;
		m_firstToWrite			= nullptr;
		m_firstToRead			= nullptr;
		m_currentObj			= nullptr;
		m_currentIncThreshold	= 0;
		m_currentDecThreshold	= 0;
		m_resizeRate			= 20;
		m_state					= State::NONE;
		m_type					= Type::STATIC;
		m_procFn				= nullptr;

		#ifdef TONEARM_DEBUG
			VGSAssert(capacity > 1, 
				"Capacity has to be bigger than 1");
			VGSAssert(capacity % 2 == 0, 
				"Capacity has to be even");
			VGSAssert(m_relativeIncThreshold < 100, 
				"ResizableMessagePool increase threshold should be less than 100");
			VGSAssert(m_relativeDecThreshold < m_relativeIncThreshold, 
				"ResizableMessagePool decrease threshold should be less than increase threshold");
		#endif

		// calculate actual thresholds based on percent values
		RecalcThresholds();
		Allocate();
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::Allocate()
	{
		// allocate memory
		m_objects		= new T[m_capacity];

		// point to first objects
		m_firstToWrite	= &m_objects[0];
		m_firstToRead	= &m_objects[m_capacity / 2];
		m_currentObj	= m_firstToWrite;
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::RecalcThresholds()
	{
		m_currentIncThreshold = (m_capacity / 2) * m_relativeIncThreshold / 100;
		m_currentDecThreshold = (m_capacity / 2) * m_relativeDecThreshold / 100;
	}

	template <class T, class C>
	T* ResizableMessagePool<T, C>::GetMsg()
	{
		T* ptr = nullptr;

		if (!(m_usedAmount > m_capacity / 2 - 1))
		{
			ptr = m_currentObj;

			++m_currentObj;
			++m_usedAmount;
		}

		// check if it needs a resize
		if (m_type == Type::INCREASABLE || m_type == Type::BOTH)
		{
			if (m_usedAmount > m_currentIncThreshold)
			{
				m_state = State::REQUIRESINCRESIZE;
			}
		}
		
		if (m_type == Type::DECREASABLE || m_type == Type::BOTH)
		{
			if (m_usedAmount < m_currentDecThreshold)
			{
				m_state = State::REQUIRESDECRESIZE;
			}
		}

		// the object has to be initiallised outside
		return ptr;
	}

	template <class T, class C>
	T* ResizableMessagePool<T, C>::Swap(bool& resized, int& used)
	{
		if (m_usedAmount == 0)
		{
			resized = false;
			used = 0;
			return nullptr;
		}

		// check if pool need a resize
		if (m_state == State::NONE)
		{
			T* tmp			= m_firstToRead;
			m_firstToRead	= m_firstToWrite;
			m_firstToWrite	= tmp;
			m_currentObj	= m_firstToWrite;

			resized = false;
		}
		else
		{
			// means memory has to be freed outside
			resized = true;
			Resize();
		}

		used = m_usedAmount;
		m_usedAmount = 0;
		
		return m_firstToRead;
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::FreePool(T* in)
	{
		delete [] in;
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::Resize()
	{
		int change = m_capacity;
		// change capacity by m_resizeRate coefficient
		if (m_state == State::REQUIRESINCRESIZE)
		{
			m_capacity += change;
		}
		else
		{
			m_capacity -= change;
		}

		// after changing capacity recalculate thresholds
		// and reallocate block of memory
		RecalcThresholds();
		Allocate();
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::Process()
	{
		bool freeAfter;
		int	 usedAmount;
	
		m_mutex.lock();
		T* msgs = Swap(freeAfter, usedAmount);
		m_mutex.unlock();

		// handle
		if (msgs)
		{
			CALL_PROC_FN(m_callOnObj, m_procFn)(msgs, usedAmount);
		}
		// delete if needed
		if (freeAfter)
		{
			FreePool(msgs);
		}
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::Lock()
	{
		m_mutex.lock();
	}

	template <class T, class C>
	void ResizableMessagePool<T, C>::Unlock()
	{
		m_mutex.unlock();
	}

} // namespace vgs

#endif __RESIZABLEMESSAGEPOOL_H__