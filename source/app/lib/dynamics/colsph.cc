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

#include "colsph.h"
#include "../geom/plane/plane.h"
#include "../geom/polygon/p3_coll.h"
#include "../tool_os/memman.h"

int l3d_collidable_sphere::detect_collision_with(l3d_collidable *target)
{
  l3d_plane *plane;
  l3d_polygon_3d_collidable *poly;
  l3d_collidable_sphere *sphere;


  if( (plane=dynamic_cast<l3d_plane *>(target)) ) {
    // sphere-plane test
    l3d_real dist;

    // we hopped clear across the plane between old position and
    // new position. so, set position to be flush along the inside
    // of the plane. flush along means the position is the intersection
    // point plus the sphere's radius along the plane normal.

    int status;
    status = plane->intersect_with_segment(old_position, new_position);

    if (// status &&
      //      plane->intersection_t <= int_to_l3d_real(1) &&
      //      plane->intersection_t >= 0)
      plane->side_of_point(old_position) >= 0 &&
      plane->side_of_point(new_position) < 0 )
    {

      //        printf("moving collision: plane %f %f %f %f\n",
      //           plane->a,
      //           plane->b,
      //           plane->c,
      //           plane->d);
      //        printf("moving collision: old position %f %f %f %f\n",
      //           old_position.X_,
      //           old_position.Y_,
      //           old_position.Z_,
      //           old_position.W_);
      //        printf("moving collision: new position %f %f %f %f\n",
      //           new_position.X_,
      //           new_position.Y_,
      //           new_position.Z_,
      //           new_position.W_);
      //        printf("moving collision: intersection is at %f %f %f %f\n",
      //           plane->intersection.X_,
      //           plane->intersection.Y_,
      //           plane->intersection.Z_,
      //           plane->intersection.W_);
#define EPSILON_COLLISION float_to_l3d_real(0.1)

      bounce.set
      (plane->intersection.X_ +
       l3d_mulrr(plane->a, collision_sphere_radius + EPSILON_COLLISION) -
       new_position.X_,

       plane->intersection.Y_ +
       l3d_mulrr(plane->b, collision_sphere_radius + EPSILON_COLLISION) -
       new_position.Y_,

       plane->intersection.Z_ +
       l3d_mulrr(plane->c, collision_sphere_radius + EPSILON_COLLISION) -
       new_position.Z_,

       int_to_l3d_real(0));

      // we formulate a bounce vector rel. to the invalid position instead
      // of explicitly assigning a valid position, so that the bounce
      // vector can be used externally to alter other things as well
      // (e.g. to eliminate any velocity in the direction of the bounce)

      if(prevent_collision) {
        new_position = new_position + bounce;
      }

      //        printf("moving collision: new new position, radius %f: %f %f %f %f\n",
      //           collision_sphere_radius,
      //           new_position.X_,
      //           new_position.Y_,
      //          new_position.Z_,
      //           new_position.W_);

      return 1;
    } else if ((dist=plane->side_of_point(new_position)) <collision_sphere_radius
               && dist>0 //can only collide with front of poly
              )
    {

      //        printf("static collision: plane %f %f %f %f\n",
      //         plane->a,
      //         plane->b,
      //         plane->c,
      //         plane->d);
      //        printf("static collision: dist %f\n", dist);

      //        printf("static collision: old position %f %f %f %f\n",
      //         old_position.X_,
      //         old_position.Y_,
      //         old_position.Z_,
      //         old_position.W_);
      //        printf("static collision: new position %f %f %f %f\n",
      //         new_position.X_,
      //         new_position.Y_,
      //         new_position.Z_,
      //         new_position.W_);

      //        printf("static collision: intersection is at %f %f %f %f\n",
      //         plane->intersection.X_,
      //         plane->intersection.Y_,
      //         plane->intersection.Z_,
      //         plane->intersection.W_);


      bounce.set
      (l3d_mulrr(plane->a,
                 collision_sphere_radius - dist + EPSILON_COLLISION),
       l3d_mulrr(plane->b,
                 collision_sphere_radius - dist + EPSILON_COLLISION),
       l3d_mulrr(plane->c,
                 collision_sphere_radius - dist + EPSILON_COLLISION),
       int_to_l3d_real(0));

      if(prevent_collision) {
        new_position = new_position + bounce;
      }

      //        printf("static collision: new new position %f %f %f %f\n",
      //         new_position.X_,
      //         new_position.Y_,
      //         new_position.Z_,
      //         new_position.W_);

      return 2;

    }
    else return 0;

  }
  else if ( (poly = dynamic_cast<l3d_polygon_3d_collidable *>(target)) ) {
    // sphere-poly test

    // same as sphere-plane except we also check for containment of
    // intersection point w/in polygon before bouncing

    l3d_real dist;

    // we hopped clear across the plane between old position and
    // new position. so, set position to be flush along the inside
    // of the plane. flush along means the position is the intersection
    // point plus the sphere's radius along the plane normal.

    int status;
    status = poly->plane.intersect_with_segment(old_position, new_position);

    if (// status &&
      //      poly->plane.intersection_t <= int_to_l3d_real(1) &&
      //      poly->plane.intersection_t >= 0)
      poly->plane.side_of_point(old_position) >= 0 &&
      poly->plane.side_of_point(new_position) < 0 &&
      poly->contains_point(poly->plane.intersection) )
    {

      //            printf("moving collision: plane %f %f %f %f\n",
      //                   poly->plane.a,
      //                   poly->plane.b,
      //                   poly->plane.c,
      //                   poly->plane.d);
      //            printf("moving collision: old position %f %f %f %f\n",
      //                   old_position.X_,
      //                   old_position.Y_,
      //                   old_position.Z_,
      //                   old_position.W_);
      //            printf("moving collision: new position %f %f %f %f\n",
      //                   new_position.X_,
      //                   new_position.Y_,
      //                   new_position.Z_,
      //                   new_position.W_);
      //            printf("moving collision: intersection is at %f %f %f %f\n",
      //                   poly->plane.intersection.X_,
      //                   poly->plane.intersection.Y_,
      //                   poly->plane.intersection.Z_,
      //                   poly->plane.intersection.W_);

      bounce.set
      (poly->plane.intersection.X_ +
       l3d_mulrr(poly->plane.a, collision_sphere_radius + EPSILON_COLLISION) -
       new_position.X_,

       poly->plane.intersection.Y_ +
       l3d_mulrr(poly->plane.b, collision_sphere_radius + EPSILON_COLLISION) -
       new_position.Y_,

       poly->plane.intersection.Z_ +
       l3d_mulrr(poly->plane.c, collision_sphere_radius + EPSILON_COLLISION) -
       new_position.Z_,

       int_to_l3d_real(0));

      // we formulate a bounce vector rel. to the invalid position instead
      // of explicitly assigning a valid position, so that the bounce
      // vector can be used externally to alter other things as well
      // (e.g. to eliminate any velocity in the direction of the bounce)

      if(prevent_collision) {
        new_position = new_position + bounce;
      }

      //            printf("moving collision: new new position, radius %f: %f %f %f %f\n",
      //                   collision_sphere_radius,
      //                   new_position.X_,
      //                   new_position.Y_,
      //                   new_position.Z_,
      //                   new_position.W_);

      return 1;
    } else if ((dist=poly->plane.side_of_point(new_position)) <collision_sphere_radius
               && dist>0 //can only collide with front of poly
              )
    {

//        // sphere overlaps plane. but where does it overlap plane? compute
//        // this point by taking the vector from the center to the plane.
//        /*
//          l3d_point pos_to_plane
//          (new_position.X_ - poly->plane.a,
//          new_position.Y_ - poly->plane.b,
//          new_position.Z_ - poly->plane.c,
//          int_to_l3d_real(1));

//          // now from new_position to pos_to_plane is a ray moving directly
//          // from the new_position to the plane. where it intersects is the
//          // center of intersection of the sphere overlap with the plane.

//          // NOTE! that this depends on the fact that the intersection
//          // is computed EVEN THOUGH it does NOT necessarily lie
//          // between p0 and p1 (if the radius is large for instance, the
//          // vector addition just moves ONE unit towards the plane).
//          // this is one reason that we allow the 3d intersection routine
//          // to compute intersetions even if they are not within
//          // p0 and p1 (i.e. t is not between 0 and 1).

//          poly->plane.intersect_with_segment(new_position,
//          pos_to_plane);
//        */

      //- check for collision with polygon interior


      l3d_point plane_intersection;
      plane_intersection.set
      (new_position.X_ + l3d_mulrr(dist, -poly->plane.a),
       new_position.Y_ + l3d_mulrr(dist, -poly->plane.b),
       new_position.Z_ + l3d_mulrr(dist, -poly->plane.c),
       int_to_l3d_real(1));

      if(poly->contains_point(poly->plane.intersection)) {
        if(poly->contains_point(plane_intersection)) {

          //              printf("static collision: plane %f %f %f %f\n",
          //                     poly->plane.a,
          //                     poly->plane.b,
          //                     poly->plane.c,
          //                     poly->plane.d);
          //              printf("static collision: dist %f\n", dist);

          //              printf("static collision: old position %f %f %f %f\n",
          //                     old_position.X_,
          //                     old_position.Y_,
          //                     old_position.Z_,
          //                     old_position.W_);
          //              printf("static collision: new position %f %f %f %f\n",
          //                     new_position.X_,
          //                     new_position.Y_,
          //                     new_position.Z_,
          //                     new_position.W_);

          //              printf("static collision: intersection is at %f %f %f %f\n",
          //                     poly->plane.intersection.X_,
          //                     poly->plane.intersection.Y_,
          //                     poly->plane.intersection.Z_,
          //                     poly->plane.intersection.W_);

          bounce.set
          (l3d_mulrr(poly->plane.a,
                     collision_sphere_radius - dist + EPSILON_COLLISION),
           l3d_mulrr(poly->plane.b,
                     collision_sphere_radius - dist + EPSILON_COLLISION),
           l3d_mulrr(poly->plane.c,
                     collision_sphere_radius - dist + EPSILON_COLLISION),
           int_to_l3d_real(0));

          //                  printf("bounce %f %f %f %f\n",
          //                         bounce.X_,
          //                         bounce.Y_,
          //                         bounce.Z_,
          //                         bounce.W_);

          if(prevent_collision) {
            new_position = new_position + bounce;
          }

          //                  printf("static collision: new new position %f %f %f %f\n",
          //                         new_position.X_,
          //                         new_position.Y_,
          //                         new_position.Z_,
          //                         new_position.W_);

          return 2;
        }

        //- check for collision with polygon edges

        l3d_real smallest_dist = int_to_l3d_real(9999);
        l3d_point nearest_point;
        for(int ivtx=0; ivtx<poly->clip_ivertices->num_items; ivtx++) {
          l3d_point a =
            (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].
            transformed;
          int next_ivtx = poly->next_clipidx_right
                          (ivtx, poly->clip_ivertices->num_items);
          l3d_point b =
            (**poly->vlist)[(*poly->clip_ivertices)[next_ivtx].ivertex].
            transformed;

          l3d_point p = new_position;
          l3d_vector c = p - a;
          l3d_vector V = normalized(b-a);
          l3d_real d = l3d_sqrt(dot(b-a,b-a));
          l3d_real t = dot(V,c);

          l3d_point pt;

          if(t<0) {
            pt = a;
          } else if(t>d) {
            pt = b;
          } else {
            pt = a + V*t;
          }

          l3d_real temp_dist;
          temp_dist = l3d_sqrt(dot(pt-new_position,
                                   pt-new_position));
          if(temp_dist <smallest_dist)
          {
            smallest_dist = temp_dist;
            nearest_point = pt;
          }
        }

        if( smallest_dist < collision_sphere_radius) {
          //              printf("static edge collision\n");
          bounce.set
          (l3d_mulrr(poly->plane.a,
                     collision_sphere_radius - dist + EPSILON_COLLISION),
           l3d_mulrr(poly->plane.b,
                     collision_sphere_radius - dist + EPSILON_COLLISION),
           l3d_mulrr(poly->plane.c,
                     collision_sphere_radius - dist + EPSILON_COLLISION),
           int_to_l3d_real(0));

          if(prevent_collision) {
            new_position = new_position + bounce;
          }


          return 2;

        }

        else return 0;
      }
      else return 0;
    }
    return 0;
  }
  else if ( (sphere = dynamic_cast<l3d_collidable_sphere *>(target)) ) {
    // sphere-sphere test

    int result = 0;

    l3d_real dist;
    dist = l3d_sqrt(dot(new_position - target->new_position,
                        new_position - target->new_position));

    if(dist < collision_sphere_radius + sphere->collision_sphere_radius)
    {
      result = 1;

      l3d_real bounce_dist = collision_sphere_radius
                             + sphere->collision_sphere_radius
                             - dist;

      bounce = normalized(new_position - sphere->new_position);
      bounce.set(l3d_mulrr(bounce.X_, bounce_dist + EPSILON_COLLISION),
                 l3d_mulrr(bounce.Y_, bounce_dist + EPSILON_COLLISION),
                 l3d_mulrr(bounce.Z_, bounce_dist + EPSILON_COLLISION),
                 int_to_l3d_real(0));

      if(prevent_collision) {
        new_position = new_position + bounce;
      }
    }

    return result;
  } else {
    printf("unknown collidable test sphere with ???\n");
    return 0;
  }

}
