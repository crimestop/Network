#ifndef NETWORK_H
#define NETWORK_H

#include "error.hpp"
#include "node.hpp"
#include "tensor_tools.hpp"
#include "traits.hpp"
#include "tree.hpp"
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <stdexcept>
#include <vector>

namespace net {

	struct no_absorb {
		template <typename NodeVal, typename EdgeVal, typename EdgeKey>
		NodeVal operator()(const NodeVal & ten1, const EdgeVal & ten2, const EdgeKey & ind) const {
			return ten1;
		}
	};

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey>
	struct default_traits;
	/**
	 * \brief 网络包含格点和格点上的半边，两个半边可以相连接
	 *
	 * 每个格点和半边拥有一个名字，网络可以通过格点名称寻找格点和半边
	 * \see node, half_edge
	 * \tparam NodeVal 每个格点中附着的信息类型
	 * \tparam EdgeVal 每个边上附着的信息类型
	 * \tparam NodeKey 格点名字的类型
	 * \tparam EdgeKey 边的名字的类型
	 * \tparam Trait 以上类型的特征，包括输入输出和比较
	 */

	template <
			typename NodeVal,
			typename EdgeVal,
			typename NodeKey = std::string,
			typename EdgeKey = stdEdgeKey,
			typename Trait = default_traits<NodeVal, EdgeVal, NodeKey, EdgeKey>>
	class network : public std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less> {
		/**
		 * \brief 网络的字符串输出
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::ostream & operator<<(std::ostream &, const network<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

		/**
		 * \brief 网络的字符串输入
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::istream & operator>>(std::istream &, network<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

		/**
		 * \brief 网络的二进制输出
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::ostream & operator<(std::ostream &, const network<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

		/**
		 * \brief 网络的二进制输出
		 */
		template <typename NodeVal1, typename EdgeVal1, typename NodeKey1, typename EdgeKey1, typename Trait1>
		friend std::istream & operator>(std::istream &, network<NodeVal1, EdgeVal1, NodeKey1, EdgeKey1, Trait1> &);

	public:
		// constructor
		network() = default;
		// copy constructor
		network(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &);
		// copy assignment
		network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & operator=(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &);
		// move constructor
		network(network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &&) = default;
		// move assignment
		network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & operator=(network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &&) = default;
		// destructor
		//~network();

		using IterNode = typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::iterator;

		using NodeType = node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>;
		using NodeKeyType = NodeKey;
		using NodeKeySetType = std::set<NodeKey, typename Trait::nodekey_less>;
		using NodeValType = NodeVal;
		using EdgeKeyType = EdgeKey;
		using EdgeValType = EdgeVal;
		using TraitType = Trait;

		IterNode add(const NodeKey &);

		void add(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &);

		void add_edge(const NodeKey &, const NodeKey &, const EdgeKey &, const EdgeKey &, const EdgeVal & = EdgeVal());
		void add_edge(IterNode, IterNode, const EdgeKey &, const EdgeKey &, const EdgeVal & = EdgeVal());

		void add_edge(const NodeKey &, const NodeKey &, const EdgeVal & = EdgeVal());
		void add_edge(IterNode, IterNode, const EdgeVal & = EdgeVal());


		void connect_edge(const NodeKey &, const NodeKey &, const EdgeKey &, const EdgeKey &);
		void connect_edge(IterNode, IterNode, const EdgeKey &, const EdgeKey &);

		void add_half_edge(const NodeKey &, const EdgeKey &, const EdgeVal & = EdgeVal());
		void add_half_edge(IterNode, const EdgeKey &, const EdgeVal & = EdgeVal());

		void del(const NodeKey &);
		void del(IterNode);
		void del_edges(const NodeKey &, const NodeKey &);
		void del_edges(IterNode, IterNode);

		void del_edge(const NodeKey &, const EdgeKey &);
		void del_edge(IterNode, const EdgeKey &);

		void del_half_edge(const NodeKey &, const EdgeKey &);
		void del_half_edge(IterNode, const EdgeKey &);

		void break_edge(const NodeKey &, const EdgeKey &);
		void break_edge(IterNode, const EdgeKey &);

