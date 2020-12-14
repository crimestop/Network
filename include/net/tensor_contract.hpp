#ifndef NET_TENSOR_CONTRACT_HPP
#define NET_TENSOR_CONTRACT_HPP
#include "tensor_network.hpp"
#include <TAT/TAT.hpp>
#include "network.hpp"
#include "tensor_tools.hpp"
#include "traits.hpp"
#include "rational.hpp"
#include <random>
#include <variant>
#include <cstdlib>
#include <functional>

namespace net{
	inline bool contract_test_mode=false;
	inline bool contract_trace_mode=false;
	class Engine{
	public:
		template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeVal contract(network<NodeVal,int,NodeKey,EdgeKey,Trait> &, const std::set<NodeKey,typename Trait::nodekey_less> &);

		int coarse_grain_to=500;
		int cut_part=2;
		int refine_sweep=1000;
		int max_quickbb_size=16;
		double uneven=0.6;
		std::default_random_engine rand=std::default_random_engine(0);
	private:
		template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey inner_contract(network<NodeVal,int,NodeKey,EdgeKey,Trait> &, const std::set<NodeKey,typename Trait::nodekey_less> &);

		template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey contract_quickbb(network<NodeVal,int,NodeKey,EdgeKey,Trait> &, const std::set<NodeKey,typename Trait::nodekey_less> &);

		template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey contract_quickbb2(network<NodeVal,int,NodeKey,EdgeKey,Trait> &, const std::set<NodeKey,typename Trait::nodekey_less> &);

		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		std::vector<std::set<NodeKey,typename Trait::nodekey_less>> divide(network<NodeVal,int,NodeKey,EdgeKey,Trait> &,
			const std::set<NodeKey,typename Trait::nodekey_less> &);

		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		double refine(network<NodeVal,int,NodeKey,EdgeKey,Trait> &,
			const std::set<NodeKey,typename Trait::nodekey_less> &, std::vector<std::set<NodeKey,typename Trait::nodekey_less>> &);
		
		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		void adjust(network<NodeVal,int,NodeKey,EdgeKey,Trait> &,
			const std::set<NodeKey,typename Trait::nodekey_less> &, std::vector<std::set<NodeKey,typename Trait::nodekey_less>> &,double);
	};

	template<typename contract_type>
	struct lift_contract{
		template <typename NodeVal,typename EdgeKey, typename Comp>
		static NodeVal run(const NodeVal& ten1,const NodeVal& ten2,const std::set<std::pair<EdgeKey,EdgeKey>,Comp> & inds){
			return std::make_tuple(contract_type::run(std::get<0>(ten1),std::get<0>(ten2),inds),std::get<1>(ten1),std::get<2>(ten1));
		}
	};

	struct kset_contract{
		template <typename NodeVal,typename EdgeKey, typename Comp>
		static NodeVal run(const NodeVal& g1,const NodeVal& g2,const std::set<std::pair<EdgeKey,EdgeKey>,Comp> & inds){
			NodeVal res=g1;
			res.insert(g2.begin(),g2.end());
			return res;
		}
	};


	template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeVal Engine::contract(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat, const std::set<NodeKey,typename Trait::nodekey_less> & part){

		network<std::tuple<NodeVal,int,net::rational>,int,NodeKey,EdgeKey,Trait>  temp;
		temp = lat.template fmap<decltype(temp)>([](const NodeVal & ten){return std::make_tuple(ten,0,net::rational(0,1));},[](const int & m){return m;});
		std::string final_site=inner_contract<lift_contract<contract_type>>(temp,part);
		return std::get<0>(temp[final_site]);
	}

	template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeKey Engine::inner_contract(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat,
		const std::set<NodeKey,typename Trait::nodekey_less> & part){

		if(part.size()>max_quickbb_size){
			std::vector<std::set<NodeKey,typename Trait::nodekey_less>> subparts=divide(lat,part);
			std::set<NodeKey,typename Trait::nodekey_less> new_part;
			for(auto & p:subparts)
				new_part.insert(inner_contract<contract_type>(lat,p));
			return inner_contract<contract_type>(lat,new_part);
			// for(auto & p:subparts)
			// 	new_part.insert(contract_quickbb<contract_type>(lat,p));
			// return contract_quickbb<contract_type>(lat,new_part);
		}else{
			return contract_quickbb<contract_type>(lat,part);
		}

	}

