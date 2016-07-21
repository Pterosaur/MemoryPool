#pragma once
#include <boost/shared_ptr.hpp>

//说明：在作用域内创建对象，出了作用则自动析构

template<typename Object>
class ConstructSingletonInScope
{
private:
	static boost::shared_ptr<Object> m_op;
public:
	template<typename ... Args>
	ConstructSingletonInScope(Args ... args)
	{
		if (!m_op)
			m_op.reset(new Object(args...));
	}
	~ConstructSingletonInScope()
	{
		m_op.reset((Object *)NULL);
	}
	static Object& Instance()
	{
		if (m_op)
			return *m_op;
#define OBJECTNAME(OBJ) #OBJ
		throw std::logic_error(OBJECTNAME(Object)"have not been created.");
#undef OBJECTNAME
	}
};
template<typename Object>
boost::shared_ptr<Object> ConstructSingletonInScope<Object>::m_op;

template<typename Object>
struct Singleton
{
	static Object& Instance()
	{
		return ConstructSingletonInScope<Object>::Instance();
	}
};