		IterNode rename(const NodeKey &, const NodeKey &);
		IterNode rename(const IterNode &, const NodeKey &);

		int edge_num(const NodeKey &);
		int edge_num(const IterNode &);

		template <typename absorb_type, typename contract_type>
		void absorb(const NodeKey &, const NodeKey &, const absorb_type &, const contract_type &);
		template <typename absorb_type, typename contract_type>
		void absorb(IterNode, IterNode, const absorb_type &, const contract_type &);

		template <typename split_type>
		void
		split(const NodeKey &,
				const NodeKey &,
				const NodeKey &,
				const std::unordered_set<EdgeKey> &,
				const EdgeKey &,
				const EdgeKey &,
				const split_type &);

		template <typename split_type>
		std::pair<IterNode, IterNode>
		split(IterNode,
				const NodeKey &,
				const NodeKey &,
				const std::unordered_set<EdgeKey> &,
				const EdgeKey &,
				const EdgeKey &,
				const split_type &);

		template <typename split_type>
		void
		split(const NodeKey &,
				const NodeKey &,
				const std::unordered_set<EdgeKey> &,
				const EdgeKey &,
				const EdgeKey &,
				const split_type &);

		template <typename split_type>
		IterNode
		split(IterNode, const NodeKey &, const std::unordered_set<EdgeKey> &, const EdgeKey &, const EdgeKey &, const split_type &);
#ifdef NET_GRAPH_VIZ
		/**
		 * \brief 画出网络的图并输出到文件
		 */
		void draw_to_file(const std::string &, const std::string &, const bool) const;
		/**
		 * \brief 画出网络的图并输出到文件，强调网络的一部分
		 */
		void draw_to_file(const std::string &, const std::string &, const std::vector<std::set<NodeKey, typename Trait::nodekey_less>> &, const bool) const;
		/**
		 * \brief 画出网络的图并输出
		 */
		void draw(const std::string &, const bool) const;
		/**
		 * \brief 画出网络的图并输出，强调网络的一部分
		 */
		void draw(const std::string &, const std::vector<std::set<NodeKey, typename Trait::nodekey_less>> &, const bool) const;
#endif
		/**
		 * \brief 将网络转化为graphviz格式的字符串
		 */

		std::string gviz(const std::string &, const std::vector<std::set<NodeKey, typename Trait::nodekey_less>> &, const bool) const;

		std::string gviz_legend(const std::vector<std::set<NodeKey, typename Trait::nodekey_less>> &) const;
		/**
		 * \brief 判断网络是否包含一个格点
		 */
		bool contains(const NodeKey &);

		bool consistency(std::ostream & diagnosis = std::cout) const;

		template <typename absorb_type, typename contract_type>
		NodeVal contract(const absorb_type &, const contract_type &) const;

		template <typename absorb_type, typename contract_type>
		NodeVal contract(std::set<NodeKey, typename Trait::nodekey_less>, const absorb_type &, const contract_type &) const;

		template <typename TreeType, typename absorb_type, typename contract_type>
		NodeVal contract_tree(std::shared_ptr<TreeType>, const absorb_type &, const contract_type &) const;


		template <typename absorb_type, typename contract_type>
		NodeKey absorb(std::set<NodeKey, typename Trait::nodekey_less>, const absorb_type &, const contract_type &);

		template <typename TreeType, typename absorb_type, typename contract_type>
		NodeKey absorb_tree(std::shared_ptr<TreeType>, const absorb_type &, const contract_type &);

		template <typename absorb_type, typename contract_type>
		void
		tn_contract1(const NodeKey &, const std::set<NodeKey, typename Trait::nodekey_less> &, NodeVal &, const absorb_type &, const contract_type &)
				const;
		template <typename absorb_type, typename contract_type>
		void
		tn_contract1(IterNode, const std::set<NodeKey, typename Trait::nodekey_less> &, NodeVal &, const absorb_type &, const contract_type &) const;


		template <typename absorb_type, typename contract_type>
		NodeVal tn_contract2(
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const NodeVal &,
				const std::set<NodeKey, typename Trait::nodekey_less> &,
				const NodeVal &,
				const absorb_type &,
				const contract_type &) const;


