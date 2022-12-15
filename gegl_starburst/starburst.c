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

property_color (col, _("Color"), "#01ff00")
    ui_meta     ("role", "color-primary")


property_color (col2, _("Color"), "#4f9500")

    ui_meta     ("role", "color-secondary")



property_double (m_angle, _("Size of blinds"), 85.0)
    description (_("Size of blinds"))
    value_range (79.0, 87.0)
    ui_meta     ("unit", "degree")
    ui_meta     ("direction", "cw")

property_double (r_angle, _("Rotation of starburst"), 0.0)
    description (_("Rotation applied to the result"))
    value_range (0.0, 360.0)
    ui_meta     ("unit", "degree")
    ui_meta     ("direction", "cw")

property_int    (n_segs, _("Amount of blinds"), 18)
    description (_("Number of blinds to use"))
    value_range (2, 24)

property_double (c_x, _("Offset X"), 0.5)
    description (_("position of symmetry center in output"))
    value_range (0.0, 1.0)
    ui_meta     ("unit", "relative-coordinate")
    ui_meta     ("axis", "x")

property_double (c_y, _("Offset Y"), 0.5)
    description (_("position of symmetry center in output"))
    value_range (0.0, 1.0)
    ui_meta     ("unit", "relative-coordinate")
    ui_meta     ("axis", "y")

property_int  (radius, _("Remove the 1 pixel line artifact"), 2)
  value_range (0, 2)
  ui_range    (0, 2)
  ui_meta     ("unit", "pixel-distance")
  description (_("It is on by default but you can turn it off to make the filter faster"))


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     starburst
#define GEGL_OP_C_SOURCE starburst.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *over, *crop, *mirrors, *dst, *col, *col2, *mb, *cropx;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

  crop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);

  cropx    = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);



  mirrors    = gegl_node_new_child (gegl,
                                  "operation", "gegl:mirrors",
                                  NULL);


  over    = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);


  dst    = gegl_node_new_child (gegl,
                                  "operation", "gegl:dst-atop",
                                  NULL);

  col    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color",
                                  NULL);
  col2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color",
                                  NULL);

 mb    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);



  gegl_node_link_many (input, over, crop, mirrors, dst, cropx, mb, output, NULL);
 gegl_node_link (input, col);
 gegl_node_link (input, col2);
  gegl_node_connect_from (over, "aux", col, "output");
  gegl_node_connect_from (dst, "aux", col2, "output");



  gegl_operation_meta_redirect (operation, "col", col, "value");
  gegl_operation_meta_redirect (operation, "col2", col2, "value"); 
  gegl_operation_meta_redirect (operation, "r_angle", mirrors, "r-angle"); 
  gegl_operation_meta_redirect (operation, "m_angle", mirrors, "m-angle"); 
  gegl_operation_meta_redirect (operation, "n_segs", mirrors, "n-segs"); 
  gegl_operation_meta_redirect (operation, "c_x", mirrors, "c-x");    
  gegl_operation_meta_redirect (operation, "c_y", mirrors, "c-y");   
  gegl_operation_meta_redirect (operation, "radius", mb, "radius");     






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
    "reference-hash", "45ed16360128agbr25493xc254001b2ac",
    "description", _("Make a starburst using GEGL "
                     ""),
    NULL);
}

#endif
