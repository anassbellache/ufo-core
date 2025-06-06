/*
 * Copyright (C) 2011-2013 Karlsruhe Institute of Technology
 *
 * This file is part of Ufo.
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <glib-object.h>
#include "test-suite.h"

static void
ignore_log (const gchar     *domain,
            GLogLevelFlags   flags,
            const gchar     *message,
            gpointer         data)
{
}

int main(int argc, char *argv[])
{
#if !(GLIB_CHECK_VERSION (2, 36, 0))
    g_type_init ();
#endif

    g_test_init (&argc, &argv, NULL);
    g_test_bug_base ("https://github.com/ufo-kit/ufo-core/issues");

    g_log_set_handler ("Ufo", G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, ignore_log, NULL);
    g_log_set_handler ("ocl", G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, ignore_log, NULL);

    test_add_buffer ();
    test_add_graph ();
    test_add_profiler ();
    test_add_node ();
    test_add_max_input_nodes();
    test_add_opencl_fallback();
    test_add_mex_handle();

    g_test_run();

    return 0;
}
