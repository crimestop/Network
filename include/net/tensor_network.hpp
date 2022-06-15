#ifndef NET_TENSOR_NETWORK_HPP
#define NET_TENSOR_NETWORK_HPP

#include "network.hpp"
#include "tensor_contract/tensor_contract_engine.hpp"
#include "tensor_contract/tensor_contract_tools.hpp"
#include "tensor_tools.hpp"
#include "traits.hpp"
#include "tree.hpp"
#include <TAT/TAT.hpp>
#include <functional>
#include <random>
#include <type_traits>
#include <variant>

namespace net {

	namespace tensor {
		// template <typename T,typename EdgeKey=stdEdgeKey>
		// using Tensor=TAT::Tensor<T,TAT::NoSymmetry,EdgeKey>;

		template <typename T, typename SiteKey = std::string, typename EdgeKey = stdEdgeKey>
		using TensorNetworkEnv = network<
				Tensor<T, EdgeKey>,
				Tensor<T, EdgeKey>,
				SiteKey,
				EdgeKey,
				default_traits<Tensor<T, EdgeKey>, Tensor<T, EdgeKey>, SiteKey, EdgeKey>>;
		template <typename T, typename SiteKey = std::string, typename EdgeKey = stdEdgeKey>
		using TensorNetworkNoEnv =
				network<Tensor<T, EdgeKey>, std::monostate, SiteKey, EdgeKey, default_traits<Tensor<T, EdgeKey>, std::monostate, SiteKey, EdgeKey>>;

		template <typename NetType>
		typename NetType::NodeValType init_node_rand(
				const typename NetType::NodeType & this_node,
				const unsigned int D,
				const double min,
				const double max,
				std::default_random_engine & R) {
			auto distribution = std::uniform_real_distribution<double>(min, max);
			std::vector<unsigned int> dims(this_node.edges.size(), D);
			std::vector<typename NetType::EdgeKeyType> inds;
			for(auto & b: this_node.edges){
				inds.push_back(b.first);
			}
			//typename NetType::NodeValType result(inds, {dims.begin(), dims.end()});
			typename NetType::NodeValType result(inds,std::vector<TAT::Edge<TAT::NoSymmetry>>(dims.begin(), dims.end()));
			return result.set([&distribution, &R]() { return distribution(R); });
		}

		template <typename NetType>
		typename NetType::NodeValType
		init_node_rand_phy(const typename NetType::NodeType & this_node, const unsigned int D, const unsigned int dphy, std::default_random_engine & R) {
			auto distribution = std::uniform_real_distribution<double>(-1., 1.);
			std::vector<typename NetType::EdgeKeyType> inds;
			for (auto & b : this_node.edges) {
				inds.push_back(b.first);
			}
			std::vector<unsigned int> dims(this_node.edges.size(), D);
			inds.push_back(this_node.key + ".phy");
			dims.push_back(dphy);
			//typename NetType::NodeValType result(inds, {dims.begin(), dims.end()});
			typename NetType::NodeValType result(inds, std::vector<TAT::Edge<TAT::NoSymmetry>>(dims.begin(), dims.end()));

			if constexpr (std::is_same_v<typename NetType::NodeValType::scalar_t, double>)
				result.set([&distribution, &R]() { return distribution(R); });
			else if constexpr (std::is_same_v<typename NetType::NodeValType::scalar_t, std::complex<double>>)
				result.set([&distribution, &R]() { return std::complex<double>(distribution(R), distribution(R)); });
			return result;
		}

		template <typename NetType>
		typename NetType::EdgeValType init_edge_one(const typename NetType::NodeType & this_node, const typename NetType::EdgeKeyType & edge1, const unsigned int D) {
			auto egit = this_node.edges.find(edge1);
			if(egit->second.nb_num!=0){
				typename NetType::EdgeValType result({edge1, egit->second.nbind}, {D, D});
				result.zero();
				for (int i = 0; i < D; ++i) {
					result.block()[i * (D + 1)] = 1.;
				}
				return result;
			}else 
				return typename NetType::EdgeValType();
		}
		template <typename NetType>
		typename NetType::EdgeValType init_edge_null(const typename NetType::NodeType & this_node, const typename NetType::EdgeKeyType & edge1) {
			return typename NetType::EdgeValType();
		}

		struct default_dec {
			template <typename TensorType, typename EdgeKey, typename EdgeKeySet, typename EdgeVal>
			void operator()(
					const TensorType & ten1,
					TensorType & ten2,
					TensorType & ten3,
					const EdgeKeySet & inds,
					const EdgeKey & ind1,
					const EdgeKey & ind2,
					EdgeVal & env) const {
				ten2 = ten1;
				ten3 = ten1;
			}
		};

