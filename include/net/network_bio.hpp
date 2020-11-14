#ifndef NETWORK_BIO_H
#define NETWORK_BIO_H

#include <map>
#include <vector>
#include "site.hpp"
#include "network.hpp"

namespace net{

	template <typename T>
	std::ostream & operator<(std::ostream & os,const T & m);
	template <typename T>
	std::istream & operator>(std::istream & is,T & m);

	std::ostream & operator<(std::ostream & os,const std::string & m);
	std::istream & operator>(std::istream & is,std::string & m);

	template <typename T>
	std::ostream & operator<(std::ostream & os,const std::set<T> & m);
	template <typename T>
	std::istream & operator>(std::istream & is,std::set<T> & m);

	template <typename T,typename V>
	std::ostream & operator<(std::ostream & os,const std::map<T,V> & m);
	template <typename T,typename V>
	std::istream & operator>(std::istream & is,std::map<T,V> & m);

	template <typename T>
	std::ostream & operator<(std::ostream & os,const std::vector<T> & m);
	template <typename T>
	std::istream & operator>(std::istream & is,std::vector<T> & m);


	template <typename T,typename V>
	std::ostream & operator<(std::ostream & os,const bond<T,V> & b){
		return os<b.name<b.ind<b.val;
	}

	template <typename T,typename V>
	std::istream & operator>(std::istream & is,bond<T,V> & b){
		return is>b.name>b.ind>b.val;
	}

	template <typename T,typename V>
	std::ostream & operator<(std::ostream & os,const site<T,V> & s){
		return os<s.bonds<s.position<s.val;
	}

	template <typename T,typename V>
	std::istream & operator>(std::istream & is,site<T,V> & s){
		is>s.bonds;
		is>s.position;
		is>s.val;
		return is;
	}

	template <typename T,typename V>
	std::ostream & operator<(std::ostream & os,const network<T,V> & n){
		return os<n.name<n.name_at<n.sites;
	}

	template <typename T,typename V>
	std::istream & operator>(std::istream & is,network<T,V> &n){
		is>n.name>n.name_at>n.sites;
		for(auto & s:n.sites){
			for (auto & b: s.second.bonds){
				b.second.neighbor=&(n.sites[b.second.name]);
			}
		}
		return is;
	}

	template <typename T,typename V>
	std::ostream & operator<(std::ostream & os,const std::map<T,V> & m){
		unsigned int len=m.size();
		os<len;
		for(auto & i : m){
			os<i.first<i.second;
		}
		return os;
	}

	template <typename T,typename V>
	std::istream & operator>(std::istream & is,std::map<T,V> & m){
		unsigned int len;
		is>len;
		T a;
		V b;
		m.clear();
		for (int i=0;i<len;++i){
			is>a>b;
			m[a]=b;
		}
		return is;
	}

	template <typename T>
	std::ostream & operator<(std::ostream & os,const std::vector<T> & m){
		unsigned int len=m.size();
		os<len;
		for(auto & i : m){
			os<i;
		}
		return os;
	}

	template <typename T>
	std::istream & operator>(std::istream & is,std::vector<T> & m){
		unsigned int len;
		is>len;
		T a;
		m.clear();
		for (int i=0;i<len;++i){
			is>a;
			m.push_back(a);
		}
		return is;
	}

	template <typename T>
	std::ostream & operator<(std::ostream & os,const std::set<T> & m){
		unsigned int len=m.size();
		os<len;
		for(auto & i : m){
			os<i;
		}
		return os;
	}

	template <typename T>
	std::istream & operator>(std::istream & is,std::set<T> & m){
		unsigned int len;
		is>len;
		T a;
		m.clear();
		for (int i=0;i<len;++i){
			is>a;
			m.insert(a);
		}
		return is;
	}

	std::ostream & operator<(std::ostream & os,const std::string & m){
		unsigned int len=m.size();
		os<len;
		return os.write(&m[0], len);
	}
	std::istream & operator>(std::istream & is,std::string & m){
		unsigned int len;
		is>len;
		m.resize(len);
		return is.read(&m[0], len);
	}

	template <typename T>
	std::ostream & operator<(std::ostream & os,const T & m){
		return os.write(reinterpret_cast<const char*>(&m), sizeof m);
	}
	template <typename T>
	std::istream & operator>(std::istream & is,T & m){
		return is.read(reinterpret_cast<char*>(&m), sizeof m);
	}

}
#endif