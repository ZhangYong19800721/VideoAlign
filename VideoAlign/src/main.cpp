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

extern std::vector<uint64_t> VideoDNA(const char * input_filename,
		char * weight, double dDNAPS);
extern int find(const std::vector<uint64_t>& gene1,
		const std::vector<uint64_t>& gene2);

int main(int argc, char* argv[]) {
	/*
	 "{\"base\":{\"entityId\":10000003,\"basePath\":\"/home/zy/workspace/VideoAlign/movie.mp4\"},\"seqs\":[{\"guid\":\"split001\",\"seqPath\":\"/home/zy/workspace/VideoAlign/split001.mp4\"},{\"guid\":\"split002\",\"seqPath\":\"/home/zy/workspace/VideoAlign/split002.mp4\"},{\"guid\":\"split003\",\"seqPath\":\"/home/zy/workspace/VideoAlign/split003.mp4\"}]}"
	 */
	time_t now = time(0);
	char current_time[100];
	strftime(current_time, sizeof(current_time), "%Y%m%d-%H%M%S",
			localtime(&now)); // get the current system time, in YMD-HMS format.

	char * cmd = argv[0];
	const char * json = argv[1];
	char path[1000];
	strcpy(path, cmd);
	char * idx = strrchr(path, '/'); // find the last one of /
	*idx = '\0'; // now the cstring path is ok
	char log[1000];
	strcpy(log, path);
	strcat(strcat(strcat(log, "/VideoAlignLog-"), current_time), ".txt");
	mylog.open(log, std::ios::out);

	mylog << "argv[0] = " << argv[0] << std::endl;
	mylog << "argv[1] = " << argv[1] << std::endl;

	char weight[1000];
	strcpy(weight, path);
	strcat(weight, "/weight.mat");
	mylog << "weight_file_path = " << weight << std::endl;

	const double FPS = 25;

	Document doc_in;    // the document for input json
	doc_in.Parse(json); // parse the json string

	const Value& seqs_in = doc_in["seqs"];

	Document doc_out;     // the document object for output
	doc_out.SetObject();  // set the document as an object
	Value baseEntityId;   // declare a value
	int entityId = doc_in["base"]["entityId"].GetInt();
	baseEntityId.SetInt(entityId); // the output baseEntityId should be the same as input entityId
	doc_out.AddMember("baseEntityId", baseEntityId, doc_out.GetAllocator()); // add the baseEntityId to document

	Value seqs_out(kArrayType);
	const char * long_video_name = doc_in["base"]["basePath"].GetString();
	std::vector<unsigned long> longDNA = VideoDNA(long_video_name, weight,
			1.0 / FPS);

	if (longDNA.empty()) {
		std::cout << "Error: Problem encountered when extract DNA from "
				<< long_video_name << std::endl;
		exit(EXTRACT_DNA_ERROR);
	}

	std::vector<std::vector<unsigned long> > shortDNA;
	for (SizeType i = 0; i < seqs_in.Size(); i++) {
		const char * short_video_name = seqs_in[i]["seqPath"].GetString();
		std::vector<unsigned long> shortDNA_i = VideoDNA(short_video_name,
				weight, 1.0 / FPS);
		if (shortDNA_i.empty()) {
			std::cout << "Error: Problem encountered when extract DNA from "
					<< long_video_name << std::endl;
			mylog << "Error: Problem encountered when extract DNA from "
					<< long_video_name << std::endl;
			exit(EXTRACT_DNA_ERROR);
		}
		if (shortDNA_i.size() > longDNA.size()) {
			std::cout << "Error: long_DNA should be longer than short_DNA. "
					<< "Long video " << long_video_name << "has "
					<< longDNA.size() << "frames. " << "Short video "
					<< short_video_name << "has " << shortDNA_i.size()
					<< std::endl;
			mylog << "Error: long_DNA should be longer than short_DNA. "
					<< "Long video " << long_video_name << "has "
					<< longDNA.size() << "frames. " << "Short video "
					<< short_video_name << "has " << shortDNA_i.size()
					<< std::endl;
			exit(LONG_SHORT_DNA_ERROR);
		}
		shortDNA.push_back(shortDNA_i);
	}

	for (SizeType i = 0; i < seqs_in.Size(); i++) { // use SizeType
		Value item(kObjectType);
		const char * cstr_guid = seqs_in[i]["guid"].GetString();
		Value guid;
		guid.SetString(cstr_guid, strlen(cstr_guid));
		unsigned int startFrame = find(longDNA, shortDNA[i]);
		unsigned int frameCount = shortDNA[i].size();
		long inPoint = long(startFrame * (1e7 / FPS));
		long outPoint = long(inPoint + frameCount * (1e7 / FPS));
		Value seqInPoint(inPoint);
		Value seqOutPoint(outPoint);
		item.AddMember("guid", guid, doc_out.GetAllocator());
		item.AddMember("seqInPoint", seqInPoint, doc_out.GetAllocator());
		item.AddMember("seqOutPoint", seqOutPoint, doc_out.GetAllocator());
		seqs_out.PushBack(item, doc_out.GetAllocator());
	}
	doc_out.AddMember("seqs", seqs_out, doc_out.GetAllocator()); // add the baseEntityID to document

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc_out.Accept(writer);  // dump the document to json string

	std::cout << buffer.GetString() << std::endl;
	mylog.close();
	return 0;
}
