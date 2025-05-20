#include <glib.h>
#include "test-suite.h"
#include "mex_stub.h"
#include <string.h>

/* Counter to verify function call */
static int call_counter;

int mex_error_code = 0;
void mexErrMsgIdAndTxt(const char *id, const char *fmt, ...)
{
    (void)id; (void)fmt;
    mex_error_code = 1;
}

/* --- stub command handlers --- */
void Buffer_new_mex(int nlhs, mxArray **plhs, int nrhs, const mxArray **prhs)
{ call_counter++; }
#define STUB(name) \
    void name(int nlhs, mxArray **plhs, int nrhs, const mxArray **prhs) { (void)nlhs;(void)plhs;(void)nrhs;(void)prhs; }
STUB(Buffer_free_mex)
STUB(Buffer_getData_mex)
STUB(Buffer_getSize_mex)
STUB(PluginManager_new_mex)
STUB(PluginManager_free_mex)
STUB(PluginManager_list_mex)
STUB(PluginManager_getTask_mex)
STUB(TaskGraph_new_mex)
STUB(TaskGraph_free_mex)
STUB(TaskGraph_addNode_mex)
STUB(TaskGraph_connect_mex)
STUB(TaskGraph_listNodes_mex)
STUB(TaskGraph_loadFromFile_mex)
STUB(TaskGraph_saveToFile_mex)
STUB(TaskGraph_run_mex)
STUB(Scheduler_new_mex)
STUB(Scheduler_free_mex)
STUB(Scheduler_setResources_mex)
STUB(Scheduler_getResources_mex)
STUB(Scheduler_run_mex)
STUB(Scheduler_runAsync_mex)
STUB(Scheduler_stop_mex)
#undef STUB

/* Include the actual dispatch implementation */
#include "../../matlab/src/ufo_dispatch.c"

/* helper to look up command in table */
static MexFn lookup(const char *name)
{
    if (!name)
        return NULL;
    for (const CmdEntry *e = cmd_table; e->cmd; ++e) {
        if (strcmp(name, e->cmd) == 0)
            return e->fn;
    }
    return NULL;
}

typedef enum {
    UFO_DISPATCH_OK = 0,
    UFO_DISPATCH_BAD_ARG = -1,
    UFO_DISPATCH_UNKNOWN_CMD = -2
} DispatchResult;

static DispatchResult dispatch_call(const char *cmd)
{
    if (!cmd)
        return UFO_DISPATCH_BAD_ARG;
    MexFn fn = lookup(cmd);
    if (!fn)
        return UFO_DISPATCH_UNKNOWN_CMD;

    /* Simulate mexCallMATLAB(cmd, nBytes) */
    mxArray cmdArr = { cmd };
    mxArray argArr = { NULL };
    const mxArray *rhs[2] = { &cmdArr, &argArr };

    fn(0, NULL, 2, rhs);
    return UFO_DISPATCH_OK;
}

/* ------------------ test cases --------------------- */
static void test_buffer_new(void)
{
    call_counter = 0;
    DispatchResult r = dispatch_call("Buffer_new");
    g_assert_cmpint(r, ==, UFO_DISPATCH_OK);
    g_assert_cmpint(call_counter, ==, 1);
}

static void test_unknown(void)
{
    DispatchResult r = dispatch_call("Foo");
    g_assert_cmpint(r, ==, UFO_DISPATCH_UNKNOWN_CMD);
}

static void test_null_arg(void)
{
    DispatchResult r = dispatch_call(NULL);
    g_assert_cmpint(r, ==, UFO_DISPATCH_BAD_ARG);
}

static void test_table_complete(void)
{
    for (const CmdEntry *e = cmd_table; e->cmd; ++e) {
        MexFn fn = lookup(e->cmd);
        g_assert_nonnull(fn);
    }
}

void test_add_dispatch(void)
{
    g_test_add_func("/dispatch/DT-01_buffer_new", test_buffer_new);
    g_test_add_func("/dispatch/DT-02_unknown", test_unknown);
    g_test_add_func("/dispatch/DT-03_null", test_null_arg);
    g_test_add_func("/dispatch/DT-04_table_complete", test_table_complete);
}
