#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>
#include <ctime>
using namespace rapidjson;

std::ofstream mylog; // a global variable for log

enum ERRORCODE {
	EXTRACT_DNA_ERROR = 991, LONG_SHORT_DNA_ERROR, OTHER_ERROR
};

extern std::vector<uint64_t> VideoDNA(const char * input_filename,char * weight, double dDNAPS);
extern int find(const std::vector<uint64_t>& gene1,const std::vector<uint64_t>& gene2);
extern std::vector<std::pair<int, int> > local_alignment(const std::vector<unsigned long>& gene1, const std::vector<unsigned long>& gene2, int fps);
extern std::vector<std::pair<std::pair<int, int>, bool> > global_alignment(const std::vector<unsigned long>& gene1, const std::vector<unsigned long>& gene2, int fps);

int main(int argc, char* argv[]) {
	time_t now = time(0);
	char current_time[100];
	strftime(current_time, sizeof(current_time), "%Y%m%d-%H%M%S",
			localtime(&now)); // get the current system time, in YMD-HMS format.

	char * cmd = argv[0];
	char path[1000];
	strcpy(path, cmd);
	char * idx = strrchr(path, '/'); // find the last one of /
	*idx = '\0'; // now the cstring path is ok
	char log[1000];
	strcpy(log, path);
	strcat(strcat(strcat(log, "/VideoAlign2Log-"), current_time), ".txt");
	mylog.open(log, std::ios::out);

	const char * video1_filename = argv[1];
	const char * video2_filename = argv[2];

	mylog << "command = " << cmd << std::endl;
	mylog << "video1 = " << video1_filename << std::endl;
	mylog << "video2 = " << video2_filename << std::endl;

	char weight[1000];
	strcpy(weight, path);
	strcat(weight, "/weight.mat");
	mylog << "weight = " << weight << std::endl;

	const double FPS = 25;

	std::vector<unsigned long> DNA1 = VideoDNA(video1_filename, weight, 1.0 / FPS);
	std::vector<unsigned long> DNA2 = VideoDNA(video2_filename, weight, 1.0 / FPS);

	if (DNA1.empty()) {
		std::cout << "Error: Problem encountered when extract DNA from "
				<< video1_filename << std::endl;
		exit(EXTRACT_DNA_ERROR);
	}

	if (DNA2.empty()) {
		std::cout << "Error: Problem encountered when extract DNA from "
				<< video2_filename << std::endl;
		exit(EXTRACT_DNA_ERROR);
	}

	std::vector<std::pair<std::pair<int,int>,bool> > result = global_alignment(DNA1,DNA2,FPS/5);
	for (int i=result.size()-1;i>=0;--i) {
		std::cout << result[i].first.first << ", " << result[i].first.second << ", " << result[i].second << std::endl;
	}

	mylog.close();
	return 0;
}
