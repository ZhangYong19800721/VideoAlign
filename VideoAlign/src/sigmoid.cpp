#include <itpp/itbase.h>
using namespace itpp;

mat sigmoid(const mat& x) {
    mat E = mat(x.rows(),x.cols()); E.ones();
    mat y = elem_div(E,(1 + exp(-x)));
    return y;
}

