#ifndef NET_TENSOR_CONTRACT_TOOLS_HPP
#define NET_TENSOR_CONTRACT_TOOLS_HPP
#include "../network.hpp"
#include "../rational.hpp"
#include "../tensor_tools.hpp"
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

	template <typename contract_type>
	struct lift_contract {
		contract_type contract_fun;
		lift_contract(contract_type cf) : contract_fun(cf){};
		template <typename NodeVal, typename NoUse>
		NodeVal operator()(const NodeVal & ten1, const NodeVal & ten2, const NoUse & inds) const {
			return std::make_tuple(contract_fun(std::get<0>(ten1), std::get<0>(ten2), inds), std::get<1>(ten1), std::get<2>(ten1));
		}
	};

	template <typename absorb_type>
	struct lift_absorb {
		absorb_type absorb_fun;
		lift_absorb(absorb_type af) : absorb_fun(af){};
		template <typename NodeVal, typename EdgeVal, typename NoUse>
		NodeVal operator()(const NodeVal & ten1, const EdgeVal & eg, const NoUse & ind) const {
			return std::make_tuple(absorb_fun(std::get<0>(ten1), eg, ind), std::get<1>(ten1), std::get<2>(ten1));
		}
	};


	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void get_component(
			node<NodeVal, int, NodeKey, EdgeKey, Trait> & n,
			const NodeKey & p,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			std::set<NodeKey, typename Trait::nodekey_less> & treated,
			std::set<NodeKey, typename Trait::nodekey_less> & component) {
		treated.insert(p);
		component.insert(p);
		for (auto & eg : n.edges) //???
			if (part.count(eg.second.nbkey) > 0 && treated.count(eg.second.nbkey) == 0 && std::get<1>(n.val) == std::get<1>(eg.second.nbitr->second.val))
				get_component(eg.second.nbitr->second, eg.second.nbkey, part, treated, component);
	}

	template <typename NodeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::vector<std::set<NodeKey, typename Trait::nodekey_less>> disconnect(
			network<NodeVal, int, NodeKey, EdgeKey, Trait> & lat,
			const std::set<NodeKey, typename Trait::nodekey_less> & part,
			const std::vector<std::set<NodeKey, typename Trait::nodekey_less>> & subparts) {
		for (int i = 0; i < subparts.size(); ++i)
			for (auto & s : subparts[i])
				std::get<1>(lat[s].val) = i;

		std::set<NodeKey, typename Trait::nodekey_less> treated, newsubpart;
		std::vector<std::set<NodeKey, typename Trait::nodekey_less>> newsubparts;
		while (treated.size() < part.size())
			for (auto & p : part)
				if (treated.count(p) == 0) {
					newsubparts.push_back({});
					get_component(lat[p], p, part, treated, newsubparts.back());
				}

		return newsubparts;
	}


	template <typename KeySetType>
	struct keyset {
		KeySetType node_set;
		static keyset<KeySetType> absorb(const keyset<KeySetType> & a, const int & b) {
			return a;
		};
		static keyset<KeySetType> contract(const keyset<KeySetType> & a, const keyset<KeySetType> & b) {
			keyset<KeySetType> r = a;
			r.node_set.insert(b.node_set.begin(), b.node_set.end());
			return r;
		}
		keyset() = default;
		template <typename NodeType>
		keyset(const typename KeySetType::key_type & k, const NodeType & n) {
			node_set.insert(k);
		}
		std::string show()const{
			return "";
		}
		keyset<KeySetType> forget_history(const typename KeySetType::key_type & k) {
			keyset<KeySetType> ci;
			ci.node_set.insert(k);
			return ci;
		}
	};

	template <typename KeySetType>
	struct contract_info {
		KeySetType node_set;
		long long int this_weight = 1; // record the size of the tensor
		long long int hist_max_weight = 1; // maximal space cost
		long long int contraction_cost = 1; // total time cost
		long long int legs = 1; // record the total dimension of common legs
		static contract_info<KeySetType> absorb(contract_info<KeySetType> & c, const int & d) {
			contract_info<KeySetType> r = c;
			r.legs *= d;
			return r;
		};
		static contract_info<KeySetType> contract(contract_info<KeySetType> & c, contract_info<KeySetType> & d) {
			contract_info<KeySetType> r;
			r.legs = 1;
			r.node_set = c.node_set;
			r.node_set.insert(d.node_set.begin(), d.node_set.end());
			r.this_weight = c.this_weight / c.legs / d.legs * d.this_weight / c.legs / d.legs;
			r.contraction_cost = c.contraction_cost + d.contraction_cost + c.this_weight / c.legs / d.legs * d.this_weight;
			r.hist_max_weight = std::max(std::max(c.this_weight, d.this_weight), r.this_weight);
			c.legs=1;
			d.legs=1;
			return r;
		}
		std::string show()const{
			std::ostringstream os;
			os.precision(4);
			os<<std::fixed<<"C "<<contraction_cost<<"\nW "<<this_weight<<"\nH "<<hist_max_weight;
			return os.str();
		}
		contract_info() = default;
		template <typename NodeType>
		contract_info(const typename KeySetType::key_type & k, const NodeType & n) {
			node_set.insert(k);
			this_weight = net::tensor::get_size(n);
			hist_max_weight = this_weight;
		}
		contract_info<KeySetType> forget_history(const typename KeySetType::key_type & k) {
			contract_info<KeySetType> ci;
			ci.node_set.insert(k);
			ci.this_weight = this_weight;
			ci.hist_max_weight = hist_max_weight;
			ci.contraction_cost = contraction_cost;
			ci.legs = legs;
			return ci;
		}
	};

	constexpr double exp_sum_log(const double & a,
										  const double & b) { // return log(exp(a)+exp(b))
		double ratio = 0.;
		if (a > b + 10) {
			ratio = std::pow(10,b - a);
			return a + ratio - ratio * ratio / 2. + ratio * ratio * ratio / 3.;
		} else if (b > a + 10) {
			ratio = std::pow(10,a - b);
			return b + ratio - ratio * ratio / 2. + ratio * ratio * ratio / 3.;
		} else
			return b + std::log10(1 + std::pow(10,a - b));
	}

	// a logged version of contract_info
	template <typename KeySetType>
	struct contract_info2 {
		KeySetType node_set;
		double this_weight = 0.;
		double hist_max_weight = 0.;
		double contraction_cost = 0.;
		double legs = 0.;
		static contract_info2<KeySetType> absorb(contract_info2<KeySetType> & c, const int & d) {
			contract_info2<KeySetType> r = c;
			r.legs += std::log10(double(d));
			return r;
		}
		static contract_info2<KeySetType> contract(contract_info2<KeySetType> & c, contract_info2<KeySetType> & d) {
			contract_info2<KeySetType> r;
			r.legs = 0.;
			r.node_set = c.node_set;
			// std::cout<<"before\n";
			// for (auto & test: d.node_set){
			// 	std::cout<<'\n';
			// 	std::cout<<test<<'\n';
			// } 
			r.node_set.insert(d.node_set.begin(), d.node_set.end());

			//std::cout<<"after\n";
			r.this_weight = c.this_weight + d.this_weight - 2 * c.legs - 2 * d.legs;
			r.contraction_cost = exp_sum_log(exp_sum_log(c.contraction_cost, d.contraction_cost), c.this_weight - c.legs - d.legs + d.this_weight);
			r.hist_max_weight = std::max(std::max(c.this_weight, d.this_weight), r.this_weight);
			c.legs=0.;
			d.legs=0.;
			return r;
		}
		std::string show()const{
			std::ostringstream os;
			os.precision(4);
			os<<std::fixed<<"C "<<contraction_cost<<"\nW "<<this_weight<<"\nH "<<hist_max_weight;
			return os.str();
		}
		contract_info2() = default;
		template <typename NodeType>
		contract_info2(const typename KeySetType::key_type & k, const NodeType & n) {
			node_set.insert(k);
			this_weight = std::log10(double(net::tensor::get_size(n)));
			hist_max_weight = this_weight;
		}
		contract_info2<KeySetType> forget_history(const typename KeySetType::key_type & k) {
			contract_info2<KeySetType> ci;
			ci.node_set.insert(k);
			ci.this_weight = this_weight;
			ci.hist_max_weight = hist_max_weight;
			ci.contraction_cost = contraction_cost;
			ci.legs = legs;
			return ci;
		}
	};


} // namespace net
#endif
