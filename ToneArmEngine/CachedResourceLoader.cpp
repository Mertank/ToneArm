/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	CachedResourceLoader

	Created by: Karl Merten
	Created on: 28/04/2014

========================
*/
#include "CachedResourceLoader.h"
#include "PathUtils.h"
#include "InitializationFileResource.h"

#define TICKS_BEFORE_DELETION 1

namespace vgs {
/*
========
CachedResourceLoader::CachedResourceLoader

	CachedResourceLoader default constructor
========
*/
CachedResourceLoader::CachedResourceLoader( void )
	: m_loadingThActive(false)
	, m_loadingTh(nullptr)
{}
/*
========
CachedResourceLoader::~CachedResourceLoader

	CachedResourceLoader destructor
========
*/
CachedResourceLoader::~CachedResourceLoader( void ) {
	DELETE_STD_VECTOR_POINTER( CachedResource, m_cachedResources )
	delete m_loadingTh;
}

void CachedResourceLoader::Startup( void )
{
	m_loadingThActive = true;
	m_loadingTh = new std::thread(std::bind(&CachedResourceLoader::LoadingThFn, this));
}

void CachedResourceLoader::Shutdown( void )
{
	m_loadingThActive = false;

	if (m_loadingTh) {
		m_loadingTh->join();
	}
}

/*
========
CachedResourceLoader::Update

	Checks every resource to see if it is not referenced anymore.
	If it's not it will be deleted.
========
*/
void CachedResourceLoader::Update( float dt ) {
	BEGIN_STD_VECTOR_ITERATOR( CachedResource*, m_cachedResources )
		if ( currentItem->ptr.use_count() == 1 ) {
			if ( currentItem->ticksBeforeDelete > 0 ) {
				--( currentItem->ticksBeforeDelete );
			} else {				
				RemoveCachedResource( iter );		
				if ( m_cachedResources.empty() ) { break; }
				continue;
			}
		} else {
			currentItem->ticksBeforeDelete = TICKS_BEFORE_DELETION;
		}
	END_STD_VECTOR_ITERATOR;

	// swap queues
	TaskPoll tasks;
	{
		std::lock_guard<std::mutex> lock(m_loadedPollMutex);
		tasks.swap(m_loadedPoll);
	}
	// call callbacks on loaded resources
	while (!tasks.empty())
	{
		Task task = tasks.front();
		
		CachedResource* cachedResource = new CachedResource();
		cachedResource->SetPath(task.path.c_str());	
		cachedResource->SetResource(task.data);
		m_cachedResources.push_back(cachedResource);
		
		task.fn(cachedResource);
		tasks.pop();
	}
}
/*
========
CachedResourceLoader::RemoveCachedResource

	Removes a resource from the list of loaded resources
	and deletes it.
========
*/
void CachedResourceLoader::RemoveCachedResource( std::vector<CachedResource*>::iterator& index ) {
	std::vector<CachedResource*>::iterator last = m_cachedResources.end();
	--last;

	if ( index == last ) {
		delete ( *index );
		m_cachedResources.pop_back();
		if ( m_cachedResources.empty() ) {
			index = m_cachedResources.begin();
		} else {
			index = m_cachedResources.end() - 1;
		}
	} else {
		delete ( *index );
		// FIXME: crashed here
		( *index ) = ( *last );
		m_cachedResources.pop_back();
	}

	if ( m_cachedResources.empty() ) {
		index = m_cachedResources.begin();
	}
}

void CachedResourceLoader::LoadResourceAsync(const char* pathToResource, CallbackFn fn, const char key)
{
	// check if resource was already loaded
	CachedResource* res = GetResource(pathToResource);
	Task task(pathToResource, key, fn);

	if (!res)
	{
		// add task to waiting list
		std::lock_guard<std::mutex> lock(m_waitingPollMutex);
		m_waitingPoll.push(task);
	}
	else
	{
		// forward it to loaded poll
		std::lock_guard<std::mutex> lock(m_loadedPollMutex);
		task.data = res->ptr;
		m_loadedPoll.push(task);
	}
}

void CachedResourceLoader::LoadingThFn()
{
	std::chrono::milliseconds duration(50);
	TaskPoll poll;
	Task* taskRef = nullptr;

	do 
	{
		swap:
		{
			std::lock_guard<std::mutex> lock(m_waitingPollMutex);
			poll.swap(m_waitingPoll);
		}

		if (!poll.empty())
		{
			while ((taskRef = poll.empty() ? nullptr : &poll.front()) != nullptr)
			{
				Resource* res = LoadResource(taskRef->path.c_str(), taskRef->key);
				// if resource was loaded properly
				if (res)
				{
					taskRef->data = std::shared_ptr<Resource>(res);
					std::lock_guard<std::mutex> lock(m_loadedPollMutex);
					m_loadedPoll.push(Task(*taskRef));
				}

				poll.pop();
			}
			// check one last time before going to sleep
			goto swap;
		}

		std::this_thread::sleep_for(duration);
	}
	while (m_loadingThActive);
}

Resource* CachedResourceLoader::LoadResource(const char* pathToResource, const char key)
{
	std::string format;
	PathUtils::GetExtension(std::string(pathToResource), format);

	Resource* r = Resource::FindAndClone(format.c_str());

	if (r && r->Load(pathToResource, key))
	{
		return r;
	}
	else
	{
		delete r;
		return nullptr;
	}
}

CachedResource* CachedResourceLoader::GetResource( const char* identifier )
{
	std::vector<CachedResource*>::const_iterator iter = std::find_if(m_cachedResources.begin(), m_cachedResources.end(),
		[identifier](CachedResource* item)->bool 
		{ 
			return StringUtils::IsEqual(identifier, item->identifier); 
		});

	return iter != m_cachedResources.end() ? *iter : nullptr;
}

}