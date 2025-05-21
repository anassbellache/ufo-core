#pragma once
#ifndef UFO_MEX_API_H
#define UFO_MEX_API_H

#include <mex.h>
#include <stdint.h>
#include "mexUfo_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Opaque MATLAB handle type
/// Internally these will be mapped to GObject pointers via a map in mexUfo_handle.c.
typedef int64_t UFO_Handle;

// -----------------------------------------------------------------------------
// PluginManager API
// -----------------------------------------------------------------------------

/// UFO_pm_new()
///  [h] = ufo_mex('pm_new');
void UFO_pm_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_pm_delete()
///  ufo_mex('pm_delete', h);
void UFO_pm_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_pm_getTask()
///  t = ufo_mex('pm_getTask', h, taskName);
void UFO_pm_getTask(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

// -----------------------------------------------------------------------------
// TaskGraph API
// -----------------------------------------------------------------------------

/// UFO_tg_new()
///  [tg] = ufo_mex('tg_new');
void UFO_tg_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_tg_delete()
///  ufo_mex('tg_delete', tg);
void UFO_tg_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_tg_connect()
///  ufo_mex('tg_connect', tg, srcNode, dstNode);
void UFO_tg_connect(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

// -----------------------------------------------------------------------------
// Scheduler API
// -----------------------------------------------------------------------------

/// UFO_sched_new()
///  [sched] = ufo_mex('sched_new');
void UFO_sched_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_sched_delete()
///  ufo_mex('sched_delete', sched);
void UFO_sched_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_sched_setResources()
///  ufo_mex('sched_setResources', sched, resourcesHandle);
void UFO_sched_setResources(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_sched_run()
///  ufo_mex('sched_run', sched, taskGraph);
void UFO_sched_run(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_sched_poll()
///  [reports] = ufo_mex('sched_poll', sched);
void UFO_sched_poll(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

// -----------------------------------------------------------------------------
// Buffer API
// -----------------------------------------------------------------------------

/// UFO_buf_new()
///  [buf] = ufo_mex('buf_new', sizeInBytes);
void UFO_buf_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_buf_delete()
///  ufo_mex('buf_delete', buf);
void UFO_buf_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_buf_getData()
///  [ptr] = ufo_mex('buf_getData', buf);
void UFO_buf_getData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

/// UFO_buf_getSize()
///  [nBytes] = ufo_mex('buf_getSize', buf);
void UFO_buf_getSize(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

// -----------------------------------------------------------------------------
// MEX Gateway
// -----------------------------------------------------------------------------

/// mexFunction — the single entry point.  You’ll dispatch on prhs[0] (a string)
/// and forward to one of the functions above.
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

#ifdef __cplusplus
}
#endif

#endif // UFO_MEX_API_H
