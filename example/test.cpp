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

void test_kahypar(){

  kahypar_context_t* context = kahypar_context_new();
  kahypar_configure_context_from_file(context, "km1_kKaHyPar_sea20.ini");

  const kahypar_hypernode_id_t num_vertices = 7;
  const kahypar_hyperedge_id_t num_hyperedges = 4;

  std::unique_ptr<kahypar_hyperedge_weight_t[]> hyperedge_weights = std::make_unique<kahypar_hyperedge_weight_t[]>(4);

  // force the cut to contain hyperedge 0 and 2
  hyperedge_weights[0] = 1;  hyperedge_weights[1] = 1000; 
  hyperedge_weights[2] = 1;  hyperedge_weights[3] = 1000;
	     	 
  std::unique_ptr<size_t[]> hyperedge_indices = std::make_unique<size_t[]>(5);

  hyperedge_indices[0] = 0; hyperedge_indices[1] = 2;
  hyperedge_indices[2] = 6; hyperedge_indices[3] = 9;  	  
  hyperedge_indices[4] = 12;

  std::unique_ptr<kahypar_hyperedge_id_t[]> hyperedges = std::make_unique<kahypar_hyperedge_id_t[]>(12);

  // hypergraph from hMetis manual page 14
  hyperedges[0] = 0;  hyperedges[1] = 2;
  hyperedges[2] = 0;  hyperedges[3] = 1;
  hyperedges[4] = 3;  hyperedges[5] = 4;
  hyperedges[6] = 3;  hyperedges[7] = 4;	
  hyperedges[8] = 6;  hyperedges[9] = 2;
  hyperedges[10] = 5; hyperedges[11] = 6;
  	
  const double imbalance = 0.03;
  const kahypar_partition_id_t k = 2;
  	
  kahypar_hyperedge_weight_t objective = 0;

  std::vector<kahypar_partition_id_t> partition(num_vertices, -1);

  kahypar_partition(num_vertices, num_hyperedges,
       	            imbalance, k,
               	    /*vertex_weights */ nullptr, hyperedge_weights.get(),
               	    hyperedge_indices.get(), hyperedges.get(),
       	            &objective, context, partition.data());

  for(int i = 0; i != num_vertices; ++i) {
    std::cout << i << ":" << partition[i] << std::endl;
  }

  kahypar_context_free(context);
}

std::string str(int p){
	std::ostringstream os;
	os<<std::setw(4) << std::setfill('0') << p;
	return os.str();
}

//#define str std::to_string
int main(){

	// test_kahypar();
	// return 0;

	// net::tree<std::set<std::string>> tree1({"A"});
	// net::tree<std::set<std::string>> tree2({"B"});

	// net::tree<std::set<std::string>> * tree3= net::Tree_combine<set_contract>::run(&tree1,&tree2,1);
	// //net::tree<std::set<std::string>> * tree3= new net::tree<std::set<std::string>>(set_contract::run(tree1.val,tree2.val),&tree1,&tree2);

	// for(auto & i: tree3->val ) std::cout<<i<<" **************\n";
	// 	std::cout<<"finish **************\n";

	int L1=24,L2=24,dim=4;

	using namespace std::placeholders;
	int seed =std::random_device()();
	std::default_random_engine random_engine(seed);



	net::tensor::TensorNetworkNoEnv<double> lat2;
	for(int i=0;i<400;++i){
		lat2.add("ten"+str(i));
	}
	generate_random_regular_network(lat2,5,random_engine);
	std::cout<<"finish\n"; 
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
	auto ctree2 = net::get_contract_tree_qbb<net::contract_info2>(lat2,eg);
	std::cout<<"quickbb "<<ctree2->val.hist_max_weight<<','<<ctree2->val.contraction_cost<<'\n';
	//ctree2->draw();

	auto ctree3 = net::get_contract_tree_naive<net::contract_info2>(lat2,eg);
	std::cout<<"naive "<<ctree3->val.hist_max_weight<<','<<ctree3->val.contraction_cost<<'\n';
	//ctree3->draw();


	for(int i=2;i<3;++i){
		eg.cut_part=i;
		for(int j=0;j<50;++j){
			eg.uneven=0.98-0.02*j;
			std::cout<<"part = "<<eg.cut_part<<" uneven = "<<eg.uneven<<' ';
			//std::vector<double> test_time;
			std::vector<double> test_count;
			for(int k=0;k<1;++k){
				auto ctree = net::get_contract_tree<net::contract_info2>(lat2,eg);
				// start_time=std::chrono::steady_clock::now();
				// net::tensor::Tensor<double> ten = lat2.contract<net::no_absorb,net::tensor::tensor_contract>(ctree);
				// cumu_time=std::chrono::steady_clock::now()-start_time;
				// test_time.push_back(cumu_time.count());
				// std::cout<<cumu_time.count()<<' ';
				test_count.push_back(ctree->val.contraction_cost);
				std::cout<<' '<<ctree->val.hist_max_weight<<','<<ctree->val.contraction_cost<<' ';
				//if(j==0)ctree->draw();
				delete ctree;
			}
			//std::vector<double>::iterator mintime = std::min_element(test_time.begin(), test_time.end());
			//std::cout<<"min = "<<*mintime<<'\n';
			std::vector<double>::iterator mincount = std::min_element(test_count.begin(), test_count.end());
			std::cout<<"min = "<<(*mincount)<<'\n';
		}
	}


	//auto ctree = net::get_contract_tree<net::keyset>(lat2,eg);
	//net::tree<typename decltype(temp)::NodeKeySetType>* ctree = eg.contract<net::Tree_combine<set_contract>>(temp2,includes);
	//net::tensor::Tensor<double> ten = eg.contract<net::tensor::tensor_contract>(temp,includes);
	//auto ctree2 = net::get_contract_tree_quickbb<double>(lat2,eg);

	//ctree->draw();
	//timer benchmark;


	// start_time=std::chrono::steady_clock::now();
	// net::tensor::Tensor<double> ten2 = lat2.contract_tree(ctree2,net::no_absorb(),net::tensor::contract());
	// cumu_time=std::chrono::steady_clock::now()-start_time;
	// std::cout<<"quickbb "<<cumu_time.count()<<'\n';
	// start_time=std::chrono::steady_clock::now();
	// net::tensor::Tensor<double> ten3 = lat2.contract(net::no_absorb(),net::tensor::contract());
	// cumu_time=std::chrono::steady_clock::now()-start_time;
	// std::cout<<"naive "<<cumu_time.count()<<'\n';


	//benchmark.print();
	return 0;
}