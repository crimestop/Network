#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <set>
#include <initializer_list>
#include <cstdlib>
#include <stdexcept>
#include "error.hpp"
#include "site.hpp"

namespace net{

	template <typename T,typename V>
	using dec_type = std::function<void(T&,T&,T&,const std::set<std::string> &,const std::string & ,const std::string & ,V&)>;
	template <typename T,typename V>
	using absorb_type = std::function<void(T&,V&,const std::string &)>;
	template <typename T>
	using contract_type = std::function<T(T&,T&,const std::set<std::pair<std::string,std::string>> &)>;
	template <typename T>
	using init_site_type = std::function<T(const std::vector<std::string> &)>;
	template <typename V>
	using init_bond_type = std::function<V(const std::string &,const std::string &)>;

   /**
    * \brief 网络是一些点和点间关联的并
    *
    * 每个格点拥有一个名字，整个网络也有自己的名字，网络除了可以通过格点名称寻找格点外还可以通过整数数组来寻找格点
    * \see site, bond
    * \tparam T 每个格点中附着的信息类型
    * \tparam V 每个边上附着的信息类型
    */
    template <typename T,typename V>
	class network;

	template <typename T,typename V>
	class network{
	template <typename T1,typename V1>
	friend std::ostream & operator<<(std::ostream &, const network<T1,V1> &);
	template <typename T1,typename V1>
	friend std::istream & operator>>(std::istream &, network<T1,V1> &);
	template <typename T1,typename V1>
	friend std::ostream & operator<(std::ostream &, const network<T1,V1> &);
	template <typename T1,typename V1>
	friend std::istream & operator>(std::istream &, network<T1,V1> &);

	template <typename T1,typename V1,typename T2, typename V2>
	friend network<T2,V2> fmap(const network<T1,V1> &,std::function<T2(const T1 &)> f1,std::function<V2(const V1 &)> f2);

	public:
		//constructor
		network()=default;
		network(const std::string &);
		//copy constructor
		network(const network<T,V>&);
		//copy assignment
		network<T,V>& operator=(const network<T,V>&);
		//move constructor
		network(network<T,V>&&)=default;
		//move assignment
		network<T,V>& operator=(network<T,V>&&)=default;
		//destructor
		//~network();

		template <typename T1,typename V1>
		network<T1,V1> fmap(std::function<T1(T&)>,std::function<V1(V&)>);

		void add(const std::string &);
		void add(const std::string &,const std::vector<int> &);
		void set_bond(const std::string &,const std::string &,const std::string &,const std::string &);
		void set_bond(const std::vector<int> &,const std::vector<int> &,const std::string &,const std::string &);
		void set_bond(const std::string &,const std::string &);
		void set_bond(const std::vector<int> &,const std::vector<int> &);

		void del(const std::string &);
		void del(const std::vector<int> &);
		void del_bond(const std::string &,const std::string &);
		void del_bond(const std::vector<int> &,const std::vector<int> &);
		void del_leg(const std::string &,const std::string &);
		void del_leg(const std::vector<int> &,const std::string &);

		void rename(const std::string &,const std::string &);
		void rename(const std::vector<int> &,const std::string &);

		void move(const std::vector<int> &,const std::vector<int> &);
		void set_pos(const std::string &,const std::vector<int> &);

		void absorb(const std::string &,const std::string &,absorb_type<T,V> , contract_type<T>);
		void absorb(const std::vector<int> &,const std::vector<int> &, absorb_type<T,V> , contract_type<T> );
		void split(const std::string &,const std::string &,const std::string &, const std::set<std::string> &,dec_type<T,V>);
		void split(const std::string &,const std::string &, const std::set<std::string> &,dec_type<T,V>);
	#ifdef NET_GRAPH_VIZ
		void draw_to_file(const std::string &,const bool);
		void draw_to_file(const std::string &,const std::set<std::string> &,const bool);
	#ifdef NET_SHOW_FIG
		void draw(const bool);
		void draw(const std::set<std::string> &,const bool);
	#endif
	#endif
		std::string gviz(const std::set<std::string> &,const bool);
		bool exist(const std::string &);

		T& operator[](const std::string &);
		T& operator[](const std::vector<int> &);
		void set_val(const std::string &,const T& );
		void set_val(const std::vector<int> &,const T& );
		void init_sites(init_site_type<T> );
		void init_bonds(init_bond_type<V>);
		T contract(absorb_type<T,V>, contract_type<T>);

