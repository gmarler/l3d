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

#ifndef __LIST_H
#define __LIST_H
#include "../tool_os/memman.h"

//
// a factory which creates, copies, and destroys lists of items.
//

template<class Item> class l3d_list_item_factory {
  public:
  virtual Item *create(void) { return new Item ; }
    // HWAT DUZ THIS DO  virtual void copy(Item *dest, Item *source, int count) {
    //    register int i;
    //    for(i=0; i<count; i++) {
    //      dest[i] = source[i];
    //    }
    //  }
    virtual ~l3d_list_item_factory() {}
};

//
// a dynamically growable list with an initial size which uses a
// factory to create its elements.
//
// immediately after allocation, NOTHING is marked as used.
// clients should call next_index() after every allocation operation,
// and not directly jump around assigning random vertcies (otherwise
// the list never knows when to grow).

template<class Item> class l3d_list
{
  protected:
    bool is_my_factory;
  public:
    int max_items;
    l3d_list_item_factory<Item> *factory; // must be public to call clone on it

    Item **items;
    int num_items;

    l3d_list(int initial_size) {
      factory = new l3d_list_item_factory<Item>;
      is_my_factory = true;
      num_items = 0;
      max_items = initial_size;

      items = new Item* [max_items]; // create slots
      for(int i=0; i<max_items; i++) {
        items[i] = factory->create(); // fill slots
      }
    };

    l3d_list(void) {
      factory = new l3d_list_item_factory<Item>;
      is_my_factory = true;
      num_items = 0;
      max_items = 10; // initial size if none is specified

      items = new Item* [max_items]; // create slots
      for(int i=0; i<max_items; i++) {
        items[i] = factory->create(); // fill slots
      }
    };


    l3d_list(int initial_size, l3d_list_item_factory<Item> *factory) {
      this->factory = factory;
      is_my_factory = false;
      num_items = 0;
      max_items = initial_size;

      items = new Item* [max_items];  // create slots
      for(int i=0; i<max_items; i++) {
        items[i] = factory->create(); // fill slots with user-provided factory
      }
    }

    ~l3d_list(void) {
      for(int i=0; i<max_items; i++) {
        delete items[i];// delete objects in the slots
      }
      delete [] items; // delete the slots themselves
      if(is_my_factory) { delete factory; }
    };

    int next_index(void) {
      if ( num_items < max_items ) {
        // num_items is valid. so, we just return it.

        return num_items ++;
      } else {
        // num_items is not valid, so, expand list so that it is valid.

        int oldsize = max_items;
        max_items += 20;

        Item **bigger_items = new Item* [max_items];
        int i;
        for(i=0; i<oldsize; i++) {
          bigger_items[i] = items[i]; // point in new list to object from old list
        }
        for(; i < max_items; i++) {
          bigger_items[i] = factory->create(); // create new empty objects to fill new slots
        }
        delete [] items;
        items = bigger_items;

        return num_items++;
      }
    }

    Item & operator [] (int idx) const { return *items[idx]; }

    //   l3d_list(l3d_list<Item> &r) {
    //     while(num_items < r.num_items) {
    //       next_index(); // make sure we have enough space to copy the src list
    //     }
    //     factory->copy(items,
    // 		  r.items,
    // 		  r.num_items);
    //     num_items = r.num_items;
    //     return *this;
    //     }

    l3d_list<Item> & operator = ( const l3d_list<Item> &r ) {
      // DELME printf("ja, copy\n");
      while(num_items < r.num_items) {
        next_index(); // make sure we have enough space to copy the src list
      }

      // at this point, each list has the same amount of slots filled with objects.
      // so we copy the contents of each object from the other list into our list.

      for(int i=0; i<r.num_items; i++) {
        *items[i] = *(r.items[i]); // virtual assignment, since type of items unknown
      }

      num_items = r.num_items;

      return *this;
      // no copy constructor needed since we are returning a reference to this
      // object, i.e. the same one

    }

};

// represents a contiguous list of items consisting of two parts:
// a fixed part, and a variable part. The fixed part cannot change
// in size. The variable part consists of a variable number of items
// whose number and value may vary greatly.
//
// to external users of the list, both the fixed part and the variable
// part must be accessible in the same way.
//
// 0 1 2 3 ... n-1 n .... n+num_varying_items-1 .... n+max_varying_items-1
// ############### -----------------------  ...  -------------------------
//    fixed                               variable
//
// immediately after allocation, the fixed part of the list has already
// been marked as used and is waiting to be filled. the varying part
// has not been used. when the varying part changes, next_varying_index()
// should be called, as it can cause list growth.

template<class Item> class l3d_two_part_list {
  protected:
    l3d_list_item_factory<Item> *factory;
  public:
    l3d_list<Item> *list;
    int num_fixed_items;
    int num_varying_items;
    int max_varying_items; //normally should not be touched from outsiders

    l3d_two_part_list<Item> & operator= (const l3d_two_part_list<Item> &r) {
      *list = *r.list;
      num_fixed_items = r.num_fixed_items;
      num_varying_items = r.num_varying_items;
      factory = r.factory;
      max_varying_items = r.max_varying_items;
      return *this;
    }

    l3d_two_part_list(int fixed_size) {
      num_fixed_items = fixed_size;
      max_varying_items = fixed_size >> 1; // faster than divide, since we
      // might be allocating lists in time-critical loops
      list = new l3d_list<Item> (fixed_size + max_varying_items);
      for(register int i=0; i<fixed_size+max_varying_items; i++) {
        list->next_index(); // occupy all allocated spaces
      }
      num_varying_items = 0;
    };

    l3d_two_part_list(int fixed_size, l3d_list_item_factory<Item> *factory) {
      this->factory = factory;
      num_fixed_items = fixed_size;
      max_varying_items = fixed_size >> 1; // faster than divide, since we
      // might be allocating lists in time-critical loops
      list = new l3d_list<Item> (fixed_size + max_varying_items, factory);
      for(register int i=0; i<fixed_size+max_varying_items; i++) {
        list->next_index();
      }
      num_varying_items = 0;
    };

    ~l3d_two_part_list(void) {
      delete list;
    };

    int next_varying_index(void) {
      if ( num_varying_items < max_varying_items) {
        return num_fixed_items + num_varying_items++;
      } else {
        list->next_index();
        max_varying_items++;
        return num_fixed_items + num_varying_items++;
      }
    };

    Item & operator[] (int index) const { return *(list->items[index]); }
};

#endif
