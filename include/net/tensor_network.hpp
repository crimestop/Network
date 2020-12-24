#ifndef NET_TENSOR_NETWORK_HPP
#define NET_TENSOR_NETWORK_HPP

#include <TAT/TAT.hpp>
#include "network.hpp"
#include "tensor_tools.hpp"
#include "traits.hpp"
#include "tensor_contract.hpp"
#include <random>
#include <variant>
#include <functional>

namespace net{

	namespace tensor{
		// template <typename T,typename EdgeKey=stdEdgeKey>
		// using Tensor=TAT::Tensor<T,TAT::NoSymmetry,EdgeKey>;

		template <typename T,typename SiteKey=std::string,typename EdgeKey=stdEdgeKey>
		using TensorNetworkEnv=network<Tensor<T,EdgeKey>,Tensor<T,EdgeKey>,SiteKey,
			EdgeKey,default_traits<Tensor<T,EdgeKey>,Tensor<T,EdgeKey>,SiteKey,EdgeKey>>;
		template <typename T,typename SiteKey=std::string,typename EdgeKey=stdEdgeKey>
		using TensorNetworkNoEnv=network<Tensor<T,EdgeKey>,std::monostate,SiteKey,
			EdgeKey,default_traits<Tensor<T,EdgeKey>,std::monostate,SiteKey,EdgeKey>>;

		template <typename EdgeKey=stdEdgeKey>
		Tensor<double,EdgeKey> init_node_rand(const std::vector<EdgeKey> & str_inds,
			const unsigned int D,const double min,const double max,std::default_random_engine & R){

			auto distribution = std::uniform_real_distribution<double>(min,max);
			std::vector<unsigned int> dims(str_inds.size(),D);
			Tensor<double,EdgeKey> result(str_inds,{dims.begin(), dims.end()});
			return result.set([&distribution, &R]() { return distribution(R); });
		}

		template <typename T,typename EdgeKey=stdEdgeKey>
		Tensor<T,EdgeKey> init_node_rand_phy(const std::vector<EdgeKey> & str_inds,
			const unsigned int D,const unsigned int dphy,std::default_random_engine & R){

			auto distribution = std::uniform_real_distribution<double>(-1.,1.);
			const std::vector<EdgeKey> inds=str_inds
			std::vector<unsigned int> dims(str_inds.size(),D);
			dims.push_back(dphy);
			Tensor<T,EdgeKey> result(str_inds,{dims.begin(), dims.end()});
			if constexpr (std::is_same<T,double>)
				result.set([&distribution, &R]() { return distribution(R); });
			else if constexpr (std::is_same<T,std::complex<double>>)
				result.set([&distribution, &R]() { return std::complex<double>(distribution(R),distribution(R)); });
			return result;
		}

		template<typename T,typename EdgeKey=stdEdgeKey>
		Tensor<T,EdgeKey> init_edge_one(const unsigned int D, const EdgeKey & edge1, const EdgeKey & edge2){
			Tensor<T,EdgeKey> result({edge1,edge2},{D,D});
			result.zero();
			for(int i=0;i<D;++i){
				result.block()[i*(D+1)]=1.;
			}
		}

		template <typename T,typename EdgeKey=stdEdgeKey,typename V=Tensor<T,EdgeKey>>
		void default_dec(const Tensor<T,EdgeKey>& ten1,Tensor<T,EdgeKey>& ten2,Tensor<T,EdgeKey>& ten3,
			const std::set<EdgeKey> & inds,const EdgeKey & ind1,const EdgeKey & ind2,V& env){
			ten2=ten1;
			ten3=ten1;
		}

		struct tensor_absorb{
			template <typename T,typename EdgeKey=stdEdgeKey>
			static Tensor<T,EdgeKey> run(const Tensor<T,EdgeKey>& ten1,const Tensor<T,EdgeKey>& ten2,const EdgeKey & ind){
				return ten1.contract(ten2,{{ind,ten2.names[0]}}).edge_rename({{ten2.names[1],ind}});
			}
		};

		struct tensor_contract{
			template <typename T,typename EdgeKey=stdEdgeKey>
			static Tensor<T,EdgeKey> run(const Tensor<T,EdgeKey>& ten1,const Tensor<T,EdgeKey>& ten2,
				const std::set<std::pair<EdgeKey,EdgeKey>> & inds){
				return Tensor<T,EdgeKey>::contract(ten1,ten2,{inds.begin(),inds.end()});
			}
		};

		template <typename T,typename EdgeKey=stdEdgeKey>
		std::monostate zero_map(const Tensor<T,EdgeKey> &ten){
			return std::monostate();
		}

		inline std::string conjugate_string(const std::string & s){
			return "conjg_"+s;
		}
		inline std::function<std::string(const std::string &)> conjugate_string_fun=conjugate_string;

		inline std::monostate conjugate_mono(const std::monostate & m){
			return m;
		}
		inline std::function<std::monostate(const std::monostate &)> conjugate_mono_fun=conjugate_mono;

		template <typename T>
		Tensor<T> conjugate_tensor(const Tensor<T> & t){
			std::map<std::string,std::string> name_map;
			for (auto & m:t.names){
				name_map[m]=conjugate_string(m);
			}
			return t.conjugate().edge_rename(name_map);
		}
		template <typename T>
		std::function<Tensor<T>(const Tensor<T> &)> conjugate_tensor_fun=conjugate_tensor<T>;
		
		template <typename T>
		TensorNetworkEnv<T> conjugate_tnenv(const TensorNetworkEnv<T> & t){
			return t.template fmap<TensorNetworkEnv<T>>(conjugate_tensor_fun<T>,conjugate_tensor_fun<T>,conjugate_string_fun,conjugate_string_fun);
		}
		template <typename T>
		TensorNetworkNoEnv<T> conjugate_tnnoenv(const TensorNetworkNoEnv<T> & t){
			return t.template fmap<TensorNetworkNoEnv<T>>(conjugate_tensor_fun<T>,conjugate_mono_fun,conjugate_string_fun,conjugate_string_fun);
		}
		template <typename T>
		TensorNetworkEnv<T> double_tnenv(const TensorNetworkEnv<T> & t){
			TensorNetworkEnv<T> result=conjugate_tnenv(t);
			result.add(t);
			return result;
		}
		template <typename T>
		TensorNetworkNoEnv<T> double_tnnoenv(const TensorNetworkNoEnv<T> & t){
			TensorNetworkNoEnv<T> result=conjugate_tnnoenv(t);
			result.add(t);
			return result;
		}

		template <typename Network>
		typename Network::NodeValType contract_quickbb(const Network & n){
			typename Network::NodeValType result;
			Engine eg;
			auto ctree = net::get_contract_tree_qbb<net::keyset>(n,eg);
			result=n.template contract_tree<no_absorb,tensor::tensor_contract>(ctree);
			delete ctree;
			return result;
		}

	}
}

#endif
