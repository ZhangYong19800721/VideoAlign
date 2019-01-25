#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <iostream>
#include <itpp/itbase.h>

class Extractor {
private:
    itpp::mat w1,w2,w3;
    itpp::mat b1,b2,b3;
    itpp::mat T;

public:
    itpp::mat encode(const itpp::mat& v0);

public:
    Extractor(const char * matfile);
    Extractor(const Extractor& orig);
    virtual ~Extractor();
};

#endif /* EXTRACTOR_H */

