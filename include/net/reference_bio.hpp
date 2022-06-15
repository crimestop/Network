/**
 * \file defalut_bio.hpp
 * \brief 可供参考的二进制i/o
 * 
 */

#ifndef NET_REFERENCE_BIO_HPP
#define NET_REFERENCE_BIO_HPP

#include "type.hpp"
#include <complex>
#include <map>
#include <set>
#include <vector>

namespace net {

	template <typename T, std::enable_if_t<is_scalar_v<T>, int> = 0>
	std::ostream & reference_write_bin(std::ostream & os, const T & m) {
		return os.write(reinterpret_cast<const char *>(&m), sizeof m);
	}

	template <typename T, std::enable_if_t<is_scalar_v<T>, int> = 0>
	std::istream & reference_read_bin(std::istream & is, T & m) {
		return is.read(reinterpret_cast<char *>(&m), sizeof m);
	}

	template <typename T, std::enable_if_t<!is_scalar_v<T>, int> = 0>
	std::ostream & reference_write_bin(std::ostream & os, const T & m) {
		return os < m;
	}

	template <typename T, std::enable_if_t<!is_scalar_v<T>, int> = 0>
	std::istream & reference_read_bin(std::istream & is, T & m) {
		return is > m;
	}

	std::ostream & reference_write_bin(std::ostream & os, const std::string & m);
	std::istream & reference_read_bin(std::istream & is, std::string & m);

	template <typename T>
	std::ostream & reference_write_bin(std::ostream & os, const std::set<T> & m);
	template <typename T>
	std::istream & reference_read_bin(std::istream & is, std::set<T> & m);

	template <typename T, typename V>
	std::ostream & reference_write_bin(std::ostream & os, const std::pair<T, V> & m);
	template <typename T, typename V>
	std::istream & reference_read_bin(std::istream & is, std::pair<T, V> & m);

	template <typename T, typename V>
	std::ostream & reference_write_bin(std::ostream & os, const std::map<T, V> & m);
	template <typename T, typename V>
	std::istream & reference_read_bin(std::istream & is, std::map<T, V> & m);

	template <typename T>
	std::ostream & reference_write_bin(std::ostream & os, const std::vector<T> & m);
	template <typename T>
	std::istream & reference_read_bin(std::istream & is, std::vector<T> & m);

	template <typename T, typename V>
	std::ostream & reference_write_bin(std::ostream & os, const std::map<T, V> & m) {
		unsigned int len = m.size();
		reference_write_bin(os, len);
		for (auto & i : m) {
			reference_write_bin(reference_write_bin(os, i.first), i.second);
		}
		return os;
	}

	template <typename T, typename V>
	std::istream & reference_read_bin(std::istream & is, std::map<T, V> & m) {
		unsigned int len;
		reference_read_bin(is, len);
		T a;
		V b;
		m.clear();
		for (int i = 0; i < len; ++i) {
			reference_read_bin(reference_read_bin(is, a), b);
			m[a] = b;
		}
		return is;
	}

	template <typename T>
	std::ostream & reference_write_bin(std::ostream & os, const std::set<T> & m) {
		unsigned int len = m.size();
		reference_write_bin(os, len);
		for (auto & i : m) {
			reference_write_bin(os, i);
		}
		return os;
	}

	template <typename T>
	std::istream & reference_read_bin(std::istream & is, std::set<T> & m) {
		unsigned int len;
		reference_read_bin(is, len);
		T a;
		m.clear();
		for (int i = 0; i < len; ++i) {
			reference_read_bin(is, a);
			m.insert(a);
		}
		return is;
	}

	template <typename T, typename V>
	std::ostream & reference_write_bin(std::ostream & os, const std::pair<T, V> & m) {
		reference_write_bin(os, m.first);
		reference_write_bin(os, m.second);
		return os;
	}

	template <typename T, typename V>
	std::istream & reference_read_bin(std::istream & is, std::pair<T, V> & m) {
		reference_read_bin(is, m.first);
		reference_read_bin(is, m.second);
		return is;
	}

	inline std::ostream & reference_write_bin_str(std::ostream & os, const std::string & m) {
		unsigned int len = m.size();
		reference_write_bin(os, len);
		return os.write(&m[0], len);
	}

	inline std::istream & reference_read_bin(std::istream & is, std::string & m) {
		unsigned int len;
		reference_read_bin(is, len);
		m.resize(len);
		return is.read(&m[0], len);
	}

	template <typename T>
	std::ostream & reference_write_bin(std::ostream & os, const std::vector<T> & m) {
		unsigned int len = m.size();
		reference_write_bin(os, len);
		if (is_scalar_v<T>)
			return os.write(reinterpret_cast<const char *>(&m[0]), len * (sizeof m[0]));
		else {
			for (auto & i : m) {
				reference_write_bin(os, i);
			}
			return os;
		}
	}

	template <typename T>
	std::istream & reference_read_bin(std::istream & is, std::vector<T> & m) {
		unsigned int len;
		T a;
		reference_read_bin(is, len);
		if (is_scalar_v<T>) {
			m.resize(len);
			return is.read(reinterpret_cast<char *>(&m[0]), len * (sizeof m[0]));
		} else {
			m.clear();
			m.reserve(len);
			for (int i = 0; i < len; ++i) {
				reference_read_bin(is, a);
				m.push_back(a);
			}
			return is;
		}
	}

} // namespace net
#endif