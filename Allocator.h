#pragma once

#include "MemoryPool.h"

template <class T>
class Allocator
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template <class U>
	struct rebind
	{
		typedef Allocator<U> other;
	};


	Allocator()
	{
		return;
	}

	template <class U>
	Allocator(const Allocator<U>& c)
	{
	}
	//�����ڴ�
	static pointer allocate(size_type n = 1)
	{
		if (n > 1)
			return m_stdAllocator.allocate(n);
		else
			return static_cast<pointer>(MemoryPool<sizeof(value_type)>::get_mutable_instance().Allocate());
	}
	//�ͷ��ڴ�
	static void deallocate(pointer p, size_type n = 1)
	{
		if (n > 1)
			m_stdAllocator.deallocate(p, n);
		else
			MemoryPool<sizeof(value_type)>::get_mutable_instance().Deallocate(p);
	}
	//�����������
	static void construct(pointer p, const value_type& other)
	{
		new(p)value_type(other);
	}
	//�ͷŶ���
	static void destroy(pointer p)
	{
		p->~value_type();
	}
	//����һ���¶���
	template<typename ... Args>
	static pointer New(Args & ... args)
	{
		pointer p = allocate();
		new(p)value_type(args...);
		return p;
	}
	//ɾ��һ������
	static void Delete(pointer p)
	{
		destroy(p);
		deallocate(p);
	}
	//�õ���ַ
	static pointer address(reference x)
	{
		return (pointer)&x;
	}
	//�õ���ַ
	static const_pointer const_address(const_reference x)
	{
		return (const_pointer)&x;
	}
	//���������
	static size_type max_size()
	{
		return -1;
	}
private:
	//��ǰ�������޷�����ʱ��STL�Դ�������
	static std::allocator<value_type> m_stdAllocator;
};

template<typename value_type>
std::allocator<value_type> Allocator<value_type>::m_stdAllocator;