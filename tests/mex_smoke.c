#include "mex.h"

int main(void)
{
    mxArray *lhs[1];
    mxArray *rhs1[1];
    rhs1[0] = mxCreateString("Buffer_new");
    if (mexCallMATLAB(1, lhs, 1, rhs1, "ufo_mex"))
        return 1;

    mxArray *rhs2[2];
    rhs2[0] = mxCreateString("Buffer_free");
    rhs2[1] = lhs[0];
    if (mexCallMATLAB(0, NULL, 2, rhs2, "ufo_mex"))
        return 2;

    mxDestroyArray(rhs1[0]);
    mxDestroyArray(rhs2[0]);
    mxDestroyArray(lhs[0]);
    return 0;
}
