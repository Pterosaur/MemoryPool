#pragma once

#include <memory>
#include <tuple>
#include <vector>
#include <assert.h>

//类名：静态链表
//说明：不同于STL特性：1.写时拷贝2.迭代器与容器生存周期独立

namespace gz
{
	//类名：静态链表节点
	template<typename ValueType>
	struct StaticListNode
	{
	public:
		//地址
		typedef size_t Address;
		//空地址
		const static Address NullAddress = 0;

		//存储值
		ValueType m_value;
		//前一个元素的地址
		Address m_front;
		//后一个元素的地址
		Address m_next;
		//是否在连表内
		bool m_isInList;

	public:
		StaticListNode()
		{
			m_front = NullAddress;
			m_next = NullAddress;
			m_isInList = false;
		}
		StaticListNode(const ValueType& v):
			StaticListNode()
		{
			m_value = v;
		}
	};

	//类名：静态链表底层数据
	template<typename ValueType>
	class StaticListBuffer
	{
	public:
		//节点
		typedef StaticListNode<ValueType> Node;
		//地址
		typedef typename Node::Address Address;
		//容量
		typedef size_t Size; 

	private:
		//实际存储数据
		std::vector <Node> m_buffer;
	public:
		StaticListBuffer(Size capacity = 0):
			m_buffer(capacity)
		{
		};
		StaticListBuffer(const StaticListBuffer &slb)
		{
			*this = slb;
		};
		template<typename Iterator>
		StaticListBuffer(Iterator begin, Iterator end)
		{
			m_buffer.assign(begin,end);
		}
		StaticListBuffer& operator = (const StaticListBuffer &slb)
		{
			if (this != &slb)
				m_buffer.assign(slb.m_buffer.begin(), slb.m_buffer.end());
		};
		~StaticListBuffer()
		{
		};
		const Node& operator[](Address pos) const
		{
			return m_buffer[pos];
		}
		Node& operator[](Address pos)
		{
			return m_buffer[pos];
		}
		//获得目前容量
		size_t capacity() const
		{
			return m_buffer.size();
		}
		//首元素
		typename std::vector<Node>::const_iterator cbegin() const
		{
			return m_buffer.cbegin();
		}
		//尾元素的下一个
		typename std::vector<Node>::const_iterator cend() const
		{
			return m_buffer.cend();
		}
	};

	//类名：静态链表底层数据所有者
	template<typename ValueType>
	class StaticListBufferOwner
	{
	public:
		//底层数据
		typedef StaticListBuffer<ValueType> Buffer;
		typedef typename Buffer::Size Size;
		typedef typename Buffer::Address Address;
		typedef typename Buffer::Node Node;
		typedef std::shared_ptr<Buffer > BufferPtr;
	private:
		//底层数据指针
		BufferPtr m_buffer;
	public:
		StaticListBufferOwner(Size capacity = 0) :
			m_buffer(new Buffer(capacity))
		{
		};
		StaticListBufferOwner(const StaticListBufferOwner &sbo)
		{
			*this = sbo;
		}
		//浅拷贝
		StaticListBufferOwner& operator= (const StaticListBufferOwner &sbo)
		{
			m_buffer = sbo.m_buffer;
			return *this;
		}
		~StaticListBufferOwner()
		{
		};
		const Node& operator[](Address pos) const
		{
			return (*m_buffer)[pos];
		}
		Node& operator[](Address pos)
		{
			return (*m_buffer)[pos];
		}
		Size capacity() const
		{
			return m_buffer->capacity();
		}
		bool unique() const
		{
			return m_buffer.unique();
		}
		//深拷贝自身
		void copySelf()
		{
			m_buffer.reset(new Buffer(m_buffer->cbegin(),m_buffer->cend()));
		}
		//重置自身
		void resetSelf()
		{
			m_buffer.reset(new Buffer());
		}
	};