		void tn_contract1(const std::string &,const std::set<std::string> &, T&, absorb_type<T,V>, contract_type<T>);
		T tn_contract2(const std::set<std::string> &, T&, const std::set<std::string> &, T&, absorb_type<T,V>, contract_type<T>);

	private:
		std::map<std::string,site<T,V>> sites;
		std::string name="";
		std::map<std::vector<int>,std::string> name_at;
	};

	template <typename T1,typename V1,typename T2, typename V2>
	network<T2,V2> fmap(const network<T1,V1> &,std::function<T2(const T1 &)> f1,std::function<V2(const V1 &)> f2);

	std::string to_str(std::vector<int> v);

	template<typename T,typename V>	// this is not default because sites[i].bonds[j].neighbor needs redirection
	network<T,V>::network(const network<T,V>& N){
		sites=N.sites;
		name=N.name;
		name_at=N.name_at;
		for(auto & s:sites){
			for (auto & b: s.second.bonds){
				b.second.neighbor=&(sites[b.second.name]);
			}
		}
	}
	template<typename T,typename V>	// this is not default because sites[i].bonds[j].neighbor needs redirection
	network<T,V>& network<T,V>::operator=(const network<T,V>& N){
		if(this != &N){
			sites=N.sites;
			name=N.name;
			name_at=N.name_at;
			for(auto & s:sites){
				for (auto & b: s.second.bonds){
					b.second.neighbor=&(sites[b.second.name]);
				}
			}
		}
		return *this;
	}

	template<typename T,typename V>
	void network<T,V>::init_sites(init_site_type<T> init_fun){

		for (auto& site:sites){
			std::vector<std::string> inds;
			for (auto& b:site.second.bonds){
				inds.push_back(b.first);
			}
			site.second.val = init_fun(inds);
		}
	}

	template<typename T,typename V>
	void network<T,V>::init_bonds(init_bond_type<V> init_fun){

		for (auto& site:sites){
			for (auto& b:site.second.bonds){
				b.second.val=init_fun(b.first,b.second.ind);
			}
		}
	}

	template<typename T,typename V>
	network<T,V>::network(const std::string & s) : name(s){}

	template<typename T,typename V>
	T& network<T,V>::operator[](const std::string & name){
		return sites[name].val;
	}
	template<typename T,typename V>
	T& network<T,V>::operator[](const std::vector<int> & pos){
		return sites[name_at[pos]].val;
	}

	template<typename T,typename V>
	void network<T,V>::set_val(const std::string & name,const T& value){
		sites[name].val=value;
	}
	template<typename T,typename V>
	void network<T,V>::set_val(const std::vector<int> & pos,const T& value){
		sites[name_at[pos]].val=value;
	}

	template<typename T,typename V>
	bool network<T,V>::exist(const std::string & name){
		return (sites.count(name)==1);
	}

	template<typename T,typename V>
	void network<T,V>::add(const std::string & name){
		auto [s1,succ1]=sites.insert(make_pair(name,site<T,V>()));
		if(!succ1)throw key_exist_error("In network.add, site "+name+" already exists!");
	}

	template<typename T,typename V>
	void network<T,V>::add(const std::string & name, const std::vector<int> & pos){
		auto [s1,succ1]=name_at.insert(make_pair(pos,name));
		if(!succ1)throw key_exist_error("In network.add, site at ["+to_str(pos)+"] already exists!");
		auto [s2,succ2]=sites.insert(make_pair(name,site<T,V>()));
		if(!succ2)throw key_exist_error("In network.add, site "+name+" already exists!");
		s2->second.position=pos;
	}

	template<typename T,typename V>
	void network<T,V>::del(const std::string & name){

		auto site_itr = sites.find(name);
		if(site_itr == sites.end()){
			throw key_unfound_error("In network.del, site "+name+" is not found!");
		}

		for(auto & b: site_itr->second->bonds){
			b.second.neighbor->bonds.erase(b.second.ind);
		}
		sites.erase(site_itr);
	}

	template<typename T,typename V>
	void network<T,V>::del(const std::vector<int> & pos){

		auto name_itr =name_at.find(pos);
		if(name_itr==name_at.end()){
			throw key_unfound_error("In network.del, site at ["+to_str(pos)+"] is not found!");
		}
		del(name_itr->second);
		name_at.erase(name_itr);
	}


