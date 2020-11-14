#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include "timer.h"
#include <TAT/TAT.hpp>
#include <net/net.hpp>
#include <net/tensor_tool.hpp>
#define str std::to_string
#define vec std::vector

int main(){
	using namespace std::placeholders;
	
	net::network<net::tensor::Tensor<double>,net::tensor::Tensor<double>> lat("test");
	int seed =std::random_device()();
	std::default_random_engine random_engine(seed);
	timer benchmark;

	lat.add("A");
	lat.add("B");
	lat.add("C");
	lat.add("D");
	//lat.add("A");
	lat.set_bond("A","B","A.B","B.A");
	lat.set_bond("A","C","A.C","C.A");
	lat.set_bond("B","C","B.C","C.B");
	lat.set_bond("C","D","C.D","D.C");
	//lat.set_bond("A","B","A.B","B.new");

	lat.draw(true);
	int n=0;

	benchmark.start("square");

	net::network<net::tensor::Tensor<double>,net::tensor::Tensor<double>> lat2("square");
	for(int i=0;i<4;++i){
		for (int j=0;j<4;++j){
			//std::cout<<i<<j<<std::endl;
			lat2.add("ten"+str(i)+"_"+str(j),vec({i,j}));
			lat2.set_val("ten"+str(i)+"_"+str(j),1);
		}
	}
	for(int i=0;i<4;++i){
		for (int j=0;j<3;++j){
			//std::cout<<i<<j<<std::endl;
			lat2.set_bond(vec({i,j}),vec({i,j+1}));
		}
	}
	for(int i=0;i<3;++i){
		for (int j=0;j<4;++j){
			//std::cout<<i<<j<<std::endl;
			lat2.set_bond(vec({i,j}),vec({i+1,j}));
		}
	}

	TAT::Tensor<double> a,b;
	//std::ofstream("ten.dat", std::ios::out)<<a;
	std::ifstream("ten.dat", std::ios::in)>>b;

	// auto test=std::bind(net::tensor::init_site_rand, _1,8,-1l,1l,std::ref(random_engine));

	// std::vector<std::string> a={"A.1","A.2"};

	// net::tensor::Tensor<double> testt=test(a);

	lat2.init_sites(std::bind(net::tensor::init_site_rand, _1,8,-1l,1l,std::ref(random_engine)));

	lat2.absorb("ten1_1","ten1_2",net::tensor::no_absorb<double>,net::tensor::contract<double>);
	benchmark.stop("square");


	std::ofstream("testout.dat", std::ios::out|std::ios::binary)<lat2;
	net::network<net::tensor::Tensor<double>,net::tensor::Tensor<double>> lat3;
	std::ifstream("testout.dat", std::ios::in|std::ios::binary)>lat3;


	benchmark.start("contract");
	double tot = lat2.contract(net::tensor::no_absorb<double>,net::tensor::contract<double>);
	benchmark.stop("contract");

	benchmark.start("absorb");
	net::group<net::tensor::Tensor<double>,net::tensor::Tensor<double>> tnt;
	tnt.belong(lat2);

	for(int i=0;i<4;++i){
		for (int j=0;j<4;++j){
			//std::cout<<i<<"----"<<j<<std::endl;
			if (lat2.exist("ten"+str(i)+"_"+str(j))){
				tnt.absorb("ten"+str(i)+"_"+str(j),net::tensor::no_absorb<double>,net::tensor::contract<double>);
				//tnt.draw("test",true);
			}	
		}
	}

	benchmark.stop("absorb");


	benchmark.print();

	lat2.draw(true);
	lat3.draw(true);
	return 0;
}
