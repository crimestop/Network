#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include "timer.h"
#include <TAT/TAT.hpp>
#include <net/net.hpp>
#include <net/gviz.hpp>
#include <net/tensor_network.hpp>
#define str std::to_string

int main(){
	using namespace std::placeholders;
	
	net::tensor::TensorNetworkEnv<double> lat,cjlat;
	int seed =std::random_device()();
	std::default_random_engine random_engine(seed);
	timer benchmark;




			// std::vector<std::string> inds{"a","b","c"};
			// std::vector<unsigned int> dims{1,2,3};
			// TAT::Tensor<double,TAT::NoSymmetry,std::string> result(inds, {dims.begin(), dims.end()});





	lat.add("A");
	lat.add("B");
	lat.add("C");
	lat.add("D");
	//lat.add("A");
	lat.add_edge("A","B");
	lat.add_edge("A","C");
	lat.add_edge("B","C");
	lat.add_edge("C","D");
	//lat.add_edge("A","B","A.B","B.new");

	std::cout<<lat<<std::endl;

	lat.draw("test",true);
	cjlat=net::tensor::double_tnenv(lat);

	cjlat.draw("cjtest",true);



	int n=0;
	// net::network<std::monostate,std::monostate> lat_map;
	// lat_map=net::fmap<>(lat,net::tensor::zero_map<double>,net::tensor::zero_map<double>);
	// lat_map.draw(true);

	benchmark.start("square");

	net::tensor::TensorNetworkEnv<double> lat2;
	for(int i=0;i<4;++i){
		for (int j=0;j<4;++j){
			//std::cout<<i<<j<<"\n";
			lat2.add("ten"+str(i)+"_"+str(j));
			//lat2["ten"+str(i)+"_"+str(j)].val=1;
		}
	}
	for(int i=0;i<4;++i){
		for (int j=0;j<3;++j){
			//std::cout<<i<<j<<std::endl;
			lat2.add_edge("ten"+str(i)+"_"+str(j),"ten"+str(i)+"_"+str(j+1));
		}
	}
	for(int i=0;i<3;++i){
		for (int j=0;j<4;++j){
			lat2.add_edge("ten"+str(i)+"_"+str(j),"ten"+str(i+1)+"_"+str(j));
		}
	}

	lat2.draw("square",true);
	//TAT::Tensor<double> a,b;
	//std::ofstream("ten.dat", std::ios::out)<<a;
	//std::ifstream("ten.dat", std::ios::in)>>b;

	// auto test=std::bind(net::tensor::init_node_rand, _1,8,-1l,1l,std::ref(random_engine));

	// std::vector<std::string> a={"A.1","A.2"};

	// net::tensor::Tensor<double> testt=test(a);

	lat2.gfope(std::bind(net::tensor::init_node_rand<net::tensor::TensorNetworkEnv<double>>, _1,8,-1.,1.,std::ref(random_engine)),
		net::tensor::init_edge_null<net::tensor::TensorNetworkEnv<double>>);

	lat2.absorb("ten1_1","ten1_2",net::no_absorb(),net::tensor::contract());
	benchmark.stop("square");


	//std::cout<<"here1\n";
	// std::ofstream("testout.dat", std::ios::out|std::ios::binary)<lat2;
	// net::network<net::tensor::Tensor<double>,net::tensor::Tensor<double>> lat3;
	// std::ifstream("testout.dat", std::ios::in|std::ios::binary)>lat3;


	benchmark.start("contract");
	auto tot = lat2.contract(net::no_absorb(),net::tensor::contract());
	benchmark.stop("contract");
	//std::cout<<"here2\n";

	benchmark.start("absorb");
	net::group<net::tensor::Tensor<double>,net::tensor::Tensor<double>> tnt;
	tnt.belong(lat2);

	for(int i=0;i<4;++i){
		for (int j=0;j<4;++j){
			//std::cout<<i<<"----"<<j<<"\n";
			if (lat2.contains("ten"+str(i)+"_"+str(j))){
				tnt.absorb("ten"+str(i)+"_"+str(j),net::no_absorb(),net::tensor::contract());
				//tnt.draw("test",true);
			}	
		}
	}

	benchmark.stop("absorb");


	benchmark.print();

	if(lat2.consistency(std::cout)) std::cout<<"Network is consistent!"<<std::endl;

	lat2.draw("square",true);
	lat2.consistency();
	//lat3.draw(true);
	return 0;
}
