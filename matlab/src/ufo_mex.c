#include "mex.h"
#include "ufo_mex_api.h"
#include "mexUfo_handle.h"
#include <string.h>
#include <stdbool.h>

/* forward declaration from ufo_commands.c */
void PluginManager_list_mex(int, mxArray **, int, const mxArray **);

typedef void (*mexFunctionPtr)(int, mxArray **, int, const mxArray **);

typedef struct {
    const char *name;
    mexFunctionPtr fn;
} Dispatch;

// --- Static helpers ---


// Returns pointer to function for verb, or NULL if not found (binary search)
static mexFunctionPtr find_verb(const char *name) {
    // Table must be sorted alphabetically by .name
    static const Dispatch table[] = {
        {"Buffer_delete",      UFO_buf_delete},
        {"Buffer_free",        UFO_buf_delete}, // alias
        {"Buffer_getData",     UFO_buf_getData},
        {"Buffer_getSize",     UFO_buf_getSize},
        {"Buffer_new",         UFO_buf_new},
        {"PluginManager_free", UFO_pm_delete},
        {"PluginManager_getTask", UFO_pm_getTask},
        {"PluginManager_listPlugins", PluginManager_list_mex},
        {"PluginManager_new",  UFO_pm_new},
        {"Resources_delete",   UFO_res_delete},
        {"Resources_free",     UFO_res_delete}, // alias
        {"Resources_new",      UFO_res_new},
        {"Scheduler_delete",   UFO_sched_delete},
        {"Scheduler_free",     UFO_sched_delete}, // alias
        {"Scheduler_getResources", UFO_sched_getResources},
        {"Scheduler_new",      UFO_sched_new},
        {"Scheduler_poll",     UFO_sched_poll},
        {"Scheduler_run",      UFO_sched_run},
        {"Scheduler_runAsync", UFO_sched_runAsync},
        {"Scheduler_setResources", UFO_sched_setResources},
        {"Scheduler_stop",     UFO_sched_stop},
        {"TaskGraph_addNode",  UFO_tg_addNode},
        {"TaskGraph_connect",  UFO_tg_connect},
        {"TaskGraph_delete",   UFO_tg_delete},
        {"TaskGraph_free",     UFO_tg_delete}, // alias
        {"TaskGraph_listNodes",UFO_tg_listNodes},
        {"TaskGraph_loadFromFile", UFO_tg_loadFromFile},
        {"TaskGraph_new",      UFO_tg_new},
        {"TaskGraph_run",      UFO_tg_run},
        {"TaskGraph_saveToFile", UFO_tg_saveToFile},
        {"Task_free",        UFO_task_delete},
        {"mexVersion",         NULL}
    };
    const size_t n = sizeof(table)/sizeof(table[0]);
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int cmp = strcmp(name, table[mid].name);
        if (cmp == 0)
            return table[mid].fn;
        else if (cmp < 0)
            hi = mid;
        else
            lo = mid + 1;
    }
    return NULL;
}

// --- mexFunction ---

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    static bool initialised = false;
    if (!initialised) {
        mexLock();
        mexUfo_handle_init();
        initialised = true;
    }

    if (nrhs < 1 || !mxIsChar(prhs[0]) || mxGetNumberOfElements(prhs[0]) < 1) {
        mexErrMsgIdAndTxt("ufo:mex:BadInput", "First argument must be a non-empty command string.");
    }

    char *verb = mxArrayToString(prhs[0]);
    if (!verb)
        mexErrMsgIdAndTxt("ufo:mex:ConversionFailed", "Could not convert command to string.");

    // Special case: version string
    if (strcmp(verb, "mexVersion") == 0) {
        if (nlhs > 1)
            mexErrMsgIdAndTxt("ufo:mex:BadOutput", "Too many outputs for 'mexVersion'");
        plhs[0] = mxCreateString("ufo-mex 0.1");
        mxFree(verb);
        return;
    }

    mexFunctionPtr fp = find_verb(verb);
    if (!fp) {
        mxFree(verb);
        mexErrMsgIdAndTxt("ufo:mex:UnknownVerb", "Unknown command '%s'", verb);
    }

    // Forward remaining arguments (prhs+1, nrhs-1)
    fp(nlhs, plhs, nrhs-1, prhs+1);

    mxFree(verb);
}
