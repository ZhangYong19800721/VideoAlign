#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

extern unsigned int hamming(const unsigned long A);

int find(const vector<unsigned long>& gene1, const vector<unsigned long>& gene2) {
	const int INVALID = -1;
	if (gene1.empty() || gene2.empty())
		return INVALID;

	vector<long> score_vec;
	for (unsigned int shift = 0; shift <= (gene1.size() - gene2.size()); ++shift) {
		long score = 0, best_score = 0;
		for (unsigned int i = 0; i < gene2.size(); ++i) {
			unsigned int dis = hamming(gene1[shift + i] ^ gene2[i]);
			score += dis <= 2 ? 8 : -5;
			best_score = score > best_score ? score : best_score;
			//if (best_score - score > 100) // if 20 frames dismatch then stop comparison
			//	break;
		}
		score_vec.push_back(best_score);
	}

	vector<long>::iterator maxpos = std::max_element(score_vec.begin(),score_vec.end());
	unsigned int bestshift = maxpos - score_vec.begin();

	if (*maxpos < (gene2.size() * 2.0 * 8.0 / 3.0 - gene2.size() * 1.0 * 5.0 / 3.0)) {
		return INVALID;
	} else {
		return bestshift;
	}
}
