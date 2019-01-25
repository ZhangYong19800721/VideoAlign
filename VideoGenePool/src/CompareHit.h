/*
 * CompareHit.h
 *
 *  Created on: Nov 20, 2017
 *      Author: root
 */

#ifndef COMPAREHIT_H_
#define COMPAREHIT_H_

#include "SearchHit.h"

class CompareHit {
public:
	bool operator () (const SearchHit& a, const SearchHit& b);
};

#endif /* COMPAREHIT_H_ */
