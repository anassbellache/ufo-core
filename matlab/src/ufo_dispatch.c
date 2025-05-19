/*
 *  ufo_dispatcher.c  –  central command router for ufo_mex
 *
 *  Each row of the table maps a MATLAB command-string
 *     ufo_mex('<command>', arg1, …)
 *  to a C function with signature
 *     void f(int nlhs, mxArray **plhs, int nrhs, const mxArray **prhs);
 */

#include "mex.h"
#include "ufo_mex_api.h"
#include <string.h>

/* ----------------   forward declarations  ---------------- */
/* (Buffer) */
void Buffer_new_mex       (int, mxArray**, int, const mxArray**);
void Buffer_free_mex      (int, mxArray**, int, const mxArray**);
void Buffer_getData_mex   (int, mxArray**, int, const mxArray**);
void Buffer_getSize_mex   (int, mxArray**, int, const mxArray**);
/* (PluginManager) */
void PluginManager_new_mex      (int, mxArray**, int, const mxArray**);
void PluginManager_free_mex     (int, mxArray**, int, const mxArray**);
void PluginManager_list_mex     (int, mxArray**, int, const mxArray**);
void PluginManager_getTask_mex  (int, mxArray**, int, const mxArray**);
/* (TaskGraph)  – include only the ones you implemented */
void TaskGraph_new_mex          (int, mxArray**, int, const mxArray**);
void TaskGraph_free_mex         (int, mxArray**, int, const mxArray**);
void TaskGraph_addNode_mex      (int, mxArray**, int, const mxArray**);
void TaskGraph_connect_mex      (int, mxArray**, int, const mxArray**);
void TaskGraph_listNodes_mex    (int, mxArray**, int, const mxArray**);
void TaskGraph_loadFromFile_mex (int, mxArray**, int, const mxArray**);
void TaskGraph_saveToFile_mex   (int, mxArray**, int, const mxArray**);
void TaskGraph_run_mex          (int, mxArray**, int, const mxArray**);
/* (Scheduler) */
void Scheduler_new_mex      (int, mxArray**, int, const mxArray**);
void Scheduler_free_mex     (int, mxArray**, int, const mxArray**);
void Scheduler_setResources_mex (int, mxArray**, int, const mxArray**);
void Scheduler_getResources_mex (int, mxArray**, int, const mxArray**);
void Scheduler_run_mex      (int, mxArray**, int, const mxArray**);
void Scheduler_runAsync_mex (int, mxArray**, int, const mxArray**);
void Scheduler_stop_mex     (int, mxArray**, int, const mxArray**);

/* ----------------   dispatch table  ---------------------- */
typedef void (*MexFn)(int,mxArray**,int,const mxArray**);

typedef struct { const char *cmd; MexFn fn; } CmdEntry;

static const CmdEntry cmd_table[] = {
    /* Buffer */
    { "Buffer_new",       Buffer_new_mex      },
    { "Buffer_free",      Buffer_free_mex     },
    { "Buffer_getData",   Buffer_getData_mex  },
    { "Buffer_getSize",   Buffer_getSize_mex  },

    /* PluginManager */
    { "PluginManager_new",        PluginManager_new_mex     },
    { "PluginManager_free",       PluginManager_free_mex    },
    { "PluginManager_listPlugins",PluginManager_list_mex    },
    { "PluginManager_getTask",    PluginManager_getTask_mex },

    /* TaskGraph */
    { "TaskGraph_new",          TaskGraph_new_mex          },
    { "TaskGraph_free",         TaskGraph_free_mex         },
    { "TaskGraph_addNode",      TaskGraph_addNode_mex      },
    { "TaskGraph_connect",      TaskGraph_connect_mex      },
    { "TaskGraph_listNodes",    TaskGraph_listNodes_mex    },
    { "TaskGraph_loadFromFile", TaskGraph_loadFromFile_mex },
    { "TaskGraph_saveToFile",   TaskGraph_saveToFile_mex   },
    { "TaskGraph_run",          TaskGraph_run_mex          },

    /* Scheduler */
    { "Scheduler_new",          Scheduler_new_mex          },
    { "Scheduler_free",         Scheduler_free_mex         },
    { "Scheduler_setResources", Scheduler_setResources_mex },
    { "Scheduler_getResources", Scheduler_getResources_mex },
    { "Scheduler_run",          Scheduler_run_mex          },
    { "Scheduler_runAsync",     Scheduler_runAsync_mex     },
    { "Scheduler_stop",         Scheduler_stop_mex         },

    { NULL, NULL }  /* sentinel */
};

/* ----------------   gateway   ---------------------------- */
void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    /* basic arg check */
    if (nrhs < 1 || !mxIsChar(prhs[0]))
        mexErrMsgIdAndTxt ("ufo_mex:BadInput",
                           "First argument must be a command string.");

    char *cmd = mxArrayToString (prhs[0]);
    if (!cmd)
        mexErrMsgIdAndTxt ("ufo_mex:BadInput",
                           "Failed to convert command to char*.");

    /* linear scan (few dozen entries) is fine */
    const CmdEntry *e;
    for (e = cmd_table; e->cmd; ++e)
        if (strcmp (cmd, e->cmd) == 0) {
            mxFree (cmd);
            e->fn (nlhs, plhs, nrhs, prhs);
            return;
        }

    /* not found */
    mexErrMsgIdAndTxt ("ufo_mex:UnknownCommand",
                       "Unknown command '%s'", cmd);
}
