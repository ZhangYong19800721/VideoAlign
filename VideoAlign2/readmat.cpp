#include <iostream>
#include "mat.h"
// read the name variable in a mat file
//
double* readmat(const char *file, const char *name, size_t &rows, size_t &cols) {
	MATFile *pmat;
	mxArray *pa;
	// Open mat file
	pmat = matOpen(file, "r");
	if (pmat == NULL) {
		printf("can not open file: %s\n", file);
		return 0;
	}
	pa = matGetVariable(pmat, name);
	rows = mxGetM(pa);  // number of rows
	cols = mxGetN(pa);  // number of cols
	double* data = (double*) mxGetData(pa);
	if (matClose(pmat) != 0) {
		printf("can not close file：%s\n", file);
		return 0;
	}
	return data;
}
