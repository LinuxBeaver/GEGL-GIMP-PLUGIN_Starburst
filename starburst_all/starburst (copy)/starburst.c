/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_int (x, _("More or less blinds"), 30)
    description (_("Lesser makes more blinds. Higher makes less"))
    value_range (10, 55)
    ui_range    (10, 55)
    ui_gamma    (1.5)
    ui_meta     ("unit", "pixel-distance")
    ui_meta     ("axis", "x")


property_int (offset, _("Offset of blinds"), 0.0)
    description (_("Horizontal offset (from origin) for start of grid"))
    ui_range    (-128, 228)
    ui_meta     ("unit", "pixel-coordinate")
    ui_meta     ("axis", "x")


property_color  (color1, _("Color 1"), "#fdff00")
    description (_("The first starburst color"))
    ui_meta     ("role", "color-primary")

property_color  (color2, _("Color 2"), "#00d0ff")
    description (_("The second starburst color"))
    ui_meta     ("role", "color-secondary")





#else

#define GEGL_OP_META
#define GEGL_OP_NAME     starburst
#define GEGL_OP_C_SOURCE starburst.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *over, *crop, *blinds, *polar;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

  crop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);


  over    = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);


  blinds    = gegl_node_new_child (gegl,
                                  "operation", "gegl:blinds",
                                  NULL);

  polar    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzpolar-coordinates-for-starburst",
                                  NULL);






  gegl_node_link_many (input, over, crop, polar, output, NULL);
  gegl_node_link (input, blinds);
  gegl_node_connect_from (over, "aux", blinds, "output");

  gegl_node_connect_from (over, "aux", blinds, "output");

  gegl_operation_meta_redirect (operation, "x", blinds, "x");
  gegl_operation_meta_redirect (operation, "y", blinds, "y");
  gegl_operation_meta_redirect (operation, "offset", blinds, "x-offset");
  gegl_operation_meta_redirect (operation, "color1", blinds, "color1");
  gegl_operation_meta_redirect (operation, "color2", blinds, "color2");




}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:starburst",
    "title",       _("Starburst"),
    "categories",  "Artistic",
    "reference-hash", "45ed26360228agbr25493xc254001b2ac",
    "description", _("Make a starburst using GEGL "
                     ""),
    NULL);
}

#endif
