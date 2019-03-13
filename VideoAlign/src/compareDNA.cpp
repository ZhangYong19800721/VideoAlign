#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#include <itpp/itbase.h>

extern unsigned int hamming(const unsigned long A);

int find(const vector<std::pair<unsigned long, unsigned long> >& gene1,
		 const vector<std::pair<unsigned long, unsigned long> >& gene2) {
	const int INVALID = -1;
	if (gene1.empty() || gene2.empty())
		return INVALID;

	vector<long> score_vec;
	for (unsigned int shift=0; shift<gene1.size(); ++shift) {
		long score = 0;
		for (unsigned int i=0; i<gene2.size(); ++i) {
			unsigned int dis1 = (shift+i)<gene1.size()? hamming(gene1[shift + i].first ^ gene2[i].first) : 64;
			if (dis1 <= 2) {
				score += 9;
			}
			else {
				unsigned int dis2 = (shift+i)<gene1.size()? hamming(gene1[shift + i].second ^ gene2[i].second) : 64;
				score += dis2 <= 2 ? 8 : -5;
			}
		}
		score_vec.push_back(score);
	}

	vector<long>::iterator maxpos = std::max_element(score_vec.begin(),score_vec.end());
	unsigned int bestshift = maxpos - score_vec.begin();

	if (*maxpos < (gene2.size() * 2.0 * 8.0 / 3.0 - gene2.size() * 1.0 * 5.0 / 3.0)) {
		return INVALID;
	} else {
		return bestshift;
	}
}
