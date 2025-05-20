/*  ufo_buffer.c   –  MEX glue for UFO::Buffer
 *
 *  Public MEX commands registered in ufo_dispatcher.c:
 *      Buffer_new      (bytes)             → returns uint64 handle
 *      Buffer_free     (handle)
 *      Buffer_getSize  (handle)            → double scalar
 *      Buffer_getData  (handle)            → uint8 row-vector
 *
 *  All handle bookkeeping is centralised in mexUfo_handle.* .
 */

#include "ufo_mex_api.h"
#include "mexUfo_handle.h"
#include "ufo.h"
#include <mex.h>
#include <glib.h>
#include <string.h>
#include <inttypes.h>   /* PRIu64 */

/* ------------------------------------------------------------------ */
/* Helper to convert RHS[1] to size_t bytes                            */
/* ------------------------------------------------------------------ */
static size_t parse_nbytes (const mxArray *arg)
{
    if (mxIsUint64(arg) && mxGetNumberOfElements(arg) == 1) {
        return *(const uint64_t *) mxGetData(arg);
    }
    if (mxIsDouble(arg) && mxGetNumberOfElements(arg) == 1) {
        double v = mxGetScalar(arg);
        if (!mxIsFinite(v) || v < 0.0 || floor(v) != v)
            mexErrMsgIdAndTxt ("ufo_mex:BadArg",
                               "nBytes must be a finite, non-negative integer.");

        return (size_t) v;
    }
    mexErrMsgIdAndTxt ("ufo_mex:BadArg",
                       "nBytes must be a uint64 or double scalar.");
    return 0; /* never reached, but keeps compiler happy */
}

/* ------------------------------------------------------------------ */
/* MEX:  buf = Buffer.new(nBytes)                                      */
/* ------------------------------------------------------------------ */
void Buffer_new_mex (int nlhs, mxArray *plhs[],
                     int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt ("ufo_mex:BadArg",
                           "Usage: buf = Buffer.new(nBytes)");

    const size_t nbytes = parse_nbytes (prhs[1]);

    GError *err = NULL;
    UfoBuffer *buf = ufo_buffer_new (nbytes, &err);

    if (!buf) {
        /* copy message before freeing */
        const char *msg = (err && err->message) ? err->message
                                                : "ufo_buffer_new failed";
        if (err) g_error_free (err);
        mexErrMsgIdAndTxt ("ufo_mex:BufferNew", "%s", msg);
    }

    /* Register MATLAB handle (adds +1 ref) */
    mxArray *handleArr = ufoHandle_create (UFO_BUFFER (buf), "buffer");

    /* Balance the extra ref we just added */
    g_object_unref (buf);

    plhs[0] = handleArr;
}

/* ------------------------------------------------------------------ */
/* MEX:  Buffer.free(buf)                                              */
/* ------------------------------------------------------------------ */
void Buffer_free_mex (int nlhs, mxArray *plhs[],
                      int nrhs, const mxArray *prhs[])
{
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt ("ufo_mex:BadArg",
                           "Usage: Buffer.free(buf)");

    /* Simply remove from registry – entry_free() will unref exactly once */
    ufoHandle_remove (prhs[1]);
}

/* ------------------------------------------------------------------ */
/* MEX:  n = Buffer.getSize(buf)                                       */
/* ------------------------------------------------------------------ */
void Buffer_getSize_mex (int nlhs, mxArray *plhs[],
                         int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt ("ufo_mex:BadArg",
                           "Usage: n = Buffer.getSize(buf)");

    UfoBuffer *buf = ufoHandle_getBuffer (prhs[1]);

    const size_t n = ufo_buffer_get_size (buf);
    plhs[0] = mxCreateDoubleScalar ((double) n);
}

/* ------------------------------------------------------------------ */
/* MEX:  data = Buffer.getData(buf)                                    */
/*        – returns uint8 row vector                                   */
/*        – copies from host memory, triggers transfer if needed       */
/* ------------------------------------------------------------------ */
void Buffer_getData_mex (int nlhs, mxArray *plhs[],
                         int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt ("ufo_mex:BadArg",
                           "Usage: data = Buffer.getData(buf)");

    UfoBuffer *buf = ufoHandle_getBuffer (prhs[1]);

    /* Ensure host pointer is valid; UFO guarantees download when needed */
    void   *host_ptr = ufo_buffer_get_host_ptr (buf);
    size_t  nbytes   = ufo_buffer_get_size     (buf);

    if (!host_ptr)
        mexErrMsgIdAndTxt ("ufo_mex:Buffer",
                           "ufo_buffer_get_host_ptr returned NULL.");

    /* MATLAB can’t hold >2^48 elements; check */
    const mwSize nelem = (mwSize) nbytes;
    if ( (size_t) nelem != nbytes )
        mexErrMsgIdAndTxt ("ufo_mex:TooBig",
                           "Buffer is larger than MATLAB can copy at once.");

    mxArray *arr = mxCreateNumericMatrix (1, nelem,
                                          mxUINT8_CLASS, mxREAL);
    if (!arr)
        mexErrMsgIdAndTxt ("ufo_mex:OOM",
                           "Failed to allocate MATLAB array of %" PRIu64 " bytes.",
                           (uint64_t)nbytes);

    memcpy (mxGetData(arr), host_ptr, nbytes);
    plhs[0] = arr;
}