		struct qr {
			template <typename TensorType, typename EdgeKey, typename EdgeKeySet, typename EdgeVal>
			void operator()(
					const TensorType & ten1,
					TensorType & ten2,
					TensorType & ten3,
					const EdgeKeySet & inds,
					const EdgeKey & ind1,
					const EdgeKey & ind2,
					EdgeVal & env) const {
				auto qr_res = ten1.qr('R', inds, ind1, ind2);
				ten2 = qr_res.Q;
				ten3 = qr_res.R;
			}
		};

		struct auto_qr {
			template <typename TensorType, typename EdgeKey, typename EdgeKeySet, typename EdgeVal>
			void operator()(
					const TensorType & ten1,
					TensorType & ten2,
					TensorType & ten3,
					const EdgeKeySet & inds,
					const EdgeKey & ind1,
					const EdgeKey & ind2,
					EdgeVal & env) const {


				auto svd_res = ten1.svd(inds, ind2, ind1,"SVD_U","SVD_V", TAT::RelativeCut(1e-5));

				ten3 = svd_res.U.contract(svd_res.S, {{ind2, "SVD_U"}}).edge_rename({{"SVD_V",ind2}});
				ten2 = svd_res.V;

				auto final = ten3.contract(ten2,{{ind2,ind1}});
			}
		};


		struct svd {
			int Dc = -1;
			svd() = default;
			svd(int d) : Dc(d){};
			template <typename TensorType, typename EdgeKey, typename EdgeKeySet, typename EdgeVal>
			void operator()(
					const TensorType & ten1,
					TensorType & ten2,
					TensorType & ten3,
					const EdgeKeySet & inds,
					const EdgeKey & ind1,
					const EdgeKey & ind2,
					EdgeVal & env) const {
				auto svd_res = ten1.svd(inds, ind2, ind1, Dc);
				ten3 = svd_res.U;
				ten2 = svd_res.V;
				env = svd_res.S;
			}
		};

		struct svd2 {
			int Dc = -1;
			svd2() = default;
			svd2(int d) : Dc(d){};
			template <typename TensorType, typename EdgeKey, typename EdgeKeySet, typename EdgeVal>
			void operator()(
					const TensorType & ten1,
					TensorType & ten2,
					TensorType & ten3,
					const EdgeKeySet & inds,
					const EdgeKey & ind1,
					const EdgeKey & ind2,
					EdgeVal & env) const {
				auto svd_res = ten1.svd(inds, ind2, ind1, Dc, ind2, ind1);
				ten3 = svd_res.U;
				ten2 = svd_res.V;
				// std::cout<<"<this test\n";
				// std::cout<<"ten1\n";
				// diminfo(ten1,std::cout);
				// std::cout<<"inds\n";
				// for(auto a: inds) std::cout<<a<<'\n';
				// std::cout<<"ind1\n";
				// std::cout<<ind1<<'\n';
				// std::cout<<"ind2\n";
				// std::cout<<ind2<<'\n';
				// std::cout<<"ten3\n";
				// diminfo(ten3,std::cout);
				// std::cout<<"ten2\n";
				// diminfo(ten2,std::cout);
				env = svd_res.S;
				env /= env.template norm<-1>();
				// std::cout<<'\n';
				// std::cout<<env<<"\n";
				// std::cout<<ten2<<"\n";
				// std::cout<<ten3<<"\n";
				ten2 = ten2.contract(env, {{ind1, ind2}});
				ten3 = ten3.contract(env, {{ind2, ind1}});

				// std::cout<<ten2<<"\n";
				// std::cout<<ten3<<"\n";
				int D = get_dim(env, 0);
				for (int i = 0; i < D * D; i += D + 1)
					env.block()[i] = 1. / env.block()[i];
				// std::cout<<env<<"\n";
				// std::cout<<"this test>\n";
			}
		};

		struct absorb {
			template <typename TensorType, typename EdgeKey>
			TensorType operator()(const TensorType & ten1, const TensorType & ten2, const EdgeKey & ind) const {
				return ten1.contract(ten2, {{ind, ten2.names[0]}}).edge_rename({{ten2.names[1], ind}});
			}
		};

		struct contract {
			template <typename TensorType, typename IndType>
			TensorType operator()(const TensorType & ten1, const TensorType & ten2, const IndType & inds) const {
				return ten1.contract(ten2, {inds.begin(), inds.end()});
			}
		};

		template <typename T, typename EdgeKey = stdEdgeKey>
		std::monostate zero_map(const Tensor<T, EdgeKey> & ten) {
			return std::monostate();
		}

