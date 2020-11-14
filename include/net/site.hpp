#ifndef SITE_H
#define SITE_H

#include <map>
#include <string>
#include <vector>

namespace net{
	template <typename T,typename V>
	class site;

   /**
    * \brief 描述着连接格点的边上信息，实际上这是一个半边而不是是一个完整的边
    *
    * \see site
    */
	template <typename T,typename V>
	class bond{
	template <typename T1,typename V1>
	friend std::ostream & operator<<(std::ostream &,const bond<T1,V1> &);
	template <typename T1,typename V1>
	friend std::istream & operator>>(std::istream &, bond<T1,V1> &);
	template <typename T1,typename V1>
	friend std::ostream & operator<(std::ostream &,const bond<T1,V1> &);
	template <typename T1,typename V1>
	friend std::istream & operator>(std::istream &, bond<T1,V1> &);
	public:
        /**
         * \brief 所指向的格点的名称
         */
		std::string name;
		/**
		 * \brief 所指向格点连接自身的边的名称
		 */
		std::string ind;
		site<T,V> * neighbor;
		/**
		 * \brief 边上的附着信息
		 */
		V val=V();

		bond()=default;
		bond(const std::string & s1,const std::string & s2,site<T,V> * s): name(s1),ind(s2),neighbor(s){};
		bond(const std::string & s1,const std::string & s2,site<T,V> * s, const V & E):name(s1),ind(s2),neighbor(s),val(E){};
		bond(const bond<T,V>&)=default;
	};

   /**
    * \brief 格点存储了网络中点内的信息
    *
    * 每个格点中有一个中心元素val, 和边的信息
    * 每个边拥有一个名字和一个bond对象
    * \see network
    * \see bond
    */
	template <typename T,typename V>
	class site{
	template <typename T1,typename V1>
	friend std::ostream & operator<<(std::ostream &,const site<T1,V1> &);
	template <typename T1,typename V1>
	friend std::istream & operator>>(std::istream &, site<T1,V1> &);
	template <typename T1,typename V1>
	friend std::ostream & operator<(std::ostream &,const site<T1,V1> &);
	template <typename T1,typename V1>
	friend std::istream & operator>(std::istream &, site<T1,V1> &);
	public:
		//constructor
		site()=default;
		site(const T& s):val(s){};
		//copy constructor
		site(const site<T,V>&)=default;
		//copy assignment
		site<T,V>& operator=(const site<T,V>&)=default;
		//move constructor
		site(site<T,V>&&)=default;
		//move assignment
		site<T,V>& operator=(site<T,V>&&)=default;

		void clean();

		/**
		 * \brief 格点所附着的信息
		 */
		T val=T();
		/**
		 * \brief 格点所相连的边, 存储了另一测的指针等信息
		 * \see bond
		 */
		std::map<std::string,bond<T,V>> bonds;
		std::vector<int> position;
	};
}


#endif
