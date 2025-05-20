#include "mexUfo_handle.h"
#include "test-suite.h"
#include <glib-object.h>
#include <errno.h>

static void setup(void) {
    mexUfo_handle_init();
}

static void teardown(void) {
    mexUfo_handle_shutdown();
}

/* HR-01 */
static void test_alloc_returns_nonzero(void) {
    GObject *obj = g_object_new(G_TYPE_OBJECT, NULL);
    UFO_Handle id = ufo_handle_alloc(obj, "object");
    g_assert_cmpuint(id, !=, 0);
    ufo_handle_free(id);
    g_object_unref(obj);
}

/* HR-02 */
static void test_lookup_returns_pointer(void) {
    GObject *obj = g_object_new(G_TYPE_OBJECT, NULL);
    UFO_Handle id = ufo_handle_alloc(obj, "object");
    gpointer got = ufo_handle_lookup(id, "object");
    g_assert(got == obj);
    ufo_handle_free(id);
    g_object_unref(obj);
}

/* HR-03 */
static void test_lookup_after_free_is_null(void) {
    GObject *obj = g_object_new(G_TYPE_OBJECT, NULL);
    UFO_Handle id = ufo_handle_alloc(obj, "object");
    ufo_handle_free(id);
    gpointer got = ufo_handle_lookup(id, "object");
    g_assert(got == NULL);
    g_object_unref(obj);
}

/* HR-04 */
static void test_lookup_unknown_returns_null(void) {
    gpointer got = ufo_handle_lookup(0x1234, "object");
    g_assert(got == NULL);
}

/* HR-05 */
static void test_double_free_detected(void) {
    GObject *obj = g_object_new(G_TYPE_OBJECT, NULL);
    UFO_Handle id = ufo_handle_alloc(obj, "object");
    g_assert_cmpint(ufo_handle_free(id), ==, 0);
    g_assert_cmpint(ufo_handle_free(id), ==, -1);
    g_object_unref(obj);
}

/* HR-06 */
static void test_alloc_fail_sets_errno(void) {
    ufo_handle_test_force_alloc_fail = TRUE;
    errno = 0;
    UFO_Handle id = ufo_handle_alloc(g_object_new(G_TYPE_OBJECT, NULL), "object");
    g_assert_cmpuint(id, ==, 0);
    g_assert_cmpint(errno, ==, ENOMEM);
    ufo_handle_test_force_alloc_fail = FALSE;
}

/* HR-07 */
static void test_registry_shrinks_to_zero(void) {
    GObject *a = g_object_new(G_TYPE_OBJECT, NULL);
    GObject *b = g_object_new(G_TYPE_OBJECT, NULL);
    UFO_Handle id1 = ufo_handle_alloc(a, "object");
    UFO_Handle id2 = ufo_handle_alloc(b, "object");
    g_assert_cmpint(ufo_handle_free(id1), ==, 0);
    g_assert_cmpint(ufo_handle_free(id2), ==, 0);
    g_assert_cmpuint(ufo_handle_num_entries(), ==, 0);
    g_object_unref(a);
    g_object_unref(b);
}

void test_add_mex_handle(void) {
    g_test_add("/mex-handle/alloc",        void, NULL, setup, test_alloc_returns_nonzero, teardown);
    g_test_add("/mex-handle/lookup",       void, NULL, setup, test_lookup_returns_pointer, teardown);
    g_test_add("/mex-handle/free_lookup", void, NULL, setup, test_lookup_after_free_is_null, teardown);
    g_test_add_func("/mex-handle/unknown", test_lookup_unknown_returns_null);
    g_test_add("/mex-handle/double_free",  void, NULL, setup, test_double_free_detected, teardown);
    g_test_add_func("/mex-handle/alloc_fail", test_alloc_fail_sets_errno);
    g_test_add("/mex-handle/shrink",       void, NULL, setup, test_registry_shrinks_to_zero, teardown);
}
