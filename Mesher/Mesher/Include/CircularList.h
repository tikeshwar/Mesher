#pragma once

template <typename T>
class Node
{
public:
	Node(T data):mData(data), mPrev(nullptr), mNext(nullptr){}
	~Node(){ mPrev = mNext = nullptr;}
	T data(){return mData;}
	Node* prev(){return mPrev;}
	Node* next(){return mNext;}
	void setData(T data){mData = data;}
	void setPrev(Node* prev){mPrev = prev;}
	void setNext(Node* next){mNext = next;}
public:
	T mData;
	Node* mPrev;
	Node* mNext;
};
template <typename L>
class CircularList
{
public:
	CircularList():mStart(nullptr), mSize(0){}
	~CircularList(){ clear();}
	CircularList(CircularList & inCircularList)
	{
		mStart = nullptr;
		mSize = 0;
		auto iter = inCircularList.start();
		while(1)
		{
			insert(iter->data());
			iter = iter->next();
			if(iter == inCircularList.start())
				break;
		}
	}
	void insert(L data)
	{
		if(mStart == nullptr)
		{
			mStart = new Node<L>(data);
			mStart->setNext(mStart);
			mStart->setPrev(mStart);
		}
		else
		{
			auto newNode = new Node<L>(data);
			auto lastNode = mStart->prev();
			newNode->setNext(mStart); 
			newNode->setPrev(lastNode);
			mStart->setPrev(newNode);
			lastNode->setNext(newNode);
		}
		mSize++;
	}
	void erase(Node<L>* pos)
	{
		if(pos == mStart)
		{
			mStart = pos->next();
		}
		pos->prev()->setNext(pos->next());
		pos->next()->setPrev(pos->prev());
		delete pos;
		mSize--;
	}
	bool erase(L data)
	{
		auto findItr = find(data);
		if(findItr == nullptr)
			return false;
		else
		{
			erase(findItr);
			return true;
		}
	}
	Node<L>* find(L data)
	{
		auto iter = mStart;
		while(iter->next() != mStart)
		{
			if(iter->data() == data)
				return iter;
			iter = iter->next();
		}
		return nullptr;
	}
	void clear()
	{
		auto iter = mStart;
		while(mSize)
		{
			erase(mStart);
		}
	}
	void operator=(CircularList & inCircularList)
	{
		for(auto it = inCircularList.start(); it->next() != inCircularList.start(); it = it->next())
		{
			insert(it->data());
		}
	}
	size_t size(){return mSize;}
	Node<L>* start(){return mStart;}
private:
	Node<L>* mStart;
	size_t mSize;
};