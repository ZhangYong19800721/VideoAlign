/*
 * SearchHit.h
 *
 *  Created on: Nov 19, 2017
 *      Author: root
 */

#ifndef SEARCHHIT_H_
#define SEARCHHIT_H_

#include <string>
#include <vector>
#include <utility>

class SearchHit {
public:
	unsigned long m_FileID; // the id of the hit file.
	std::string m_FileName; // the name of the hit file.
	long m_Score; // the score of the hit file
	std::vector<std::pair<long,long> > m_IndxPair; // greedy alignment of the hit file.
};

#endif /* SEARCHHIT_H_ */
