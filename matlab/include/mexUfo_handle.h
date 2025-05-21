#pragma once
#include <glib.h>
#include <stdint.h>
#include <mex.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t UFO_Handle;

void mexUfo_handle_init(void);
void mexUfo_handle_shutdown(void);

UFO_Handle ufo_handle_alloc(gpointer obj, const char *type_name);
gpointer    ufo_handle_lookup(UFO_Handle id, const char *expected_lower);
int         ufo_handle_free(UFO_Handle id);
size_t      ufo_handle_num_entries(void);

/* Test helper: when set to TRUE, ufo_handle_alloc simulates ENOMEM */
extern gboolean ufo_handle_test_force_alloc_fail;

/* MATLAB convenience wrappers */
mxArray *ufoHandle_create(gpointer obj, const char *type_name);
void     ufoHandle_remove(const mxArray *arr);
UfoBuffer        *ufoHandle_getBuffer(const mxArray *arr);
UfoPluginManager *ufoHandle_getPluginManager(const mxArray *arr);
UfoTaskGraph     *ufoHandle_getTaskGraph(const mxArray *arr);
UfoBaseScheduler *ufoHandle_getScheduler(const mxArray *arr);
UfoTask          *ufoHandle_getTask(const mxArray *arr);
UfoResources     *ufoHandle_getResources(const mxArray *arr);
#ifdef __cplusplus
}
#endif
