/*
 * GenePool.h
 *
 *
 */

#ifndef GENEPOOL_H_
#define GENEPOOL_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <vector>
#include <hash_map>
#include <utility>
#include <algorithm>
#include <queue>
#include "SearchHit.h"
#include "CompareHit.h"

using namespace __gnu_cxx;

class GenePool {
public:
	typedef unsigned long VDID;
	typedef unsigned long GENE;
	typedef long          INDX;
	typedef long          SCOR;
	typedef std::vector<GENE> GENESEQ;
	typedef std::vector<INDX> INDXSEQ;
	typedef std::vector<std::pair<INDX,INDX> > INDXPAIRSEQ;
	typedef hash_map<VDID,std::string> VDID2NAME_MAP;
	typedef hash_map<VDID,INDXSEQ> VDID2INDX_MAP;
	typedef hash_map<VDID,GENESEQ> VDID2GENE_MAP;
	typedef hash_map<GENE,hash_map<VDID,INDXSEQ> > GENE2INDX_MAP;

private:
	VDID2NAME_MAP m_VDID2NAME;
	VDID2GENE_MAP m_VDID2GENE;
	GENE2INDX_MAP m_GENE2INDX;

public:
	int load(VDID vdid, const std::string filename);
	long find(const std::string filename, VDID& id_, std::string& match_file_name_, INDXPAIRSEQ &s_);
	bool search(const std::string filename, std::vector<SearchHit>& result_hits);
	long GreedyMatch(const GENESEQ& gene1, const GENESEQ& gene2, INDX x1, INDX x2, INDXPAIRSEQ &s_, std::vector<int>& dis_vec_);

public:
	GenePool();
	virtual ~GenePool();
};

#endif /* GENEPOOL_H_ */
