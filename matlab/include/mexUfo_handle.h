#pragma once
#ifndef MEX_UFO_HANDLE_H
#define MEX_UFO_HANDLE_H

#include <mex.h>
#include <glib.h>
#include <ufo/ufo.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialization and teardown */
void mexUfo_handle_init(void);
void mexUfo_handle_shutdown(void);

/* Allocate MATLAB handle for a GObject (takes ownership) */
mxArray *ufoHandle_create(gpointer obj, const char *type_name);

/* Remove handle from registry */
void ufoHandle_remove(const mxArray *arr);

/* Typed getters */
UfoBuffer        *ufoHandle_getBuffer       (const mxArray *arr);
UfoPluginManager *ufoHandle_getPluginManager(const mxArray *arr);
UfoTaskGraph     *ufoHandle_getTaskGraph    (const mxArray *arr);
UfoBaseScheduler *ufoHandle_getScheduler    (const mxArray *arr);
UfoTask          *ufoHandle_getTask         (const mxArray *arr);
UfoResources     *ufoHandle_getResources    (const mxArray *arr);

/* Compatibility wrappers matching older API */
mxArray *createUfoHandle(UFO_Handle h, const char *cls);
static inline void removeHandle(const mxArray *a) { ufoHandle_remove(a); }
static inline UfoBuffer *getUfoHandle_Buffer(const mxArray *a) { return ufoHandle_getBuffer(a); }
static inline UfoPluginManager *getUfoHandle_PluginManager(const mxArray *a) { return ufoHandle_getPluginManager(a); }
static inline UfoTaskGraph *getUfoHandle_TaskGraph(const mxArray *a) { return ufoHandle_getTaskGraph(a); }
static inline UfoBaseScheduler *getUfoHandle_Scheduler(const mxArray *a) { return ufoHandle_getScheduler(a); }
static inline UfoTask *getUfoHandle_Task(const mxArray *a) { return ufoHandle_getTask(a); }
static inline UfoResources *getUfoHandle_Resources(const mxArray *a) { return ufoHandle_getResources(a); }

#ifdef __cplusplus
}
#endif

#endif /* MEX_UFO_HANDLE_H */