		inline std::string conjugate_string(const std::string & s) {
			return "conjg_" + s;
		}
		inline std::function<std::string(const std::string &)> conjugate_string_fun = conjugate_string;

		inline std::monostate conjugate_mono(const std::monostate & m) {
			return m;
		}
		inline std::function<std::monostate(const std::monostate &)> conjugate_mono_fun = conjugate_mono;

		template <typename T>
		Tensor<T> conjugate_tensor(const Tensor<T> & t) {
			std::unordered_map<std::string, std::string> name_map;
			for (auto & m : t.names) {
				name_map[m] = conjugate_string(m);
			}
			return t.conjugate().edge_rename(name_map);
		}
		template <typename T>
		std::function<Tensor<T>(const Tensor<T> &)> conjugate_tensor_fun = conjugate_tensor<T>;

		template <typename T>
		TensorNetworkEnv<T> conjugate_tnenv(const TensorNetworkEnv<T> & t) {
			return t.template fmap<TensorNetworkEnv<T>>(conjugate_tensor_fun<T>, conjugate_tensor_fun<T>, conjugate_string_fun, conjugate_string_fun);
		}
		template <typename T>
		TensorNetworkNoEnv<T> conjugate_tnnoenv(const TensorNetworkNoEnv<T> & t) {
			return t.template fmap<TensorNetworkNoEnv<T>>(conjugate_tensor_fun<T>, conjugate_mono_fun, conjugate_string_fun, conjugate_string_fun);
		}
		template <typename T>
		TensorNetworkEnv<T> double_tnenv(const TensorNetworkEnv<T> & t) {
			TensorNetworkEnv<T> result = conjugate_tnenv(t);
			result.add(t);
			return result;
		}
		template <typename T>
		TensorNetworkNoEnv<T> double_tnnoenv(const TensorNetworkNoEnv<T> & t) {
			TensorNetworkNoEnv<T> result = conjugate_tnnoenv(t);
			result.add(t);
			return result;
		}
		template <template <typename> typename TreeVal, typename NetType, typename Engine>
		std::shared_ptr<net::tree<TreeVal<typename NetType::NodeKeySetType>>> get_contract_tree(const NetType & lat, Engine & eg, const std::string & method) {
			net::network<std::shared_ptr<net::tree<TreeVal<typename NetType::NodeKeySetType>>>, int, typename NetType::NodeKeyType, typename NetType::EdgeKeyType> temp;


			temp = lat.template gfmap<typename decltype(temp)::NodeValType, typename decltype(temp)::EdgeValType, typename decltype(temp)::TraitType>(
					[](const typename NetType::NodeType & node) {
						return std::make_shared<net::tree<TreeVal<typename NetType::NodeKeySetType>>>(TreeVal<typename NetType::NodeKeySetType>(node.key, node.val));
					},
					[](const typename NetType::NodeType & node1,
						const typename NetType::EdgeKeyType & ind1) { return net::tensor::get_dim(node1.val, ind1); });

			std::set<std::string> includes;
			for (auto & n : lat)
				includes.insert(n.first);
			if(method=="partition"){
				return eg.contract_part(
						temp,
						includes,
						net::Tree_act<TreeVal<typename NetType::NodeKeySetType>>(),
						net::Tree_combine<TreeVal<typename NetType::NodeKeySetType>>());
			}else if(method=="quickbb"){
				return eg.contract_qbb(
						temp,
						includes,
						net::Tree_act<TreeVal<typename NetType::NodeKeySetType>>(),
						net::Tree_combine<TreeVal<typename NetType::NodeKeySetType>>());
			}else if(method=="exact"){
				return eg.contract_exact(
						temp,
						includes,
						net::Tree_act<TreeVal<typename NetType::NodeKeySetType>>(),
						net::Tree_combine<TreeVal<typename NetType::NodeKeySetType>>());
			}else if(method=="naive"){
				return eg.contract_naive(
						temp,
						includes,
						net::Tree_act<TreeVal<typename NetType::NodeKeySetType>>(),
						net::Tree_combine<TreeVal<typename NetType::NodeKeySetType>>());
			}else{
				return std::shared_ptr<net::tree<TreeVal<typename NetType::NodeKeySetType>>>(); // null ptr
			}
		}

		template <typename Network>
		typename Network::NodeValType contract_tn(const Network & n,Engine & eg, const std::string & method) {
			typename Network::NodeValType result;
			auto ctree = get_contract_tree<contract_info2>(n, eg,method);
			result = n.template contract_tree(ctree, no_absorb(), contract());

			//std::cout<<result;
			return result;
		}


	} // namespace tensor
} // namespace net

#endif