	template<typename T,typename V>
	void network<T,V>::del_bond(const std::string & name1,const std::string & name2){

		auto site_itr = sites.find(name1);
		if(site_itr == sites.end()){
			throw key_unfound_error("In network.del_bond, site "+name1+" is not found!");
		}

		if(sites.count(name2)==0){
			throw key_unfound_error("In network.del_bond, site "+name2+" is not found!");
		}

		for(auto bond_itr=site_itr->second.bonds.begin(); bond_itr != site_itr->second.bonds.end(); ) {
			if (bond_itr->second.name==name2) {
				bond_itr->second.neighbor->bonds.erase(bond_itr->second.ind);
				bond_itr=site_itr->second.bonds.erase(bond_itr);
			} else {
				++bond_itr;
			}
		}
	}

	template<typename T,typename V>
	void network<T,V>::del_bond(const std::vector<int> & pos1,const std::vector<int> & pos2){

		auto name_itr1 =name_at.find(pos1);
		if(name_itr1==name_at.end()){
			throw key_unfound_error("In network.del_bond, site at ["+to_str(pos1)+"] is not found!");
		}
		auto name_itr2 =name_at.find(pos2);
		if(name_itr2==name_at.end()){
			throw key_unfound_error("In network.del_bond, site at ["+to_str(pos2)+"] is not found!");
		}
		del_bond(name_itr1->second,name_itr2->second);
	}

	template<typename T,typename V>
	void network<T,V>::del_leg(const std::string & name,const std::string & ind){

		auto site_itr = sites.find(name);
		if(site_itr == sites.end()){
			throw key_unfound_error("In network.del_ind, site "+name+" is not found!");
		}

		site_itr->second.bonds.erase(ind);
	}

	template<typename T,typename V>
	void network<T,V>::del_leg(const std::vector<int> & pos,const std::string & ind){

		auto name_itr =name_at.find(pos);
		if(name_itr==name_at.end()){
			throw key_unfound_error("In network.del_ind, site at ["+to_str(pos)+"] is not found!");
		}
		del_leg(name_itr->second,ind);
	}

	template<typename T,typename V>
	void network<T,V>::rename(const std::string & old_name,const std::string & new_name){

		auto site_handle = sites.extract(old_name);
		if(site_handle.empty()){
			throw key_unfound_error("In network.rename, site "+old_name+" is not found!");
		}

		for(auto & b: site_handle.value.bonds){
			(b.second.neighbor->bonds)[b.second.ind].name=new_name;
		}
		site_handle.key=new_name;
		
		auto status = sites.insert(std::move(site_handle));
		if(!status.inserted)throw key_exist_error("In network.rename, site "+new_name+" already exists!");
	}

	template<typename T,typename V>
	void network<T,V>::rename(const std::vector<int> & pos,const std::string & new_name){

		auto name_itr =name_at.find(pos);
		if(name_itr==name_at.end()){
			throw key_unfound_error("In network.rename, site at ["+to_str(pos)+"] is not found!");
		}
		rename(name_itr->second,new_name);
		name_itr->second=new_name;
	}

	template<typename T,typename V>
	void network<T,V>::set_pos(const std::string & name,const std::vector<int> & pos){

		auto site_itr = sites.find(name);
		if(site_itr == sites.end()){
			throw key_unfound_error("In network.set_pos, site "+name+" is not found!");
		}
		if(site_itr->second.position==pos) return;

		if(site_itr->second.position.size()==0){
			site_itr->second.position=pos;
		}else{
			sites.name_at.erase(site_itr->second.position);
			site_itr->second.position=pos;
		}
		auto [it,succ] = sites.name_at.insert({pos,name});
		if(!succ) throw key_exist_error("In network.set_pos, site ["+to_str(pos)+"] already exists!");
	}

	template<typename T,typename V>
	void network<T,V>::move(const std::vector<int> & old_pos,const std::vector<int> & new_pos){

		auto name_itr =name_at.find(old_pos);
		if(name_itr==name_at.end()){
			throw key_unfound_error("In network.move, site at ["+to_str(old_pos)+"] is not found!");
		}
		if(old_pos==new_pos) return;
		auto [it,succ] = sites.name_at.insert({new_pos,name_itr->second});
		if(!succ) throw key_exist_error("In network.move, site ["+to_str(new_pos)+"] already exists!");
		sites.name_at.erase(name_itr);
	}

	template<typename T,typename V>
	void network<T,V>::set_bond(const std::string & name1,const std::string & name2,
		const std::string & ind1,const std::string & ind2){
		site<T,V> * site1,* site2;
		try{
			site1=&sites.at(name1);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site "+name1+" is not found!");
		}
		try{
			site2=&sites.at(name2);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site at "+name2+" is not found!");
		}
		auto [s1,succ1]=site1->bonds.insert(make_pair(ind1,bond<T,V>(name2,ind2,site2)));
		if(!succ1)throw key_exist_error("In network.set_bond, ind "+ind1+" of site "+name1+" already linked!");
		auto [s2,succ2]=site2->bonds.insert(make_pair(ind2,bond<T,V>(name1,ind1,site1)));
		if(!succ2)throw key_exist_error("In network.set_bond, ind "+ind2+" of site "+name2+" already linked!");
	}

