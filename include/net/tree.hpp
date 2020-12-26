#ifndef NET_TREE_HPP
#define NET_TREE_HPP
#include "gviz_themes.hpp"

namespace net{

	template<typename Data>
	struct tree{
		Data val;
		//tree<Data> * parent=nullptr;
		tree<Data> * left_child=nullptr;
		tree<Data> * right_child=nullptr;

		tree()=default;
		tree(const Data & D):val(D){};
		tree(const Data & D,tree<Data>* Tree1,tree<Data>* Tree2):val(D),left_child(Tree1),right_child(Tree2){};
		tree(const tree<Data>&)=default;
		~tree();

		std::string gviz();
	#ifdef NET_GRAPH_VIZ
		void draw();
	#endif
	};

	template<typename Data>
	tree<Data>::~tree(){
		if(left_child != nullptr) delete left_child;
		if(right_child != nullptr) delete right_child;
	}

	template<typename contract_type>
	struct Tree_combine{
		template<typename Data,typename NoUse>
		tree<Data>* operator()( tree<Data>* const & a,tree<Data>* const & b,const NoUse & c) const{
			return new tree<Data>(contract_type::contract(a->val,b->val),a,b);
		}
	};

	template<typename absorb_type>
	struct Tree_act{
		template<typename Data,typename Data2,typename NoUse>
		tree<Data>* operator()( tree<Data>* const & a, const Data2 & b,const NoUse & c) const{
			a->val=absorb_type::absorb(a->val,b);
			return a;
		}
	};

	template<typename Data>
	void gviz_nodes(tree<Data>* node, std::ostream& dot_content, const std::string & nodename){

		std::string nodelable="";
		if(node->left_child==nullptr && node->right_child==nullptr)
			for(auto & s:node->val){
				nodelable+=s;
			}
		dot_content<<"  "<<nodename<<" [ color="<<gviz_theme["node_strokecolor"]<<", label = \""<<
			nodelable<<"\", fontcolor="<<gviz_theme["node_fontcolor"]<<", fontname="<<gviz_theme["node_fontname"]<<"];\n";

		if(node->left_child!=nullptr){
			dot_content<<"  "<<nodename<<" -> "<<nodename<<"l"<<" [fontcolor="<<gviz_theme["edge_fontcolor"]<<", fontname="<<
				gviz_theme["edge_fontname"]<<", color="<<gviz_theme["edge_strokecolor"]<<"];\n";
			gviz_nodes<Data>(node->left_child,dot_content,nodename+'l');
		}
		if(node->right_child!=nullptr){
			dot_content<<"  "<<nodename<<" -> "<<nodename<<"r"<<" [fontcolor="<<gviz_theme["edge_fontcolor"]<<", fontname="<<
				gviz_theme["edge_fontname"]<<", color="<<gviz_theme["edge_strokecolor"]<<"];\n";
			gviz_nodes<Data>(node->right_child,dot_content,nodename+'r');
		}
	}

	template<typename Data>
	std::string tree<Data>::gviz(){

		std::stringstream dot_content;
		std::string grp;
		
		dot_content<<"digraph G {\n";
		dot_content<<"  scale=0.6\n";
		dot_content<<"  dpi=160\n";
		dot_content<<"  bgcolor="<<gviz_theme["global_bgcolor"]<<"\n";
		dot_content<<"  fontcolor="<<gviz_theme["global_fontcolor"]<<"\n";
		dot_content<<"  fontname="<<gviz_theme["global_fontname"]<<"\n";
		dot_content<<"  label = \"figure of tree\"\n";

		std::string nodename="r";

		gviz_nodes<Data>(this,dot_content,nodename);

		dot_content<<"}\n";

		// std::cout<<dot_content.str();
		return dot_content.str();

	}
#ifdef NET_GRAPH_VIZ
	std::string render(std::string dot_content,const std::string & engine);
	void show_fig(const std::string & fig_content,bool tmux,bool st);

	template<typename Data>
	void tree<Data>::draw(){
		show_fig(render(gviz(),"dot"),true,true);
	}
#endif


}
#endif