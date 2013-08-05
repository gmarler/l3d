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

// generated 2000/8/4 17:28:52 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// DO NOT EDIT THIS FILE ! It was created using
// glade-- /home/nlingfx/lib3d.C085/source/utils/blendat/blend_at.glade
// for gtk 1.2.8 and gtkmm 1.2.1
//
// Please modify the corresponding derived classes in ./src/win_main.cc

// we do not yet fully support gettext
#define _N(x) x
#define N_(x) x
#define _(x) x

#include "win_main.hh"

Win_main_Glade::Win_main_Glade(
) : Gtk::Window(GTK_WINDOW_TOPLEVEL)
{   win_main = this;
  save1 = new Gtk::MenuItem("Save");
  exit1 = new Gtk::MenuItem("Exit");
  file1_menu = new Gtk::Menu();
  file1 = new Gtk::MenuItem("File");
  properties1 = new Gtk::MenuItem("Properties");
  preferences1 = new Gtk::MenuItem("Preferences");
  edit1_menu = new Gtk::Menu();
  edit1 = new Gtk::MenuItem("Edit");
  menubar1 = new Gtk::MenuBar();
  toolbar1 = new Gtk::Toolbar(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
  btn_refresh = new Gtk::Button("Refresh");
  btn_save = new Gtk::Button("Save");
  btn_newemptyprop = new Gtk::Button("New Empty Property");
  btn_newfaceprop = new Gtk::Button("New Face Property");
  hbox4 = new Gtk::HBox(false, 0);
  hseparator1 = new Gtk::HSeparator();
  text_props = new Gtk::Text();
  scrolledwindow4 = new Gtk::ScrolledWindow();
  vbox4 = new Gtk::VBox(false, 0);
  label3 = new Gtk::Label("ASCII property display");
  text1 = new Gtk::Text();
  scrolledwindow5 = new Gtk::ScrolledWindow();
  label18 = new Gtk::Label("Help");
  notebook2 = new Gtk::Notebook();
  lbl_status = new Gtk::Label("Unmodified");
  lbl_id = new Gtk::Label("ID:");
  packer1 = new Gtk::Packer();
  hbox5 = new Gtk::HBox(false, 0);
  vbox1 = new Gtk::VBox(false, 0);
  save1->set_name("save1");
  save1->activate.connect(SigC::slot((Win_main*)this, &Win_main::on_save1_activate));
  exit1->set_name("exit1");
  exit1->activate.connect(SigC::slot((Win_main*)this, &Win_main::on_exit1_activate));
  file1_menu->set_name("file1_menu");
  file1_menu->append(*save1);
  file1_menu->append(*exit1);
  file1->set_name("file1");
  file1->set_submenu(*file1_menu);
  file1->activate.connect(SigC::slot((Win_main*)this, &Win_main::on_file1_activate));
  properties1->set_name("properties1");
  properties1->activate.connect(SigC::slot((Win_main*)this, &Win_main::on_properties1_activate));
  preferences1->set_name("preferences1");
  preferences1->activate.connect(SigC::slot((Win_main*)this, &Win_main::on_preferences1_activate));
  edit1_menu->set_name("edit1_menu");
  edit1_menu->append(*properties1);
  edit1_menu->append(*preferences1);
  edit1->set_name("edit1");
  edit1->set_submenu(*edit1_menu);
  edit1->activate.connect(SigC::slot((Win_main*)this, &Win_main::on_edit1_activate));
  menubar1->set_name("menubar1");
  menubar1->append(*file1);
  menubar1->append(*edit1);
  toolbar1->set_name("toolbar1");
  toolbar1->set_space_size(5);
  toolbar1->set_tooltips(true);
  btn_refresh->set_name("btn_refresh");
  btn_refresh->clicked.connect(SigC::slot((Win_main*)this, &Win_main::on_btn_refresh_clicked));
  btn_save->set_name("btn_save");
  btn_save->clicked.connect(SigC::slot((Win_main*)this, &Win_main::on_btn_save_clicked));
  btn_newemptyprop->set_name("btn_newemptyprop");
  _tooltips.set_tip(*btn_newemptyprop, "Create new empty property line", "");
  btn_newemptyprop->clicked.connect(SigC::slot((Win_main*)this, &Win_main::on_btn_newemptyprop_clicked));
  btn_newfaceprop->set_name("btn_newfaceprop");
  _tooltips.set_tip(*btn_newfaceprop, "Create new property lines based on currently selected faces", "");
  btn_newfaceprop->clicked.connect(SigC::slot((Win_main*)this, &Win_main::on_btn_newfaceprop_clicked));
  hbox4->set_name("hbox4");
  hbox4->pack_start(*btn_refresh, false, false, 0);
  hbox4->pack_start(*btn_save, false, false, 0);
  hbox4->pack_start(*btn_newemptyprop, false, false, 0);
  hbox4->pack_start(*btn_newfaceprop, false, false, 0);
  hseparator1->set_name("hseparator1");
  text_props->set_name("text_props");
  text_props->set_editable(true);
  text_props->set_word_wrap(false);
  text_props->set_point(0);
  text_props->changed.connect(SigC::slot((Win_main*)this, &Win_main::on_text_props_changed));
  scrolledwindow4->set_name("scrolledwindow4");
  scrolledwindow4->set_policy(GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  scrolledwindow4->add(*text_props);
  vbox4->set_name("vbox4");
  vbox4->pack_start(*scrolledwindow4);
  label3->set_name("label3");
  label3->set_alignment(0.5, 0.5);
  label3->set_padding(0, 0);
  text1->set_name("text1");
  text1->set_editable(false);
  text1->set_word_wrap(false);
  text1->set_point(0);
  {

    gint _dummy(0);
    text1->insert_text("Known Properties, Type SECTOR:\n"
                       "\n"
                       "NAME name : unique name\n"
                       "\n"
                       "FACE num PROP : Sets PROP for face number num. PROP can be IS_TEX_SPACE name image, or USES_TEX_SPACE name, to specify that a face defines or uses a texture space. If PROP is IS_INVISIBLE, the face only defines a texture space, and will not be written to the world file. Multiple property specifications for once face must be given on separate FACE lines.\n"
                       "\n"
                       "Known Properties, Type ACTOR:\n"
                       "\n"
                       "NAME name : unique name for this actor\n"
                       "\n"
                       "PARENT name : assigns this actor to sector &quot;name&quot;\n"
                       "\n"
                       "MESH vid.obj : sets the mesh for this actor to be vid.obj\n"
                       "\n"
                       "TEXCOORDS filename.uv : sets the filename for the file containing the mesh texture coordinates\n"
                       "\n"
                       "TEXTURE filename.ppm : sets the filename for the texture image used for this mesh (only one texture per mesh currently allowed)\n"
                       "\n"
                       "PLUGIN plugin.so : defines the name of the plug-in controlling the object's behavior\n"
                       "", 915, &_dummy);
  }
  scrolledwindow5->set_name("scrolledwindow5");
  scrolledwindow5->set_policy(GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  scrolledwindow5->add(*text1);
  label18->set_name("label18");
  label18->set_alignment(0.5, 0.5);
  label18->set_padding(0, 0);
  notebook2->set_usize(-1, 400);
  notebook2->set_name("notebook2");
  notebook2->set_show_tabs(true);
  notebook2->set_tab_border(3);
  notebook2->pages().push_back(Gtk::Notebook_Helpers::TabElem(*vbox4, *label3));
  notebook2->pages().push_back(Gtk::Notebook_Helpers::TabElem(*scrolledwindow5, *label18));
  lbl_status->set_name("lbl_status");
  lbl_status->set_alignment(0.5, 0.5);
  lbl_status->set_padding(0, 0);
  lbl_id->set_name("lbl_id");
  lbl_id->set_alignment(0.5, 0.5);
  lbl_id->set_padding(0, 0);
  packer1->set_name("packer1");
  packer1->add(*lbl_id, GTK_SIDE_TOP, GTK_ANCHOR_EAST, 0);
  hbox5->set_name("hbox5");
  hbox5->pack_start(*lbl_status, false, false, 0);
  hbox5->pack_start(*packer1);
  vbox1->set_name("vbox1");
  vbox1->pack_start(*menubar1, false, false, 0);
  vbox1->pack_start(*toolbar1, false, false, 0);
  vbox1->pack_start(*hbox4);
  vbox1->pack_start(*hseparator1, false, true, 4);
  vbox1->pack_start(*notebook2);
  vbox1->pack_start(*hbox5, false, false, 0);
  win_main->set_name("win_main");
  win_main->set_title("Blender Object Attributes Editor");
  win_main->add(*vbox1);
  win_main->focus_in_event.connect(SigC::slot((Win_main*)this, &Win_main::on_win_main_focus_in_event));
  save1->show();
  exit1->show();
  file1_menu->show();
  file1->show();
  properties1->show();
  preferences1->show();
  edit1_menu->show();
  edit1->show();
  menubar1->show();
  toolbar1->show();
  btn_refresh->show();
  btn_save->show();
  btn_newemptyprop->show();
  btn_newfaceprop->show();
  hbox4->show();
  hseparator1->show();
  text_props->show();
  scrolledwindow4->show();
  vbox4->show();
  label3->show();
  text1->show();
  scrolledwindow5->show();
  label18->show();
  notebook2->show();
  lbl_status->show();
  lbl_id->show();
  packer1->show();
  hbox5->show();
  vbox1->show();
  win_main->show();
  glademm_set_Widget("text_props", text_props);
}

Win_main_Glade::~Win_main_Glade()
{   delete save1;
  delete exit1;
  delete file1_menu;
  delete file1;
  delete properties1;
  delete preferences1;
  delete edit1_menu;
  delete edit1;
  delete menubar1;
  delete toolbar1;
  delete btn_refresh;
  delete btn_save;
  delete btn_newemptyprop;
  delete btn_newfaceprop;
  delete hbox4;
  delete hseparator1;
  delete text_props;
  delete scrolledwindow4;
  delete vbox4;
  delete label3;
  delete text1;
  delete scrolledwindow5;
  delete label18;
  delete notebook2;
  delete lbl_status;
  delete lbl_id;
  delete packer1;
  delete hbox5;
  delete vbox1;
}