	// find neighbor with least contraction count of a node within given part
	// EdgeVal = int
	template <typename IterNode, typename NodeSet,typename EdgeKey, typename Trait>
	std::pair<int,IterNode> search_quick(IterNode & it1, const NodeSet & part){
		std::map<EdgeKey,std::pair<int,IterNode>,typename Trait::edgekey_less> legs;
		for(auto &e:it1->second.edges){
			if(part.count(e.second.nbkey)>0){
				if(legs.count(e.second.nbkey)==0)
					legs[e.second.nbkey]={std::get<1>(it1->second.val)*
						std::get<1>(e.second.nbitr->second.val)/e.second.val,e.second.nbitr};
				else
					legs[e.second.nbkey].first/=e.second.val;
			}
		}
		int min_count=-1;
		IterNode min_nbitr;
		for(auto &l:legs){
			if(min_count<0 || l.second.first<min_count){
				min_count=l.second.first;
				min_nbitr=l.second.second;
			}
		}
		return {min_count,min_nbitr};
	}

	template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeKey Engine::contract_quickbb(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat,
		const std::set<NodeKey,typename Trait::nodekey_less> & part){

		//if(contract_test_mode) 
		//	lat.draw("lat before quickbb",{part},true);

		if(contract_trace_mode) std::cout<<"in_quickbb \n";
		if(part.size()==1)
			return *(part.begin());


		int count;
		int least_count=-1;
		using NodeItrType=typename network<NodeVal,int,NodeKey,EdgeKey,Trait>::IterNode;
		using KeySet=std::set<NodeKey,typename Trait::nodekey_less>;
		NodeItrType least_contract1,least_contract2,nb_itr;

		for(auto & p:part){
			auto site_it=lat.nodes.find(p);
			std::get<1>(site_it->second.val)=calc_weight(site_it,lat.nodes,KeySet());
		}
		for (auto & p:part){
			auto site_it=lat.nodes.find(p);
			std::tie(count,nb_itr) = search_quick<NodeItrType,KeySet,EdgeKey,Trait>(site_it,part);
			if(least_count<0 || count<least_count){
				least_count=count;
				least_contract1=site_it;
				least_contract2=nb_itr;
			}

		}
		int contract_size;
		auto root_itr=least_contract1;
		lat.template absorb<no_absorb,contract_type>(root_itr,least_contract2);
		contract_size=2;
		while(contract_size<part.size()){
			std::tie(count,nb_itr)=search_quick<NodeItrType,KeySet,EdgeKey,Trait>(root_itr,part);
			lat.template absorb<no_absorb,contract_type>(root_itr,nb_itr);
			++contract_size;
		}

		if(contract_trace_mode) std::cout<<"out_quickbb \n";
		return root_itr->first;

	}


	template <typename contract_type,typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeKey Engine::contract_quickbb2(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat,
		const std::set<NodeKey,typename Trait::nodekey_less> & part){

		//if(contract_test_mode) 
		//	lat.draw("lat before quickbb",{part},true);

		if(contract_trace_mode) std::cout<<"in_quickbb \n";
		if(part.size()==1)
			return *(part.begin());

		using NodeItrType=typename network<NodeVal,int,NodeKey,EdgeKey,Trait>::IterNode;
		using KeySet=std::set<NodeKey,typename Trait::nodekey_less>;
		NodeItrType least_contract1,least_contract2,nb_itr;

		int count;
		int least_count;
		KeySet treated_sites;
		for(auto & p:part){
			auto site_it=lat.nodes.find(p);
			std::get<1>(site_it->second.val)=calc_weight(site_it,lat.nodes,KeySet());
		}

		while(treated_sites.size()<part.size()-1){
			least_count=-1;
			for (auto & p:part){
				if(treated_sites.count(p)==0){
					auto site_it=lat.nodes.find(p);
					std::tie(count,nb_itr) = search_quick<NodeItrType,KeySet,EdgeKey,Trait>(site_it,part);
					if(least_count<0 || count<least_count){
						least_count=count;
						least_contract1=site_it;
						least_contract2=nb_itr;
					}
				}
			}
			lat.template absorb<no_absorb,contract_type>(least_contract1,least_contract2);
			treated_sites.insert(least_contract2->first);
		}

		if(contract_trace_mode) std::cout<<"out_quickbb \n";
		return least_contract1->first;

	}

