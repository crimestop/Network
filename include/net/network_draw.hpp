#ifndef NETWORK_DRAW_H
#define NETWORK_DRAW_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <initializer_list>
#include <stdexcept>
#include <cstdlib>
#include "error.hpp"
#include "site.hpp"
#include "network.hpp"

#ifdef NET_GRAPH_VIZ
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#endif

namespace net{

	#ifdef NET_GRAPH_VIZ
	std::string render(std::string dot_content);
	#ifdef NET_SHOW_FIG
	void show_fig(const std::string & fig_content,bool tmux);
	#endif
	#endif

	static std::string base64_encode(const std::string &in);

	#ifdef NET_GRAPH_VIZ
	template<typename T,typename V>
	void network<T,V>::draw_to_file(const std::string & filename,const bool label_bond){

		draw(filename,{},label_bond);
	}

	template<typename T,typename V>
	void network<T,V>::draw_to_file(const std::string & filename,const std::set<std::string> & contains,const bool label_bond){

		std::ofstream fig_file(filename, std::ios::binary);
		if(sites.size()>0){
			fig_file<<render(gviz(contains,label_bond));
		}

	}

	std::string render(std::string dot_content){
	    GVC_t *gvc;
	    Agraph_t *g;
	    char * cres;
	    std::string res;
	    unsigned int  len;
	    gvc = gvContext();
	    g = agmemread(dot_content.c_str());
	    gvLayout(gvc, g, "sfdp");
	    gvRenderData(gvc, g, "png", &cres,&len);
		res = std::string(cres,len);
		gvFreeRenderData (cres);
	    gvFreeLayout(gvc, g);
	    agclose(g);
	    return res;
	}

	#ifdef NET_SHOW_FIG
	template<typename T,typename V>
	void network<T,V>::draw(const bool label_bond){

		draw({},label_bond);
	}

	template<typename T,typename V>
	void network<T,V>::draw(const std::set<std::string> & contains,const bool label_bond){

		if(sites.size()==0){
			std::cout<<"-----------------------------------------------"<<std::endl;
			std::cout<<"empty lattice"<<std::endl;
			std::cout<<"-----------------------------------------------"<<std::endl;
		}else{
			show_fig(render(gviz(contains,label_bond)),false);
		}

	}

	void show_fig(const std::string & fig_content,bool tmux){
#ifdef __APPLE__
		if(tmux){
			std::cout<<"\033Ptmux;\033\033]";
		}else{
			std::cout<<"\033]";
		}
		std::cout<<"1337;File=;inline=1:";
		std::cout<<base64_encode(fig_content);
		if(tmux){
			std::cout<<"\a\033\\";
		}else{
			std::cout<<"\a";
		}
		std::cout<<std::endl;

#else
      std::ofstream("/tmp/net_tmp.png") << fig_content;
      std::system("gwenview /tmp/net_tmp.png || eog /tmp/net_tmp.png");
#endif
	}
	#endif

	#endif

	template<typename T,typename V>
	std::string network<T,V>::gviz(const std::set<std::string> & contains,const bool label_bond){

		std::stringstream dot_content;
		int nattr;
		std::set<std::string> drawn_sites;
		
		dot_content<<"digraph G {"<<std::endl;
		dot_content<<"  scale=0.6"<<std::endl;
		dot_content<<"  dpi=160"<<std::endl;
		dot_content<<"  bgcolor=\"transparent\""<<std::endl;
		dot_content<<"  fontcolor=White"<<std::endl;
		dot_content<<"  fontname=\"Monaco\""<<std::endl;
		if(name==""){
			dot_content<<"  label = \"figure of network\""<<std::endl;
		}else{
			dot_content<<"  label = \"figure of "+name+"\""<<std::endl;
		}
		

		for(auto s_it:sites){
			auto & name1=s_it.first;
			if (contains.count(name1)==1){
				dot_content<<"  "+name1+" [ color=Red, label = \""+name1+"\", fontcolor=White, fontname=\"Monaco\"]"<<std::endl;
			}else{
				dot_content<<"  "+name1+" [ color=White, label = \""+name1+"\", fontcolor=White, fontname=\"Monaco\"]"<<std::endl;
			}
		}
		dot_content<<"subgraph bond {"<<std::endl;
		dot_content<<"  edge[dir=none]"<<std::endl;

		for(auto & s_it:sites){
			auto & name1=s_it.first;
			for(auto & b_it:s_it.second.bonds){
				auto & ind1=b_it.first;
				auto & name2=b_it.second.name;
				auto & ind2=b_it.second.ind;
				nattr=0;
				if (drawn_sites.count(name2)==0){
					dot_content<<"  "+name1+" -> "+name2+" [fontcolor=White, fontname=\"Monaco\",";
					if(label_bond){
						dot_content<<"taillabel = \""+ind1.substr(ind1.find('.')+1)+
						"\",headlabel =\""+ind2.substr(ind1.find('.')+1)+"\"";
						nattr=nattr+1;
					}
					if(contains.count(name1)==1 && contains.count(name2)==1){
						if (nattr>0) dot_content<<",";
						dot_content<<"color=Red";
						nattr=nattr+1;
					}else{
						if (nattr>0) dot_content<<",";
						dot_content<<"color=White";
						nattr=nattr+1;
					}
					if (nattr>0) dot_content<<",";
					dot_content<<" len=3]"<<std::endl;
				}
			}
			drawn_sites.insert(name1);
		}

		dot_content<<"}"<<std::endl;
		dot_content<<"}"<<std::endl;
		return dot_content.str();

	}


	// base64 functions are copied from https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
	// originally by Manuel Martinez

	static std::string base64_encode(const std::string &in) {

	    std::string out;

	    int val=0, valb=-6;
	    for (unsigned char c : in) {
	        val = (val<<8) + c;
	        valb += 8;
	        while (valb>=0) {
	            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
	            valb-=6;
	        }
	    }
	    if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
	    while (out.size()%4) out.push_back('=');
	    return out;
	}

	static std::string base64_decode(const std::string &in) {

	    std::string out;

	    std::vector<int> T(256,-1);
	    for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i; 

	    int val=0, valb=-8;
	    for (unsigned char c : in) {
	        if (T[c] == -1) break;
	        val = (val<<6) + T[c];
	        valb += 6;
	        if (valb>=0) {
	            out.push_back(char((val>>valb)&0xFF));
	            valb-=8;
	        }
	    }
	    return out;
	}
}
#endif
