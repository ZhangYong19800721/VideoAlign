#include "Extractor.h"
#include <iomanip>

extern itpp::mat sigmoid(const itpp::mat& x);
extern double* readmat(const char *file, const char *name, size_t &rows, size_t &cols);

itpp::mat Extractor::encode(const itpp::mat& v0) {
	itpp::mat v1 = this->w1 * v0 + this->b1;
    v1 = sigmoid(v1);
    itpp::mat v2 = this->w2 * v1 + this->b2;
    v2 = sigmoid(v2);
    itpp::mat v3 = this->w3 * v2 + this->b3;
    v3 = sigmoid(v3);
    itpp::mat code = v3 - this->T;
    return code;
}

Extractor::Extractor(const char * matfile) {
    double * data;
    size_t rows;
    size_t cols;

    data = readmat(matfile, "w1", rows, cols);
    this->w1 = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->w1(i, j) = data[i + j * rows];
        }
    }

    data = readmat(matfile, "w2", rows, cols);
    this->w2 = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->w2(i, j) = data[i + j * rows];
        }
    }

    data = readmat(matfile, "w3", rows, cols);
    this->w3 = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->w3(i, j) = data[i + j * rows];
        }
    }

    data = readmat(matfile, "b1", rows, cols);
    this->b1 = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->b1(i, j) = data[i + j * rows];
        }
    }

    data = readmat(matfile, "b2", rows, cols);
    this->b2 = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->b2(i, j) = data[i + j * rows];
        }
    }

    data = readmat(matfile, "b3", rows, cols);
    this->b3 = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->b3(i, j) = data[i + j * rows];
        }
    }

    data = readmat(matfile, "T", rows, cols);
    this->T = itpp::mat(rows, cols);
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            this->T(i, j) = data[i + j * rows];
        }
    }
}

Extractor::Extractor(const Extractor& orig) {
}

Extractor::~Extractor() {
}

