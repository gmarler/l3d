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

#include "nl_blender_xcontroller.h"
#include "nl_vertex.h"
#include "nl_blender_config.h"

blender_xcontroller::blender_xcontroller(Display *dpy, blender_config *c) {
  this->dpy = dpy;
  config = c;
  find_blender_window();
}

blender_xcontroller::~blender_xcontroller(void) {
  delete config;
}

int blender_xcontroller::recursive_find_blender_window(Window w) {
  Window root_return, parent_return, *children;
  unsigned int nchildren;

  if(blender_w) return 1;

  XQueryTree(dpy, w, &root_return, &parent_return,
             &children, &nchildren);
  if(children==NULL) {
    return 0;
  } else {
    int i;
    for(i=0; i<nchildren && !blender_w;  i++) {
      char *wn;
      XFetchName(dpy, children[i], &wn);
      if(wn && strcmp(wn, "Blender")==0) {
        blender_w = children[i];
      } else {
        recursive_find_blender_window(children[i]);
      }
      XFree(wn);
    }
    XFree(children);
  }
}


void blender_xcontroller::find_blender_window(void)
{
  blender_w = 0;
  recursive_find_blender_window(DefaultRootWindow(dpy));
}

void blender_xcontroller::press_key(int code)
{
  XEvent event;
  unsigned long mask;

  event.type = KeyPress;
  event.xkey.type = KeyPress;
  event.xkey.serial = 30;
  event.xkey.send_event = FALSE;
  event.xkey.display = dpy;
  event.xkey.window = blender_w;
  event.xkey.root = DefaultRootWindow(dpy);
  event.xkey.subwindow = 0;
  event.xkey.time =1954543190;
  event.xkey.x = event.xkey.y = 0;
  event.xkey.x_root = event.xkey.y_root = 0;
  event.xkey.state = 0;
  event.xkey.keycode = code;
  event.xkey.same_screen= TRUE;
  mask = 1;
  XSendEvent(dpy, blender_w,
             TRUE, mask, &event);

  XFlush(dpy);
}

void blender_xcontroller::release_key(int code)
{
  XEvent event;
  unsigned long mask;

  event.type = KeyRelease;
  event.xkey.type = KeyRelease;
  event.xkey.serial = 30;
  event.xkey.send_event = FALSE;
  event.xkey.display = dpy;
  event.xkey.window = (Window) blender_w;
  event.xkey.root = DefaultRootWindow(dpy);
  event.xkey.subwindow = 0;
  event.xkey.time =1954543190;
  event.xkey.x = event.xkey.y = 0;
  event.xkey.x_root = event.xkey.y_root = 0;
  event.xkey.state = 0;
  event.xkey.keycode = code;
  event.xkey.same_screen= TRUE;
  mask = 1;
  XSendEvent(dpy, blender_w,
             TRUE, mask, &event);

  XFlush(dpy);
}

