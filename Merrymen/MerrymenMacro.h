#ifndef __MERRYMEN_MACRO_H__
#define __MERRYMEN_MACRO_H__

#define DECLARE_CHARACTER_CLASS(__type__)																								\
	DECLARE_RTTI;																														\
																																		\
	public:																																\
		static	__type__* Create(const char* modelFile, const char* iniFile, const char* categoryName, bool online = true);				\
		virtual	~__type__() {};																											\
		bool	Init(const char* modelFile, const char* iniFile, const char* categoryName, bool online);								\
		void	Update(float dt) override;																								\
																																		\
	private:																															\
			__type__();																													\

#define IMPLEMENT_CHARACTER_CLASS(__type__, __base_type__)																				\
	IMPLEMENT_RTTI(__type__, __base_type__);																							\
																																		\
	/* default constructor */																											\
	__type__::__type__() {}																												\
																																		\
	/* allocates a __type__ object and initializes it */																				\
	__type__* __type__::Create(const char* modelFile, const char* iniFile, const char* categoryName, bool online)						\
	{																																	\
		__type__* tc = new __type__();																									\
																																		\
		if (tc && tc->Init(modelFile, iniFile, categoryName, online)) {																	\
			return tc;																													\
		}																																\
																																		\
		delete tc;																														\
		return nullptr;																													\
	}																																	\

#endif