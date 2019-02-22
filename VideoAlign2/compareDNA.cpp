#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#include <itpp/itbase.h>

extern unsigned int hamming(const unsigned long A);
extern std::ofstream mylog;

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

bool isShortMatch(const vector<unsigned long>& gene1, unsigned int pos1, const vector<unsigned long>& gene2, unsigned int pos2, int fps) {
	for(int i = 0; i < fps; ++i){
		for(int j=0; j < fps; ++j){
			unsigned int dis = hamming(gene1[pos1+i] ^ gene2[pos2+j]);
			if(dis <= 2) {
				return true;
			}
		}
	}
	return false;
}

std::vector<std::pair<int, int> > local_alignment(const vector<unsigned long>& gene1, const vector<unsigned long>& gene2, int fps) {
	unsigned int size1 = gene1.size(); // the frame count for video1
	unsigned int size2 = gene2.size(); // the frame count for video2
	unsigned int seconds1 = size1 / fps;  // the time duration (seconds) for video1
	unsigned int seconds2 = size2 / fps;  // the time duration (seconds) for video2
	static const int gap = -3;
	static const int match = 8;
	static const int dismatch = -5;

	itpp::imat scoreMat = itpp::zeros_i(seconds1+1,seconds2+1);
	itpp::imat routeMat = itpp::zeros_i(seconds1+1,seconds2+1);
	for(unsigned int i = 1; i <= seconds1; ++i) {
		for(unsigned int j = 1; j <= seconds2; ++j) {
			vector<int> score;
			score.push_back(scoreMat(i-1,j) + gap);
			score.push_back(scoreMat(i,j-1) + gap);
			int delta = isShortMatch(gene1,(i-1)*fps,gene2,(j-1)*fps,fps)? match : dismatch;
			score.push_back(scoreMat(i-1,j-1) + delta);
			score.push_back(0);
			vector<int>::iterator maxpos = std::max_element(score.begin(),score.end());
			scoreMat(i,j) = *maxpos;
			routeMat(i,j) = maxpos - score.begin() + 1;
		}
	}

	int max_row = -1, max_col = -1;
	itpp::max_index(scoreMat,max_row,max_col);
	int row = max_row, col = max_col;
	vector<std::pair<int,int> > result;
	bool flag = true;
	while(flag) {
		switch(routeMat(row,col)) {
		case 0:
		case 4:
			flag = false; // stop while
			break;
		case 1:
			result.push_back(std::make_pair(row--,(col,-1)));
			break;
		case 2:
			result.push_back(std::make_pair((row,-1),col--));
			break;
		case 3:
			result.push_back(std::make_pair(row--,col--));
			break;
		default:
			cout << "Error in local_alignment." << endl;
			exit(1);
		}
	}

	return result;
}

std::vector<std::pair<std::pair<int, int>, bool> > global_alignment(const vector<unsigned long>& gene1, const vector<unsigned long>& gene2, int fps) {
	unsigned int size1 = gene1.size(); // the frame count for video1
	unsigned int size2 = gene2.size(); // the frame count for video2
	unsigned int seconds1 = size1 / fps;  // the time duration (seconds) for video1
	unsigned int seconds2 = size2 / fps;  // the time duration (seconds) for video2
	static const int gap = -3;
	static const int match = 8;
	static const int dismatch = -5;
	vector<int> score;
	vector<int>::iterator maxpos;

	itpp::imat scoreMat = itpp::zeros_i(seconds1+1,seconds2+1); // score matrix
	itpp::imat routeMat = itpp::zeros_i(seconds1+1,seconds2+1); // route matrix
	itpp::bmat matchMat = itpp::zeros_b(seconds1+1,seconds2+1); // match matrix

	for(unsigned int i = 1; i < seconds1+1; ++i) // init the first col of score matrix
		scoreMat(i,0) = scoreMat(i-1,0) + gap;

	for(unsigned int j = 1; j < seconds2+1; ++j) // init the first row of score matrix
		scoreMat(0,j) = scoreMat(0,j-1) + gap;

	for(unsigned int i = 1; i <= seconds1; ++i) {
		for(unsigned int j = 1; j <= seconds2; ++j) {
			bool is_match = isShortMatch(gene1,(i-1)*fps,gene2,(j-1)*fps,fps);
			matchMat(i,j) = is_match;
			score.clear(); // reset the score vector
			int delta = is_match? match : dismatch;
			score.push_back(scoreMat(i-1,j-1) + delta);
			score.push_back(scoreMat(i-1,j) + gap);
			score.push_back(scoreMat(i,j-1) + gap);
			maxpos = std::max_element(score.begin(),score.end());
			scoreMat(i,j) = *maxpos;
			routeMat(i,j) = maxpos - score.begin() + 1;
		}
	}

	int row = seconds1, col = seconds2;
	int max_row = -1, max_col = -1;
	itpp::max_index(scoreMat,max_row,max_col);
	std::vector<std::pair<std::pair<int, int>, bool> > result;

	while(row > max_row || col > max_col) {
		switch(routeMat(row,col)) {
		case 1:
			result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
			if (row > max_row)
				row--;
			if (col > max_row)
				col--;
			break;
		case 2:
			result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
			if (row > max_row)
				row--;
			else
				col--;
			break;
		case 3:
			result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
			if (col > max_row)
				col--;
			else
				row--;
			break;
		default:
			cout << "Error in local_alignment." << endl;
			exit(1);
		}
	}

	while(row > 0 || col > 0) {
		switch(routeMat(row,col)) {
		case 0:
			if (row > 0) {
				result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
				row--;
			} else if (col > 0) {
				result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
				col--;
			}
			break;
		case 1:
			result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
			row--;
			col--;
			break;
		case 2:
			result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
			row--;
			break;
		case 3:
			result.push_back(std::make_pair(std::make_pair(row,col),matchMat(row,col)));
			col--;
			break;
		default:
			cout << "Error in local_alignment." << endl;
			exit(1);
		}
	}

	return result;
}
