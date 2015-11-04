#ifndef __VGSMACRO_H__
#define __VGSMACRO_H__

#define FLOAT_EPSILON  0.001f

#define CREATE_METH(__type__)																	\
static __type__* Create()																		\
{																								\
	__type__* node = new __type__();															\
	if (node && node->Init())																	\
	{																							\
		return node;																			\
	}																							\
	delete node;																				\
	return nullptr;																				\
}																								\

#define CREATE_MODEL_NODE_METH(__type__)														\
public:																							\
	static __type__* Create(const char* file)													\
	{																							\
		__type__* node = new __type__();														\
		if (node && node->Init(file))															\
		{																						\
			return node;																		\
		}																						\
		delete node;																			\
		return nullptr;																			\
	}																							\

#define CREATE_RENDERABLE_NODE_METH(__type__, __renderer__)										\
public:																							\
	static __type__* Create()																	\
	{																							\
		__type__* node = new __type__();														\
		node->SaveRenderModule((__renderer__));													\
		if (node && node->Init())																\
		{																						\
			return node;																		\
		}																						\
		delete node;																			\
		return nullptr;																			\
	}																							\
private:																						\
	void SaveRenderModule(RenderModule* module)													\
	{																							\
		m_renderableModule = module;															\
	}																							\
	RenderModule* m_renderableModule;															\

#define CREATE_LAYER_METH(__type__, __file__)													\
static __type__* Create()																		\
{																								\
	__type__* node = new __type__();															\
	if (node && node->Init(__file__))															\
	{																							\
		return node;																			\
	}																							\
	delete node;																				\
	return nullptr;																				\
}

#define CREATE_SINGLETON( __type__ )															\
public:																							\
	static __type__* const GetInstance( void ) {												\
		if ( !m_instance ) {																	\
			m_instance = new __type__();														\
		}																						\
		return m_instance;																		\
	}																							\
	static void DestroyInstance( void ) {														\
		delete m_instance;																		\
		m_instance = NULL;																		\
	}																							\
private:																						\
	__type__( void );																			\
	__type__& operator=( const __type__& o ) {}													\
	__type__( const __type__& ) {}																\
private:																						\
	static __type__* m_instance;																\


#define CREATE_INIT_SINGLETON( __type__ )														\
public:																							\
	static __type__* const GetInstance( void ) {												\
		if ( !m_instance ) {																	\
			m_instance = new __type__();														\
			if ( !m_instance->Init() ) {														\
				delete m_instance;																\
				m_instance = NULL;																\
			}																					\
		}																						\
		return m_instance;																		\
	}																							\
	static void DestroyInstance( void ) {														\
		delete m_instance;																		\
		m_instance = NULL;																		\
	}																							\
private:																						\
	__type__( void );																			\
	__type__& operator=( const __type__& o ) {}													\
	__type__( const __type__& ) {}																\
	bool Init( void );																			\
private:																						\
	static __type__* m_instance;																\


#define INITIALIZE_SINGLETON( __type__ )														\
__type__* __type__::m_instance = NULL;															\

#define BEGIN_STD_VECTOR_ITERATOR( __type__, __container__ )									\
	{																							\
	unsigned int counter = 0;																	\
	for ( std::vector<__type__>::iterator iter = __container__.begin();							\
		  iter != __container__.end(); ++iter, ++counter ) {									\
		__type__ currentItem = ( *iter );														\

#define BEGIN_STD_LIST_ITERATOR( __type__, __container__ )										\
	{																							\
	unsigned int counter = 0;																	\
	for ( std::list<__type__>::iterator iter = __container__.begin();							\
		  iter != __container__.end(); ++iter, ++counter ) {									\
		__type__ currentItem = ( *iter );														\

#define END_STD_LIST_ITERATOR																	\
	}																							\
	}																							\

#define END_STD_VECTOR_ITERATOR END_STD_LIST_ITERATOR

#define DELETE_STD_VECTOR_POINTER( __type__, __container__ )									\
	BEGIN_STD_VECTOR_ITERATOR( __type__*, __container__ )										\
		delete currentItem;																		\
	END_STD_VECTOR_ITERATOR																		\
	__container__.clear();																		\

#define DELETE_STD_LIST_POINTER( __type__, __container__ )										\
	BEGIN_STD_LIST_ITERATOR( __type__, __container__ )											\
		delete currentItem;																		\
	END_STD_LIST_ITERATOR																		\
	__container__.clear();																		\

#define IS_ODD( __dataType__, __num__ ) ( __num__ & ( ( __dataType__ )1 ) )

#define IS_EVEN( __dataType__, __num__ ) !IS_ODD( __dataType__, __num__ )						\

#endif __VGSMACRO_H__