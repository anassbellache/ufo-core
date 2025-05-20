#include <glib.h>
#include <ufo/ufo.h>
#include "test-suite.h"

static void test_cpu_env_fallback(void)
{
    // TODO: set environment variable UFO_DEVICE_TYPE=CPU
    // build a simple graph with a GPU-only filter and run
    // verify scheduler completes successfully on CPU
}

static void test_gpu_unavailable(void)
{
    // TODO: simulate unavailable GPU devices and run scheduler
    // expect fallback to CPU and warning logged
}

void test_add_opencl_fallback(void)
{
    g_test_add_func("/opencl/cpu_env", test_cpu_env_fallback);
    g_test_add_func("/opencl/gpu_unavailable", test_gpu_unavailable);
}

