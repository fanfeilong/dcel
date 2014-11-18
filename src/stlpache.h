#ifndef DCEL_STLPACHE_H
#define DCEL_STLPACHE_H

#include "stlpache.container.h"
#include "stlpache.string.h"
#include "stlpache.file.h"

#include <iostream>
#include <string>

namespace stlpache{
	const double PI  = 3.1415926535897932;
	const double EPS = 2.7182818;
	inline void debug(std::string str){
		std::cout<<"dcel::debug=>"<<str<<std::endl;
	}
};



#endif