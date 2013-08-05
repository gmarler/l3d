#ifndef _DIS_MSWN_H
#define _DIS_MSWN_H

#include "dispatch.h"
#include "../tool_2d/sc_mswin.h"

class l3d_dispatcher_mswin : public l3d_dispatcher {
 protected:
  static
   LRESULT CALLBACK winproc(HWND hWindow, UINT Message,
     WPARAM wp, LPARAM lp);
  static l3d_pipeline *last_pipeline;
  static l3d_event_source *last_event_source;

 public:
  void start(void);
  void set_pipeline(l3d_pipeline *p);
};

class l3d_dispatcher_factory_mswin : public l3d_dispatcher_factory {
 public:
  l3d_dispatcher *create(void) {
    return new l3d_dispatcher_mswin;
  }
};

#endif
