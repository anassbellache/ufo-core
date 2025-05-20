#pragma once
#include <mex.h>
#include <glib.h>
#include <ufo/ufo.h>

#ifdef __cplusplus
extern "C" {
#endif

void mexUfo_handle_init(void);
void mexUfo_handle_shutdown(void);

mxArray *ufoHandle_create(gpointer obj, const char *type_name);
void ufoHandle_remove(const mxArray *arr);

UfoBuffer        *ufoHandle_getBuffer       (const mxArray *arr);
UfoPluginManager *ufoHandle_getPluginManager(const mxArray *arr);
UfoTaskGraph     *ufoHandle_getTaskGraph    (const mxArray *arr);
UfoBaseScheduler *ufoHandle_getScheduler    (const mxArray *arr);
UfoTask          *ufoHandle_getTask         (const mxArray *arr);
UfoResources     *ufoHandle_getResources    (const mxArray *arr);

#ifdef __cplusplus
}
#endif
