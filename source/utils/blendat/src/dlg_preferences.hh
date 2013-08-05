// generated 2000/8/3 3:02:13 CEST by nlingfx@geri.
// using glademm V0.5.9
//
// newer (non customized) versions of this file go to dlg_preferences.hh_glade

// you might replace
//    class Foo : public Foo_Glade { ... };
// by
//    typedef Foo_Glade Foo;
// if you didn't make any modifications to the widget

#ifndef _DLG_PREFERENCES_HH
#  include "dlg_preferences_glade.hh"
#  define _DLG_PREFERENCES_HH
#include "glademm_support.hh"

class Dlg_preferences : public Dlg_preferences_Glade
{   
        
        friend class Dlg_preferences_Glade;
        void on_btn_preferences_ok_clicked();
        void on_btn_preferences_cancel_clicked();
        void on_btn_saveprefs_clicked();
        void on_btn_find_blender_win_clicked();
        gint on_dlg_preferences_focus_in_event(GdkEventFocus *ev);
};
#endif
