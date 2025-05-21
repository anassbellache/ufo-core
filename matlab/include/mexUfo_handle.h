#pragma once
#include <mex.h>
#include <glib-object.h>
#include <ufo/ufo.h>
#ifdef __cplusplus
extern "C" {
#endif

void mexUfo_handle_init(void);
void mexUfo_handle_shutdown(void);

mxArray *ufoHandle_create(gpointer obj, const char *type_name);
void ufoHandle_remove(const mxArray *arr);

UfoBuffer        *ufoHandle_getBuffer(const mxArray *arr);
UfoPluginManager *ufoHandle_getPluginManager(const mxArray *arr);
UfoTaskGraph     *ufoHandle_getTaskGraph(const mxArray *arr);
UfoBaseScheduler *ufoHandle_getScheduler(const mxArray *arr);
UfoTask          *ufoHandle_getTask(const mxArray *arr);
UfoResources     *ufoHandle_getResources(const mxArray *arr);

/* Compatibility wrappers for older names */
static inline mxArray *createUfoHandle(UFO_Handle h, const char *type_name)
{ return ufoHandle_create((gpointer) h, type_name); }
static inline void removeHandle(const mxArray *arr)
{ ufoHandle_remove(arr); }
static inline UfoBuffer *getUfoHandle_Buffer(const mxArray *arr)
{ return ufoHandle_getBuffer(arr); }
static inline UfoPluginManager *getUfoHandle_PluginManager(const mxArray *arr)
{ return ufoHandle_getPluginManager(arr); }
static inline UfoTaskGraph *getUfoHandle_TaskGraph(const mxArray *arr)
{ return ufoHandle_getTaskGraph(arr); }
static inline UfoBaseScheduler *getUfoHandle_Scheduler(const mxArray *arr)
{ return ufoHandle_getScheduler(arr); }
static inline UfoTask *getUfoHandle_Task(const mxArray *arr)
{ return ufoHandle_getTask(arr); }
static inline UfoResources *getUfoHandle_Resources(const mxArray *arr)
{ return ufoHandle_getResources(arr); }

#ifdef __cplusplus
}
#endif
