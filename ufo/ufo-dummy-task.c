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

#include <gmodule.h>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "ufo-dummy-task.h"

/**
 * SECTION:ufo-dummy-task
 * @Short_description: A dummy task that does nothing
 * @Title: UfoDummyTask
 *
 * A dummy task that can be inserted into a #UfoTaskGraph but does not do
 * anything.
 */

static void ufo_task_interface_init (UfoTaskIface *iface);

G_DEFINE_TYPE_WITH_CODE (UfoDummyTask, ufo_dummy_task, UFO_TYPE_TASK_NODE,
                         G_IMPLEMENT_INTERFACE (UFO_TYPE_TASK,
                                                ufo_task_interface_init))

#define UFO_DUMMY_TASK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_DUMMY_TASK, UfoDummyTaskPrivate))

enum {
    PROP_0,
    N_PROPERTIES
};

UfoNode *
ufo_dummy_task_new (void)
{
    return UFO_NODE (g_object_new (UFO_TYPE_DUMMY_TASK, NULL));
}

static void
ufo_dummy_task_setup (UfoTask *task,
                      UfoResources *resources,
                      GError **error)
{
}

static void
ufo_dummy_task_get_requisition (UfoTask *task,
                                UfoBuffer **inputs,
                                UfoRequisition *requisition,
                                GError **error)
{
}

static guint
ufo_dummy_task_get_num_inputs (UfoTask *task)
{
    return 0;
}

static guint
ufo_dummy_task_get_num_dimensions (UfoTask *task,
                                   guint input)
{
    return 0;
}

static UfoTaskMode
ufo_dummy_task_get_mode (UfoTask *task)
{
    return UFO_TASK_MODE_PROCESSOR | UFO_TASK_MODE_CPU;
}

static void
ufo_task_interface_init (UfoTaskIface *iface)
{
    iface->setup = ufo_dummy_task_setup;
    iface->get_num_inputs = ufo_dummy_task_get_num_inputs;
    iface->get_num_dimensions = ufo_dummy_task_get_num_dimensions;
    iface->get_mode = ufo_dummy_task_get_mode;
    iface->get_requisition = ufo_dummy_task_get_requisition;
}

static void
ufo_dummy_task_class_init (UfoDummyTaskClass *klass)
{
}

static void
ufo_dummy_task_init (UfoDummyTask *task)
{
    ufo_task_node_set_plugin_name (UFO_TASK_NODE (task), "[dummy]");
}