	template <typename IterNode, typename NodeSet1, typename NodeSet2>
	int calc_weight(const IterNode & it, const NodeSet1 & includes, const NodeSet2 & excludes){

		int weight=1;
		for (auto & e:it->second.edges)
			if(includes.count(e.second.nbkey)>0 && excludes.count(e.second.nbkey)==0)
				weight*=e.second.val;
		return weight;
	}

	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void combine_edges(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat,
		const std::set<NodeKey,typename Trait::nodekey_less> & includes){

		for(auto & i :includes){
			auto & inode=lat.nodes[i];
			std::map<NodeKey,std::pair<EdgeKey,EdgeKey>,typename Trait::nodekey_less> nbkey2ind;
			for (auto iter =inode.edges.begin();iter!=inode.edges.end();){
				//std::cout<<"combine "<<i<<' '<<iter->first<<' '<<iter->second.nbkey<<'\n';
				if(nbkey2ind.count(iter->second.nbkey)==0){
					nbkey2ind.insert({iter->second.nbkey,{iter->first,iter->second.nbind}});
					++iter;
				}else{
					//std::cout<<"combine_erase "<<i<<' '<<iter->first<<' '<<iter->second.nbkey<<'\n';
					inode.edges[nbkey2ind[iter->second.nbkey].first].val *=iter->second.val;
					iter->second.nbitr->second.edges[nbkey2ind[iter->second.nbkey].second].val *=
					iter->second.nbitr->second.edges[iter->second.nbind].val;
					iter->second.nbitr->second.edges.erase(iter->second.nbind);
					iter=inode.edges.erase(iter);
				}
			}
		}
	}

	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void get_component(node<NodeVal,int,NodeKey,EdgeKey,Trait> & n, const NodeKey & p, const std::set<NodeKey,typename Trait::nodekey_less> & part,
		std::set<NodeKey,typename Trait::nodekey_less> & treated,std::set<NodeKey,typename Trait::nodekey_less> & component){

		treated.insert(p);
		component.insert(p);
		for (auto & eg:n.edges)
			if(part.count(eg.second.nbkey)>0 && treated.count(eg.second.nbkey)==0 &&
				std::get<1>(n.val)==std::get<1>(eg.second.nbitr->second.val))
					get_component(eg.second.nbitr->second,eg.second.nbkey,part,treated,component);

	}
	
	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::vector<std::set<NodeKey,typename Trait::nodekey_less>> disconnect
	(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat, const std::set<NodeKey,typename Trait::nodekey_less> & part,
		const std::vector<std::set<NodeKey,typename Trait::nodekey_less>> & subparts){

		for(int i=0;i<subparts.size();++i)
			for(auto & s:subparts[i])
				std::get<1>(lat[s])=i;

		std::set<NodeKey,typename Trait::nodekey_less> treated,newsubpart;
		std::vector<std::set<NodeKey,typename Trait::nodekey_less>> newsubparts;
		while(treated.size()<part.size())
			for(auto & p:part)
				if(treated.count(p)==0){
					newsubparts.push_back({});
					get_component(lat.nodes[p],p,part,treated,newsubparts.back());
				}

		return newsubparts;
	}

	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::vector<std::set<NodeKey,typename Trait::nodekey_less>> Engine::divide
	(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat, const std::set<NodeKey,typename Trait::nodekey_less> & part){

		if(contract_trace_mode) std::cout<<"in_divide \n";
		using KeySet=std::set<NodeKey,typename Trait::nodekey_less>;
		KeySet coarse_part=part;
		network<KeySet,int,NodeKey,EdgeKey> fakelat;
		fakelat=lat.template fmap<decltype(fakelat)>([](const NodeVal & tp){return KeySet();},[](const int & m){return m;});
		for (auto & n:fakelat.nodes)
			n.second.val.insert(n.first);

		//粗粒化
		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************1\n";
		// 	std::cout<<"finish **************1\n";

		if(contract_trace_mode) std::cout<<"coarse_grain \n";
		combine_edges(fakelat,part);
		while(coarse_part.size()>coarse_grain_to){
			std::set<std::tuple<int,NodeKey,NodeKey>,std::greater<std::tuple<int,NodeKey,NodeKey>>> ordered_bond; // weight, from, to
																				//we may replace std::greater
			// add bonds to ordered_bond
			for(auto & p:coarse_part){
				auto & this_site=fakelat.nodes[p];
				for(auto &e:this_site.edges){
					if(coarse_part.count(e.second.nbkey)>0){
						ordered_bond.insert({e.second.val,p,e.second.nbkey});
					}
				}
			}
			// do coarse grain
			KeySet treated_sites;
			for(auto & b:ordered_bond){
				if(treated_sites.count(std::get<1>(b))==0 && treated_sites.count(std::get<2>(b))==0){
					treated_sites.insert(std::get<1>(b));
					treated_sites.insert(std::get<2>(b));
					fakelat.template absorb<no_absorb,kset_contract>(std::get<1>(b),std::get<2>(b));
					coarse_part.erase(std::get<2>(b));
					combine_edges(fakelat,{std::get<1>(b)});
				}
			}
		}
		//lat.draw(true);
		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************2\n";
		// 	std::cout<<"finish **************2\n";

		if(contract_trace_mode) std::cout<<"initial \n";
		//初始分割，分成cut_part份
		KeySet treated_sites={};
		KeySet final_sites={};
		double size_limit=double(part.size())/cut_part;
		for(int i=0;i<cut_part;++i){
			if(contract_test_mode) fakelat.draw("lat before initial_cut no. "+std::to_string(i),{part,final_sites},true);
			//find site with max weight (within part-treated)
			int max_weight=0;
			int this_weight;
			typename network<KeySet,int,NodeKey,EdgeKey>::IterNode max_weight_site_itr;
			typename network<KeySet,int,NodeKey,EdgeKey>::IterNode this_site_itr;
			for(auto p:coarse_part){
				if(treated_sites.count(p)==0){
					this_site_itr=fakelat.find(p);
					this_weight=calc_weight(this_site_itr,coarse_part,treated_sites);
					if(contract_test_mode) std::cout<<"test.divide.initial_cut.max_weight "<<p<<' '<<this_weight<<'\n';
					if(this_weight>max_weight){
						max_weight=this_weight;
						max_weight_site_itr=this_site_itr;
					}
				}
			}
			if(contract_test_mode) std::cout<<"test.divide.initial_cut.max_weight final "<<max_weight_site_itr->first<<' '<<max_weight<<'\n';
			//construct subpart
			treated_sites.insert(max_weight_site_itr->first);
			final_sites.insert(max_weight_site_itr->first);
			while(max_weight_site_itr->second.val.size()<size_limit){ // exit2: reach limit
				//std::cout<<"a \n";
				//fakelat.consistency();
				combine_edges(fakelat,{max_weight_site_itr->first});
				//std::cout<<"b \n";
				//fakelat.consistency();
				std::set<std::pair<int,NodeKey>,std::greater<std::pair<int,NodeKey>>> ordered_nb;
				for(auto & e:max_weight_site_itr->second.edges){
					if(coarse_part.count(e.second.nbkey)>0 && treated_sites.count(e.second.nbkey)==0){
						ordered_nb.insert({e.second.val,e.second.nbkey});
					}
				}
				if(ordered_nb.size()==0) // exit1: no neighbors
					break;
				for(auto & nb:ordered_nb){
					fakelat.template absorb<no_absorb,kset_contract>(max_weight_site_itr->first,nb.second);
					treated_sites.insert(nb.second);
					if(max_weight_site_itr->second.val.size()>=size_limit)
						break;
				}
			}
				//std::cout<<"c \n";
				//fakelat.consistency();
			combine_edges(fakelat,{max_weight_site_itr->first});
				//std::cout<<"d \n";
				//fakelat.consistency();
		}


		//if(contract_test_mode) 
			//fakelat.draw("lat after initial_cut",{coarse_part,final_sites},true);
		if(contract_trace_mode) std::cout<<"after_initial \n";
		//可能出现这种情况：由于连通性的原因，所有subpart搞完后，还剩下一些sites

		//int n=0;
		while(treated_sites.size()<coarse_part.size()){ 
				//for(auto & e:fakelat.nodes.at("ten3_4").edges)
					//std::cout<<"hedddddre2 "<<e.second.nbkey<<'\n';
		//std::cout<<"diff"<<treated_sites.size()<<' '<<coarse_part.size()<<'\n';
			for(auto & s:final_sites){
				//std::cout<<"here "<<s<<'\n'; 
				//auto & test_node=fakelat.nodes.at(s);
				//std::cout<<"there \n"; 
				//std::cout<<test_node.edges.size()<<"\n"; 
				//std::cout<<"there \n"; 
				for(auto & e:fakelat.nodes.at(s).edges){
					//std::cout<<"here2 "<<e.second.nbkey<<coarse_part.count(e.second.nbkey)<<final_sites.count(e.second.nbkey)<<'\n'; 
					if(coarse_part.count(e.second.nbkey)>0 &&final_sites.count(e.second.nbkey)==0){
						treated_sites.insert(e.second.nbkey);
						fakelat.template absorb<no_absorb,kset_contract>(s,e.second.nbkey);
						//std::cout<<treated_sites.size()<<"success\n";
						//std::cout<<treated_sites.size()<<"success\n";
						break;
					}
				}

			}
			//n++;
			//if(n==10) std::exit(EXIT_FAILURE);
		}
		if(contract_test_mode) fakelat.draw("lat after adjustment",{part,final_sites},true);

		if(contract_trace_mode) std::cout<<"release \n";
		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************3\n";
		// 	std::cout<<"finish **************3\n";
		//释放
		std::vector<KeySet> subparts;
		for(auto & s:final_sites)
			subparts.push_back(fakelat[s]);

		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************4\n";
		// 	std::cout<<"finish **************4\n";

		if(contract_trace_mode) std::cout<<"out_divide \n";

		// adjust(lat,part,subparts,0.1);
		// refine(lat,part,subparts);
		adjust(lat,part,subparts,0.5);
		// refine(lat,part,subparts);
		// adjust(lat,part,subparts,1);
		// refine(lat,part,subparts);
		if(contract_test_mode) lat.draw("lat after adjust",subparts,true);
		auto subparts2= disconnect(lat,part,subparts);
		if(contract_test_mode) lat.draw("lat after disconnect",subparts2,true);
		return subparts2;
	}



