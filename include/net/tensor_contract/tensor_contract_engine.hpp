#ifndef NET_TENSOR_CONTRACT_ENGINE_HPP
#define NET_TENSOR_CONTRACT_ENGINE_HPP
#include "../network.hpp"
#include "../rational.hpp"
#include "../tensor_tools.hpp"
#include "tensor_contract_tools.hpp"
#include "../traits.hpp"
#include "../group.hpp"
#include <TAT/TAT.hpp>
#include <cstdlib>
#include <functional>
#include <random>
#include <variant>
#include <memory>
#include <vector>
#include <iostream>

namespace net {
	inline bool contract_test_mode = false;
	inline bool contract_trace_mode = false;

	class Engine {
	public:
		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeVal contract_part(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);
		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeVal contract_qbb(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);
		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeVal contract_exact(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);
		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeVal contract_naive(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);

		int coarse_grain_to = 800;
		int cut_part = 2;
		int refine_sweep = 10000;
		int small_part_size = 20;
		bool rec_partition=false;
		bool verbose=false;
		std::string small_part_method="quickbb";

		//double uneven = 0.2;
		std::default_random_engine rand = std::default_random_engine(0);
		// std::default_random_engine
		// rand=std::default_random_engine(std::random_device()());

	private:
		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey contract_breadth_first(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);

		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey inner_contract_part(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);

		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey connected_inner_contract_part(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &,
				double,bool,std::string);

		template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		NodeKey contract_quickbb(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const absorb_type &,
				const contract_type &);


		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		std::vector<std::set<NodeKey, typename Trait::nodekey_less>>
		divide(network<NodeVal, int, NodeKey, EdgeKey, Trait> &, const std::set<NodeKey, typename Trait::nodekey_less> &);

		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		std::vector<std::set<NodeKey, typename Trait::nodekey_less>>
		divide_kahypar(network<NodeVal, int, NodeKey, EdgeKey, Trait> &, const std::set<NodeKey, typename Trait::nodekey_less> &,double,bool&);

		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		double refine(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				std::vector<std::set<NodeKey, typename Trait::nodekey_less>> &);

		template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
		void adjust(
				network<NodeVal, int, NodeKey, EdgeKey, Trait> &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				std::vector<std::set<NodeKey, typename Trait::nodekey_less>> &,
				double);
	};


	template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeVal Engine::contract_part(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		network<std::tuple<NodeVal, int, net::rational>, int, NodeKey, EdgeKey, Trait> temp;
		temp = lat.template fmap<decltype(temp)>(
				[](const NodeVal & ten) { return std::make_tuple(ten, 0, net::rational(0, 1)); }, [](const int & m) { return m; });
		std::string final_site = inner_contract_part(temp, part, lift_absorb(absorb_fun), lift_contract(contract_fun));
		return std::get<0>(temp[final_site].val);
	}
	template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeVal Engine::contract_qbb(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		auto temp_small_part_size=small_part_size;
		auto temp_small_part_method=small_part_method;
		small_part_size=lat.size();
		small_part_method="quickbb";
		auto res=contract_part(lat,part,absorb_fun,contract_fun);
		small_part_size=temp_small_part_size;
		small_part_method=temp_small_part_method;
		return res;
	}
	template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeVal Engine::contract_exact(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		auto temp_small_part_size=small_part_size;
		auto temp_small_part_method=small_part_method;
		small_part_size=lat.size();
		small_part_method="exact";
		auto res=contract_part(lat,part,absorb_fun,contract_fun);
		small_part_size=temp_small_part_size;
		small_part_method=temp_small_part_method;
		return res;
	}
	template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeVal Engine::contract_naive(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		network<std::tuple<NodeVal, int, net::rational>, int, NodeKey, EdgeKey, Trait> temp;
		temp = lat.template fmap<decltype(temp)>(
				[](const NodeVal & ten) { return std::make_tuple(ten, 0, net::rational(0, 1)); }, [](const int & m) { return m; });
		return std::get<0>(temp.contract(part,lift_absorb(absorb_fun), lift_contract(contract_fun)));
	}

