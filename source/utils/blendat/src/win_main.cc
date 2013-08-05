// L3D realtime 3D library, explained in book "Linux 3D Graphics Programming"
// Copyright (C) 2000  Norman Lin
// Contact: nlin@linux3dgraphicsprogramming.org (alt. nlin@geocities.com)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

// generated 2000/8/3 1:12:05 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// newer (non customized) versions of this file go to win_main.cc_glade

// This file is for your program, I won't touch it again!

#include "win_main.hh"
#include "nl_blender_controller.h"
#include "dlg_preferences.hh"
#include <stdio.h>
extern blender_controller *controller;
extern Dlg_preferences *dlg_preferences;
#include "dlg_preferences.hh"
#include <gtk/gtkmain.h>

void Win_main::on_save1_activate()
{
  gchar *data;
  FILE *fp;
  gint bytes_written, len;

  data = gtk_editable_get_chars (GTK_EDITABLE (text_props->gtkobj()), 0, -1);

  fp = fopen (controller->current_prop_filename, "w");
  if (fp == NULL)
  {
    g_free (data);
    return;
  }

  len = strlen (data);
  bytes_written = fwrite (data, sizeof (gchar), len, fp);
  fclose (fp);

  if(len == bytes_written) {
    lbl_status->set("Unmodified");
  } else {
    lbl_status->set("WARNING: incomplete save");
  }

  g_free (data);

}

void Win_main::on_exit1_activate()
{
  gtk_main_quit();
}

void Win_main::on_file1_activate()
{
}

void Win_main::on_properties1_activate()
{
  controller->read_properties();

  Gtk::Text::Context cx;
  Gdk_Font fixed_font("-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*");
  cx.set_font(fixed_font);

  text_props->set_point(0);
  text_props->forward_delete(text_props->get_length());

  int i;
  for(i=0; i<controller->property_lines.size(); i++) {
    text_props->insert ( cx, controller->property_lines[i] );
  }
  text_props->thaw ();
  char id[256];
  sprintf(id,"ID: %d", controller->current_id);
  lbl_id->set(id);
}

void Win_main::on_preferences1_activate()
{
  dlg_preferences->show();
}

void Win_main::on_edit1_activate()
{
}

void Win_main::on_btn_refresh_clicked()
{
  on_properties1_activate();
}

void Win_main::on_btn_save_clicked()
{
  on_save1_activate();
}

void Win_main::on_btn_newemptyprop_clicked()
{
  Gtk::Text::Context cx;
  Gdk_Font fixed_font("-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*");
  cx.set_font(fixed_font);
  text_props->set_point(text_props->get_length());
  text_props->insert ( cx, "PROPNAME VALUE [VALUE...]\n");
  text_props->thaw ();
}

void Win_main::on_btn_newfaceprop_clicked()
{
  text_props->set_point(text_props->get_length());
  Gtk::Text::Context cx;
  Gdk_Font fixed_font("-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*");
  cx.set_font(fixed_font);

  controller->determine_selected_face_nums();
  int i;
  for(i=0; i<controller->selected_faces.size(); i++) {
    char buf[1024];
    sprintf(buf,"FACE %d [IS_TEX_SPACE name image.ppm | USES_TEX_SPACE name | IS_INVISIBLE | ...]\n",
            controller->selected_faces[i]);
    text_props->insert ( cx, buf);
  }
  text_props->thaw ();

}

gint Win_main::on_win_main_focus_in_event(GdkEventFocus *ev)
{   return 0;
}

void Win_main::on_text_props_changed()
{
  lbl_status->set("Modified");
}