	template <typename NodeType,typename SetType>
	bool calc_gain(int cut_part,net::rational & max_gain,NodeType & n,int & this_part,int & nb_part,
		const SetType & part){
		std::vector<int> weights(cut_part,1);
		for(auto & eg:n.edges)
			if(part.count(eg.second.nbkey)>0)
				weights[std::get<1>(eg.second.nbitr->second.val)]*=eg.second.val;

		this_part=std::get<1>(n.val);
		nb_part=this_part;
		int this_weight=weights[this_part];
		int max_weight=this_weight;
		for(int i=0;i<cut_part;i++){
			if(weights[i]>max_weight){
				max_weight=weights[i];
				nb_part=i;
			}
		}
		max_gain=net::rational(max_weight,this_weight);
		return (max_weight>this_weight) ;
	}

	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	double Engine::refine(network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat,const std::set<NodeKey,typename Trait::nodekey_less> & part,
		std::vector<std::set<NodeKey,typename Trait::nodekey_less>> & subparts){


		if(contract_trace_mode) std::cout<<"in_refine \n";

		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************5\n";
		// 	std::cout<<"finish **************5\n";
		//if(contract_test_mode) lat.draw("lat before refinement",subparts,true);

		// set part label and  calculate part size
		std::vector<int> part_size(subparts.size(),0);
		for(int i=0;i<subparts.size();++i){
			for(auto & s:subparts[i])
				std::get<1>(lat[s])=i;
			part_size[i]=subparts[i].size();
		}

		//calc gain sorted set
		std::map<std::pair<net::rational,NodeKey>,std::pair<int,int>,std::greater<std::pair<net::rational,NodeKey>>> gain_rec; //gain, from, to
		net::rational gain;
		double tot_gain=1.;
		int this_part,nb_part;
		for(auto & p:part){
			auto & n=lat.nodes[p];

			if(calc_gain(cut_part,gain,n,this_part,nb_part,part)){
				gain_rec[{gain,p}]={this_part,nb_part};
				std::get<2>(n.val)=gain;
			}
		}
		if(contract_test_mode){
			std::cout<<"test.refine.build_gain start\n";
			for(auto & i: gain_rec) std::cout<<"test.refine.build_gain "<<i.first.second<<' '<<
				i.first.first<<' '<<i.second.first<<" -> "<<i.second.second<<"\n";
			std::cout<<"test.refine.build_gain finish\n";
		}

		// for(int i=0;i<subparts.size();++i){
		// 	std::cout<<"test.refine.run_gain subpart"<<i<<'\n';
		// 	for(auto & s:subparts[i]) std::cout<<"         "<<s<<'\n';
		// 	for(auto & s:subparts[i]) std::cout<<"         \n";
		// }

		int min_size=part.size()/cut_part*(1-uneven);
		int max_size=part.size()/cut_part*(1+uneven);
		for(int i=0;i<refine_sweep;i++){
			if(gain_rec.size()==0)
				break;
			for(auto g_rec=gain_rec.begin();g_rec!=gain_rec.end();++g_rec){
				if( part_size[g_rec->second.first]>min_size && part_size[g_rec->second.second]<max_size){

			// std::cout<<"test.refine.run_gain "<<g_rec->first.second<<' '<<
			// 	g_rec->first.first<<' '<<g_rec->second.first<<" -> "<<g_rec->second.second<<"\n";

					part_size[g_rec->second.first]--;
					part_size[g_rec->second.second]++;
					auto this_name=g_rec->first.second;
					auto & this_node= lat.nodes[this_name];
					std::get<1>(this_node.val)=g_rec->second.second;
					subparts[g_rec->second.first].erase(g_rec->first.second);
					subparts[g_rec->second.second].insert(g_rec->first.second);
					tot_gain*=double(g_rec->first.first);

					gain_rec.erase(g_rec);
					if(calc_gain(cut_part,gain,this_node,this_part,nb_part,part)){
						gain_rec[{gain,this_name}]={this_part,nb_part};
						std::get<2>(this_node.val)=gain;
					}
					for(auto & eg:this_node.edges){
						if(part.count(eg.second.nbkey)>0){
							auto lookup_nb=gain_rec.find({std::get<2>(eg.second.nbitr->second.val),eg.second.nbkey});
							if(lookup_nb != gain_rec.end()){
								gain_rec.erase(lookup_nb);
							}
							if(calc_gain(cut_part,gain,eg.second.nbitr->second,this_part,nb_part,part)){
								gain_rec[{gain,eg.second.nbkey}]={this_part,nb_part};
								std::get<2>(eg.second.nbitr->second.val)=gain;
							}
						}
					}
					break;
				}
			}
		}


		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************6\n";
		// 	std::cout<<"finish **************6\n";

		//if(contract_test_mode) lat.draw("lat after refinement",subparts,true);

		if(contract_trace_mode) std::cout<<"out_refine \n";

		return tot_gain;

	}

