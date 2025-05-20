/* mexUfo_handle.c  –  central handle registry for MATLAB <--> UFO-Core
 *
 * One uint64 key  ↔  one GObject*  (+ type tag string)
 *
 *  – safe on 64-bit MATLAB, O(1) lookup
 *  – automatic final clean-up via explicit init/shutdown
 */

#include "mexUfo_handle.h"
#include <mex.h>
#include <glib-object.h>
#include <errno.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Registry structures                                                */
/* ------------------------------------------------------------------ */
typedef struct {
    gpointer  obj;         /* GObject* (UfoBuffer, UfoTaskGraph, …)   */
    char     *type_name;   /* lowercase canonical string              */
} HandleEntry;

/* by-value uint64 key (fits in pointer on 64-bit) */
static GHashTable *g_registry  = NULL;
static GMutex      g_registry_mtx;
static uint64_t    g_next_id   = 1;
gboolean ufo_handle_test_force_alloc_fail = FALSE;

/* ------------------------------------------------------------------ */
/* Helpers                                                            */
/* ------------------------------------------------------------------ */
static void entry_free (gpointer data)
{
    HandleEntry *e = (HandleEntry *) data;
    if (!e) return;

    if (e->obj)
        g_object_unref (e->obj);

    g_free (e->type_name);
    g_free (e);
}

static void registry_ensure(void)
{
    if (g_registry)
        return;

    g_mutex_init(&g_registry_mtx);
    g_registry = g_hash_table_new_full(g_direct_hash,
                                       g_direct_equal,
                                       NULL,
                                       entry_free);
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

void
mexUfo_handle_init(void)
{
    registry_ensure();
}

void
mexUfo_handle_shutdown(void)
{
    if (g_registry) {
        g_hash_table_destroy(g_registry);
        g_registry = NULL;
        g_mutex_clear(&g_registry_mtx);
        g_next_id = 1;
    }
}

UFO_Handle
ufo_handle_alloc(gpointer obj, const char *type_name)
{
    registry_ensure();

    if (ufo_handle_test_force_alloc_fail) {
        errno = ENOMEM;
        return 0;
    }

    g_return_val_if_fail(G_IS_OBJECT(obj), 0);
    g_return_val_if_fail(type_name != NULL, 0);

    g_mutex_lock(&g_registry_mtx);
    const uint64_t id = g_next_id++;

    HandleEntry *e = g_new0(HandleEntry, 1);
    e->obj = g_object_ref(obj);
    e->type_name = g_ascii_strdown(type_name, -1);
    g_hash_table_insert(g_registry, GUINT_TO_POINTER((guintptr) id), e);
    g_mutex_unlock(&g_registry_mtx);

    return id;
}

gpointer
ufo_handle_lookup(UFO_Handle id, const char *expected_lower)
{
    registry_ensure();

    g_mutex_lock(&g_registry_mtx);
    HandleEntry *e = g_hash_table_lookup(g_registry, GUINT_TO_POINTER((guintptr) id));
    g_mutex_unlock(&g_registry_mtx);

    if (!e)
        return NULL;

    if (expected_lower && g_ascii_strcasecmp(e->type_name, expected_lower) != 0)
        return NULL;

    return e->obj;
}

int
ufo_handle_free(UFO_Handle id)
{
    registry_ensure();

    g_mutex_lock(&g_registry_mtx);
    gboolean ok = g_hash_table_remove(g_registry, GUINT_TO_POINTER((guintptr) id));
    g_mutex_unlock(&g_registry_mtx);

    return ok ? 0 : -1;
}

size_t
ufo_handle_num_entries(void)
{
    return g_registry ? (size_t) g_hash_table_size(g_registry) : 0;
}

/* Create or wrap a GObject into a MATLAB uint64 handle */
mxArray *ufoHandle_create (gpointer obj, const char *type_name)
{
    UFO_Handle id = ufo_handle_alloc(obj, type_name);

    mxArray *arr = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    if (!arr) {
        ufo_handle_free(id);
        mexErrMsgIdAndTxt("ufo_mex:AllocFailed", "Failed to allocate MATLAB handle.");
    }

    *(uint64_t *) mxGetData(arr) = id;
    return arr;
}

/* Remove & unref */
void ufoHandle_remove (const mxArray *arr)
{
    registry_ensure ();

    if (!mxIsUint64 (arr) || mxGetNumberOfElements (arr) != 1)
        mexErrMsgIdAndTxt ("ufo_mex:BadHandle",
                           "Handle must be a 1×1 uint64.");

    const uint64_t id = *(const uint64_t *) mxGetData (arr);

    if (ufo_handle_free(id) != 0)
        mexErrMsgIdAndTxt ("ufo_mex:BadHandle",
                           "Handle %" PRIu64 " not found (double free?).", id);
}

/* Internal lookup – returns borrowed pointer */
static HandleEntry *lookup (const mxArray *arr,
                            const char    *expected_lower)
{
    registry_ensure ();

    if (!mxIsUint64 (arr) || mxGetNumberOfElements (arr) != 1)
        mexErrMsgIdAndTxt ("ufo_mex:BadHandle",
                           "Handle must be a 1×1 uint64.");

    const uint64_t id = *(const uint64_t *) mxGetData (arr);

    g_mutex_lock (&g_registry_mtx);
    HandleEntry *e = g_hash_table_lookup (g_registry,
                                          GUINT_TO_POINTER ((guintptr) id));
    g_mutex_unlock (&g_registry_mtx);

    if (!e)
        mexErrMsgIdAndTxt ("ufo_mex:BadHandle",
                           "Unknown handle %" PRIu64 ".", id);

    if (g_ascii_strcasecmp (e->type_name, expected_lower) != 0)
        mexErrMsgIdAndTxt ("ufo_mex:BadHandle",
                           "Handle %" PRIu64 " is of type '%s', expected '%s'.",
                           id, e->type_name, expected_lower);

    return e;
}

/* Typed getters – exported in ufo_mex_api.h */
UfoBuffer        *ufoHandle_getBuffer       (const mxArray *arr)
{ return UFO_BUFFER (lookup (arr, "buffer")->obj); }

UfoPluginManager *ufoHandle_getPluginManager(const mxArray *arr)
{ return UFO_PLUGIN_MANAGER (lookup (arr, "pluginmanager")->obj); }

UfoTaskGraph     *ufoHandle_getTaskGraph    (const mxArray *arr)
{ return UFO_TASK_GRAPH (lookup (arr, "taskgraph")->obj); }

UfoBaseScheduler *ufoHandle_getScheduler    (const mxArray *arr)
{ return UFO_BASE_SCHEDULER (lookup (arr, "scheduler")->obj); }

UfoTask *ufoHandle_getTask(const mxArray *arr)
{ return UFO_TASK(lookup(arr, "task")->obj); }

UfoResources *ufoHandle_getResources(const mxArray *arr)
{ return UFO_RESOURCES(lookup(arr, "resources")->obj); }

/* Init/cleanup ---------------------------------------------------- */

void mexUfo_handle_init(void)
{
    registry_ensure();
}

void mexUfo_handle_shutdown(void)
{
    if (!g_registry)
        return;
    g_hash_table_destroy(g_registry);
    g_registry = NULL;
    g_mutex_clear(&g_registry_mtx);
}
