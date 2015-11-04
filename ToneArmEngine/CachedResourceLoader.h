/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CachedResourceLoader

		Loads resources, and stores them in a cache.
		Is updated every frame, if an asset is not referenced for a certain amount of frames it is deleted.

		Created by: Karl Merten
		Created on: 28/04/2014

========================
*/

#ifndef __CACHEDRESOURCELOADER_H__
#define __CACHEDRESOURCELOADER_H__

#include "Resource.h"
#include "EngineModule.h"
#include "VGSMacro.h"
#include "StringUtils.h"
#include "BinaryFileResource.h"
#include "PathUtils.h"

#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <unordered_map>
#include <functional>
#include <chrono>

#define PASS_CALLBACKFN(__fn__, __obj__) std::bind(__fn__, __obj__, std::placeholders::_1)
#define PASS_STATIC_CALLBACKFN(__fn__) std::bind(__fn__, std::placeholders::_1)

namespace vgs {

struct CachedResource;
struct Task;

typedef size_t									hash_t;
typedef std::queue<Task>						TaskPoll;
typedef std::function<void(CachedResource*)>	CallbackFn;

struct Task
{
	Task()
		: hash(0)
		, path("")
		, key(0x00)
		, data(nullptr)
		, fn(nullptr)
	{}

	Task(std::string path, const char key, CallbackFn fn)
		: hash(StringUtils::Hash(path.c_str()))
		, path(path)
		, key(key)
		, data(nullptr)
		, fn(fn)
	{}

	hash_t						hash;
	std::string					path;
	char						key;
	std::shared_ptr<Resource>	data;
	CallbackFn					fn;
};

struct CachedResource {
	char*						identifier;
	std::shared_ptr<Resource>	ptr;
	unsigned int				ticksBeforeDelete;

	CachedResource( void ) :
		identifier( NULL ),
		ptr(),
		ticksBeforeDelete( 1 )
	{}

	~CachedResource( void ) {
		delete[] identifier;
	}

	void SetPath( const char* newPath ) {
		delete[] identifier;
		identifier = StringUtils::CopyCString( newPath );
	}

	void SetResource( Resource* res ) {
		ptr = std::shared_ptr<Resource>( res );
	}

	void SetResource( std::shared_ptr<Resource> res ) {
		ptr = res;
	}
};

class CachedResourceLoader : public EngineModule {											
public:
											~CachedResourceLoader( void );								

											CREATE_METH( CachedResourceLoader )
	bool									Init( void ) { return true; }

	virtual void							Startup( void );
	virtual void							Update( float dt );
	virtual void							Shutdown( void );

	CachedResource*							GetResource( const char* identifier );
	template <class T> 
	std::shared_ptr<T>						LoadResource( const char* pathToResource, const char key = 0x00 );
	void									LoadResourceAsync(const char* pathToResource, CallbackFn fn, const char key = 0x00);

private:
											CachedResourceLoader( void );
	void									RemoveCachedResource( std::vector<CachedResource*>::iterator& index );

	// to be used on loading thread only
	Resource*								LoadResource( const char* pathToResource, const char key = 0x00 );
	void									LoadingThFn();

	std::vector<CachedResource*>			m_cachedResources;

	std::thread*							m_loadingTh;
	bool									m_loadingThActive;

	TaskPoll								m_waitingPoll;
	std::mutex								m_waitingPollMutex;

	TaskPoll								m_loadedPoll;
	std::mutex								m_loadedPollMutex;
};

template <class T>
std::shared_ptr<T> CachedResourceLoader::LoadResource( const char* pathToResource, const char key ) {
	CachedResource* retResource = GetResource( pathToResource );
		
	if ( !retResource ) {
		retResource = new CachedResource();
	
		retResource->SetPath( pathToResource );	
	
		T* r = new T();

		if ( !r->Load( pathToResource, key ) ) {
			delete retResource;
			delete r;

			return std::shared_ptr<T>( NULL );
		} else {
			retResource->SetResource( r );
			m_cachedResources.push_back( retResource );
			return std::static_pointer_cast<T>( retResource->ptr );
		}
	}

	return std::static_pointer_cast<T>( retResource->ptr );
}

}

#endif //__CACHEDRESOURCELOADER_H__