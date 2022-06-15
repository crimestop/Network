#ifndef NET_NODE_HPP
#define NET_NODE_HPP

#include "error.hpp"
#include "network.hpp"
#include "traits.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace net {

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	class node;
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	class network;
	template <typename T>
	std::string to_string(const T & m);

	/**
	 * \brief 描述着（某条格点上的）半边的信息，包括这条半边与哪条格点的哪条半边相连
	 *
	 * \see node
	 * \tparam NodeVal 每个格点中附着的信息类型
	 * \tparam EdgeVal 每个边上附着的信息类型
	 * \tparam NodeKey 格点名字的类型
	 * \tparam EdgeKey 边的名字的类型
	 * \tparam Trait 以上类型的特征，包括输入输出和比较
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	struct half_edge {


		using EdgeItrType = typename std::map<EdgeKey, half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::edgekey_less>::iterator ;

		/**
		 * \brief 指这条半边所连接的半边的数目（目前仅支持nb_num=0/1）
		 */
		int nb_num;
		/**
		 * \brief 所连接的半边从属的格点的名称（如果nb_num=1）
		 */
		NodeKey nbkey;
		/**
		 * \brief 所连接的半边的名称（如果nb_num=1）
		 */
		EdgeKey nbind;
		/**
		 * \brief 所连接的半边从属的格点的指针（如果nb_num=1）
		 */
		typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode nbitr;
		/**
		 * \brief 边上的附着信息
		 */
		EdgeVal val;

		/**
		 * \brief 所连接的半边的指针（如果nb_num=1）
		 */
		EdgeItrType nbegitr;

		half_edge() : nb_num(0) {};
		half_edge(const EdgeVal & E) : val(E), nb_num(0) {};
		half_edge(const NodeKey & s1, const EdgeKey & s2, typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode s) :
				nbkey(s1), nbind(s2), nbitr(s), nb_num(1){};
		half_edge(const NodeKey & s1, const EdgeKey & s2, typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode s, const EdgeVal & E) :
				nbkey(s1), nbind(s2), nbitr(s), val(E), nb_num(1){};
		half_edge(const NodeKey & s1, const EdgeKey & s2, const EdgeVal & E) : nbkey(s1), nbind(s2), val(E), nb_num(1){};
		half_edge(const half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &) = default;
	};

	/**
	 * \brief 存储了网络中一个格点内的信息
	 *
	 * 每个格点中有一个中心元素val, 和一些半边
	 * 每个边拥有一个名字和一个half edge对象
	 * \see network
	 * \see half_edge
	 * \tparam NodeVal 每个格点中附着的信息类型
	 * \tparam EdgeVal 每个边上附着的信息类型
	 * \tparam NodeKey 格点名字的类型
	 * \tparam EdgeKey 边的名字的类型
	 * \tparam Trait 以上类型的特征，包括输入输出和比较
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	class node {
		/**
		 * \brief 格点的字符串输出
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::ostream & output_node_text(std::ostream &, const node<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

		/**
		 * \brief 格点的字符串输入
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::istream & input_node_text(std::istream &, node<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

		/**
		 * \brief 格点的二进制输出
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::ostream & output_node_bin(std::ostream &, const node<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

		/**
		 * \brief 格点的二进制输入
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::istream & input_node_bin(std::istream &, node<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

	public:
		// constructor
		node() = default;
		node(const NodeKey & k) : key(k){};
		node(const NodeKey & k, const NodeVal & s) : key(k),val(s){};
		// copy constructor
		node(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &) = default;
		// copy assignment
		node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & operator=(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &) = default;
		// move constructor
		node(node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &&) = default;
		// move assignment
		node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & operator=(node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &&) = default;

		using EdgeType = half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>;
		using NodeKeyType = NodeKey;
		using NodeValType = NodeVal;
		using EdgeKeyType = EdgeKey;
		using EdgeValType = EdgeVal;
		using TraitType = Trait;

		typename half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::EdgeItrType 
			add_half_edge(const EdgeKey & ind1, const EdgeVal & edgev) ;

		void clean();

		template <typename Condition>
		void delete_half_edge(Condition &&);
		template <typename Condition>
		void break_edge(Condition &&);
		template <typename Condition>
		void delete_edge(Condition &&);
		void delete_nbedge();

		void reset_nbkey_of_nb(const NodeKey &);

		template <typename absorb_type, typename contract_type>
		void absorb_nb(const NodeKey &, const NodeVal &, const absorb_type &, const contract_type &);
		template <typename absorb_type, typename Condition>
		void
		harmless_absorb_nb(NodeVal &, std::set<std::pair<EdgeKey, EdgeKey>, typename Trait::edge2key_less> &, const absorb_type &, Condition &&) const;

		void transfer_edge(const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode &);
		template <typename Condition>
		void transfer_edge(const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode &, Condition &&);
		template <typename Condition>
		void transfer_edge(
				const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode &,
				const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode &,
				Condition &&);

		void relink(std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less> &);

		bool consistency(const std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less> &, const NodeKey & t, std::ostream &)const;

		void fope(std::function<NodeVal(const NodeVal &)> f1, std::function<EdgeVal(const EdgeVal &)> f2);

		template <typename NodeType2>
		NodeType2
		fmap(const NodeKey & this_key,std::function<typename NodeType2::NodeValType(const NodeVal &)> f1,
			  std::function<typename NodeType2::EdgeValType(const EdgeVal &)> f2) const;

		template <typename NodeType2>
		NodeType2
		fmap(const NodeKey & new_key, std::function<typename NodeType2::NodeValType(const NodeVal &)> f1,
			  std::function<typename NodeType2::EdgeValType(const EdgeVal &)> f2,
			  std::function<typename NodeType2::NodeKeyType(const NodeKey &)> f3,
			  std::function<typename NodeType2::EdgeKeyType(const EdgeKey &)> f4) const;

		template <typename NodeVal2, typename EdgeVal2, typename Trait2>
		node<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2>
		gfmap(const NodeKey & this_key,std::function<NodeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2) const;


		void gfope(std::function<NodeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2);
		/**
		 * \brief 格点的名字
		 */
		NodeKey key;
		/**
		 * \brief 格点上附着的信息
		 */
		NodeVal val;
		/**
		 * \brief 格点所包含的半边, 存储了半边之间如何连接的信息
		 * \see half_edge
		 */
		std::map<EdgeKey, half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::edgekey_less> edges;
	};
	/**
	 * \brief 根据条件cond(edge_itr)是否为真，决定是否删除一条半边
	 *
	 * 如果要删除的半边连接着另一条半边，那么保留另一条半边，使之悬空
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename Condition>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::delete_half_edge(Condition && cond) {
		for (auto edge_itr = edges.begin(); edge_itr != edges.end();) {
			if (cond(edge_itr)) {
				if(edge_itr->second.nbnum!=0){
					edge_itr->second.nbegitr->second.nbnum=0;
					edge_itr->second.nbegitr->second.nbkey=edge_itr->second.nbkey;
					edge_itr->second.nbegitr->second.nbind=edge_itr->second.nbind;
					edge_itr->second.nbegitr->second.nbitr=edge_itr->second.nbitr;
					edge_itr->second.nbegitr->second.nbegitr=edge_itr->second.nbegitr;
				}
				edge_itr = edges.erase(edge_itr);
			} else {
				++edge_itr;
			}
		}
	}

	/**
	 * \brief 根据条件cond(edge_itr)是否为真，决定是否将一条边拆开，成为两条不相连的半边
	 *
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename Condition>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::break_edge(Condition && cond) {
		for (auto edge_itr = edges.begin(); edge_itr != edges.end();++edge_itr) {
			if (cond(edge_itr)) {
				if(edge_itr->second.nb_num!=0){
					edge_itr->second.nbegitr->second.nb_num=0;
					edge_itr->second.nbegitr->second.nbkey=edge_itr->second.nbkey;
					edge_itr->second.nbegitr->second.nbind=edge_itr->second.nbind;
					edge_itr->second.nbegitr->second.nbitr=edge_itr->second.nbitr;
					edge_itr->second.nbegitr->second.nbegitr=edge_itr->second.nbegitr;

					edge_itr->second.nb_num=0;
					edge_itr->second.nbkey=this->key;
					edge_itr->second.nbind=edge_itr->first;
					edge_itr->second.nbitr=edge_itr;
					edge_itr->second.nbegitr=edge_itr;
				}
			}
		}
	}

	/**
	 * \brief 根据条件cond(edge_itr)是否为真，决定是否删除一条半边，同时删除与其相连的半边
	 *
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename Condition>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::delete_edge(Condition && cond) {
		for (auto edge_itr = edges.begin(); edge_itr != edges.end();) {
			if (cond(edge_itr)) {
				if(edge_itr->second.nb_num!=0){
					edge_itr->second.nbitr->second.edges.erase(edge_itr->second.nbegitr);
				}
				edge_itr = edges.erase(edge_itr);
			} else {
				++edge_itr;
			}
		}
	}

	/**
	 * \brief 根据条件cond(edge_itr)是否为真，决定是否删除半边所连接的另一条半边
	 *
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::delete_nbedge() {
		for (auto & b : edges) {
			if(b.second.nb_num!=0){
				b.second.nbitr->second.edges.erase(b.second.nbegitr);
			}
		}
		edges.clear();
	}

	
	/**
	 * \brief  对格点的每一条边，更新邻居对应的边的nbkey为newkey。用于格点的改名。
	 *
	 */
	 
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::reset_nbkey_of_nb(const NodeKey & newkey) {
		for (auto & b : edges) {
			if(b.second.nb_num!=0){
				b.second.nbegitr->second.nbkey = newkey;
			}
		}
	}

	/**
	 * \brief  对格点的指定nbkey每一条边，记录这条边的(ind,nbind)，吸收边的val到格点的val，然后删掉这条边。
	 *
	 *  A(this) <----> B(nb) <----> C   =>  A <---- B <----> C
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::absorb_nb(
			const NodeKey & nbkey,
			const NodeVal & nbval,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		std::set<std::pair<EdgeKey, EdgeKey>, typename Trait::edge2key_less> ind_pairs;

		// set ind_pairs and erase iterator in node1
		for (auto iter = edges.begin(); iter != edges.end();) {
			if (iter->second.nb_num!=0 && iter->second.nbkey == nbkey) {
				ind_pairs.insert({iter->first, iter->second.nbind});
				val = absorb_fun(val, iter->second.val, iter->first);
				iter = edges.erase(iter);
			} else {
				++iter;
			}
		}
		val = contract_fun(val, nbval, ind_pairs);
	}

	/**
	 * \brief  对格点的指定nbkey每一条边，如果符合条件，记录这条边的(ind,nbind)，吸收边的val到给定的的val，将ind-pair填入ind_pairs
	 *
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename Condition>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::harmless_absorb_nb(
			NodeVal & thisval,
			std::set<std::pair<EdgeKey, EdgeKey>, typename Trait::edge2key_less> & ind_pairs,
			const absorb_type & absorb_fun,
			Condition && cond) const {
		for (auto & b : edges) {
			// std::cout<<"edge"<<b.first<<' '<<b.second.nbkey<<'\n';
			if (b.second.nb_num!=0 && cond(b)) {
				// std::cout<<"edgehere"<<b.first<<' '<<b.second.nbkey<<'\n';
				ind_pairs.insert({b.first, b.second.nbind});
				thisval = absorb_fun(thisval, b.second.val, b.first);
			}
		}
	}

	/**
	 * \brief 对格点的每一条边，将它转移为格点newit的边
	 *
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::transfer_edge(
			const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode & newit) {
			while(edges.size()>0) {
				auto nh = edges.extract(edges.begin());
				auto status = newit->second.edges.insert(std::move(nh));
				if(status.position->second.nb_num!=0){
					status.position->second.nbegitr->second.nbkey = newit->first;
					status.position->second.nbegitr->second.nbitr = newit;
					status.position->second.nbegitr->second.nbegitr = status.position;
				}
			}
	}

	/**
	 * \brief
	 * 对格点的每一条边，如果条件成立，将它和邻居的这条边转移为格点newit和邻居的边
	 *
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename Condition>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::transfer_edge(
			const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode & newit,
			Condition && cond) {
		for (auto iter = edges.begin(); iter != edges.end();) {
			if (cond(iter)) {
				auto nh = edges.extract(iter++);
				auto status = newit->second.edges.insert(std::move(nh));
				if(status.position->second.nb_num!=0){
					status.position->second.nbegitr->second.nbkey = newit->first;
					status.position->second.nbegitr->second.nbitr = newit;
					status.position->second.nbegitr->second.nbegitr = status.position;
				}
			} else {
				++iter;
			}
		}
	}

	/**
	 * \brief
	 * 对格点的每一条边，如果条件成立，将它和邻居的这条边转移为格点newit和邻居的边;
	 *                       否则，将它和邻居的这条边转移为格点newit2和邻居的边
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename Condition>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::transfer_edge(
			const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode & newit,
			const typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode & newit2,
			Condition && cond) {
		for (auto iter = edges.begin(); iter != edges.end();) {
			auto nh = edges.extract(iter++);
			if (cond(iter)) {
				auto status = newit->second.edges.insert(std::move(nh));
				if(status.position->second.nb_num!=0){
					status.position->second.nbegitr->second.nbkey = newit->first;
					status.position->second.nbegitr->second.nbitr = newit;
					status.position->second.nbegitr->second.nbegitr = status.position;
				}
			} else {
				auto status = newit2->second.edges.insert(std::move(nh));
				if(status.position->second.nb_num!=0){
					status.position->second.nbegitr->second.nbkey = newit2->first;
					status.position->second.nbegitr->second.nbitr = newit2;
					status.position->second.nbegitr->second.nbegitr = status.position;
				}
			}
		}
	}
	/**
	 * \brief
	 * 添加一条半边
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	typename half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::EdgeItrType 
		node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_half_edge(const EdgeKey & ind1, const EdgeVal & edgev) {

		auto [egit1, succ1] = edges.insert({ind1, typename node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::EdgeType(edgev)});
		if (!succ1)
			throw key_exist_error("In node.add_edge_node, ind " + to_string(ind1) + " already exist!");
		return egit1;
	}

	/**
	 * \brief
	 * 添加两条半边并连起来
	 */

	template <typename IterNode, typename EdgeKey, typename EdgeVal>
	void add_edge_node(IterNode it1, IterNode it2, const EdgeKey & ind1,const EdgeKey & ind2, const EdgeVal & edgev) {

		auto egit1 = it1->second.add_half_edge(ind1,edgev);
		auto egit2 = it2->second.add_half_edge(ind2,edgev);
		egit1->second.nb_num=1;
		egit1->second.nbkey= it2->first;
		egit1->second.nbind= ind2;
		egit1->second.nbitr=it2;
		egit1->second.nbegitr=egit2;

		egit2->second.nb_num=1;
		egit2->second.nbkey= it1->first;
		egit2->second.nbind= ind1;
		egit2->second.nbitr=it1;
		egit2->second.nbegitr=egit1;
	}

	/**
	 * \brief
	 * 将两条半边连起来
	 */
	template <typename IterNode, typename EdgeKey, typename EdgeVal>
	void connect_edge_node(IterNode it1, IterNode it2, const EdgeKey & ind1,const EdgeKey & ind2) {

		auto egit1 = it1->second.edges.find(ind1);
		if (egit1 == it1->second.edges.end()) 
			throw key_unfound_error("In node.connect_edge_node, ind " + to_string(ind1) + " is not found!");
		if(egit1->second.nb_num==1)
			throw key_exist_error("In node.connect_edge_node, ind " + to_string(ind1) + " already linked!");
		auto egit2 = it2->second.edges.find(ind2);
		if (egit2 == it2->second.edges.end()) 
			throw key_unfound_error("In node.connect_edge_node, ind " + to_string(ind2) + " is not found!");
		if(egit2->second.nb_num==1)
			throw key_exist_error("In node.connect_edge_node, ind " + to_string(ind2) + " already linked!");

		egit1->second.nb_num=1;
		egit1->second.nbkey=it2->first;
		egit1->second.nbind=ind2;
		egit1->second.nbitr=it2;
		egit1->second.nbegitr=egit2;

		egit2->second.nb_num=1;
		egit2->second.nbkey=it1->first;
		egit2->second.nbind=ind1;
		egit2->second.nbitr=it1;
		egit2->second.nbegitr=egit1;

	}

	/**
	 * \brief
	 * 更新所有半边的nbitr和nbegitr指针
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::relink(
			std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less> & nodes) {
		for (auto & b : edges){
			if(b.second.nb_num==1){
				b.second.nbitr = nodes.find(b.second.nbkey);
				b.second.nbegitr = b.second.nbitr->second.edges.find(b.second.nbind);
			}
		}
	}

	/**
	 * \brief
	 * 从旧的格点得到新的格点，其格点和边上的信息由旧的格点通过映射得到
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename NodeVal2, typename EdgeVal2, typename Trait2>
	node<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2> node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::gfmap( const NodeKey & this_key,
			std::function<NodeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2) const {
		node<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2> result(this_key,f1(*this));
		for (auto & b : edges)
			if(b.second.nb_num==1){
				result.edges[b.first] = half_edge<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2>(b.second.nbkey, b.second.nbind, f2(*this, b.first));
			}else{
				result.edges[b.first] = half_edge<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2>(f2(*this, b.first));
			}
		return result;
	}

	/**
	 * \brief
	 * 从旧的格点得到新的格点，其格点和边上的信息由旧的格点通过映射得到
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename NodeType2>
	NodeType2 node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::fmap( const NodeKey & this_key,
			std::function<typename NodeType2::NodeValType(const NodeVal &)> f1,
			std::function<typename NodeType2::EdgeValType(const EdgeVal &)> f2) const {
		NodeType2 result(this_key, f1(val));
		for (auto & b : edges)
			if(b.second.nb_num==1){
				result.edges[b.first] = typename NodeType2::EdgeType(b.second.nbkey, b.second.nbind, f2(b.second.val));
			}else{
				result.edges[b.first] = typename NodeType2::EdgeType(f2(b.second.val));
			}
			
		return result;
	}

	/**
	 * \brief
	 * 从旧的格点得到新的格点，其格点和边的名字，以及格点和边上的信息，由旧的格点通过映射得到
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename NodeType2>
	NodeType2 node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::fmap( const NodeKey & new_key,
			std::function<typename NodeType2::NodeValType(const NodeVal &)> f1,
			std::function<typename NodeType2::EdgeValType(const EdgeVal &)> f2,
			std::function<typename NodeType2::NodeKeyType(const NodeKey &)> f3,
			std::function<typename NodeType2::EdgeKeyType(const EdgeKey &)> f4) const {
		NodeType2 result(new_key,f1(val));
		for (auto & b : edges)
			if(b.second.nb_num==1){
				result.edges[f4(b.first)] = typename NodeType2::EdgeType(f3(b.second.nbkey), f4(b.second.nbind), f2(b.second.val));
			}else{
				result.edges[f4(b.first)] = typename NodeType2::EdgeType(f2(b.second.val));
			}
		return result;
	}

	/**
	 * \brief
	 * 利用函数更新格点和边上的信息
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::fope(std::function<NodeVal(const NodeVal &)> f1, std::function<EdgeVal(const EdgeVal &)> f2) {
		val = f1(val);
		for (auto & b : edges)
			b.second.val = f2(b.second.val);
	}
	/**
	 * \brief
	 * 利用函数更新格点和边上的信息
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::gfope(std::function<NodeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2) {
		auto temp = f1(*this);
		for (auto & b : edges)
			b.second.val = f2(*this,b.first);
		val = temp;
	}

	/**
	 * \brief
	 * 检查格点的一致性
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	bool node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::consistency(
			const std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less> & nodes,
			const NodeKey & this_key, std::ostream & diagnosis) const {
		if(key!=this_key){
			diagnosis << "Network is not consistent, node at " + to_string(this_key) + " has wrong key!\n";
			return false;	
		}
		for (auto & b : edges) { // check if b is consistent
			if(b.second.nb_num!=0){
				if (nodes.count(b.second.nbkey) == 0) {
					diagnosis << "Network is not consistent, neighbor " + to_string(b.second.nbkey) + " is not found!\n";
					return false;
				} else if (nodes.at(b.second.nbkey).edges.count(b.second.nbind) == 0) {
					diagnosis << "Network is not consistent, neighbor " + to_string(b.second.nbkey) + " has no index named " + to_string(b.second.nbind) +
											 " !\n";
					return false;
				} else if (b.second.nbitr != nodes.find(b.second.nbkey)) {
					diagnosis << "Network is not consistent, pointer to neighbor " + to_string(b.second.nbkey) + " is not correctly pointed !\n";
					return false;
				} else if (b.second.nbegitr != b.second.nbitr->second.edges.find(b.second.nbind)) {
					diagnosis << "Network is not consistent, pointer to neighbor edge " + to_string(b.second.nbind) + " is not correctly pointed !\n";
					return false;
				} else if (b.second.nbegitr->second.nb_num == 0) {
					diagnosis << "Network is not consistent, neighboring half edge to " + to_string(b.second.nbind) + " is dangling !\n";
					return false;
				} else if (b.second.nbegitr->second.nbkey != this_key) {
					diagnosis << "Network is not consistent, neighboring half edge to " + to_string(b.second.nbind) + " is not matching 0 !\n";
					return false;
				} else if (b.second.nbegitr->second.nbind != b.first) {
					diagnosis << "Network is not consistent, neighboring half edge to " + to_string(b.second.nbind) + " is not matching 1 !\n";
					return false;
				}
			}
		}

		return true;
	}
} // namespace net

#endif
