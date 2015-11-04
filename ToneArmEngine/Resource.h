/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Resource

		Base class for resources.

		Created by: Karl Merten
		Created on: 28/04/2014

========================
*/

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "RTTI.h"
#include "StringUtils.h"

#include <vector>
#include <mutex>
#include <memory>

namespace vgs {

class Resource;

// put in Resource class
#define DECLARE_PROTOTYPE(__type__)												\
public:																			\
	static bool IsRegistered() { return s_protoRegistered; }					\
	static void	Register() { s_protoRegistered = true; }						\
protected:																		\
	virtual Resource* Clone() const  { return new __type__(); }					\
	static bool s_protoRegistered;												\
private:																		\
	static __type__ __type__##Prototype;										\

// put in Resource cpp
#define IMPLEMENT_PROTOTYPE(__type__)											\
bool __type__::s_protoRegistered = false;										\
__type__ __type__::__type__##Prototype;											\

// put in Resource ctor
#define REGISTER_PROTOTYPE(__type__, __format__)								\
{																				\
	if (!__type__::IsRegistered())												\
	{																			\
		__type__::Register(); /* fuck off IntelliSense */						\
		Resource::RegisterPrototype(this, vgs::StringUtils::Hash(__format__));	\
	}																			\
}

typedef std::vector<Resource*>		ResPtrV;

class Resource {

public:
									DECLARE_RTTI
									Resource( void );
	virtual 						~Resource( void );

	virtual bool					Load( const char* path, const char key = 0x00 ) { return true; }
	virtual unsigned int			GetSize( void ) { return m_resourceSize; };

	static Resource*				FindAndClone(const char* format);

protected:
	static void						RegisterPrototype(Resource* res, unsigned long fhash);
	static std::mutex&				GetPtotoMutex();
	static ResPtrV&					GetPrototypes();

	virtual Resource*				Clone() const			{ return nullptr; }
	unsigned long					GetFormat() const		{ return m_formatHash; }

	unsigned long					m_formatHash;
	unsigned int					m_resourceSize;
};

}

#endif //__RESOURCE_H__