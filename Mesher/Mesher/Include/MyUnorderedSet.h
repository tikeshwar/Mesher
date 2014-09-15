#pragma once

#include <unordered_set>

template <typename K, typename Pred, typename Eq>
class MyOrderedSet:public unordered_set<K, Pred, Eq>
{
	typedef unordered_set<K, Pred, Eq> UnorderedSet;
	typedef typename UnorderedSet::iterator MOSIterator;
public:
	MyOrderedSet(){}
	~MyOrderedSet(){UnorderedSet::~UnorderedSet();}

	bool insert(K k)
	{
		auto mItr = UnorderedSet::equal_range(k);
		//if(mItr.first != multimap::end() && mItr.first == mItr.second)
		for(auto it = mItr.first; it != mItr.second; it++)
		{
			if(it->second == v)
				return false;
		}
		multimap::insert(KVPair(k,v));
		return true;
	}
	bool erase(K k, V v)
	{
		auto mItr = multimap::equal_range(k);
		for(auto it = mItr.first; it != mItr.second; it++)
		{
			if(it->second == v)
			{
				multimap::erase(it);
				return true;
			}
		}
		return false;
	}
	bool find(K k, V v)
	{
		auto mItr = multimap::equal_range(k);
		for(auto it = mItr.first; it != mItr.second; it++)
		{
			if(it->second == v)
				return true;
		}
		return false;
	}
};