	//randomly choose a neighbor, return true when part has changed.

	template <typename NodeType,typename SetType>
	void calc_weight(int cut_part,const NodeType & n,std::vector<int> & weight,
		const SetType & part){

		weight=std::vector<int>(cut_part,1);
		for(auto & eg:n.edges)
			if(part.count(eg.second.nbkey)>0)
				weight[std::get<1>(eg.second.nbitr->second.val)]*=eg.second.val;
	}


	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void Engine::adjust (network<NodeVal,int,NodeKey,EdgeKey,Trait> & lat,const std::set<NodeKey,typename Trait::nodekey_less> & part,
		std::vector<std::set<NodeKey,typename Trait::nodekey_less>> & subparts,double alpha){


		if(contract_trace_mode) std::cout<<"in_adjust \n";
		std::vector<std::set<NodeKey,typename Trait::nodekey_less>> best_subparts=subparts,temp_subparts;
		std::vector<NodeKey> part_vec={part.begin(),part.end()};

		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************5\n";
		// 	std::cout<<"finish **************5\n";
		if(contract_test_mode) lat.draw("lat before adjustment",subparts,true);

		// set part label and  calculate part size
		std::vector<int> part_size(subparts.size(),0);
		for(int i=0;i<subparts.size();++i){
			for(auto & s:subparts[i])
				std::get<1>(lat[s])=i;
			part_size[i]=subparts[i].size();
		}

		//calc gain sorted set

		int min_size=part.size()/cut_part*(1-0.5*uneven);
		int max_size=part.size()/cut_part*(1+0.5*uneven);

		std::uniform_int_distribution<> site_dist(0,part.size()-1);
		std::uniform_int_distribution<> part_dist(0,cut_part-2);
		std::uniform_real_distribution<> monte_carlo;

		std::map<NodeKey,std::vector<int>,typename Trait::nodekey_less> weights;
		for(auto & p:part)
			calc_weight(cut_part,lat.nodes[p],weights[p],part);

		double cumu_gain=1,max_gain=1,further_gain;
		int i=0;
		while(i<refine_sweep){

			auto this_site=part_vec[site_dist(rand)];
			auto & this_node=lat.nodes[this_site];
			auto & this_weight=weights[this_site];
			int this_part=std::get<1>(this_node.val);
			int next_part=part_dist(rand);
			if(next_part>=this_part) next_part++;

			if( part_size[this_part]>min_size && part_size[next_part]<max_size &&
				monte_carlo(rand)<std::pow(double(this_weight[next_part])/double(this_weight[this_part]),alpha)){
					cumu_gain*=double(this_weight[next_part])/this_weight[this_part];

					part_size[this_part]--;
					part_size[next_part]++;
					std::get<1>(this_node.val)=next_part;
					subparts[this_part].erase(this_site);
					subparts[next_part].insert(this_site);
					calc_weight(cut_part,this_node,this_weight,part);
					for(auto & eg:this_node.edges)
						calc_weight(cut_part,lat.nodes[eg.second.nbkey],weights[eg.second.nbkey],part);

					temp_subparts=subparts;
					further_gain=refine(lat,part,temp_subparts);

					if(cumu_gain*further_gain>max_gain){
						max_gain=cumu_gain*further_gain;
						best_subparts=temp_subparts;
					}
					++i;
				}

		}


		// for(auto & i: std::get<0>(lat["ten0_1"])->val ) std::cout<<i<<" **************6\n";
		// 	std::cout<<"finish **************6\n";

		if(contract_test_mode) lat.draw("lat after adjustment",best_subparts,true);

		if(contract_trace_mode) std::cout<<"out_refine \n";

		subparts = best_subparts;
	}