		template <typename NetType2>
		NetType2
		fmap(std::function<typename NetType2::NodeValType(const NodeVal &)> f1,
			  std::function<typename NetType2::EdgeValType(const EdgeVal &)> f2) const;

		template <typename NetType2>
		NetType2
		fmap(std::function<typename NetType2::NodeValType(const NodeVal &)> f1,
			  std::function<typename NetType2::EdgeValType(const EdgeVal &)> f2,
			  std::function<typename NetType2::NodeKeyType(const NodeKey &)> f3,
			  std::function<typename NetType2::EdgeKeyType(const EdgeKey &)> f4) const;

		template <typename NodeVal2, typename EdgeVal2, typename Trait2>
		network<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2>
		gfmap(std::function<NodeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2) const;

		void fope(std::function<NodeVal(const NodeVal &)>, std::function<EdgeVal(const EdgeVal &)>);

		void gfope(std::function<NodeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2);
	};

	template <typename T>
	std::string to_string(const T & m) {
		std::stringstream a;
		a << m;
		return a.str();
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	// this is not default because [i].edges[j].neighbor needs redirection
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::network(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & N) {
		std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less>::operator=(N);
		for (auto & s : *this)
			s.second.relink(*this);
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	// this is not default because [i].edges[j].neighbor needs redirection
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::operator=(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & N) {
		if (this != &N) {
			std::map<NodeKey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>, typename Trait::nodekey_less>::operator=(N);
			for (auto & s : *this)
				s.second.relink(*this);
		}
		return *this;
	}

	// valid for c++17

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	bool network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::contains(const NodeKey & nodekey) {
		return (this->count(nodekey) == 1);
	}

	/**
	 * \brief 加一个格点，返回这个格点的iterator
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add(const NodeKey & nodekey) {
		auto [s1, succ1] = this->insert(make_pair(nodekey, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>(nodekey)));
		if (!succ1) {
			throw key_exist_error("In network.add, node " + to_string(nodekey) + " already exists!");
		}
		return s1;
	}

	/**
	 * \brief 加一个网络
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & n) {
		for (auto & s : n)
			add(s.first)->second = s.second;
		for (auto & s : *this)
			s.second.relink(*this);
	}
	/**
	 * \brief 删除一个格点
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del(const NodeKey & nodekey) {
		auto node_itr = this->find(nodekey);
		if (node_itr == this->end()) {
			throw key_unfound_error("In network.del, node " + to_string(nodekey) + " is not found!");
		}

		node_itr->second.delete_nbedge();
		this->erase(node_itr);
	}
	/**
	 * \brief 删除一个格点
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del(network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it) {
		it->second.delete_nbedge();
		this->erase(it);
	}

	/**
	 * \brief 删除两个格点之间的所有边
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del_edges(const NodeKey & nodekey1, const NodeKey & nodekey2) {
		auto node_itr1 = this->find(nodekey1);
		if (node_itr1 == this->end()) {
			throw key_unfound_error("In network.del_edge, node " + to_string(nodekey1) + " is not found!");
		}

		if (this->count(nodekey2) == 0) {
			throw key_unfound_error("In network.del_edge, node " + to_string(nodekey2) + " is not found!");
		}

		node_itr1->second.delete_edge([&nodekey2](auto & egitr) { return egitr->second.nbkey == nodekey2; });
	}

	/**
	 * \brief 删除两个格点之间的所有边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del_edges(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it1,
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it2) {
		it1->second.delete_edge([&it2](auto & egitr) { return egitr->second.nbkey == it2->first; });
	}

	/**
	 * \brief 删除一个腿连着的边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del_edge(const NodeKey & nodekey, const EdgeKey & ind) {
		auto node_itr = this->find(nodekey);
		if (node_itr == this->end()) {
			throw key_unfound_error("In network.del_leg, node " + to_string(nodekey) + " is not found!");
		}
		node_itr->second.delete_edge([&ind](auto & egitr) { return egitr->first == ind; });
	}

	/**
	 * \brief 删除一个腿连着的边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del_edge(network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it, const EdgeKey & ind) {
		it->second.delete_edge([&ind](auto & egitr) { return egitr->first == ind; });
	}

	/**
	 * \brief 删除一个半边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del_half_edge(const NodeKey & nodekey, const EdgeKey & ind) {
		auto node_itr = this->find(nodekey);
		if (node_itr == this->end()) {
			throw key_unfound_error("In network.del_leg, node " + to_string(nodekey) + " is not found!");
		}
		node_itr->second.delete_half_edge([&ind](auto & egitr) { return egitr->first == ind; });
	}

	/**
	 * \brief 删除一个半边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::del_half_edge(network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it, const EdgeKey & ind) {
		it->second.delete_half_edge([&ind](auto & egitr) { return egitr->first == ind; });
	}


	/**
	 * \brief 将一个半边组成的边拆开
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::break_edge(const NodeKey & nodekey, const EdgeKey & ind) {
		auto node_itr = this->find(nodekey);
		if (node_itr == this->end()) {
			throw key_unfound_error("In network.del_leg, node " + to_string(nodekey) + " is not found!");
		}
		node_itr->second.break_edge([&ind](auto & egitr) { return egitr->first == ind; });
	}

	/**
	 * \brief 将一个半边组成的边拆开
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::break_edge(network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it, const EdgeKey & ind) {
		it->second.break_edge([&ind](auto & egitr) { return egitr->first == ind; });
	}
	/**
	 * \brief 重命名一个格点
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::rename(const NodeKey & old_key, const NodeKey & new_key) {
		auto node_handle = this->extract(old_key);
		if (node_handle.empty()) {
			throw key_unfound_error("In network.rename, node " + to_string(old_key) + " is not found!");
		}
		node_handle.key = new_key;

		auto status = this->insert(std::move(node_handle));
		if (!status.inserted)
			throw key_exist_error("In network.rename, node " + to_string(new_key) + " already exists!");
		status.position->second.reset_nbkey_of_nb(new_key);

		return status.position;
	}
	/**
	 * \brief 重命名一个格点
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::rename(
			const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode & it,
			const NodeKey & new_key) {
		auto node_handle = this->extract(it);
		if (node_handle.empty()) {
			throw key_unfound_error("In network.rename, node " + to_string(it->first) + " is not found!");
		}
		node_handle.key = new_key;
		node_handle.value.key = new_key;

		auto status = this->insert(std::move(node_handle));
		if (!status.inserted)
			throw key_exist_error("In network.rename, node " + to_string(new_key) + " already exists!");
		status.position->second.reset_nbkey_of_nb(new_key);

		return status.position;
	}

	/**
	 * \brief 返回边数目
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	int network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::edge_num(const NodeKey & nk) {
		return (*this)[nk].edges.size();
	}

	/**
	 * \brief 返回边数目
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	int network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::edge_num(const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode & it) {
		return it->second.edges.size();
	}

	/**
	 * \brief 加一条半边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_half_edge(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it1,
			const EdgeKey & ind1,
			const EdgeVal & edgeval) {
		it1->second.add_half_edge(ind1,edgeval);
	}
	
	/**
	 * \brief 加一条半边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_half_edge(
			const NodeKey & nodekey1,
			const EdgeKey & ind1,
			const EdgeVal & edgeval) {
		auto node_itr1 = this->find(nodekey1);
		if (node_itr1 == this->end()) {
			throw key_unfound_error("In network.add_half_edge, node " + to_string(nodekey1) + " is not found!");
		}
		node_itr1->second.add_half_edge(ind1,edgeval);
	}

	/**
	 * \brief 加一条边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_edge(
			const NodeKey & nodekey1,
			const NodeKey & nodekey2,
			const EdgeKey & ind1,
			const EdgeKey & ind2,
			const EdgeVal & edgeval) {
		auto node_itr1 = this->find(nodekey1);
		if (node_itr1 == this->end()) {
			throw key_unfound_error("In network.add_edge, node " + to_string(nodekey1) + " is not found!");
		}
		auto node_itr2 = this->find(nodekey2);
		if (node_itr2 == this->end()) {
			throw key_unfound_error("In network.add_edge, node " + to_string(nodekey2) + " is not found!");
		}

		add_edge_node(node_itr1,node_itr2,ind1,ind2,edgeval);
	}

	/**
	 * \brief 加一条边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_edge(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it1,
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it2,
			const EdgeKey & ind1,
			const EdgeKey & ind2,
			const EdgeVal & edgeval) {
		add_edge_node(it1,it2,ind1,ind2,edgeval);
	}
	/**
	 * \brief 加一条边。根据格点名字和Trait::bind自动生成边的名字
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_edge(const NodeKey & nodekey1, const NodeKey & nodekey2, const EdgeVal & edgeval) {
		add_edge(nodekey1, nodekey2, Trait::bind(nodekey1, nodekey2), Trait::bind(nodekey2, nodekey1), edgeval);
	}
	/**
	 * \brief 加一条边。根据格点名字和Trait::bind自动生成边的名字
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::add_edge(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it1,
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it2,
			const EdgeVal & edgeval) {
		add_edge(it1, it2, Trait::bind(it1->first, it2->first), Trait::bind(it2->first, it1->first), edgeval);
	}


	/**
	 * \brief 连接两条半边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::connect_edge(
			const NodeKey & nodekey1,
			const NodeKey & nodekey2,
			const EdgeKey & ind1,
			const EdgeKey & ind2) {
		auto node_itr1 = this->find(nodekey1);
		if (node_itr1 == this->end()) {
			throw key_unfound_error("In network.connect_edge, node " + to_string(nodekey1) + " is not found!");
		}
		auto node_itr2 = this->find(nodekey2);
		if (node_itr2 == this->end()) {
			throw key_unfound_error("In network.connect_edge, node " + to_string(nodekey2) + " is not found!");
		}

		connect_edge_node(node_itr1,node_itr2,ind1,ind2);
	}

	/**
	 * \brief 连接两条半边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::connect_edge(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it1,
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it2,
			const EdgeKey & ind1,
			const EdgeKey & ind2) {
		connect_edge_node(it1,it2,ind1,ind2);
	}
	/**
	 * \brief 缩并两个格点，以及它们相连的边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::absorb(
			const NodeKey & nodekey1,
			const NodeKey & nodekey2,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		auto node_itr1 = this->find(nodekey1);
		if (node_itr1 == this->end()) {
			throw key_unfound_error("In network.absorb, node " + to_string(nodekey1) + " is not found!");
		}
		auto node_itr2 = this->find(nodekey2);
		if (node_itr2 == this->end()) {
			throw key_unfound_error("In network.absorb, node " + to_string(nodekey2) + " is not found!");
		}

		node_itr1->second.absorb_nb(nodekey2, node_itr2->second.val, absorb_fun, contract_fun);
		node_itr2->second.transfer_edge(node_itr1, [&node_itr1](auto & egitr) { return egitr->second.nbitr != node_itr1; });
		this->erase(node_itr2);
	}

	/**
	 * \brief 缩并两个格点，以及它们相连的边
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::absorb(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode node_itr1,
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode node_itr2,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		node_itr1->second.absorb_nb(node_itr2->first, node_itr2->second.val, absorb_fun, contract_fun);
		node_itr2->second.transfer_edge(node_itr1, [&node_itr1](auto & egitr) { return egitr->second.nbitr != node_itr1; });
		this->erase(node_itr2);
	}
	/**
	 * \brief 将格点1分解为格点2和格点3，inds为属于格点3点半边，格点2和格点3通过半边ind2和ind3相连。
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename split_type>
	std::pair<
			typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode,
			typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode>
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::split(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode s1,
			const NodeKey & nodekey2,
			const NodeKey & nodekey3,
			const std::unordered_set<EdgeKey> & inds,
			const EdgeKey & ind2,
			const EdgeKey & ind3,
			const split_type & split_fun) {
		auto s2 = add(nodekey2);
		auto s3 = add(nodekey3);

		s1->second.transfer_edge(s2, s3, [&inds](auto & egitr) { return inds.count(egitr->first) == 0; });

		EdgeVal env;
		split_fun(s1->second.val, s2->second.val, s3->second.val, inds, ind2, ind3, env);
		add_edge_node(s2,s3,ind2,ind3,env);
		this->erase(s1);
		return std::make_pair(s2, s3);
	}
	/**
	 * \brief 将格点1分解为格点2和格点3，inds为属于格点3点半边，格点2和格点3通过半边ind2和ind3相连。
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename split_type>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::split(
			const NodeKey & nodekey1,
			const NodeKey & nodekey2,
			const NodeKey & nodekey3,
			const std::unordered_set<EdgeKey> & inds,
			const EdgeKey & ind2,
			const EdgeKey & ind3,
			const split_type & split_fun) {
		auto s1 = this->find(nodekey1);
		auto s2 = add(nodekey2);
		auto s3 = add(nodekey3);

		s1->second.transfer_edge(s2, s3, [&inds](auto & egitr) { return inds.count(egitr->first) == 0; });

		EdgeVal env;
		split_fun(s1->second.val, s2->second.val, s3->second.val, inds, ind2, ind3, env);
		add_edge_node(s2,s3,ind2,ind3,env);
		this->erase(s1);
	}
	/**
	 * \brief 将格点1分解为格点1和格点2，inds为属于格点2点半边，格点1和格点2通过半边ind2和ind3相连。
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename split_type>
	typename network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::split(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode s1,
			const NodeKey & nodekey2,
			const std::unordered_set<EdgeKey> & inds,
			const EdgeKey & ind1,
			const EdgeKey & ind2,
			const split_type & split_fun) {
		auto s2 = add(nodekey2);

		s1->second.transfer_edge(s2, [&inds](auto & egitr) { return inds.count(egitr->first) == 1; });

		auto temp = s1->second.val;
		EdgeVal env;
		split_fun(temp, s1->second.val, s2->second.val, inds, ind1, ind2, env);
		// s1->second.set_edge(ind1, nodekey2, ind2, s2, env);
		// s2->second.set_edge(ind2, s1->first, ind1, s1, env);
		add_edge_node(s1,s2,ind1,ind2,env);
		return s2;
	}
	/**
	 * \brief 将格点1分解为格点1和格点2，inds为属于格点2点半边，格点1和格点2通过半边ind2和ind3相连。
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename split_type>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::split(
			const NodeKey & nodekey1,
			const NodeKey & nodekey2,
			const std::unordered_set<EdgeKey> & inds,
			const EdgeKey & ind1,
			const EdgeKey & ind2,
			const split_type & split_fun) {

		auto s1 = this->find(nodekey1);
		auto s2 = add(nodekey2);

		s1->second.transfer_edge(s2, [&inds](auto & egitr) { return inds.count(egitr->first) == 1; });

		auto temp = s1->second.val;

		EdgeVal env;
		split_fun(temp, s1->second.val, s2->second.val, inds, ind1, ind2, env);
		// s1->second.set_edge(ind1, nodekey2, ind2, s2, env);
		// s2->second.set_edge(ind2, nodekey1, ind1, s1, env);
		add_edge_node(s1,s2,ind1,ind2,env);
	}
	/**
	 * \brief 缩并整个网络（不改变网络，只得到值）
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	NodeVal network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::contract(const absorb_type & absorb_fun, const contract_type & contract_fun) const {
		NodeVal tot;
		std::set<NodeKey, typename Trait::nodekey_less> contracted;
		for (auto s : *this) {
			if (contracted.count(s.first) == 0) {
				tn_contract1(s.first, contracted, tot, absorb_fun, contract_fun);
				contracted.insert(s.first);
			}
		}
		return tot;
	}

	/**
	 * \brief 缩并一部分网络（改变网络）
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	NodeKey network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::absorb(
			std::set<NodeKey, typename Trait::nodekey_less> part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		if(part.size()==0){
			return NodeKey();
		}else{
			auto it0=part.begin();
			for(auto it =part.begin();it!=part.end();++it){
				if(it!=it0) absorb(*it0,*it,absorb_fun,contract_fun);
			}
			return *it0;
		}
	}

	/**
	 * \brief 缩并一个树（改变网络）
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename TreeType, typename absorb_type, typename contract_type>
	NodeKey network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::absorb_tree(
			std::shared_ptr<TreeType> ctree,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) {
		if (!ctree)
			return NodeKey();
		else if (!ctree->left_child && !ctree->right_child)
			return absorb(ctree->val.node_set, absorb_fun, contract_fun);
		else if (!ctree->left_child && ctree->right_child)
			return absorb_tree<TreeType>(ctree->right_child, absorb_fun, contract_fun);
		else if (ctree->left_child && !ctree->right_child)
			return absorb_tree<TreeType>(ctree->left_child, absorb_fun, contract_fun);
		else{
			NodeKey left = absorb_tree<TreeType>(ctree->left_child, absorb_fun, contract_fun);
			NodeKey right = absorb_tree<TreeType>(ctree->right_child, absorb_fun, contract_fun);
			if(left == NodeKey()){
				return right;
			}else if(right == NodeKey()){
				return left;
			}else{
				absorb(left,right, absorb_fun, contract_fun);
				return left;
			}

		}
	}

	/**
	 * \brief 缩并一部分网络（不改变网络，只得到值）
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	NodeVal network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::contract(
			std::set<NodeKey, typename Trait::nodekey_less> part,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) const {
		NodeVal tot;
		std::set<NodeKey, typename Trait::nodekey_less> contracted;
		for (auto p : part) {
			if (contracted.count(p) == 0) {
				tn_contract1(p, contracted, tot, absorb_fun, contract_fun);
				contracted.insert(p);
			}
		}
		return tot;
	}

	/**
	 * \brief 缩并一个树（不改变网络，只得到值）
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename TreeType, typename absorb_type, typename contract_type>
	NodeVal network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::contract_tree(
			std::shared_ptr<TreeType> ctree,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) const {
		if (!ctree)
			return NodeVal();
		else if (!ctree->left_child && !ctree->right_child)
			return contract(ctree->val.node_set, absorb_fun, contract_fun);
		else if (!ctree->left_child && ctree->right_child)
			return contract_tree<TreeType>(ctree->right_child, absorb_fun, contract_fun);
		else if (ctree->left_child && !ctree->right_child)
			return contract_tree<TreeType>(ctree->left_child, absorb_fun, contract_fun);
		else
			return tn_contract2(
					ctree->left_child->val.node_set,
					contract_tree<TreeType>(ctree->left_child, absorb_fun, contract_fun),
					ctree->right_child->val.node_set,
					contract_tree<TreeType>(ctree->right_child, absorb_fun, contract_fun),
					absorb_fun,
					contract_fun);
	}


	/**
	 * \brief 缩并的辅助函数
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::tn_contract1(
			const NodeKey & nodekey,
			const std::set<NodeKey, typename Trait::nodekey_less> & group,
			NodeVal & ten,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) const {
		auto node_itr1 = this->find(nodekey);
		if (node_itr1 == this->end()) {
			throw key_unfound_error("In network.tn_contract1, node " + to_string(nodekey) + " is not found!");
		}

		if (group.size() == 0) {
			ten = node_itr1->second.val;
		} else {
			auto node_t = node_itr1->second.val;
			std::set<std::pair<EdgeKey, EdgeKey>, typename Trait::edge2key_less> ind_pairs;
			node_itr1->second.harmless_absorb_nb(node_t, ind_pairs, absorb_fun, [&group](auto & eg) { return group.count(eg.second.nbkey) == 1; });
			ten = contract_fun(node_t, ten, ind_pairs);
		}
	}

	/**
	 * \brief 缩并的辅助函数
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	void network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::tn_contract1(
			network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::IterNode it1,
			const std::set<NodeKey, typename Trait::nodekey_less> & group,
			NodeVal & ten,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) const {
		if (group.size() == 0) {
			ten = it1->second.val;
		} else {
			auto node_t = it1->second.val;
			std::set<std::pair<EdgeKey, EdgeKey>, typename Trait::edge2key_less> ind_pairs;

			it1->second.harmless_absorb_nb(node_t, ind_pairs, absorb_fun, [&group](auto & eg) { return group.count(eg.second.nbkey) == 1; });

			ten = contract_fun(node_t, ten, ind_pairs);
		}
	}

	/**
	 * \brief 缩并的辅助函数
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename absorb_type, typename contract_type>
	NodeVal network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::tn_contract2(
			const std::set<NodeKey, typename Trait::nodekey_less> & group1,
			const NodeVal & ten1,
			const std::set<NodeKey, typename Trait::nodekey_less> & group2,
			const NodeVal & ten2,
			const absorb_type & absorb_fun,
			const contract_type & contract_fun) const {
		if (group1.size() == 0)
			return ten2;
		if (group2.size() == 0)
			return ten1;
		std::set<std::pair<EdgeKey, EdgeKey>, typename Trait::edge2key_less> ind_pairs;
		auto ten1_temp = ten1;
		for (auto & nk : group1)
			this->at(nk).harmless_absorb_nb(ten1_temp, ind_pairs, absorb_fun, [&group2](auto & eg) { return group2.count(eg.second.nbkey) == 1; });
		return contract_fun(ten1_temp, ten2, ind_pairs);
	}
	/**
	 * \brief 利用格点上的信息的函数和边上信息的函数从一个网络得到另一个网络
	 */
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename NetType2>
	NetType2 network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::fmap(
			std::function<typename NetType2::NodeValType(const NodeVal &)> f1,
			std::function<typename NetType2::EdgeValType(const EdgeVal &)> f2) const {
		NetType2 result;
		for (auto & s : *this)
			result[s.first] = s.second.template fmap<typename NetType2::NodeType>(s.first,f1, f2);
		for (auto & s : result)
			s.second.relink(result);
		return result;
	}
	/**
	 * \brief
	 * 利用格点上的信息的函数和边上信息的函数从一个网络得到另一个网络，同时做sitekey和edgekey的变换
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename NetType2>
	NetType2 network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::fmap(
			std::function<typename NetType2::NodeValType(const NodeVal &)> f1,
			std::function<typename NetType2::EdgeValType(const EdgeVal &)> f2,
			std::function<typename NetType2::NodeKeyType(const NodeKey &)> f3,
			std::function<typename NetType2::EdgeKeyType(const EdgeKey &)> f4) const {
		NetType2 result;
		for (auto & s : *this){
			result[f3(s.first)] = s.second.template fmap<typename NetType2::NodeType>(f3(s.first), f1, f2, f3, f4);
		}
		for (auto & s : result)
			s.second.relink(result);
		return result;
	}
	/**
	 * \brief
	 * 利用格点上的信息的函数和边上信息的函数从一个网络得到另一个网络
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	template <typename NodeVal2, typename EdgeVal2, typename Trait2>
	network<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2> network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::gfmap(
			std::function<NodeVal2(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal2(
					const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2) const {
		network<NodeVal2, EdgeVal2, NodeKey, EdgeKey, Trait2> result;
		for (auto & s : *this)
			result[s.first] = s.second.template gfmap<NodeVal2,EdgeVal2,Trait2>(s.first, f1, f2);
		for (auto & s : result)
			s.second.relink(result);
		return result;
	}
	/**
	 * \brief
	 * 利用函数更新格点和边上的信息
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::fope(std::function<NodeVal(const NodeVal &)> f1, std::function<EdgeVal(const EdgeVal &)> f2) {
		for (auto & s : *this)
			s.second.fope(f1, f2);
	}
	/**
	 * \brief
	 * 利用函数更新格点和边上的信息，可用于初始化
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	void
	network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::gfope(
			std::function<NodeVal(const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &)> f1,
			std::function<EdgeVal(
					const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> &,const EdgeKey &)> f2) {
		for (auto & s : *this)
			s.second.gfope(f1, f2);
	}

	/**
	 * \brief 判断网络是否是没有冲突
	 */

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	bool network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait>::consistency(std::ostream & diagnosis) const {
		for (auto & s : *this)
			if (!s.second.consistency(*this,s.first, diagnosis)) {
				diagnosis << "Error at node " << s.first << '\n';
				return false;
			}
		return true;
	}

} // namespace net
#endif