	template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeKey Engine::inner_contract_part(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {

		auto parts=disconnect(lat,part,{part});
		for(auto & p:parts)
			connected_inner_contract_part(lat, p, absorb_fun, contract_fun,0.,false," ");
		// for(auto & s:lat){
		// 	std::cout<<s.first<<'\n';
		// }
		// 	std::cout<<"---\n";
		auto s0=lat.begin()->first;
		std::set<std::string> sites;
		for(auto & s:lat)
			if(s.first !=s0)
				sites.insert(s.first);
		for(auto & s:sites)
			lat.absorb(s0,s,absorb_fun, contract_fun);
		return lat.begin()->first;

	}

	template <typename contract_type, typename absorb_type, typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	NodeKey Engine::connected_inner_contract_part(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun,double uneven, bool fix_uneven,std::string log) {

		//std::cout<<"in inner"<<std::endl;
		NodeKey contract_res1,contract_res2;
		
		double new_eneven,best_eneven,min_cost,this_cost;
		//network<NodeVal, int, NodeKey, EdgeKey, Trait>  temp2 = lat;

		bool failed;
		//std::cout<<max_quickbb_size<<','<<part.size()<<'\n';
		if (part.size() > small_part_size) {
			if(!fix_uneven){
				min_cost=-1.;
				for(int j=0;j<50;++j){
					NodeKey contract_res;
					network<NodeVal, int, NodeKey, EdgeKey, Trait>  temp = lat;
					new_eneven=0.02*j;
					//std::cout<<"part = "<<eg.cut_part<<" uneven = "<<eg.uneven<<'\n';
					std::vector<std::set<NodeKey, typename Trait::nodekey_less>> subparts = divide_kahypar(temp, part,new_eneven,failed);

					// std::cout<<"portal1 "<<new_eneven;
					// for (auto & p : subparts)
					// 	std::cout<<' '<<p.size();
					// std::cout<<std::endl;
					
					std::set<NodeKey, typename Trait::nodekey_less> new_part;
					for (auto & p : subparts){
						new_part.insert(connected_inner_contract_part(temp, p, absorb_fun, contract_fun,new_eneven,true," "));
					}
					contract_res=contract_quickbb(temp, new_part, absorb_fun, contract_fun);
					this_cost=std::get<0>(temp[contract_res].val)->val.contraction_cost;
					//std::cout<<new_eneven<<' '<<this_cost<<'\n';
					if(min_cost<0 || this_cost<min_cost){
						min_cost=this_cost;
						best_eneven=new_eneven;
					}
					if(failed) break;
				}
				if(verbose) std::cout<<log<<' '<<best_eneven<<' '<<min_cost<<'\n';
				std::vector<std::set<NodeKey, typename Trait::nodekey_less>> subparts = divide_kahypar(lat, part,best_eneven,failed);
				std::set<NodeKey, typename Trait::nodekey_less> new_part;
				int sp=0;
				for (auto & p : subparts){
					new_part.insert(connected_inner_contract_part(lat, p, absorb_fun, contract_fun,best_eneven,!rec_partition,log+std::to_string(sp)));
					sp++;
				}
				//std::cout<<"out inner"<<std::endl;
				return contract_quickbb(lat, new_part, absorb_fun, contract_fun);

			}else{
				std::vector<std::set<NodeKey, typename Trait::nodekey_less>> subparts = divide_kahypar(lat, part,uneven,failed);
				std::set<NodeKey, typename Trait::nodekey_less> new_part;

					// std::cout<<"portal2 "<<part.size()<<subparts.size()<<uneven;
					// for (auto & p : subparts)
					// 	std::cout<<' '<<p.size();
					// std::cout<<std::endl;
				for (auto & p : subparts)
					new_part.insert(connected_inner_contract_part(lat, p, absorb_fun, contract_fun,uneven,true," "));
				//std::cout<<"out inner"<<std::endl;
				return contract_quickbb(lat, new_part, absorb_fun, contract_fun);

			}

		} else {
			//std::cout<<"out inner"<<std::endl;
			if(!fix_uneven){
				if(small_part_method=="exact"){
					return contract_breadth_first(lat, part, absorb_fun, contract_fun);
				}
				else{
					return contract_quickbb(lat, part, absorb_fun, contract_fun);
				}
			}else{
				return contract_quickbb(lat, part, absorb_fun, contract_fun);
			}
		}
	}


} // namespace net
#endif
