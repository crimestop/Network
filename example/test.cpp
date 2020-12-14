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

	int L1=20,L2=2,dim=4;

	using namespace std::placeholders;
	int seed =std::random_device()();
	std::default_random_engine random_engine(seed);


	net::tensor::TensorNetworkNoEnv<double> lat2;
	for(int i=0;i<L1;++i){
		for (int j=0;j<L2;++j){
			//std::cout<<i<<j<<"\n";
			lat2.add("ten"+str(i)+"_"+str(j));
			lat2.set_val("ten"+str(i)+"_"+str(j),1);
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


	auto ctree = net::get_contract_tree<double>(lat2,eg);

	//net::tree<typename decltype(temp)::NodeKeySetType>* ctree = eg.contract<net::Tree_combine<set_contract>>(temp2,includes);
	//net::tensor::Tensor<double> ten = eg.contract<net::tensor::tensor_contract>(temp,includes);
	eg.max_quickbb_size=1000;
	//net::tree<typename decltype(temp)::NodeKeySetType>* ctree2 = eg.contract<net::Tree_combine<set_contract>>(temp2,includes);

	auto ctree2 = net::get_contract_tree<double>(lat2,eg);

	//ctree->draw();
	timer benchmark;

	benchmark.start("divide");
	net::tensor::Tensor<double> ten = lat2.contract<net::no_absorb,net::tensor::tensor_contract>(ctree);
	benchmark.stop("divide");
	benchmark.start("quickbb");
	net::tensor::Tensor<double> ten2 = lat2.contract<net::no_absorb,net::tensor::tensor_contract>(ctree2);
	benchmark.stop("quickbb");
	benchmark.start("naive");
	net::tensor::Tensor<double> ten3 = lat2.contract<net::no_absorb,net::tensor::tensor_contract>();
	benchmark.stop("naive");


	benchmark.print();
	return 0;
}