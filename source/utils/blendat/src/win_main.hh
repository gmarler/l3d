// generated 2000/8/3 3:04:29 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// newer (non customized) versions of this file go to win_main.hh_glade

// you might replace
//    class Foo : public Foo_Glade { ... };
// by
//    typedef Foo_Glade Foo;
// if you didn't make any modifications to the widget

#ifndef _WIN_MAIN_HH
#  include "win_main_glade.hh"
#  define _WIN_MAIN_HH
#include "glademm_support.hh"

class Win_main : public Win_main_Glade
{   
        
        friend class Win_main_Glade;
        void on_save1_activate();
        void on_exit1_activate();
        void on_file1_activate();
        void on_properties1_activate();
        void on_preferences1_activate();
        void on_edit1_activate();
        void on_btn_refresh_clicked();
        void on_btn_save_clicked();
        void on_btn_newemptyprop_clicked();
        void on_btn_newfaceprop_clicked();
        void on_text_props_changed();
        gint on_win_main_focus_in_event(GdkEventFocus *ev);
};
#endif
