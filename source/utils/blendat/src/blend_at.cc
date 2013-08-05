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

// generated 2000/7/30 18:20:42 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// newer (non customized) versions of this file go to blend_at.cc_glade

// This file is for your program, I won't touch it again!

#include <gtk--/main.h>

#include "win_main.hh"
#include "dlg_preferences.hh"

#include "nl_blender_controller.h"
#include "nl_blender_xcontroller.h"
#include "nl_blender_config.h"

blender_controller *controller;
Dlg_preferences *dlg_preferences;

int main(int argc, char **argv)
{

  //
  Display *dpy;
  dpy = XOpenDisplay(NULL);
  FILE *fp;

  blender_config *config = new blender_config;
  config->blender_path = "/tmp";
  config->blender_work_filename = "tmp.obj";
  config->db_path = "./";
  config->oid_filename = "oid.txt";

  fp = fopen("~/.blend_at.config","rt");
  if(!fp) {
    fp = fopen(".blend_at.config","rt");
  }
  if(fp) {
    char line[256];
    char *tok;

    fgets(line, sizeof(line), fp);
    tok = strtok(line, " \t\r\n");
    config->blender_path = tok;

    fgets(line, sizeof(line), fp);
    tok = strtok(line, " \t\r\n");
    config->blender_work_filename = tok;

    fgets(line, sizeof(line), fp);
    tok = strtok(line, " \t\r\n");
    config->db_path = tok;

    fgets(line, sizeof(line), fp);
    tok = strtok(line, " \t\r\n");
    config->oid_filename = tok;
    fclose(fp);
  }

  //- find and create oid file if necessary
  char oid_fname[4096];
  sprintf(oid_fname, "%s/%s", config->db_path.c_str(),
          config->oid_filename.c_str());
  fp = fopen(oid_fname, "rt");
  if(!fp) {
    fp = fopen(oid_fname, "wt");
    fprintf(fp, "1\n");
  }
  fclose(fp);
  controller = new blender_xcontroller(dpy, config);
  //

  Gtk::Main m(&argc, &argv);

  Win_main *win_main = new Win_main();

  dlg_preferences = new Dlg_preferences();
  m.run();
  delete win_main;
  delete dlg_preferences;

  //
  delete controller;
  return 0;
}
