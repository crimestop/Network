#ifndef NET_GROUP_HPP
#define NET_GROUP_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <map>
#include <vector>
#include <set>
#include <initializer_list>
#include <cstdlib>
#include "network.hpp"
#include "network_draw.hpp"

namespace net{
	template <typename NodeVal, typename EdgeVal, typename NodeKey=std::string, typename EdgeKey=stdEdgeKey,
		typename Trait = default_traits<NodeVal,EdgeVal,NodeKey,EdgeKey> >
	struct group{

		//constructor
		group()=default;
		group(network<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> &);
		//copy constructor
		group(const group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>&)=default;
		//copy assignment
		group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>& operator=(const group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>&)=default;
		//move constructor
		group(group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>&&)=default;
		//move assignment
		group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>& operator=(group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>&&)=default;
		//destructor
		//~network();

		void belong(network<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> &);
		template <typename absorb_type, typename contract_type>
		void absorb(const NodeKey &,const absorb_type &,const contract_type &);
		void draw(const bool);
		const NodeVal& get_val();

		network<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> * net;
		NodeVal val;
		std::set<NodeKey,typename Trait::nodekey_less> contains;
	};

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey,typename Trait>
	const NodeVal& group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>::get_val(){
		return val;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey,typename Trait>
	group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>::group(network<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> & n){
		net=&n;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey,typename Trait>
	void group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>::belong(network<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> & n){
		net=&n;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey,typename Trait>
	template <typename absorb_type, typename contract_type>
	void group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>::absorb(const NodeKey & key,const absorb_type & absorb_fun,const contract_type & contract_fun){
		net->template tn_contract1(key,contains,val,absorb_fun,contract_fun);
		contains.insert(key);
	}

	template <typename absorb_type, typename contract_type,typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey,typename Trait>
	group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> contract(group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> & G1, group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait> & G2,
		const absorb_type & absorb_fun,const contract_type & contract_fun){
		group<NodeVal,EdgeVal,NodeKey,EdgeKey> G3;
		G3.net=G1.net;
		G3.contains=G1.contains;
		G3.insert(G2.begin(),G2.end());
		G3.ten=G3.net->template tn_contract2(G1.contains,G1.val,G2.contains,G2.val,absorb_fun,contract_fun);
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey,typename Trait>
	void group<NodeVal,EdgeVal,NodeKey,EdgeKey,Trait>::draw(const bool label_bond){
		net->draw({contains},label_bond);
	}
}
#endif