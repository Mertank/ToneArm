/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Resource

	Created by: Karl Merten
	Created on: 28/04/2014

========================
*/

#include "Resource.h"
#include "StringUtils.h"
#include "InitializationFileResource.h"
#include "LevelResource.h"
#include "ProgramsFileResource.h"
#include "SoundResource.h"

namespace vgs {
IMPLEMENT_BASE_RTTI( Resource )

/*
========
Resource::Resource

	Resource default constructor
========
*/
Resource::Resource( void ) :
	m_resourceSize( 0 ),
	m_formatHash ( 0 )
{}
/*
========
Resource::~Resource

	Resource destructor
========
*/
Resource::~Resource( void ) {
	int ducks = 0;
}

std::mutex& Resource::GetPtotoMutex()
{
	static std::mutex mutex;
	return mutex;
}

ResPtrV& Resource::GetPrototypes()
{
	static ResPtrV prototypes;
	return prototypes;
}

Resource* Resource::FindAndClone(const char* format)
{
	unsigned long hash = StringUtils::Hash(format);

	std::lock_guard<std::mutex> lock(Resource::GetPtotoMutex());
	ResPtrV& vecRef = Resource::GetPrototypes();

	ResPtrV::const_iterator iter = std::find_if(vecRef.begin(), vecRef.end(),
		[hash](const Resource* resPtr)->bool
		{
			return resPtr->GetFormat() == hash;
		});

	return iter != vecRef.end() ? (*iter)->Clone() : nullptr;
}

void Resource::RegisterPrototype(Resource* res, unsigned long fhash)
{
	res->m_formatHash = fhash;

	std::lock_guard<std::mutex> lock(Resource::GetPtotoMutex());
	Resource::GetPrototypes().push_back(res);
}

}