	//类名：静态链表常量迭代器
	template<typename ValueType>
	class ConstIterator
	{
	public:
		typedef StaticListNode<ValueType> Node;
		//数据所有者
		typedef StaticListBufferOwner<ValueType> BufferOwner;
		typedef typename BufferOwner::Size Size;
		typedef typename BufferOwner::Address Address;
		//数据所有者指针
		typedef std::shared_ptr<BufferOwner > Owner;
		//空地址
		const static Address NullAddress = Node::NullAddress;
	protected:
		//属于自身的数据
		Owner m_owner;
		//位置
		Address m_position;
		//判断迭代器是否有效
		inline void CheckValid() const
		{
			if (!(*m_owner)[m_position].m_isInList)
				throw std::runtime_error("Invalid Iterator");
		};
	public:
		ConstIterator():
			m_owner(NULL)
			, m_position(NullAddress)
		{
		}
		ConstIterator(const Owner owner, Address position) :
			m_owner(owner)
			,m_position(position)
		{
		}
		ConstIterator(const ConstIterator& ci)
		{
			*this = ci;
		}
		ConstIterator& operator=(const ConstIterator& ci)
		{
			m_owner = ci.m_owner;
			m_position = ci.m_position;
			return *this;
		}
		ConstIterator& operator++()
		{
			m_position = (*m_owner)[m_position].m_next;
			return *this;
		}
		ConstIterator operator++(int)
		{
			ConstIterator copy = *this;
			++*this;
			return copy;
		}
		ConstIterator& operator--()
		{
			m_position = (*m_owner)[m_position].m_front;
			return *this;
		}
		ConstIterator operator--(int)
		{
			ConstIterator copy = *this;
			--*this;
			return copy;
		}
		const ValueType& operator*()const
		{
			CheckValid();
			return (*m_owner)[m_position].m_value;
		}
		bool operator == (const ConstIterator& itr) const
		{
			assert(this->m_owner == itr.m_owner);
			return m_position == itr.m_position;
		}
		bool operator != (const ConstIterator& itr)const
		{
			assert(this->m_owner == itr.m_owner);
			return m_position != itr.m_position;
		}
		//在vector中的绝对位置
		Address AbsolutePosition () const
		{
			return m_position;
		}
	}; 

	//类名：静态链表迭代器
	template<typename ValueType>
	class Iterator
		:public ConstIterator<ValueType>
	{
		typedef typename ConstIterator<ValueType>::Owner Owner;
		typedef typename ConstIterator<ValueType>::Address Address;
	public:
		Iterator() :
			ConstIterator<ValueType>()
		{
		}
		Iterator(const Owner owner, Address position) :
			ConstIterator<ValueType>(owner,position)
		{
		}
		Iterator(const Iterator& ci)
		{
			*this = ci;
		}
		ValueType& operator*() const
		{
			//非独占,在外部可能发生修改
			if (!m_owner->unique())
				m_owner->copySelf();
			CheckValid();
			return (*m_owner)[m_position].m_value;
		}
	};

