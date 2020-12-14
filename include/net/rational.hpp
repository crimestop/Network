#ifndef NET_RATIONAL_HPP
#define NET_RATIONAL_HPP
#include <numeric>
#include <ostream>

namespace net{

	struct rational{
		int numerator=0;
		int denominator=1;

		rational()=default;
		rational(const int & num);
		rational(const int & num,const int & denom);
		rational(const rational&)=default;
		~rational()=default;

		operator double() const;
		operator int() const;
	};

	rational::rational(const int & num){
		numerator=num;
		denominator=1;
	}

	rational::rational(const int & num,const int & denom){
		int gcd_val;
		if(num==0 && denom==0){
			numerator=0;
			denominator=0;
		}else if(num==0 && denom>0){
			numerator=0;
			denominator=1;
		}else if(num==0 && denom<0){
			numerator=0;
			denominator=-1;
		}else if(num>0 && denom==0){
			numerator=1;
			denominator=0;
		}else if(num<0 && denom==0){
			numerator=-1;
			denominator=0;
		}else{
			gcd_val=std::gcd(num,denom);
			if(denominator<0){
				numerator= -num/gcd_val;
				denominator= -denom/gcd_val;
			}else{
				numerator=num/gcd_val;
				denominator=denom/gcd_val;			
			}
		}
	}

	rational::operator double() const{
		return double(numerator)/double(denominator);
	}
	rational::operator int() const{
		return numerator/denominator;
	}

	bool operator<(const rational & a,const rational & b){
		return a.numerator*b.denominator<a.denominator*b.numerator;
	}

	bool operator>(const rational & a,const rational & b){
		return a.numerator*b.denominator>a.denominator*b.numerator;
	}

	std::ostream & operator<<(std::ostream & os,const rational & a){
		return os<<a.numerator<<'/'<<a.denominator;
	}

	rational operator+(const rational & a,const rational & b){
		return rational(a.numerator*b.denominator+a.denominator*b.numerator,a.denominator*b.denominator);
	}

	rational operator-(const rational & a,const rational & b){
		return rational(a.numerator*b.denominator-a.denominator*b.numerator,a.denominator*b.denominator);
	}

	rational operator-(const rational & a){
		return rational(-a.numerator,a.denominator);
	}

	rational operator*(const rational & a,const rational & b){
		return rational(a.numerator*b.numerator,a.denominator*b.denominator);
	}

	rational operator/(const rational & a,const rational & b){
		return rational(a.numerator*b.denominator,a.denominator*b.numerator);
	}

	rational & operator+=(rational & a,const rational & b){
		a = a+b;
		return a;
	}

	rational & operator-=(rational & a,const rational & b){
		a = a-b;
		return a;
	}

	rational & operator*=(rational & a,const rational & b){
		a = a*b;
		return a;
	}

	rational & operator/=(rational & a,const rational & b){
		a = a/b;
		return a;
	}

}
#endif