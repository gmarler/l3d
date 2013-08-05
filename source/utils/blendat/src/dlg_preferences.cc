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

// generated 2000/8/2 22:39:00 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// newer (non customized) versions of this file go to dlg_preferences.cc_glade

// This file is for your program, I won't touch it again!

#include "dlg_preferences.hh"
#include "nl_blender_controller.h"
#include "nl_blender_config.h"
#include <stdio.h>
extern blender_controller *controller;

void Dlg_preferences::on_btn_preferences_ok_clicked()
{

  controller->config->blender_path = entry_blender_working_dir->get_text();
  controller->config->blender_work_filename = entry_blender_workfile->get_text();
  controller->config->db_path = entry_attrib_dir->get_text();
  controller->config->oid_filename = entry_oid_filename->get_text();

  //- find and create oid file if necessary
  char oid_fname[4096];
  sprintf(oid_fname, "%s/%s", controller->config->db_path.c_str(),
          controller->config->oid_filename.c_str());
  FILE *fp = fopen(oid_fname, "rt");
  if(!fp) {
    fp = fopen(oid_fname, "wt");
    fprintf(fp, "1\n");
  }
  fclose(fp);

  hide();
}

void Dlg_preferences::on_btn_preferences_cancel_clicked()
{
  hide();
}

void Dlg_preferences::on_btn_find_blender_win_clicked()
{
  controller->find_blender_window();
}

gint Dlg_preferences::on_dlg_preferences_focus_in_event(GdkEventFocus *ev)
{
  entry_blender_working_dir->set_text(controller->config->blender_path);
  entry_blender_workfile->set_text(controller->config->blender_work_filename);
  entry_attrib_dir->set_text(controller->config->db_path);
  entry_oid_filename->set_text(controller->config->oid_filename);

  return 0;
}

void Dlg_preferences::on_btn_saveprefs_clicked()
{

  on_btn_preferences_ok_clicked();

  FILE *fp;
  fp = fopen(".blend_at.config","wt");
  if(fp) {
    fprintf(fp, "%s\n", controller->config->blender_path.c_str());
    fprintf(fp, "%s\n", controller->config->blender_work_filename.c_str());
    fprintf(fp, "%s\n", controller->config->db_path.c_str());
    fprintf(fp, "%s\n", controller->config->oid_filename.c_str());
    fclose(fp);
  }
}

