#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <itpp/itbase.h>
#include "GenePool.h"
using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage_case_1: VideoGenePool /path_to_gene_pool \n"
				<< "  path_to_gene_pool is the directory that contains multiple *.gene files. "
				<< "The program will use these gene files to establish the gene pool. \n\n ";
		return 1;
	}

	static boost::regex regex_gene(".*\\.(gene)");

	boost::filesystem::path gene_pool_path(argv[1]);

	std::vector<boost::filesystem::path> gene_file_list;

	try {
		if (exists(gene_pool_path)) {
			if (is_regular_file(gene_pool_path)) {
				std::cout << gene_pool_path
						<< " exists, but is not a directory! \n";
				return -1;
			} else if (is_directory(gene_pool_path)) {
				boost::filesystem::directory_iterator dit(gene_pool_path), eit;
				while (dit != eit) {
					boost::filesystem::path sub_path = dit->path();
					if (boost::regex_match(sub_path.string(), regex_gene)) {
						gene_file_list.push_back(sub_path);
					}
					++dit;
				}
			} else {
				std::cout << gene_pool_path
						<< " exists, but not a directory! \n";
				return -1;
			}
		} else {
			std::cout << gene_pool_path << " does not exist! \n";
			return -1;
		}
	} catch (const boost::filesystem::filesystem_error& ex) {
		std::cout << ex.what() << '\n';
	}

	GenePool pool = GenePool();
	for (GenePool::VDID id = 0; id < gene_file_list.size(); ++id) {
		char * gene_filename = (char*) gene_file_list[id].c_str();
		pool.load(id, gene_filename);
	}

	std::cout << "The video gene pool has been established! \n"
			<< "type: quit to shutdown the gene pool. \n"
			<< "type: search xxx.gene to search in gene pool \n";
	std::string command, query_file_name;

	while (true) {
		std::cin >> command;
		if (command.compare("quit") == 0) {
			std::cout << "The video gene pool shutdown! \n";
			return 0;
		}

		if (command.compare("search") == 0) {
			std::cin >> query_file_name;
			std::cout << "search " << query_file_name << "in gene pool ... \n";
			std::vector<SearchHit> search_result;

			itpp::Real_Timer my_timer;
			my_timer.tic();
			pool.search(query_file_name, search_result);
			my_timer.toc_print();

			SearchHit hit = search_result[0];
			cout << "name = " << hit.m_FileName << endl;
			cout << "score = " << hit.m_Score << endl;
			for (int i = 0; i < hit.m_IndxPair.size(); ++i) {
				cout << hit.m_IndxPair[i].first << "," << hit.m_IndxPair[i].second << endl;
			}

			std::cout << "please type quit or search xxx.gene \n";
		}

	}

	return 0;
}
