/*
 * CompareHit.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: root
 */

#include "CompareHit.h"

bool CompareHit::operator ()(const SearchHit& a, const SearchHit& b) {
	return a.m_Score > b.m_Score;
}