	template<typename T,typename V>
	void network<T,V>::set_bond(const std::vector<int> & pos1,const std::vector<int> & pos2,
		const std::string & ind1,const std::string & ind2){
		std::string * name1,* name2;
		try{
			name1=&name_at.at(pos1);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site at ["+to_str(pos1)+"] is not found!");
		}
		try{
			name2=&name_at.at(pos2);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site at ["+to_str(pos2)+"] is not found!");
		}
		set_bond(*name1,*name2,ind1,ind2);
	}

	template<typename T,typename V>
	void network<T,V>::set_bond(const std::string & name1,const std::string & name2){
		set_bond(name1,name2,name1+'.'+name2,name2+'.'+name1);
	}

	template<typename T,typename V>
	void network<T,V>::set_bond(const std::vector<int> & pos1,const std::vector<int> & pos2){
		std::string * name1,* name2;
		try{
			name1=&name_at.at(pos1);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site at ["+to_str(pos1)+"] is not found!");
		}
		try{
			name2=&name_at.at(pos2);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site at ["+to_str(pos2)+"] is not found!");
		}
		set_bond(*name1,*name2,*name1+'.'+(*name2),*name2+'.'+(*name1));
	}

	template<typename T,typename V>
	void network<T,V>::absorb(const std::vector<int> & pos1,const std::vector<int> & pos2,
		absorb_type<T,V> absorb_fun, contract_type<T> contract_fun){
		if (name_at.count(pos1)==0){
			throw key_unfound_error("In network.set_bond, site at ["+to_str(pos1)+"] is not found!");
		}
		if (name_at.count(pos2)==0){
			throw key_unfound_error("In network.set_bond, site at ["+to_str(pos2)+"] is not found!");
		}
		absorb(name_at[pos1],name_at[pos2],absorb_fun,contract_fun);
	}

	template<typename T,typename V>
	void network<T,V>::absorb(const std::string & name1,const std::string & name2,
		absorb_type<T,V> absorb_fun, contract_type<T> contract_fun){
		if (sites.count(name1)==0){
			throw key_unfound_error("In network.absorb, site "+name1+" is not found!");
		}
		if (sites.count(name2)==0){
			throw key_unfound_error("In network.absorb, site "+name2+" is not found!");
		}
		auto & site1=sites[name1];
		auto & site2=sites[name2];

		std::set<std::pair<std::string,std::string>> ind_pairs;

		for(auto iter=site1.bonds.begin(); iter != site1.bonds.end(); ) {
			if (iter->second.name==name2) {
				ind_pairs.insert(std::make_pair(iter->first,iter->second.ind));
				absorb_fun(site1.val,iter->second.val,iter->first);
				iter=site1.bonds.erase(iter);
			} else {
				++iter;
			}
		}

		site1.val=contract_fun(site1.val,site2.val,ind_pairs);

		for(auto & b:site2.bonds){
			if(b.second.name != name1){
				site1.bonds[b.first]=b.second;
				sites[b.second.name].bonds[b.second.ind].name=name1;
			}
		}

		//std::cout<<to_str(site2.position)<<std::endl;

		if(site2.position.size()>0) name_at.erase(site2.position);
		sites.erase(name2);
	}

	template<typename T,typename V>
	void network<T,V>::split(const std::string & name1, const std::string & name2,const std::string & name3
		,const std::set<std::string> & inds, dec_type<T,V> dec_fun){

		auto s1=sites.find(name1);
		auto &b1=s1->bonds;
		auto [s2,succ2]=sites.insert(name2);
		auto &b2=s2->bonds;
		auto [s3,succ3]=sites.insert(name3);
		auto &b3=s3->bonds;

		for(auto & b:b1){
			if(inds.count(b.first)==0){
				b2[b.first]=b.second;
				sites[b.second.name].bonds[b.second.ind].name=name2;
			}else{
				b3[b.first]=b.second;
				sites[b.second.name].bonds[b.second.ind].name=name3;
			}
		}

		T env;
		std::string ind2=name2+'.'+name3;
		std::string ind3=name3+'.'+name2;
		dec_fun(s1->val,s2->val,s3->val,inds,ind2,ind3,env);
		b2[ind2]=bond<T,V>(name3,ind3,env);
		b3[ind3]=bond<T,V>(name2,ind2,env);

		sites.erase(s1);
	}

