//============================================================================
// Name        : CompareGene.cpp
// Author      : ZhangYong
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
using namespace std;

extern vector<unsigned long> ReadGeneFile(const std::string filename);
extern unsigned int hamming(const unsigned long A);

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cout << "Under the assumption that the 2nd video is part of the 1st video, \n"
				<< "this program compare two video gene files, in order to find the start\n"
				<< "point and end point in the 1st video.";

		std::cout << "Usage_case: CompareGene long.gene short.gene \n"
				<< "  The program will print the start point and end point on the screen. \n\n";

		return 1;
	}

	static boost::regex regex_gene(".*\\.(gene)");

	boost::filesystem::path filePath1(argv[1]);
	boost::filesystem::path filePath2(argv[2]);

	try {
		if (exists(filePath1)) {
			if (is_regular_file(filePath1)) {
				if (boost::regex_match(filePath1.string(),regex_gene)) {
					// do nothing
				} else {
					std::cout << filePath1 << " is not a gene file! \n";
					return -1;
				}
			} else {
				std::cout << filePath1 << " is not a file! \n";
				return -1;
			}
		} else {
			std::cout << filePath1 << " does not exist! \n";
			return -1;
		}

		if (exists(filePath2)) {
			if (is_regular_file(filePath2)) {
				if (boost::regex_match(filePath2.string(),regex_gene)) {
					// do nothing
				} else {
					std::cout << filePath2 << " is not a gene file! \n";
					return -1;
				}
			} else {
				std::cout << filePath2 << " is not a file! \n";
				return -1;
			}
		} else {
			std::cout << filePath2 << " does not exist! \n";
			return -1;
		}
	} catch (const boost::filesystem::filesystem_error& ex) {
		std::cout << ex.what() << '\n';
	}

	vector<unsigned long> gene1 = ReadGeneFile(filePath1.string());
	vector<unsigned long> gene2 = ReadGeneFile(filePath2.string());
	vector<long> score_vec;
	for(unsigned int shift=0;shift<=(gene1.size()-gene2.size());++shift) {
		long score = 0, best_score = 0;
		for(unsigned int i=0;i<gene2.size();++i){
			unsigned int dis = hamming(gene1[shift+i]^gene2[i]);
			score += dis <= 1? 8:-5;
			best_score = score > best_score? score:best_score;
			if (best_score - score > 100) // if 20 frames dismatch then stop comparison
				break;
		}
		score_vec.push_back(best_score);
	}

	vector<long>::iterator maxpos = std::max_element(score_vec.begin(),score_vec.end());
	unsigned int bestshift = maxpos - score_vec.begin();

	if(*maxpos < (gene2.size() * 2.0 * 8.0 / 3.0 - gene2.size() * 1.0 * 5.0 / 3.0))
	{
		cout << "Problem detected! They two irrelevant videos." << endl;
	} else {
		cout << "Start_Point = " << bestshift << endl;
	}

	return 0;
}
