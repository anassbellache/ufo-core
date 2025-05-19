#include "ufo_mex_api.h"
#include <glib.h>
#include <mex.h>

// --------------- PluginManager Commands ---------------

void UFO_pm_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_new: Usage: handle = UFO_pm_new()");
    GError *err = NULL;
    UfoPluginManager *pm = ufo_plugin_manager_new(&err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:PluginManagerNew", "%s", err->message);
    plhs[0] = createUfoHandle((UFO_Handle)pm, "PluginManager");
    if (err) g_error_free(err);
}

void UFO_pm_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_delete: Usage: UFO_pm_delete(pmHandle)");
    UfoPluginManager *pm = getUfoHandle_PluginManager(prhs[1]);
    g_object_unref(pm);
    removeHandle(prhs[1]);
}

void UFO_pm_getTask(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_getTask: Usage: task = UFO_pm_getTask(pmHandle, taskName)");
    UfoPluginManager *pm = getUfoHandle_PluginManager(prhs[1]);
    if (!mxIsChar(prhs[2]))
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "Task name must be a string");
    char *name = mxArrayToString(prhs[2]);
    GError *err = NULL;
    UfoTask *task = ufo_plugin_manager_get_task(pm, name, &err);
    mxFree(name);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:PluginManagerGetTask", "%s", err->message);
    plhs[0] = createUfoHandle((UFO_Handle)task, "Task");
    if (err) g_error_free(err);
}

// --------------- TaskGraph Commands ---------------

void UFO_tg_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_new: Usage: handle = UFO_tg_new()");
    GError *err = NULL;
    UfoTaskGraph *tg = ufo_task_graph_new(&err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:TaskGraphNew", "%s", err->message);
    plhs[0] = createUfoHandle((UFO_Handle)tg, "TaskGraph");
    if (err) g_error_free(err);
}

void UFO_tg_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_delete: Usage: UFO_tg_delete(tgHandle)");
    UfoTaskGraph *tg = getUfoHandle_TaskGraph(prhs[1]);
    g_object_unref(tg);
    removeHandle(prhs[1]);
}

void UFO_tg_connect(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 4)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_connect: Usage: UFO_tg_connect(tg, srcTask, dstTask)");
    UfoTaskGraph *tg = getUfoHandle_TaskGraph(prhs[1]);
    UfoTask *src = getUfoHandle_Task(prhs[2]);
    UfoTask *dst = getUfoHandle_Task(prhs[3]);
    GError *err = NULL;
    ufo_task_graph_connect_nodes(tg, src, dst, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:TaskGraphConnect", "%s", err->message);
    if (err) g_error_free(err);
}

// --------------- Scheduler Commands ---------------

void UFO_sched_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_new: Usage: handle = UFO_sched_new()");
    GError *err = NULL;
    UfoBaseScheduler *sched = ufo_scheduler_new(&err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:SchedulerNew", "%s", err->message);
    plhs[0] = createUfoHandle((UFO_Handle)sched, "Scheduler");
    if (err) g_error_free(err);
}

void UFO_sched_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_delete: Usage: UFO_sched_delete(schedHandle)");
    UfoBaseScheduler *sched = getUfoHandle_Scheduler(prhs[1]);
    ufo_base_scheduler_abort(sched);
    g_object_unref(sched);
    removeHandle(prhs[1]);
}

void UFO_sched_setResources(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_setResources: Usage: UFO_sched_setResources(sched, resources)");
    UfoBaseScheduler *sched = getUfoHandle_Scheduler(prhs[1]);
    UfoResources *res = getUfoHandle_Resources(prhs[2]);
    ufo_base_scheduler_set_resources(sched, res);
}

void UFO_sched_run(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_run: Usage: UFO_sched_run(sched, tg)");
    UfoBaseScheduler *sched = getUfoHandle_Scheduler(prhs[1]);
    UfoTaskGraph *tg = getUfoHandle_TaskGraph(prhs[2]);
    GError *err = NULL;
    ufo_base_scheduler_run(sched, tg, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:SchedulerRun", "%s", err->message);
    if (err) g_error_free(err);
}

void UFO_sched_poll(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_poll: Usage: report = UFO_sched_poll(sched)");
    UfoBaseScheduler *sched = getUfoHandle_Scheduler(prhs[1]);
    GError *err = NULL;
    UfoTaskGraphReport *rep = ufo_base_scheduler_poll(sched, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:SchedulerPoll", "%s", err->message);
    plhs[0] = convertReportsToMx(rep);
    if (err) g_error_free(err);
}

// --------------- Buffer Commands ---------------

void UFO_buf_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_new: Usage: buf = UFO_buf_new(byteCount)");
    if (!mxIsUint64(prhs[1]) || mxGetNumberOfElements(prhs[1]) != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "byteCount must be a uint64 scalar");
    uint64_t byteCount = *(uint64_t*)mxGetData(prhs[1]);
    GError *err = NULL;
    UfoBuffer *buf = ufo_buffer_new(byteCount, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:BufferNew", "%s", err->message);
    plhs[0] = createUfoHandle((UFO_Handle)buf, "Buffer");
    if (err) g_error_free(err);
}

void UFO_buf_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_delete: Usage: UFO_buf_delete(bufHandle)");
    UfoBuffer *buf = getUfoHandle_Buffer(prhs[1]);
    g_object_unref(buf);
    removeHandle(prhs[1]);
}

void UFO_buf_getData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_getData: Usage: data = UFO_buf_getData(bufHandle)");
    UfoBuffer *buf = getUfoHandle_Buffer(prhs[1]);
    void *ptr = ufo_buffer_get_host_ptr(buf);
    size_t n = ufo_buffer_get_size(buf);
    plhs[0] = mxCreateNumericMatrix(1, n, mxUINT8_CLASS, mxREAL);
    memcpy(mxGetData(plhs[0]), ptr, n);
}

void UFO_buf_getSize(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_getSize: Usage: sz = UFO_buf_getSize(bufHandle)");
    UfoBuffer *buf = getUfoHandle_Buffer(prhs[1]);
    plhs[0] = mxCreateDoubleScalar((double)ufo_buffer_get_size(buf));
}
