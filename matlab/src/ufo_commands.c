#include "ufo_mex_api.h"
#include "mexUfo_handle.h"
#include <ufo/ufo-plugin-manager.h>
#include <ufo/ufo-task-graph.h>
#include <ufo/ufo-buffer.h>
#include <ufo/ufo-base-scheduler.h>
#include <ufo/ufo-resources.h>
#include <ufo/ufo-task.h>

#include <glib.h>
#include <mex.h>
#include <ufo/ufo-task-node.h>
#include <inttypes.h>

/* Helper converting UfoTaskGraphReport -> MATLAB struct
   This is a minimal placeholder as the detailed struct
   definition is not exposed in these bindings. */
static mxArray *convertReportsToMx(gpointer rep)
{
    (void) rep;
    const char *fields[] = { NULL };
    return mxCreateStructMatrix(1, 0, 0, fields);
}

/* Retrieve or create the task list associated with a TaskGraph */
static GPtrArray *
ensure_task_array(UfoTaskGraph *tg)
{
    GPtrArray *arr = g_object_get_data(G_OBJECT(tg), "mex_tasks");
    if (!arr) {
        arr = g_ptr_array_new_with_free_func(g_object_unref);
        g_object_set_data_full(G_OBJECT(tg), "mex_tasks", arr,
                              (GDestroyNotify) g_ptr_array_unref);
    }
    return arr;
}

/* Resolve a task argument (handle, id, or name) */
static UfoTask *
resolve_task_arg(UfoTaskGraph *tg, const mxArray *arg)
{
    if (mxIsUint64(arg)) {
        uint64_t id = *(uint64_t *) mxGetData(arg);
        gpointer obj = ufo_handle_lookup(id, "task");
        if (obj)
            return UFO_TASK(obj);

        GPtrArray *arr = g_object_get_data(G_OBJECT(tg), "mex_tasks");
        if (!arr || id >= arr->len)
            mexErrMsgIdAndTxt("ufo_mex:BadNodeId", "Invalid node id %" PRIu64, id);
        return g_ptr_array_index(arr, id);
    }
    else if (mxIsChar(arg)) {
        char *name = mxArrayToString(arg);
        GPtrArray *arr = g_object_get_data(G_OBJECT(tg), "mex_tasks");
        if (!arr) {
            mxFree(name);
            mexErrMsgIdAndTxt("ufo_mex:NoNodes", "TaskGraph contains no nodes");
        }
        for (guint i = 0; i < arr->len; i++) {
            UfoTask *t = g_ptr_array_index(arr, i);
            const char *idname = ufo_task_node_get_identifier(UFO_TASK_NODE(t));
            if (idname && g_strcmp0(idname, name) == 0) {
                mxFree(name);
                return t;
            }
        }
        mexErrMsgIdAndTxt("ufo_mex:UnknownNode", "Node '%s' not found", name);
    }

    mexErrMsgIdAndTxt("ufo_mex:BadArg", "Endpoint must be uint64 or string");
    return NULL; /* never reached */
}

typedef struct _UfoTaskGraphReport UfoTaskGraphReport;

/* helper: convert scheduler report to MATLAB struct (placeholder) */
static mxArray *convertReportsToMx(UfoTaskGraphReport *rep)
{
    (void) rep;
    return mxCreateStructMatrix(0, 0, 0, NULL);
}

// --------------- PluginManager Commands ---------------

void UFO_pm_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_new: Usage: handle = UFO_pm_new()");
    GError *err = NULL;
    UfoPluginManager *pm = ufo_plugin_manager_new(&err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:PluginManagerNew", "%s", err->message);
    plhs[0] = ufoHandle_create(pm, "PluginManager");
    if (err) g_error_free(err);
}

void UFO_pm_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_delete: Usage: UFO_pm_delete(pmHandle)");
    UfoPluginManager *pm = ufoHandle_getPluginManager(prhs[1]);
    g_object_unref(pm);
    ufoHandle_remove(prhs[1]);
}

void UFO_pm_listPlugins(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_listPlugins: Usage: names = UFO_pm_listPlugins(pm)");

    UfoPluginManager *pm = ufoHandle_getPluginManager(prhs[1]);

    GList *names = ufo_plugin_manager_get_all_task_names(pm);
    const guint count = g_list_length(names);

    plhs[0] = mxCreateCellMatrix(1, count);

    guint idx = 0;
    for (GList *l = names; l != NULL; l = l->next, ++idx) {
        const char *name = (const char *) l->data;
        mxArray *s = mxCreateString(name);
        mxSetCell(plhs[0], idx, s);
    }

    g_list_free_full(names, g_free);
}

