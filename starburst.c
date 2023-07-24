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
 * 2022 Beaver Starburst
 */

/*
June 25 2023 - Recreation of GEGL Graph - not complete but good enough for testing. *july 12 2023 update dst-over instead of dst-atop

id=1
over aux=[ color value=#00ff48 ]
crop
mirrors m-angle=77 c-x=2 c-y=2
id=2
dst-over aux=[ ref=2 color value=#ffa700 ]

--
THIS IS A GOOD PLUGIN TO STUDY IF YOU WANT TO LEARN HOW TO MAKE GEGL FILTERS.
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

/*This is where we define GUI Options. It corresponds to line 156-163*/

property_color (col, _("Color of blinds"), "#01ff00")
    description (_("Color of the starburst blinds"))
    ui_meta     ("role", "color-primary")


property_color (col2, _("Color of fill"), "#4f9500")
    description (_("Sliding this color's alpha channel to transparent by left clicking the color fill and sliding the A slider to 0 will only preserve the ray."))
    ui_meta     ("role", "color-secondary")


property_double (m_angle, _("Size of blinds (wider to thin)"), 85.0)
    description (_("Size of blinds, wider to thin"))
    value_range (79.0, 87.0)
    ui_meta     ("unit", "degree")
    ui_meta     ("direction", "cw")

property_double (r_angle, _("Rotation of starburst"), 0.0)
    description (_("Rotation applied to the starburst"))
    value_range (0.0, 360.0)
    ui_meta     ("unit", "degree")
    ui_meta     ("direction", "cw")

property_int    (n_segs, _("Amount of blinds"), 18)
    description (_("Number of blinds to use"))
    value_range (2, 24)

property_double (c_x, _("Horizontal (X) movement"), 0.5)
    description (_("Move the starburst horizontal"))
    value_range (0.0, 1.0)
    ui_meta     ("unit", "relative-coordinate")
    ui_meta     ("axis", "x")

property_double (c_y, _("Vertical (Y) movement"), 0.5)
    description (_("Move the starburst vertical"))
    value_range (0.0, 1.0)
    ui_meta     ("unit", "relative-coordinate")
    ui_meta     ("axis", "y")

property_int  (radius, _("Remove the 1 pixel line artifact"), 2)
  value_range (0, 2)
  ui_range    (0, 2)
  ui_meta     ("unit", "pixel-distance")
  description (_("Median blur to smooth one pixel line artifact. It is on by default but you can turn it off to make the filter faster"))


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     starburst
#define GEGL_OP_C_SOURCE starburst.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
/*This is where we define Operation names.. It corresponds to line 155-162 All the nodes have to be listed twice in both the order of *node, *node2, and
  node    = gegl_node_new_child (gegl,
                                  "operation", "gegl:node",
                                  NULL);  */
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
                                  "operation", "gegl:dst-over",
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

/*GEGL Crop is used to solve a potential bug that gegl:color has. It is a good faith practice to put them after gegl:color or any gegl render operation.*/

/*This is a GEGL Graph of all the filters being called, normal blend mode, crop, kaleidoscope, behind blend mode, crop(again), median blur,*/
 gegl_node_link_many (input, over, crop, mirrors, dst, cropx, mb, output, NULL);
/*Over is the NORMAL blend mode. The first color fill is blended with this. */
 gegl_node_connect_from (over, "aux", col, "output");
/*DST is the behind blend mode. The second color fill is blended with this. */
 gegl_node_connect_from (dst, "aux", col2, "output");



/*The three parts after (operation, are "defined_GUI_options", defined_operation_name, "operation_property");
Operation properties (3rd) are the only objective thing here. That are based on a GEGL Operations property, IE gaussian-blur "std-dev-x" is the property. Users are free to enter whatever name they want for GUI Options and operation names (except "gegl" and "switch") for some reason are not allowed. there may be a few more oddball rejected names, I haven't found*/

  gegl_operation_meta_redirect (operation, "col", col, "value");
  gegl_operation_meta_redirect (operation, "col2", col2, "value"); 
  gegl_operation_meta_redirect (operation, "r_angle", mirrors, "r-angle"); 
  gegl_operation_meta_redirect (operation, "m_angle", mirrors, "m-angle"); 
  gegl_operation_meta_redirect (operation, "n_segs", mirrors, "n-segs"); 
  gegl_operation_meta_redirect (operation, "c_x", mirrors, "c-x");    
  gegl_operation_meta_redirect (operation, "c_y", mirrors, "c-y");   
  gegl_operation_meta_redirect (operation, "radius", mb, "radius");     
/*I'm not happy with them names r-angle, m-angle ect.. set in mid 2022 but I don't want to change it as that will break presets.. These names were given by gegl:kaleidoscope in default. */
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
    "gimp:menu-path", "<Image>/Filters/Render/Fun",
    "gimp:menu-label", _("Starburst..."),
    NULL);
}

#endif
