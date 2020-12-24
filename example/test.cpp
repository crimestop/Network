#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include <TAT/TAT.hpp>
#include <net/net.hpp>
#include <net/tree.hpp>
#include "timer.h"
#include <chrono>
#include <algorithm>
#include <net/tensor_contract.hpp>
#include <net/tensor_network.hpp>
#define str std::to_string


int main(){

	// net::tree<std::set<std::string>> tree1({"A"});
	// net::tree<std::set<std::string>> tree2({"B"});

	// net::tree<std::set<std::string>> * tree3= net::Tree_combine<set_contract>::run(&tree1,&tree2,1);
	// //net::tree<std::set<std::string>> * tree3= new net::tree<std::set<std::string>>(set_contract::run(tree1.val,tree2.val),&tree1,&tree2);

	// for(auto & i: tree3->val ) std::cout<<i<<" **************\n";
	// 	std::cout<<"finish **************\n";

	int L1=6,L2=6,dim=4;

	using namespace std::placeholders;
	int seed =std::random_device()();
	std::default_random_engine random_engine(seed);


	net::tensor::TensorNetworkNoEnv<double> lat2;
	for(int i=0;i<L1;++i){
		for (int j=0;j<L2;++j){
			//std::cout<<i<<j<<"\n";
			lat2.add("ten"+str(i)+"_"+str(j));
			lat2["ten"+str(i)+"_"+str(j)].val=1;
		}
	}
	for(int i=0;i<L1;++i){
		for (int j=0;j<L2-1;++j){
			//std::cout<<i<<j<<std::endl;
			lat2.set_edge("ten"+str(i)+"_"+str(j),"ten"+str(i)+"_"+str(j+1));
		}
	}
	for(int i=0;i<L1-1;++i){
		for (int j=0;j<L2;++j){
			lat2.set_edge("ten"+str(i)+"_"+str(j),"ten"+str(i+1)+"_"+str(j));
		}
	}

	// std::tuple<net::tensor::Tensor<double>,int,int> transform(const net::tensor::Tensor<double> & ten){
	// 	return std::make_tuple(ten,0,0);
	// }

	//lat2.draw(true);

	lat2.init_nodes(std::bind(net::tensor::init_node_rand<net::stdEdgeKey>, _1,dim,-1.,1.,std::ref(random_engine)));

	net::Engine eg;

	// auto  temp = net::fmap<net::tensor::Tensor<double>,std::monostate,std::tuple<net::tensor::Tensor<double>,int,int>,int,
	// typename decltype(lat2)::NodeKeyType,typename decltype(lat2)::EdgeKeyType,typename decltype(lat2)::TraitType>
	// (lat2,[](const net::tensor::Tensor<double> & ten){return std::make_tuple(ten,0,0);},[](const std::monostate & m){return 0;});

	// net::network<typename decltype(lat2)::NodeValType,int,typename decltype(lat2)::NodeKeyType,typename decltype(lat2)::EdgeKeyType> temp;

	// temp= lat2.fmap<decltype(temp)>([](const net::tensor::Tensor<double> & ten){return ten;},[](const std::monostate & m){return 0;});

	// temp.init_edges([](const typename decltype(temp)::NodeType & node1,const typename decltype(temp)::NodeType & node2,
	// 	const std::string & ind1,const std::string & ind2){
	// 	return  net::tensor::get_dim(node1.val,ind1);});

	// net::network<net::tree<typename decltype(temp)::NodeKeySetType>*,int,typename decltype(temp)::NodeKeyType,typename decltype(temp)::EdgeKeyType> temp2;

	// temp2= temp.fmap<decltype(temp2)>([](const net::tensor::Tensor<double> & ten){return nullptr;},
	// 	[](const int & m){return m;});

	// for(auto &n :temp2.nodes)
	// 	n.second.val=new net::tree<typename decltype(temp)::NodeKeySetType>({n.first});


	// temp2.absorb<net::no_absorb,net::Tree_combine<set_contract>>("ten0_1","ten0_0");
	// 	for(auto & i: temp2["ten0_1"]->val ) std::cout<<i<<" **************\n";
	// 		std::cout<<"finish **************\n";




	std::chrono::steady_clock::time_point start_time;
	std::chrono::duration<double> cumu_time;

	// for(int i=4;i<5;++i){
	// 	eg.cut_part=i;
	// 	for(int j=0;j<4;++j){
	// 		eg.uneven=0.2*(4-j);
	// 		std::cout<<"part = "<<eg.cut_part<<" uneven = "<<eg.uneven<<' ';
	// 		//std::vector<double> test_time;
	// 		std::vector<long long int> test_count;
	// 		for(int k=0;k<10;++k){
	// 			auto ctree = net::get_contract_tree_test<double>(lat2,eg);
	// 			// start_time=std::chrono::steady_clock::now();
	// 			// net::tensor::Tensor<double> ten = lat2.contract<net::no_absorb,net::tensor::tensor_contract>(ctree);
	// 			// cumu_time=std::chrono::steady_clock::now()-start_time;
	// 			// test_time.push_back(cumu_time.count());
	// 			// std::cout<<cumu_time.count()<<' ';
	// 			test_count.push_back(ctree->val.contraction_cost);
	// 			std::cout<<' '<<ctree->val.hist_max_weight<<','<<ctree->val.contraction_cost<<' ';
	// 			delete ctree;
	// 		}
	// 		//std::vector<double>::iterator mintime = std::min_element(test_time.begin(), test_time.end());
	// 		//std::cout<<"min = "<<*mintime<<'\n';
	// 		std::vector<long long int>::iterator mincount = std::min_element(test_count.begin(), test_count.end());
	// 		std::cout<<"min = "<<*mincount<<'\n';
	// 	}
	// }



	for(int i=4;i<4;++i){
		eg.cut_part=i;
		for(int j=0;j<4;++j){
			eg.uneven=0.2*(4-j);
			std::cout<<"part = "<<eg.cut_part<<" uneven = "<<eg.uneven<<' ';
			//std::vector<double> test_time;
			std::vector<double> test_count;
			for(int k=0;k<10;++k){
				auto ctree = net::get_contract_tree<net::contract_info2>(lat2,eg);
				// start_time=std::chrono::steady_clock::now();
				// net::tensor::Tensor<double> ten = lat2.contract<net::no_absorb,net::tensor::tensor_contract>(ctree);
				// cumu_time=std::chrono::steady_clock::now()-start_time;
				// test_time.push_back(cumu_time.count());
				// std::cout<<cumu_time.count()<<' ';
				test_count.push_back(ctree->val.contraction_cost);
				std::cout<<' '<<ctree->val.hist_max_weight<<','<<ctree->val.contraction_cost<<' ';
				delete ctree;
			}
			//std::vector<double>::iterator mintime = std::min_element(test_time.begin(), test_time.end());
			//std::cout<<"min = "<<*mintime<<'\n';
			std::vector<double>::iterator mincount = std::min_element(test_count.begin(), test_count.end());
			std::cout<<"min = "<<*mincount<<'\n';
		}
	}
	auto ctree2 = net::get_contract_tree_qbb<net::keyset>(lat2,eg);


	auto ctree = net::get_contract_tree<net::keyset>(lat2,eg);
	//net::tree<typename decltype(temp)::NodeKeySetType>* ctree = eg.contract<net::Tree_combine<set_contract>>(temp2,includes);
	//net::tensor::Tensor<double> ten = eg.contract<net::tensor::tensor_contract>(temp,includes);
	//auto ctree2 = net::get_contract_tree_quickbb<double>(lat2,eg);

	//ctree->draw();
	timer benchmark;


	start_time=std::chrono::steady_clock::now();
	net::tensor::Tensor<double> ten2 = lat2.contract_tree<net::no_absorb,net::tensor::tensor_contract>(ctree2);
	cumu_time=std::chrono::steady_clock::now()-start_time;
	std::cout<<"quickbb "<<cumu_time.count()<<'\n';
	start_time=std::chrono::steady_clock::now();
	net::tensor::Tensor<double> ten3 = lat2.contract<net::no_absorb,net::tensor::tensor_contract>();
	cumu_time=std::chrono::steady_clock::now()-start_time;
	std::cout<<"naive "<<cumu_time.count()<<'\n';


	benchmark.print();
	return 0;
}