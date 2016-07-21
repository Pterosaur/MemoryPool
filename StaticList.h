#pragma once

#include <memory>
#include <tuple>
#include <vector>
#include <assert.h>

//��������̬����
//˵������ͬ��STL���ԣ�1.дʱ����2.�������������������ڶ���

namespace gz
{
	//��������̬����ڵ�
	template<typename ValueType>
	struct StaticListNode
	{
	public:
		//��ַ
		typedef size_t Address;
		//�յ�ַ
		const static Address NullAddress = 0;

		//�洢ֵ
		ValueType m_value;
		//ǰһ��Ԫ�صĵ�ַ
		Address m_front;
		//��һ��Ԫ�صĵ�ַ
		Address m_next;
		//�Ƿ���������
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

	//��������̬����ײ�����
	template<typename ValueType>
	class StaticListBuffer
	{
	public:
		//�ڵ�
		typedef StaticListNode<ValueType> Node;
		//��ַ
		typedef typename Node::Address Address;
		//����
		typedef size_t Size; 

	private:
		//ʵ�ʴ洢����
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
		//���Ŀǰ����
		size_t capacity() const
		{
			return m_buffer.size();
		}
		//��Ԫ��
		typename std::vector<Node>::const_iterator cbegin() const
		{
			return m_buffer.cbegin();
		}
		//βԪ�ص���һ��
		typename std::vector<Node>::const_iterator cend() const
		{
			return m_buffer.cend();
		}
	};

	//��������̬����ײ�����������
	template<typename ValueType>
	class StaticListBufferOwner
	{
	public:
		//�ײ�����
		typedef StaticListBuffer<ValueType> Buffer;
		typedef typename Buffer::Size Size;
		typedef typename Buffer::Address Address;
		typedef typename Buffer::Node Node;
		typedef std::shared_ptr<Buffer > BufferPtr;
	private:
		//�ײ�����ָ��
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
		//ǳ����
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
		//�������
		void copySelf()
		{
			m_buffer.reset(new Buffer(m_buffer->cbegin(),m_buffer->cend()));
		}
		//��������
		void resetSelf()
		{
			m_buffer.reset(new Buffer());
		}
	};

	//��������̬������������
	template<typename ValueType>
	class ConstIterator
	{
	public:
		typedef StaticListNode<ValueType> Node;
		//����������
		typedef StaticListBufferOwner<ValueType> BufferOwner;
		typedef typename BufferOwner::Size Size;
		typedef typename BufferOwner::Address Address;
		//����������ָ��
		typedef std::shared_ptr<BufferOwner > Owner;
		//�յ�ַ
		const static Address NullAddress = Node::NullAddress;
	protected:
		//�������������
		Owner m_owner;
		//λ��
		Address m_position;
		//�жϵ������Ƿ���Ч
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
		//��vector�еľ���λ��
		Address AbsolutePosition () const
		{
			return m_position;
		}
	}; 

