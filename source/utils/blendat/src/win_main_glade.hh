// generated 2000/8/4 17:28:52 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// DO NOT EDIT THIS FILE ! It was created using
// glade-- /home/nlingfx/lib3d.C085/source/utils/blendat/blend_at.glade
// for gtk 1.2.8 and gtkmm 1.2.1
//
// Please modify the corresponding derived classes in ./src/win_main.hh and./src/win_main.cc

#ifndef _WIN_MAIN_GLADE_HH
#  define _WIN_MAIN_GLADE_HH

#include <gtk--/window.h>
#include <gtk--/tooltips.h>
#include <gtk--/accelgroup.h>
#include <gtk--/list.h>
#include <gtk--/menu.h>
#include <gtk--/menubar.h>
#include <gtk--/toolbar.h>
#include <gtk--/button.h>
#include <gtk--/box.h>
#include <gtk--/separator.h>
#include <gtk--/text.h>
#include <gtk--/scrolledwindow.h>
#include <gtk--/label.h>
#include <gtk--/notebook.h>
#include <gtk--/packer.h>

class Win_main_Glade : public Gtk::Window
{   
        
        Gtk::Tooltips _tooltips;
        Gtk::Window *win_main;
        Gtk::MenuItem *save1;
        Gtk::MenuItem *exit1;
        Gtk::Menu *file1_menu;
        Gtk::MenuItem *file1;
        Gtk::MenuItem *properties1;
        Gtk::MenuItem *preferences1;
        Gtk::Menu *edit1_menu;
        Gtk::MenuItem *edit1;
        Gtk::MenuBar *menubar1;
        Gtk::Toolbar *toolbar1;
        Gtk::Button *btn_refresh;
        Gtk::Button *btn_save;
        Gtk::Button *btn_newemptyprop;
        Gtk::Button *btn_newfaceprop;
        Gtk::HBox *hbox4;
        Gtk::HSeparator *hseparator1;
public:
        Gtk::Text *text_props;
private:
        Gtk::ScrolledWindow *scrolledwindow4;
        Gtk::VBox *vbox4;
        Gtk::Label *label3;
        Gtk::Text *text1;
        Gtk::ScrolledWindow *scrolledwindow5;
        Gtk::Label *label18;
        Gtk::Notebook *notebook2;
protected:
        Gtk::Label *lbl_status;
        Gtk::Label *lbl_id;
private:
        Gtk::Packer *packer1;
        Gtk::HBox *hbox5;
        Gtk::VBox *vbox1;
protected:
        
        Win_main_Glade();
        
        ~Win_main_Glade();
};
#define GMM_TEXT_PROPS (glademm_get<Gtk::Text>("text_props"))
#endif