	template<typename T,typename V>
	void network<T,V>::split(const std::string & name1, const std::string & name2, const std::set<std::string> & inds,
		dec_type<T,V> dec_fun){

		auto s1=sites.find(name1);
		auto &b1=s1->bonds;
		auto [s2,succ2]=sites.insert(name2);
		auto &b2=s2->bonds;

		for(auto iter=b1.begin(); iter != b1.end(); ) {
			if (inds.count(iter->first)==1) {
				b2[iter->first]=iter->second;
				sites[iter->second.name]=name2;
				iter=b1.erase(iter);
			} else {
				++iter;
			}
		}

		auto temp=s1->val;
		T env;
		std::string ind1=name1+'.'+name2;
		std::string ind2=name2+'.'+name1;
		dec_fun(temp,s1->val,s2->val,inds,ind1,ind2,env);
		b1[ind1]=bond<T,V>(name2,ind2,env);
		b2[ind2]=bond<T,V>(name1,ind1,env);
	}

	template<typename T,typename V>
	T network<T,V>::contract(absorb_type<T,V> absorb_fun, contract_type<T> contract_fun){
		T tot;
		std::set<std::string> contracted;
		for(auto s:name_at){
			//std::cout<<s.second<<std::endl;
			tn_contract1(s.second,contracted,tot,absorb_fun,contract_fun);
			//for(auto & i:contracted) std::cout<<i<<'\n';
			contracted.insert(s.second);
			//std::cout<<'\n';
		}

		for(auto s:sites){
			if(contracted.count(s.first)==0){
				tn_contract1(s.first,contracted,tot,absorb_fun,contract_fun);
				contracted.insert(s.first);
			}
		}
		return tot;
	}

	template<typename T,typename V>
	void network<T,V>::tn_contract1(const std::string & sname,const std::set<std::string> & group, T& ten,
		absorb_type<T,V> absorb_fun, contract_type<T> contract_fun){

		site<T,V> * site;
		try{
			site=&sites.at(sname);
		}catch(std::out_of_range err){
			throw key_unfound_error("In network.set_bond, site "+sname+" is not found!");
		}
		if(group.size()==0){
			ten=site->val;
		}else{
			auto site_t=site->val;
			std::set<std::pair<std::string,std::string>> ind_pairs;

			for(auto & b:site->bonds) {
				//std::cout<<b.first<<b.second.name<<b.second.ind<<'\n';
				if (group.count(b.second.name)==1) {
					ind_pairs.insert(std::make_pair(b.first,b.second.ind));
					absorb_fun(site_t,b.second.val,b.first);
				}
			}

			ten=contract_fun(site_t,ten,ind_pairs);
		}
	}

	template<typename T,typename V>
	T network<T,V>::tn_contract2(const std::set<std::string> & group1, T& ten1,
		const std::set<std::string> & group2, T& ten2,
		absorb_type<T,V> absorb_fun, contract_type<T> contract_fun){

		if (group1.size()==0){
			return ten2;
		}
		if (group2.size()==0){
			return ten1;
		}
		std::set<std::pair<std::string,std::string>> ind_pairs;
		for (auto & sname:group1){
			auto & site=sites[sname];
			for(auto & b:site.bonds) {
				if (group2.count(b.second.name)==1) {
					ind_pairs.insert(std::make_pair(b.first,b.second.ind));
					absorb_fun(ten1,b.second.val,b.first);
				}
			}

		}
		return contract_fun(ten1,ten2,ind_pairs);
	}

	template <typename T1,typename V1,typename T2, typename V2>
	network<T2,V2> fmap(const network<T1,V1> & n,std::function<T2(const T1 &)> f1,std::function<V2(const V1 &)> f2){
		network<T2,V2> result;
		result.name=n.name;
		result.name_at=n.name_at;
		for (auto & s:n.sites){
			const auto [its, success] = result.sites.insert({s.first,site<T2,V2>()});
			its->second.position=s.second.position;
			its->second.val=f1(s.second.val);
			for (auto & b:s.second.bonds){
				its->second.bonds[b.first]=bond<T2,V2>(b.second.name,b.second.ind,nullptr,f2(b.second.val));
			}
		}
		for (auto & s:result.sites){
			for (auto & b:s.second.bonds){
				b.second.neighbor=&(result.sites[b.second.name]);
			}
		}
		return result;
	}

	std::string to_str(std::vector<int> v){
		std::stringstream ss;
		for (auto & i :v){
			ss<<i;
		}
		return ss.str();
	}
}
#endif
