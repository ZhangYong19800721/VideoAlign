#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
using namespace std;

vector<unsigned long> ReadGeneFile(const std::string filename) {
	unsigned long eight_bytes;
	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp == 0) {
		std::cerr << "Fail to open gene file" << std::endl;
		exit(1);
	}

	vector<unsigned long> gene;
	while(0!=fread(&eight_bytes,sizeof(unsigned long long),1,fp)) {
		gene.push_back(eight_bytes);
	}
	fclose(fp);
	return gene;
}