	//类名：静态链表
	template<typename ValueType>
	class StaticList
	{
	public:
		typedef StaticListNode<ValueType> Node;
		typedef Node& NodeReference;
		//数据所有者
		typedef StaticListBufferOwner<ValueType> BufferOwner;
		typedef typename BufferOwner::Size Size;
		typedef typename BufferOwner::Address Address;
		//迭代器
		typedef ConstIterator<ValueType> const_iterator; 
		typedef Iterator<ValueType> iterator;
		//数据所有者指针
		typedef std::shared_ptr<BufferOwner > Owner;
	private:
		//链表拥有者
		Owner m_owner;
		//标识节点,用Next记录链表头（head）位置,用Front记录链表尾（tail）位置
		NodeReference m_flagNode;
		//迭代器是否是本链表迭代器
		void CheckIteratorBelongingness(const const_iterator &itr)
		{
			itr == cbegin();
		};
	public:
		StaticList(Size capacity = 0):
			//多存放一个为无效节点，用来做标识节点（m_flagNode）
			m_owner(new BufferOwner(capacity + 1))
			, m_flagNode((*m_owner)[Node::NullAddress])
		{
		};
		template<typename DefferenceIterator>
		StaticList(DefferenceIterator begin, DefferenceIterator end):
			//多存放一个为无效节点，用来做标识节点（m_flagNode）
			m_owner(new BufferOwner(((end - begin) > 0 ? end - begin : 0) + 1))
			, m_flagNode((*m_owner)[Node::NullAddress])
		{
			InitStaticList(begin, end);
		}
		StaticList(const StaticList& sl)
		{
			*this = sl;
		};
		StaticList& operator= (const StaticList& sl)
		{
			m_owner = sl.m_owner;
		}
		~StaticList()
		{
			if (size() != 1000)
				size_t s = size();
		};
		//初始化静态链表(用区间数值初始化)
		template<typename DefferenceIterator>
		void InitStaticList(DefferenceIterator begin, DefferenceIterator end)
		{
			//链表尾位置
			Address& tail = m_flagNode.m_front;
			//链表尾的元素指向下一个
			(*m_owner)[tail].m_next = tail + 1;
			++tail;
			for (auto itr = begin; itr != end; ++itr,++tail)
			{
				(*m_owner)[tail].m_value = *itr;
				(*m_owner)[tail].m_front = tail - 1;
				(*m_owner)[tail].m_next = tail + 1;
				(*m_owner)[tail].m_isInList = true;
			}
			(*m_owner)[--tail].m_next = Node::NullAddress;
		}
		const_iterator cbegin() const
		{
			return const_iterator(m_owner, m_flagNode.m_next);
		}
		const_iterator cend() const
		{
			return const_iterator(m_owner, Node::NullAddress);
		}
		//通过位置来定位,可能取出不在链表中的元素
		const_iterator locatedByPosition(Address position) const
		{
			return const_iterator(m_owner, position);
		}
		iterator begin()
		{
			return iterator(m_owner, m_flagNode.m_next);
		}
		iterator end()
		{
			return iterator(m_owner, Node::NullAddress);
		}
		//通过位置来定位,可能取出不在链表中的元素
		iterator locatedByPosition(Address position)
		{
			return iterator(m_owner, position);
		}
		//链表是否为空
		bool empty() const
		{
			return 0 == m_flagNode.m_next;
		}
		//链表内数据容量
		Size size() const
		{
			Size s = 0;
			for (auto itr = cbegin(); itr != cend(); ++itr, ++s);
			return s;
		}
		//获得链表容量
		Size capacity() const
		{
			return m_owner->capacity() - 1;
		}
		//删除制定元素
		void erase(const const_iterator &itr)
		{
			//确保删除迭代器是本链表的
			CheckIteratorBelongingness(itr);
			//校验节点有效性与Buffer的独占性
			*static_cast<iterator &>(const_cast<const_iterator &>(itr));
			//待删除节点
			NodeReference wantEraseNode = (*m_owner)[itr.AbsolutePosition()];
			//待删除节点前一个节点
			NodeReference frontNode = (*m_owner)[wantEraseNode.m_front];
			//待删除节点后一个节点
			NodeReference nextNode = (*m_owner)[wantEraseNode.m_next];
			//设置待删除节点不在链表内
			wantEraseNode.m_isInList = false;
			frontNode.m_next = wantEraseNode.m_next;
			nextNode.m_front = wantEraseNode.m_front;
		}
		//插入位置和插入元素
		void insert(const const_iterator &whr, const const_iterator &vl)
		{
			//确保删除迭代器是本链表的
			CheckIteratorBelongingness(whr);
			CheckIteratorBelongingness(vl);
			//待插入节点已在插入位置，则什么都不用做
			if (whr == vl)
				return;
			//非独占,修改时则拷贝自身
			if (!m_owner->unique())
				m_owner->copySelf();
			//待插入节点
			NodeReference wantInsertNode = (*m_owner)[vl.AbsolutePosition()];
			//待插入位置
			NodeReference wantInsertPosition = (*m_owner)[whr.AbsolutePosition()];
			if (wantInsertNode.m_isInList)
				erase(vl);
			//如果待插入位置不在链表上，则把待插入节点删除（如果待插入节点原来在链表上）
			if (wantInsertPosition.m_isInList)
				return;
			wantInsertNode.m_isInList = true;
			wantInsertNode.m_front = wantInsertPosition.m_front;
			wantInsertNode.m_next = whr.AbsolutePosition();

			(*m_owner)[wantInsertNode.m_front].m_next = vl.AbsolutePosition();
			wantInsertPosition.m_front = vl.AbsolutePosition();

		}
		void push_back(const const_iterator &itr)
		{
			//确保删除迭代器是本链表的
			CheckIteratorBelongingness(itr);
			//非独占,修改时则拷贝自身
			if (!m_owner->unique())
				m_owner->copySelf();
			NodeReference wantPushNode = (*m_owner)[itr.AbsolutePosition()];
			//尾节点
			NodeReference tailNode = (*m_owner)[m_flagNode.m_front];
			//itr已经在链表中则先删除
			if (wantPushNode.m_isInList)
				erase(itr);
			wantPushNode.m_isInList = true;
			wantPushNode.m_front = m_flagNode.m_front;
			wantPushNode.m_next = Node::NullAddress;
			tailNode.m_next = itr.AbsolutePosition();
			m_flagNode.m_front = itr.AbsolutePosition();
		}
		void pop_front()
		{
			erase(cbegin());
		}
	};

}