/**
 * \file defalut_bio.hpp
 * \brief 可供参考的文本i/o
 * 
 */

#ifndef NET_REFERENCE_IO_HPP
#define NET_REFERENCE_IO_HPP

#include "type.hpp"
#include <map>
#include <set>
#include <vector>

namespace net {

	template <typename T, typename V>
	std::ostream & reference_write_text(std::ostream & os, const std::map<T, V> & m);
	template <typename T, typename V>
	std::istream & reference_read_text(std::istream & is, std::map<T, V> & m);
	template <typename T>
	std::ostream & reference_write_text(std::ostream & os, const std::vector<T> & m);
	template <typename T>
	std::istream & reference_read_text(std::istream & is, std::vector<T> & m);
	template <typename T>
	std::ostream & reference_write_text(std::ostream & os, const std::set<T> & m);
	template <typename T>
	std::istream & reference_read_text(std::istream & is, std::set<T> & m);
	template <typename T>
	std::ostream & reference_write_text(std::ostream & os, const T & m);
	template <typename T>
	std::istream & reference_read_text(std::istream & is, T & m);

	template <typename T, typename V>
	std::ostream & reference_write_text(std::ostream & os, const std::map<T, V> & m) {
		os << m.size();
		for (auto & i : m) {
			reference_write_text(reference_write_text(os << ' ', i.first) << ' ', i.second);
		}
		return os;
	}

	template <typename T, typename V>
	std::istream & reference_read_text(std::istream & is, std::map<T, V> & m) {
		int len;
		is >> len;
		T a;
		V b;
		m.clear();
		for (int i = 0; i < len; ++i) {
			reference_read_text(reference_read_text(is, a), b);
			m[a] = b;
		}
		return is;
	}

	template <typename T>
	std::ostream & reference_write_text(std::ostream & os, const std::vector<T> & m) {
		os << m.size();
		for (auto & i : m) {
			reference_write_text(os << ' ', i);
		}
		return os;
	}

	template <typename T>
	std::istream & reference_read_text(std::istream & is, std::vector<T> & m) {
		int len;
		is >> len;
		T a;
		m.clear();
		for (int i = 0; i < len; ++i) {
			reference_read_text(is, a);
			m.push_back(a);
		}
		return is;
	}

	template <typename T>
	std::ostream & reference_write_text(std::ostream & os, const std::set<T> & m) {
		os << m.size();
		for (auto & i : m) {
			reference_write_text(os << ' ', i);
		}
		return os;
	}

	template <typename T>
	std::istream & reference_read_text(std::istream & is, std::set<T> & m) {
		int len;
		is >> len;
		T a;
		m.clear();
		for (int i = 0; i < len; ++i) {
			reference_read_text(is, a);
			m.insert(a);
		}
		return is;
	}

	template <typename T>
	std::ostream & reference_write_text(std::ostream & os, const T & m) {
		return os << m;
	}

	template <typename T>
	std::istream & reference_read_text(std::istream & is, T & m) {
		return is >> m;
	}

} // namespace net
#endif