	//��������̬���������
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
			//�Ƕ�ռ,���ⲿ���ܷ����޸�
			if (!m_owner->unique())
				m_owner->copySelf();
			CheckValid();
			return (*m_owner)[m_position].m_value;
		}
	};

	//��������̬����
	template<typename ValueType>
	class StaticList
	{
	public:
		typedef StaticListNode<ValueType> Node;
		typedef Node& NodeReference;
		//����������
		typedef StaticListBufferOwner<ValueType> BufferOwner;
		typedef typename BufferOwner::Size Size;
		typedef typename BufferOwner::Address Address;
		//������
		typedef ConstIterator<ValueType> const_iterator; 
		typedef Iterator<ValueType> iterator;
		//����������ָ��
		typedef std::shared_ptr<BufferOwner > Owner;
	private:
		//����ӵ����
		Owner m_owner;
		//��ʶ�ڵ�,��Next��¼����ͷ��head��λ��,��Front��¼����β��tail��λ��
		NodeReference m_flagNode;
		//�������Ƿ��Ǳ����������
		void CheckIteratorBelongingness(const const_iterator &itr)
		{
			itr == cbegin();
		};
	public:
		StaticList(Size capacity = 0):
			//����һ��Ϊ��Ч�ڵ㣬��������ʶ�ڵ㣨m_flagNode��
			m_owner(new BufferOwner(capacity + 1))
			, m_flagNode((*m_owner)[Node::NullAddress])
		{
		};
		template<typename DefferenceIterator>
		StaticList(DefferenceIterator begin, DefferenceIterator end):
			//����һ��Ϊ��Ч�ڵ㣬��������ʶ�ڵ㣨m_flagNode��
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
		//��ʼ����̬����(��������ֵ��ʼ��)
		template<typename DefferenceIterator>
		void InitStaticList(DefferenceIterator begin, DefferenceIterator end)
		{
			//����βλ��
			Address& tail = m_flagNode.m_front;
			//����β��Ԫ��ָ����һ��
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
		//ͨ��λ������λ,����ȡ�����������е�Ԫ��
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
		//ͨ��λ������λ,����ȡ�����������е�Ԫ��
		iterator locatedByPosition(Address position)
		{
			return iterator(m_owner, position);
		}
		//�����Ƿ�Ϊ��
		bool empty() const
		{
			return 0 == m_flagNode.m_next;
		}
		//��������������
		Size size() const
		{
			Size s = 0;
			for (auto itr = cbegin(); itr != cend(); ++itr, ++s);
			return s;
		}
		//�����������
		Size capacity() const
		{
			return m_owner->capacity() - 1;
		}
		//ɾ���ƶ�Ԫ��
		void erase(const const_iterator &itr)
		{
			//ȷ��ɾ���������Ǳ������
			CheckIteratorBelongingness(itr);
			//У��ڵ���Ч����Buffer�Ķ�ռ��
			*static_cast<iterator &>(const_cast<const_iterator &>(itr));
			//��ɾ���ڵ�
			NodeReference wantEraseNode = (*m_owner)[itr.AbsolutePosition()];
			//��ɾ���ڵ�ǰһ���ڵ�
			NodeReference frontNode = (*m_owner)[wantEraseNode.m_front];
			//��ɾ���ڵ��һ���ڵ�
			NodeReference nextNode = (*m_owner)[wantEraseNode.m_next];
			//���ô�ɾ���ڵ㲻��������
			wantEraseNode.m_isInList = false;
			frontNode.m_next = wantEraseNode.m_next;
			nextNode.m_front = wantEraseNode.m_front;
		}
		//����λ�úͲ���Ԫ��
		void insert(const const_iterator &whr, const const_iterator &vl)
		{
			//ȷ��ɾ���������Ǳ������
			CheckIteratorBelongingness(whr);
			CheckIteratorBelongingness(vl);
			//������ڵ����ڲ���λ�ã���ʲô��������
			if (whr == vl)
				return;
			//�Ƕ�ռ,�޸�ʱ�򿽱�����
			if (!m_owner->unique())
				m_owner->copySelf();
			//������ڵ�
			NodeReference wantInsertNode = (*m_owner)[vl.AbsolutePosition()];
			//������λ��
			NodeReference wantInsertPosition = (*m_owner)[whr.AbsolutePosition()];
			if (wantInsertNode.m_isInList)
				erase(vl);
			//���������λ�ò��������ϣ���Ѵ�����ڵ�ɾ�������������ڵ�ԭ���������ϣ�
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
			//ȷ��ɾ���������Ǳ������
			CheckIteratorBelongingness(itr);
			//�Ƕ�ռ,�޸�ʱ�򿽱�����
			if (!m_owner->unique())
				m_owner->copySelf();
			NodeReference wantPushNode = (*m_owner)[itr.AbsolutePosition()];
			//β�ڵ�
			NodeReference tailNode = (*m_owner)[m_flagNode.m_front];
			//itr�Ѿ�������������ɾ��
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