void blender_xcontroller::read_properties(void)
{
  Window root_return, child_return;
  int rootx, rooty, winx, winy;
  int rootx_new, rooty_new, winx_new, winy_new;
  unsigned int mask;

  // move the pointer into the blender window, since blender keystrokes
  // will not work properly unless the pointer is in the window

  //XGrabPointer(dpy, DefaultRootWindow(dpy), False, OwnerGrabButtonMask, GrabModeSync, GrabModeSync, None, None, 1);
  //XFlush(dpy);
  XQueryPointer(dpy, DefaultRootWindow(dpy), &root_return, &child_return,
                &rootx, &rooty, &winx, &winy, &mask);
  XWarpPointer(dpy, None, blender_w, 0, 0,
               0,0, 100,100);
  XQueryPointer(dpy, DefaultRootWindow(dpy), &root_return, &child_return,
                &rootx_new, &rooty_new, &winx_new, &winy_new, &mask);


  // send keystroke events to the blender window

  // write current mesh to videoscape file

  press_key(64); // alt
  press_key(25); // w
  release_key(25); // w
  release_key(64); // alt
  usleep(250000);
  press_key(36);// enter
  usleep(250000);
  press_key(36);// enter
  usleep(250000);
  press_key(36);// enter
  usleep(250000);
  press_key(36);// enter

  FILE *fp;

  unsigned long oid;

  vector<string> new_vidscape_strings;
  vector<vertex> vidscape_vertices;

  char oid_fname[1024];
  char attrib_fname[1024];
  sprintf(oid_fname,"%s/%s",
          config->db_path.c_str(),
          config->oid_filename.c_str());

  // increment the max-object id by one, in case we need to create a new
  // object
  fp=fopen(oid_fname, "rt");
  char oid_txt[80];
  fgets(oid_txt, 80, fp);
  fclose(fp);
  sscanf(oid_txt, "%lu", &oid);
  oid++;

  int num_bits_in_oid=0;
  unsigned long temp_oid = oid;
  while(temp_oid) {
    num_bits_in_oid++;
    temp_oid>>=1;
  }

  // find the videoscape file we just created

  char vidscape_line[1024];
  string vidscape_string;
  float float1, float2, float3;
  int found=0;

  char blender_work_fname[1024];
  sprintf(blender_work_fname,"%s/%s",
          config->blender_path.c_str(),
          config->blender_work_filename.c_str());
  fp = fopen(blender_work_fname, "rt");
  int lineno = 1;
  int num_vertices=0;

  float xsum=0.0, ysum=0.0, zsum=0.0;
  float xc,yc,zc;
  float x0_0,y0_0,z0_0  ,  x0_1,y0_1,z0_1,
  x1_0,y1_0,z1_0  ,  x1_1,y1_1,z1_1,
  x2_0,y2_0,z2_0  ,  x2_1,y2_1,z2_1;

  int prev_v_vtxcount=0, prev_v_vtx0_0, prev_v_vtx0_1;
  int overlapping_edge_count=0;

  while(!feof(fp) && !found) {
    fgets(vidscape_line, 1024, fp);
    if(!feof(fp)) {

#define VIDSC_VTXCOUNT_LINENO 2

      // code to average x,y,z over all vertices

      if(lineno>VIDSC_VTXCOUNT_LINENO &&
          lineno<=VIDSC_VTXCOUNT_LINENO + num_vertices)
      {
        vertex v;
        sscanf(vidscape_line, "%f %f %f", &float1,&float2,&float3);
        xsum += float1;
        ysum += float2;
        zsum += float3;

        v.x =float1;
        v.y =float2;
        v.z =float3;
        vidscape_vertices.push_back(v);
      }

      if(lineno == VIDSC_VTXCOUNT_LINENO) {

        // this edge contains the id
        sscanf(vidscape_line, "%d", &num_vertices);

        // add 4 the # of vertices, for our magic edges
        sprintf(vidscape_line, "%d\n", num_vertices +
                (4 + 3 + 2 + num_bits_in_oid)*2);
        // 4 new edges for overlapping start signal
        // 3 new edges for x,y,z axes
        // 2 new edges for x-axis marker
        // num_bits_in_oid new edges for each bit in oid
        // *2 b/c of start vtx,end vtx
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
      } else if (lineno == VIDSC_VTXCOUNT_LINENO + num_vertices) {
        // print last normal vertex
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);

        // last videoscape vertex index, we append (just in
        // memory for now) x new vertices, and later x new edges

        // calc. center of object
        xc = xsum/num_vertices;
        yc = ysum/num_vertices;
        zc = zsum/num_vertices;

        x0_0 = xc; y0_0 = yc; z0_0 = zc;
        x0_1 = xc + blender_config::BLENDER_MAGIC_X_SIZE; y0_1 = yc; z0_1 = zc;
        x1_0 = xc; y1_0 = yc; z1_0 = zc;
        x1_1 = xc; y1_1 = yc + blender_config::BLENDER_MAGIC_X_SIZE; z1_1 = zc;
        x2_0 = xc; y2_0 = yc; z2_0 = zc;
        x2_1 = xc; y2_1 = yc; z2_1 = zc + blender_config::BLENDER_MAGIC_X_SIZE;

        // 4 flag edges (start of transmission)
        for(int i_flagedge=0; i_flagedge<4; i_flagedge++) {
          sprintf(vidscape_line, "%f %f %f\n", x0_0, y0_0, z0_0);
          vidscape_string = vidscape_line;
          new_vidscape_strings.push_back(vidscape_string);
          sprintf(vidscape_line, "%f %f %f\n", x0_1, y0_1, z0_1);
          vidscape_string = vidscape_line;
          new_vidscape_strings.push_back(vidscape_string);
        }

        // flag edges (first edge, zero-orientation == x axis)
        sprintf(vidscape_line, "%f %f %f\n", x0_0, y0_0, z0_0);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
        sprintf(vidscape_line, "%f %f %f\n", x0_1, y0_1, z0_1);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);

        // flag edge (second edge, one-orientation == y axis)
        sprintf(vidscape_line, "%f %f %f\n", x1_0, y1_0, z1_0);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
        sprintf(vidscape_line, "%f %f %f\n", x1_1, y1_1, z1_1);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);

        // flag edge (third edge, no numerical orientation; z axis)
        sprintf(vidscape_line, "%f %f %f\n", x2_0, y2_0, z2_0);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
        sprintf(vidscape_line, "%f %f %f\n", x2_1, y2_1, z2_1);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);

        // x marker on x-axis (stroke 1 \ )
        sprintf(vidscape_line, "%f %f %f\n", x0_1,
                y0_1 - 0.5,
                z0_1 - 0.5);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
        sprintf(vidscape_line, "%f %f %f\n", x0_1,
                y0_1 + 0.5,
                z0_1 + 0.5);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);

        // x marker on x-axis (stroke 2 / )
        sprintf(vidscape_line, "%f %f %f\n", x0_1,
                y0_1 - 0.5,
                z0_1 + 0.5);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
        sprintf(vidscape_line, "%f %f %f\n", x0_1,
                y0_1 + 0.5,
                z0_1 - 0.5);
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);


        // calc. x new vertices for each bit of the oid
        unsigned long temp_oid=oid;
        while(temp_oid) {
          if(temp_oid&0x1) {
            // 1-bit orientation
            sprintf(vidscape_line, "%f %f %f\n", x1_0, y1_0, z1_0);
            //printf("1 encoding 1 %s\n", vidscape_line);
            vidscape_string = vidscape_line;
            new_vidscape_strings.push_back(vidscape_string);
            sprintf(vidscape_line, "%f %f %f\n", x1_1, y1_1, z1_1);
            //printf("2 encoding 1 %s\n", vidscape_line);
            vidscape_string = vidscape_line;
            new_vidscape_strings.push_back(vidscape_string);
          }
          else {
            // 0-bit orientation
            sprintf(vidscape_line, "%f %f %f\n", x0_0, y0_0, z0_0);
            //printf("1 encoding 0 %s\n", vidscape_line);
            vidscape_string = vidscape_line;
            new_vidscape_strings.push_back(vidscape_string);
            sprintf(vidscape_line, "%f %f %f\n", x0_1, y0_1, z0_1);
            //printf("2 encoding 0 %s\n", vidscape_line);
            vidscape_string = vidscape_line;
            new_vidscape_strings.push_back(vidscape_string);
          }
          temp_oid>>=1;
        }


      } else {
        vidscape_string = vidscape_line;
        new_vidscape_strings.push_back(vidscape_string);
      }


      int sc;
      int v_vtxcount, v_vtx0_0, v_vtx0_1;
      unsigned long v_color;
      char v_color_str[255];
      if((lineno > VIDSC_VTXCOUNT_LINENO + num_vertices + 1)
          && (sc=sscanf(vidscape_line,"%d %d %d %s",
                        &v_vtxcount, &v_vtx0_0, &v_vtx0_1, v_color_str)==4)
          && (v_vtxcount==2)
        ) // could also check dist. betw vertices
      {
        // we found a 2-vertex edge. does it overlap with the previuos one?
        // if so add it to the count, if not, re-init the count.

        if(prev_v_vtxcount==2
            &&((vidscape_vertices[v_vtx0_0]==vidscape_vertices[prev_v_vtx0_0]
                && vidscape_vertices[v_vtx0_1]==vidscape_vertices[prev_v_vtx0_1])
               ||(vidscape_vertices[v_vtx0_0]==vidscape_vertices[prev_v_vtx0_1]
                  && vidscape_vertices[v_vtx0_1]==vidscape_vertices[prev_v_vtx0_0]))
          ) {
          overlapping_edge_count++;
        } else {
          overlapping_edge_count=1;
        }

        prev_v_vtxcount = v_vtxcount;
        prev_v_vtx0_0 = v_vtx0_0;
        prev_v_vtx0_1 = v_vtx0_1;
      }

      if(overlapping_edge_count==4)
      {
        // we found the start-of-transmission flag (4 overlapping vertices)

        int x_v0, x_v1,
        y_v0, y_v1,
        z_v0, z_v1;

        // read x y and z axis orientations
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &v_vtxcount, &x_v0, &x_v1);
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &v_vtxcount, &y_v0, &y_v1);
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &v_vtxcount, &z_v0, &z_v1);

        // read in (discard) the x-marker for the x-axis
        int dummy1,dummy2,dummy3;
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &dummy1, &dummy2, &dummy3);
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &dummy1, &dummy2, &dummy3);

        found = 1;
        int done = 0;
        int shift=0;
        oid = 0;
        while(!feof(fp) && !done) {
          fgets(vidscape_line, 1024, fp);
          if(!feof(fp)) {
            sscanf(vidscape_line,
                   "%d %d %d",
                   &v_vtxcount, &v_vtx0_0, &v_vtx0_1);
            if(v_vtxcount != 2) {
              done=1;
            } else {
              // get the next bit of the oid



              // was this a "0-bit" edge (x), or a "1-bit" edge (y/z)?
              int bit = !
                        (((vidscape_vertices[v_vtx0_0].x==
                           vidscape_vertices[x_v0].x &&
                           vidscape_vertices[v_vtx0_0].y==
                           vidscape_vertices[x_v0].y &&
                           vidscape_vertices[v_vtx0_0].z==
                           vidscape_vertices[x_v0].z)
                          &&
                          (vidscape_vertices[v_vtx0_1].x==
                           vidscape_vertices[x_v1].x &&
                           vidscape_vertices[v_vtx0_1].y==
                           vidscape_vertices[x_v1].y &&
                           vidscape_vertices[v_vtx0_1].z==
                           vidscape_vertices[x_v1].z))
                         ||
                         ((vidscape_vertices[v_vtx0_0].x==
                           vidscape_vertices[x_v1].x &&
                           vidscape_vertices[v_vtx0_0].y==
                           vidscape_vertices[x_v1].y &&
                           vidscape_vertices[v_vtx0_0].z==
                           vidscape_vertices[x_v1].z)
                          &&
                          (vidscape_vertices[v_vtx0_1].x==
                           vidscape_vertices[x_v0].x &&
                           vidscape_vertices[v_vtx0_1].y==
                           vidscape_vertices[x_v0].y &&
                           vidscape_vertices[v_vtx0_1].z==
                           vidscape_vertices[x_v0].z)));
              oid |= bit<<shift;
              shift++;
              //printf("building oid %d\n",oid&0x1);
              //printf("building oid after %lu\n",oid);
            }
          }
        }
        sprintf(attrib_fname, "%s/%s%lu.dat",
                config->db_path.c_str(),
                blender_config::ATTRIB_FNAME,
                oid);
        this->current_id = oid;
        //      printf("object is already tagged with oid %lu", oid);
      }

      lineno++;
    }
  }
  fclose(fp);

  if(!found) {

    // append x magic edges to the geometry using the four
    // magic vertices we defined earlier

    // we first append three magic edges which define the x,y,z axes
    // (x axis also is zero-bit)

    // we then have a series of magic edges, each just 2 vertices
    // long, each either oriented zero-wise or one-wise. we build
    // up the oid bit for bit, until we hit a non-magic-edge
    // (more than 2 vertices).

    // the order of the edges is vital! do not rearrange the
    // edge order or remove doubles in blender.



    int magic_vtx_ctr;

    for(magic_vtx_ctr=0; magic_vtx_ctr < 4*2; magic_vtx_ctr+=2) {
      // 4 b/c 4 overlapping start-signal edges; *2 b/c 2 vertices per edge

      // this edge is a flag
      sprintf(vidscape_line, "2 %d %d 0x%x\n",
              num_vertices+magic_vtx_ctr, num_vertices+magic_vtx_ctr+1,
              0x0); // the color is ignored :-(
      vidscape_string=vidscape_line;
      new_vidscape_strings.push_back(vidscape_line);
    }

    int stop=magic_vtx_ctr + 3*2;
    // 3 b/c 3 axes; *2 b/c 2 vertices per edge
    for(; magic_vtx_ctr < stop; magic_vtx_ctr+=2) {

      // this edge is an axis
      sprintf(vidscape_line, "2 %d %d 0x%x\n",
              num_vertices+magic_vtx_ctr, num_vertices+magic_vtx_ctr+1,
              0x0); // the color is ignored :-(
      vidscape_string=vidscape_line;
      new_vidscape_strings.push_back(vidscape_line);
    }

    // x-marker on x-axis
    stop=magic_vtx_ctr + 2*2;
    // 2 b/c 2 strokes for x; *2 b/c 2 vertices per edge
    for(; magic_vtx_ctr < stop; magic_vtx_ctr+=2) {
      sprintf(vidscape_line, "2 %d %d 0x%x\n",
              num_vertices+magic_vtx_ctr, num_vertices+magic_vtx_ctr+1,
              0x0); // the color is ignored :-(
      vidscape_string=vidscape_line;
      new_vidscape_strings.push_back(vidscape_line);
    }

    unsigned long temp_oid = oid;
    while(temp_oid) {
      // this edge contains the id
      sprintf(vidscape_line, "2 %d %d 0x%x\n",
              num_vertices+magic_vtx_ctr,
              num_vertices+magic_vtx_ctr+1,
              0x0); // color is ignored :-(
      magic_vtx_ctr+=2;
      vidscape_string=vidscape_line;
      new_vidscape_strings.push_back(vidscape_line);
      temp_oid >>= 1;
    }

    // update the oid in the oid file
    fp=fopen(oid_fname, "wt");
    fprintf(fp, "%d\n", oid);
    fclose(fp);

    //    printf("object not yet tagged. creating new attributes file with id %lu\n", oid);

    // smuggle the oid information into the videoscape file

    fp=fopen(blender_work_fname, "wt");
    for(int i=0; i<new_vidscape_strings.size(); i++) {
      fprintf(fp,"%s", new_vidscape_strings[i].c_str());
    }
    fclose(fp);

    // clear memory string list which contained the new file we just wrote

    for(int i=0; i<new_vidscape_strings.size(); i++) {
      new_vidscape_strings.pop_back();
    }

    // make blender reload the videoscape file with the new tags; from
    // this point on the object has its oid encoded within blender!

    press_key(53); // x == delete object
    release_key(53); // x
    usleep(250000);
    press_key(36);// enter

    press_key(67); // f1 == load file
    release_key(67); // f1
    usleep(250000);
    press_key(36);// enter == default filename, MUST HAVE BEEN SETUP EARLER!
    usleep(250000);
    press_key(36);// enter
    usleep(250000);
    press_key(36);// enter
    usleep(250000);
    press_key(36);// enter


    // create an empty attributes file
    sprintf(attrib_fname, "%s/%s%lu.dat",
            config->db_path.c_str(),
            config->ATTRIB_FNAME,
            oid);
    fp = fopen(attrib_fname, "wt");
    fprintf(fp, "NAME OBJ%d\n", oid);
    fprintf(fp, "TYPE [SECTOR | ACTOR]\n");
    fprintf(fp, "PROPNAME VALUE [VALUE ...]\n");
    fclose(fp);

  }

  // move pointer back to original location
  XWarpPointer(dpy, None, None, 0, 0,
               0,0, rootx - rootx_new,rooty-rooty_new);
  XFlush(dpy);
  //XUngrabPointer(dpy, 2);

  // open the attributes file
  int old_linecount = property_lines.size();
  int i;
  for(i=0; i<old_linecount; i++) { property_lines.pop_back(); }

  strncpy(current_prop_filename, attrib_fname, sizeof(current_prop_filename));
  fp = fopen(attrib_fname, "rt");

  if(fp) {
    char attrib_line[4096];
    while(!feof(fp)) {
      fgets(attrib_line, sizeof(attrib_line), fp);
      if(feof(fp)) break;
      property_lines.push_back(attrib_line);
    }
    fclose(fp);
  } else {
    printf("error opening %s\n", attrib_fname);
  }
}

