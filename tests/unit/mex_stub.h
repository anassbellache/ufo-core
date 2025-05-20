#include <stdlib.h>

typedef struct mxArray {
    const char *str; /* minimal stub, only for strings */
} mxArray;

static inline int mxIsChar(const mxArray *arr)
{
    return arr && arr->str != NULL;
}

static inline char *mxArrayToString(const mxArray *arr)
{
    return arr && arr->str ? strdup(arr->str) : NULL;
}

static inline void mxFree(void *p)
{
    free(p);
}

extern int mex_error_code;
void mexErrMsgIdAndTxt(const char *id, const char *fmt, ...);
