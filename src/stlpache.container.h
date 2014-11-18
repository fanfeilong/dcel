#ifndef DCEL_STLPACHE_CONTAINER_H
#define DCEL_STLPACHE_CONTAINER_H

#include <string>
#include <map>
#include <functional>
#include <algorithm>

using namespace std;


namespace stlpache
{
	template <class ForwardIterator>
	void delete_container_pointers(ForwardIterator begin,
		ForwardIterator end) {
			while (begin != end) {
				ForwardIterator temp = begin;
				*begin = NULL;
				++begin;
				if (*temp!=NULL)
				{
					delete *temp;
				}
			}
	}

	template <class T>
	void delete_elements(T *container) {
		if (!container) return;
		delete_container_pointers(container->begin(), container->end());
		container->clear();
	}

	template <class T>
	void delete_values(T *v) {
		if (!v) return;
		for (typename T::iterator i = v->begin(); i != v->end(); ++i) {
			if (!i->second)
			{
				delete i->second;
				i->second = NULL;
			}
		}
		v->clear();
	}

	template<class A, class B>
	struct delete_map{
		bool operator()(pair<A,B> x) const{
			if (!x.second){
				delete x.second;
				x.second = NULL;
			}
			return true;
		}
	};

	template<typename In, typename Out, typename Pred>
	Out copy_if(In first, In last, Out res, Pred Pr){
		while (first != last){
			if (Pr(*first))
				*res++ = *first;
			++first;
		}
		return res;
	}

	template<typename P>
	struct selected2nd {
		typename P::second_type operator()(const P& p) const {
			return  p.second; 
		}
	};

	template<typename C> 
	selected2nd<typename C::value_type > make_select2nd( const C& container ) { 
		return selected2nd<typename C::value_type >(); 
	}

	template<typename RomdomIterator>
	RomdomIterator next(RomdomIterator beg,RomdomIterator end,RomdomIterator it){
		RomdomIterator next=++it;
		if (next!=end)
			return next;
		else
			return beg;
	}

	template<typename RomdomIterator>
	RomdomIterator previous(RomdomIterator beg,RomdomIterator end,RomdomIterator it){
		if (it!=beg)
			return --it;
		else
			return --end;
	}
}

#endif