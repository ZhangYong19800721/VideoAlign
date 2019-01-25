#include <iomanip>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

extern int VideoDNA(char * input_filename, char * output_filename, double dDNAPS);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage_case_1: VideoGene input.mp4[ts|mkv] [ouput.gene] \n"
				<< "  extract the gene from a video file, the video DNA data is written to output.gene."
				<< "If output.gene is omitted, the gene file will share the same name as input file "
				<< "except the .gene extension. \n\n"

				<< "Usage_case_2: VideoGene input.mp4[ts|mkv] [ouput_path] \n"
				<< "  extract the gene from a video file, the video DNA data is written to output_path "
				<< "with the same file name as input video except for the .gene extension. \n\n"

				<< "Usage_case_3: VideoGene input_path [output_path] \n"
				<< "  extract genes from all the video files in input_path (not include the video contained "
				<< "in sub-path), the output gene files share the same filename as the input files."
				<< "output files are written to the output_path. If the output_path is omitted, the gene "
				<< "files will be written to the input_path as default.\n";
		return 1;
	}

	static boost::regex regex1(".*\\.(mp4|MP4|ts|TS|mkv|MKV|m2ts|M2TS)");
	static boost::regex regex2(".*\\.(gene)");

	boost::filesystem::path i_path(argv[1]);
	boost::filesystem::path o_path;
	double rate = 1.0;
	if (argc >= 3) {
		o_path = boost::filesystem::path(argv[2]);
	}

	if (argc >= 4) {
		sscanf(argv[3], "%lf", &rate);
	}

	std::vector<boost::filesystem::path> input_path_list;

	try {
		if (exists(i_path)) {
			if (is_regular_file(i_path)) {
				if (boost::regex_match(i_path.string(),regex1)) {
					input_path_list.push_back(i_path);
				}
			} else if (is_directory(i_path)) {
				boost::filesystem::directory_iterator dit(i_path), eit;
		        while (dit != eit) {
		        	boost::filesystem::path sub_path = dit->path();
					if (boost::regex_match(sub_path.string(),regex1)) {
						input_path_list.push_back(sub_path);
					}
					++dit;
		        }
			} else {
				std::cout << i_path << " exists, but not a file or path \n";
				return -1;
			}
		} else {
			std::cout << i_path << " does not exist! \n";
			return -1;
		}
	} catch (const boost::filesystem::filesystem_error& ex) {
		std::cout << ex.what() << '\n';
	}

	std::vector<boost::filesystem::path> output_path_list;
	if(o_path.empty()) {
		for (unsigned int i=0;i<input_path_list.size();++i) {
			std::string ifilename = input_path_list[i].string();
			std::string ofilename = ifilename + ".gene";
			output_path_list.push_back(boost::filesystem::path(ofilename));
		}
	} else if (boost::regex_match(o_path.string(),regex2)) {
		if (exists(o_path.parent_path())) {

		} else {
			std::cout << o_path.parent_path() << " does not exist! \n";
			return -1;
		}

		if (input_path_list.size()==1) {
			output_path_list.push_back(o_path);
		} else {
			std::cout << "multiple input video file but only one gene output! \n";
			return -1;
		}
	} else {
		if (exists(o_path)) {
			for (unsigned int i=0;i<input_path_list.size();++i) {
				boost::filesystem::path in_path = input_path_list[i];
				std::string ofilename = o_path.string() + "/" + in_path.filename().string() + ".gene";
				output_path_list.push_back(boost::filesystem::path(ofilename));
			}
		} else {
			std::cout << o_path << " does not exist! \n";
			return -1;
		}
	}

	for (unsigned int i=0;i<input_path_list.size();++i) {
		char * input_filename = (char*)input_path_list[i].c_str();
		char * output_filename = (char*)output_path_list[i].c_str();
		VideoDNA(input_filename, output_filename, rate);
	}

	return 0;
}