void UFO_pm_getTask(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_pm_getTask: Usage: task = UFO_pm_getTask(pmHandle, taskName)");
    UfoPluginManager *pm = ufoHandle_getPluginManager(prhs[1]);
    if (!mxIsChar(prhs[2]))
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "Task name must be a string");
    char *name = mxArrayToString(prhs[2]);
    GError *err = NULL;
    UfoTask *task = ufo_plugin_manager_get_task(pm, name, &err);
    mxFree(name);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:PluginManagerGetTask", "%s", err->message);
    plhs[0] = ufoHandle_create(task, "Task");
    if (err) g_error_free(err);
}

void PluginManager_list_mex(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "Usage: PluginManager_listPlugins(pm)");

    UfoPluginManager *pm = getUfoHandle_PluginManager(prhs[1]);
    GList *names = ufo_plugin_manager_get_all_task_names(pm);
    int count = g_list_length(names);
    mxArray *cell = mxCreateCellMatrix(1, count);
    int i = 0;
    for (GList *l = names; l != NULL; l = l->next, ++i) {
        const char *name = (const char *) l->data;
        mxArray *s = mxCreateString(name);
        mxSetCell(cell, i, s);
    }
    g_list_free_full(names, g_free);
    plhs[0] = cell;
}

// --------------- TaskGraph Commands ---------------

void UFO_tg_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_new: Usage: handle = UFO_tg_new()");
    GError *err = NULL;
    UfoTaskGraph *tg = ufo_task_graph_new(&err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:TaskGraphNew", "%s", err->message);
    plhs[0] = ufoHandle_create(tg, "TaskGraph");
    if (err) g_error_free(err);
}

void UFO_tg_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_delete: Usage: UFO_tg_delete(tgHandle)");
    UfoTaskGraph *tg = ufoHandle_getTaskGraph(prhs[1]);
    g_object_unref(tg);
    ufoHandle_remove(prhs[1]);
}

void UFO_tg_connect(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 4)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_connect: Usage: UFO_tg_connect(tg, srcTask, dstTask)");
    UfoTaskGraph *tg = ufoHandle_getTaskGraph(prhs[1]);
    UfoTask *src = resolve_task_arg(tg, prhs[2]);
    UfoTask *dst = resolve_task_arg(tg, prhs[3]);
    GError *err = NULL;
    ufo_task_graph_connect_nodes(tg, UFO_TASK_NODE(src), UFO_TASK_NODE(dst), &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:TaskGraphConnect", "%s", err->message);
    if (err) g_error_free(err);
}

void UFO_tg_addNode(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_addNode: Usage: id = UFO_tg_addNode(tg, task)");
    UfoTaskGraph *tg = ufoHandle_getTaskGraph(prhs[1]);
    UfoTask *task = ufoHandle_getTask(prhs[2]);
    GPtrArray *arr = ensure_task_array(tg);
    g_ptr_array_add(arr, g_object_ref(task));
    uint64_t id = arr->len - 1;
    plhs[0] = mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
    *(uint64_t*)mxGetData(plhs[0]) = id;
}

void UFO_tg_loadFromFile(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 0 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_loadFromFile: Usage: UFO_tg_loadFromFile(tg, filename)");
    mexErrMsgIdAndTxt("ufo_mex:NotImplemented", "TaskGraph_loadFromFile not implemented");
}

void UFO_tg_saveToFile(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 0 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_saveToFile: Usage: UFO_tg_saveToFile(tg, filename)");
    mexErrMsgIdAndTxt("ufo_mex:NotImplemented", "TaskGraph_saveToFile not implemented");
}

void UFO_tg_listNodes(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_listNodes: Usage: names = UFO_tg_listNodes(tg)");
    UfoTaskGraph *tg = ufoHandle_getTaskGraph(prhs[1]);
    GPtrArray *arr = g_object_get_data(G_OBJECT(tg), "mex_tasks");
    guint count = arr ? arr->len : 0;
    plhs[0] = mxCreateCellMatrix(1, count);
    for (guint i = 0; i < count; i++) {
        UfoTask *t = g_ptr_array_index(arr, i);
        const char *name = ufo_task_node_get_identifier(UFO_TASK_NODE(t));
        if (!name) name = "";
        mxSetCell(plhs[0], i, mxCreateString(name));
    }
}

void UFO_tg_run(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs > 1 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_tg_run: Usage: result = UFO_tg_run(tg, sched)");
    UfoTaskGraph *tg = ufoHandle_getTaskGraph(prhs[1]);
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[2]);
    GError *err = NULL;
    ufo_base_scheduler_run(sched, tg, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:TaskGraphRun", "%s", err->message);
    if (nlhs == 1)
        plhs[0] = mxCreateDoubleScalar(0);
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
    plhs[0] = ufoHandle_create(sched, "Scheduler");
    if (err) g_error_free(err);
}

