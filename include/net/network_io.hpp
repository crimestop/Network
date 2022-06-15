#ifndef NET_NETWORK_IO_HPP
#define NET_NETWORK_IO_HPP

#include "network.hpp"
#include "node.hpp"
#include "type.hpp"
#include <map>
#include <vector>

namespace net {
	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	class network;

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::ostream & operator<<(std::ostream & os, const network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & n) {
		os << n.size();
		for (auto & i : n) {
			os << ' ';
			Trait::nodekey_write_text(os, i.first);
			os << ' ';
			output_node_text(os, i.second);
		}
		return os;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::ostream & output_node_text(std::ostream & os, const node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & s) {
		os << s.edges.size() << ' ';
		for (auto & i : s.edges) {
			Trait::edgekey_write_text(os, i.first);
			os << ' ';
			output_edge_text(os, i.second);
			os << ' ';
		}
		Trait::nodekey_write_text(os, s.key);
		Trait::nodeval_write_text(os, s.val);
		return os;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::ostream & output_edge_text(std::ostream & os, const half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & b) {
		os << b.nb_num;
		os << ' ';
		Trait::edgeval_write_text(os, b.val);
		if( b.nb_num!=0){
			os << ' ';
			Trait::nodekey_write_text(os, b.nbkey);
			os << ' ';
			Trait::edgekey_write_text(os, b.nbind);
		}
		return os;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::istream & operator>>(std::istream & is, network<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & n) {
		unsigned int len;
		is >> len;
		NodeKey a;
		n.clear();
		for (int i = 0; i < len; ++i) {
			Trait::nodekey_read_text(is, a);
			input_node_text(is, n[a]);
		}
		for (auto & s : n)
			s.relink(n);
		return is;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::istream & input_node_text(std::istream & is, node<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & s) {
		unsigned int len;
		is >> len;
		EdgeKey a;
		s.edges.clear();
		for (int i = 0; i < len; ++i) {
			Trait::edgekey_read_text(is, a);
			input_edge_text(is, s.edges[a]);
		}
		Trait::nodekey_read_text(is, s.key);
		Trait::nodeval_read_text(is, s.val);
		return is;
	}

	template <typename NodeVal, typename EdgeVal, typename NodeKey, typename EdgeKey, typename Trait>
	std::istream & input_edge_text(std::istream & is, half_edge<NodeVal, EdgeVal, NodeKey, EdgeKey, Trait> & b) {
		is>>b.nb_num;
		Trait::edgeval_read_text(is, b.val);
		if(b.nb_num!=0){
			Trait::nodekey_read_text(is, b.nbkey);
			Trait::edgekey_read_text(is, b.nbind);
		}
		return is;
	}

} // namespace net
#endif
