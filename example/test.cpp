#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
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
#include <net/algorithm.hpp>

#include <memory>
#include <vector>
#include <iostream>
#include <libkahypar.h>


std::string str(int p){
	std::ostringstream os;
	os<<std::setw(4) << std::setfill('0') << p;
	return os.str();
}

//#define str std::to_string
int main(){

	int L1=24,L2=24,dim=4;

	using namespace std::placeholders;
	int seed =std::random_device()();
	std::default_random_engine random_engine(14);



	net::tensor::TensorNetworkNoEnv<double> lat2;
	for(int i=0;i<40;++i){
		lat2.add("ten"+str(i));
	}
	generate_random_regular_network(lat2,5,random_engine);
	//std::cout<<"finish\n"; 
	//lat2.draw("rand",true);

	// net::tensor::TensorNetworkNoEnv<double> lat2;
	// for(int i=0;i<L1;++i){
	// 	for (int j=0;j<L2;++j){
	// 		//std::cout<<i<<j<<"\n";
	// 		lat2.add("ten"+str(i)+"_"+str(j));
	// 		//lat2["ten"+str(i)+"_"+str(j)].val=1;
	// 	}
	// }
	// for(int i=0;i<L1;++i){
	// 	for (int j=0;j<L2-1;++j){
	// 		//std::cout<<i<<j<<std::endl;
	// 		lat2.set_edge("ten"+str(i)+"_"+str(j),"ten"+str(i)+"_"+str(j+1));
	// 	}
	// }
	// for(int i=0;i<L1-1;++i){
	// 	for (int j=0;j<L2;++j){
	// 		lat2.set_edge("ten"+str(i)+"_"+str(j),"ten"+str(i+1)+"_"+str(j));
	// 	}
	// }

	lat2.init_nodes(std::bind(net::tensor::init_node_rand<net::stdEdgeKey>, _1,dim,-1.,1.,std::ref(random_engine)));

	net::Engine eg;

	std::chrono::steady_clock::time_point start_time;
	std::chrono::duration<double> cumu_time;

	auto ctree2 = net::get_contract_tree_qbb<net::contract_info2>(lat2,eg);
	std::cout<<"quickbb "<<ctree2->val.hist_max_weight<<','<<ctree2->val.contraction_cost<<'\n';
	//ctree2->draw();

	auto ctree3 = net::get_contract_tree_naive<net::contract_info2>(lat2,eg);
	std::cout<<"naive "<<ctree3->val.hist_max_weight<<','<<ctree3->val.contraction_cost<<'\n';
	//ctree3->draw();

	auto ctree = net::get_contract_tree<net::contract_info2>(lat2,eg);
	std::cout<<"kahypar "<<ctree->val.hist_max_weight<<','<<ctree->val.contraction_cost<<' ';
	//ctree->draw();

	return 0;
}