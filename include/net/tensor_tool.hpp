#ifndef TENSOR_TOOLS_H
#define TENSOR_TOOLS_H

#include <TAT/TAT.hpp>
#include "network.hpp"
#include <random>

namespace net{
	namespace tensor{
		template <typename T>
		using Tensor=TAT::Tensor<T,TAT::NoSymmetry,std::string>;

		Tensor<double> init_site_rand(const std::vector<std::string> & str_inds, const unsigned int D,const double min,const double max,std::default_random_engine & R){

			auto distribution = std::uniform_real_distribution<double>(min,max);
			std::vector<unsigned int> dims;
			for (auto& ind:str_inds){
				dims.push_back(D);
			}
			Tensor<double> result(str_inds,{dims.begin(), dims.end()});
			return result.set([&distribution, &R]() { return distribution(R); });
		}

		template<typename T>
		int get_dim(Tensor<T> ten, int s){
			return ten->core.edges[s].map[TAT::NoSymmetry()];
		}

		template<typename T>
		int get_dim(Tensor<T> ten, std::string s){
			return ten->core.edges[ten.name_to_index[s]].map[TAT::NoSymmetry()];
		}

		template<typename T>
		Tensor<T> init_bond_one(const unsigned int D){

			Tensor<T> result({"bond.in","bond.out"},{D,D});
			result.zero();
			for(int i=0;i<D;++i){
				result[{i,i}]=1l;
			}
		}

		template <typename T,typename V=Tensor<T>>
		void default_dec(Tensor<T>& ten1,Tensor<T>& ten2,Tensor<T>& ten3,const std::set<std::string> & inds,const std::string & ind1,const std::string & ind2,V& env){
			ten2=ten1;
			ten3=ten1;
		}

		template <typename T>
		void absorb(Tensor<T>& ten1,Tensor<T>& ten2,const std::string & ind){
			ten1.contract(ten2,{{ind,"bond.in"}}).edge_rename({{"bond.out",ind}});
		}

		template <typename T,typename V=Tensor<T>>
		void no_absorb(Tensor<T>& ten1,V& ten2,const std::string & ind){ }

		template <typename T>
		Tensor<T> contract(Tensor<T>& ten1,Tensor<T>& ten2,const std::set<std::pair<std::string,std::string>> & inds){
			return Tensor<T>::contract(ten1,ten2,{inds.begin(), inds.end()});
		}

		template <typename T>
		int zero_map(const Tensor<T> &ten){
			return 0;
		}
	}
}

#endif