void UFO_sched_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_delete: Usage: UFO_sched_delete(schedHandle)");
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[1]);
    ufo_base_scheduler_abort(sched);
    g_object_unref(sched);
    ufoHandle_remove(prhs[1]);
}

void UFO_sched_setResources(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_setResources: Usage: UFO_sched_setResources(sched, resources)");
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[1]);
    UfoResources *res = ufoHandle_getResources(prhs[2]);
    ufo_base_scheduler_set_resources(sched, res);
}

void UFO_sched_run(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_run: Usage: UFO_sched_run(sched, tg)");
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[1]);
    UfoTaskGraph *tg = ufoHandle_getTaskGraph(prhs[2]);
    GError *err = NULL;
    ufo_base_scheduler_run(sched, tg, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:SchedulerRun", "%s", err->message);
    if (err) g_error_free(err);
}

void UFO_sched_poll(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_poll: Usage: report = UFO_sched_poll(sched)");
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[1]);
    GError *err = NULL;
    UfoTaskGraphReport *rep = ufo_base_scheduler_poll(sched, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:SchedulerPoll", "%s", err->message);
    plhs[0] = convertReportsToMx(rep);
    if (err) g_error_free(err);
}

void UFO_sched_getResources(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_getResources: Usage: res = UFO_sched_getResources(sched)");
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[1]);
    GError *err = NULL;
    UfoResources *res = ufo_base_scheduler_get_resources(sched, &err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:SchedulerGetResources", "%s", err->message);
    plhs[0] = ufoHandle_create(res, "Resources");
    if (err) g_error_free(err);
}

void UFO_sched_runAsync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 1 || nrhs != 3)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_runAsync: Usage: fut = UFO_sched_runAsync(sched, tg)");
    mexErrMsgIdAndTxt("ufo_mex:NotImplemented", "Scheduler_runAsync not implemented");
}

void UFO_sched_stop(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_sched_stop: Usage: UFO_sched_stop(sched)");
    UfoBaseScheduler *sched = ufoHandle_getScheduler(prhs[1]);
    ufo_base_scheduler_abort(sched);
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
    plhs[0] = ufoHandle_create(buf, "Buffer");
    if (err) g_error_free(err);
}

void UFO_buf_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_delete: Usage: UFO_buf_delete(bufHandle)");
    UfoBuffer *buf = ufoHandle_getBuffer(prhs[1]);
    g_object_unref(buf);
    ufoHandle_remove(prhs[1]);
}

void UFO_buf_getData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_getData: Usage: data = UFO_buf_getData(bufHandle)");
    UfoBuffer *buf = ufoHandle_getBuffer(prhs[1]);
    void *ptr = ufo_buffer_get_host_ptr(buf);
    size_t n = ufo_buffer_get_size(buf);
    plhs[0] = mxCreateNumericMatrix(1, n, mxUINT8_CLASS, mxREAL);
    memcpy(mxGetData(plhs[0]), ptr, n);
}

void UFO_buf_getSize(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_buf_getSize: Usage: sz = UFO_buf_getSize(bufHandle)");
    UfoBuffer *buf = ufoHandle_getBuffer(prhs[1]);
    plhs[0] = mxCreateDoubleScalar((double)ufo_buffer_get_size(buf));
}

// --------------- Resources Commands ---------------

void UFO_res_new(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 1 || nrhs != 1)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_res_new: Usage: res = UFO_res_new()");
    GError *err = NULL;
    UfoResources *res = ufo_resources_new(&err);
    if (err)
        mexErrMsgIdAndTxt("ufo_mex:ResourcesNew", "%s", err->message);
    plhs[0] = ufoHandle_create(res, "Resources");
    if (err) g_error_free(err);
}

void UFO_res_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_res_delete: Usage: UFO_res_delete(resHandle)");
    UfoResources *res = ufoHandle_getResources(prhs[1]);
    g_object_unref(res);
    ufoHandle_remove(prhs[1]);
}

// --------------- Task Commands ---------------

void UFO_task_delete(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs != 0 || nrhs != 2)
        mexErrMsgIdAndTxt("ufo_mex:BadArg", "UFO_task_delete: Usage: UFO_task_delete(taskHandle)");
    UfoTask *task = ufoHandle_getTask(prhs[1]);
    g_object_unref(task);
    ufoHandle_remove(prhs[1]);
}