void blender_xcontroller::determine_selected_face_nums(void)
{
  int i;

  // erase previous selection list
  int oldsize = this->selected_faces.size();
  for(i=0; i<oldsize; i++) {
    this->selected_faces.pop_back();
  }


  Window root_return, child_return;
  int rootx, rooty, winx, winy;
  int rootx_new, rooty_new, winx_new, winy_new;
  unsigned int mask;

  // move the pointer into the blender window, since blender keystrokes
  // will not work properly unless the pointer is in the window

  //XGrabPointer(dpy, DefaultRootWindow(dpy), False, OwnerGrabButtonMask, GrabModeSync, GrabModeSync, None, None, 1);
  //XFlush(dpy);
  XQueryPointer(dpy, DefaultRootWindow(dpy), &root_return, &child_return,
                &rootx, &rooty, &winx, &winy, &mask);
  XWarpPointer(dpy, None, blender_w, 0, 0,
               0,0, 100,100);
  XQueryPointer(dpy, DefaultRootWindow(dpy), &root_return, &child_return,
                &rootx_new, &rooty_new, &winx_new, &winy_new, &mask);


  // send keystroke events to the blender window

  // write current mesh to videoscape file

  press_key(58); // m (move to level)
  release_key(58);
  usleep(250000);
  press_key(19); // 0  (level 10)
  usleep(250000);
  release_key(19); // 0
  usleep(250000);
  press_key(36);// enter
  usleep(250000);

  // object is now in level 10
  press_key(19); // 0  (level 10)
  usleep(250000);
  release_key(19); // 0
  usleep(250000);

  // we are now in level 20
  press_key(50); // shift
  usleep(250000);
  press_key(40); // D (duplicate)
  usleep(250000);
  release_key(50); // shift
  release_key(40);
  press_key(36);// enter
  usleep(250000);
  press_key(33); // p
  usleep(250000);
  release_key(33); // p
  press_key(36);// enter
  usleep(250000);
  press_key(58); // m (move to level)
  usleep(250000);
  press_key(18); // 9  (level 9)
  release_key(18); // 9
  usleep(250000);
  press_key(36);// enter
  usleep(250000);

  // now only the duplicated face is in level 10. write it to a file.
  press_key(23);// TAB (exit editmode)
  press_key(38);// a (select all)
  press_key(64); // alt
  usleep(250000);
  press_key(25); // w  (write)
  usleep(250000);
  release_key(25); // w
  release_key(64); // alt
  usleep(250000);
  press_key(36);// enter
  usleep(250000);
  press_key(36);// enter

  // wait a while to let blender write the file
  usleep(250000);
  usleep(250000);
  usleep(250000);
  usleep(250000);

  // delete single face
  press_key(53);// x (delete)
  usleep(250000);
  press_key(36);// enter

  // read in vertices of the single face we just wrote
  int num_face_vertices;
  vector<vertex> face_vertices;
  typedef vector<int> facedef;
  vector<facedef> selected_faces;
  int num_selected_faces;
  num_selected_faces = 0;

  FILE *fp;
  char blender_work_fname[1024];
  char line[4096];
  sprintf(blender_work_fname,"%s/%s",
          config->blender_path.c_str(),
          config->blender_work_filename.c_str());
  fp = fopen(blender_work_fname, "rt");
  fgets(line, sizeof(line), fp); // header line 3DG1
  fgets(line, sizeof(line), fp); // num vertices
  sscanf(line, "%d", &num_face_vertices);

  for(i=0; i<num_face_vertices; i++) {
    vertex v;
    fgets(line, sizeof(line), fp);
    sscanf(line, "%f %f %f",
           &v.x,
           &v.y,
           &v.z);
    face_vertices.push_back(v);
  }
  while(!feof(fp)) {
    fgets(line, sizeof(line), fp);
    if(feof(fp)) break;

    char *tok;
    tok = strtok(line," \t\r\n");
    int numv;
    sscanf(tok, "%d", &numv);

    facedef a_facedef;
    a_facedef.push_back(numv);
    int iv;
    for(iv=0; iv<numv; iv++) {
      tok = strtok(NULL, " \t\r\n");
      int ivertex;
      sscanf(tok, "%d", &ivertex);
      a_facedef.push_back(ivertex);
    }
    selected_faces.push_back(a_facedef);
    num_selected_faces++;
  }
  fclose(fp);

  // change to level 9
  press_key(18); // 9  (level 9)
  release_key(18); // 9
  usleep(250000);

  // write original mesh to a file
  press_key(38);// a (deselect all)
  usleep(250000);
  press_key(38);// a (select all)
  usleep(250000);
  press_key(64); // alt
  usleep(250000);
  press_key(25); // w  (write)
  usleep(250000);
  release_key(25); // w
  release_key(64); // alt
  usleep(250000);
  press_key(36);// enter
  usleep(250000);
  press_key(36);// enter

  // move mesh back to level 1
  press_key(58); // m (move to level)
  release_key(58);
  usleep(250000);
  press_key(10); // 1  (level 1)
  usleep(250000);
  release_key(10); // 1
  usleep(250000);
  press_key(36);// enter
  usleep(250000);

  // change to level 1
  press_key(10); // 1  (level 1)
  release_key(18); // 1
  usleep(250000);

  // move pointer back to original location
  XWarpPointer(dpy, None, None, 0, 0,
               0,0, rootx - rootx_new,rooty-rooty_new);
  XFlush(dpy);
  //XUngrabPointer(dpy, 2);

  usleep(250000);
  usleep(250000);
  usleep(250000);
  usleep(250000);
  // wait a while to let blender write the file

  // read in complete mesh file and find face which overlaps with the
  // selected faces we just extracted.
  int num_mesh_vertices;
  vector<vertex> mesh_vertices;
  sprintf(blender_work_fname,"%s/%s",
          config->blender_path.c_str(),
          config->blender_work_filename.c_str());
  fp = fopen(blender_work_fname, "rt");
  fgets(line, sizeof(line), fp); // header line 3DG1
  fgets(line, sizeof(line), fp); // num vertices
  sscanf(line, "%d", &num_mesh_vertices);
  for(i=0; i<num_mesh_vertices; i++) {
    vertex v;
    fgets(line, sizeof(line), fp);
    sscanf(line, "%f %f %f",
           &v.x,
           &v.y,
           &v.z);
    mesh_vertices.push_back(v);
  }

  int found_overlapping_face;
  int face_num;
  face_num = 0;
  while(!feof(fp)) {
    fgets(line, sizeof(line), fp);
    if(feof(fp)) break;

    int selface;
    char orig_line[4096];
    strncpy(orig_line, line, sizeof(orig_line));
    for(selface=0; selface<num_selected_faces; selface++) {
      strncpy(line, orig_line, sizeof(line));

      char *tok;
      tok = strtok(line, " \t\r\n");

      int num_v;
      int iv;
      sscanf(tok, "%d", &num_v);

      int found_overlapping_vertex;
      for(iv=0; iv<num_v; iv++) {
        tok = strtok(NULL, " \t\r\n");
        int ivertex;
        sscanf(tok, "%d", &ivertex);

        found_overlapping_vertex = 0;
        int fv;
        for(fv=1; fv<=selected_faces[selface][0]; fv++) {
          //        printf("comparing match ivertex %d at %f %f %f with facev %d at %f %f %f\n",
          //          ivertex,
          //          mesh_vertices[ivertex].x,
          //          mesh_vertices[ivertex].y,
          //          mesh_vertices[ivertex].z,
          //          fv,
          //          face_vertices[fv].x,
          //          face_vertices[fv].y,
          //          face_vertices[fv].z
          //        );
          if ( mesh_vertices[ivertex] ==
               face_vertices[selected_faces[selface][fv]] )
          {
            found_overlapping_vertex = 1;
            break;
          }
        }
        if(!found_overlapping_vertex) {
          //        printf("face %d failed, couldnt match ivertex %d at %f %f %f\n",
          //          face_num, ivertex,
          //          mesh_vertices[ivertex].x,
          //          mesh_vertices[ivertex].y,
          //          mesh_vertices[ivertex].z
          //        );
          break;
        }
      }
      if(found_overlapping_vertex) {
        this->selected_faces.push_back(face_num);
        break;
      }
    }
    face_num++;
  }
  fclose(fp);
}


