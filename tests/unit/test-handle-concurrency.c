#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "mexUfo_handle.h"

/* CC-02: allocate/free handles under multiple threads */
static void test_parallel_alloc_free(void **state) {
    (void) state;
    // TODO: spawn threads that allocate and free handles in parallel
    // using cmocka_run_group_tests_parallel; after all operations the
    // handle table should contain zero entries.
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parallel_alloc_free),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