		struct set_contract{
			template<typename SetType>
			static SetType run(const SetType& a,const SetType& b){
				SetType result=a;
				result.insert(b.begin(),b.end());
				return result;
			}
		};

		template <typename T>
		net::tree<std::set<std::string>>* get_contract_tree(const tensor::TensorNetworkNoEnv<T> & lat, Engine & eg){

			net::network<net::tensor::Tensor<T>,int,std::string,stdEdgeKey> temp;

			temp= lat.template fmap<decltype(temp)>([](const net::tensor::Tensor<T> & ten){return ten;},[](const std::monostate & m){return 0;});

			temp.init_edges([](const typename decltype(temp)::NodeType & node1,const typename decltype(temp)::NodeType & node2,
				const std::string & ind1,const std::string & ind2){
				return  net::tensor::get_dim(node1.val,ind1);});

			net::network<net::tree<std::set<std::string>>*,int,std::string,stdEdgeKey> temp2;

			temp2= temp.template fmap<decltype(temp2)>([](const net::tensor::Tensor<T> & ten){return nullptr;},
				[](const int & m){return m;});

			for(auto &n :temp2.nodes)
				n.second.val=new net::tree<std::set<std::string>>({n.first});

			std::set<std::string> includes;
			for(auto & n:lat.nodes)
				includes.insert(n.first);
			return eg.contract<net::Tree_combine<set_contract>>(temp2,includes);
		}

}
#endif