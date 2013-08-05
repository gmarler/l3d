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

#include "scli_rplay.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

l3d_sound_client* l3d_sound_client_rplay_factory::create(void) {
  return new l3d_sound_client_rplay;
}

void l3d_sound_client_rplay::connect(const char *connect_string)
{
  char parms[1024];
  char server_ip[256];
  char server_port[16];

  strncpy(parms,connect_string,sizeof(parms));
  parms[sizeof(parms)-1]=0;

  char *param;
  param = strtok(parms," \t\r\n");
  strncpy(server_ip, param, sizeof(server_ip));
  server_ip[sizeof(server_ip)-1]=0;

  param = strtok(NULL," \t\r\n");
  strncpy(server_port, param, sizeof(server_port));
  server_port[sizeof(server_port)-1]=0;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(server_port));

  inet_aton(server_ip, &addr.sin_addr);

//  addr.sin_addr.s_addr = inet_addr(server_ip);

  sound_socket = socket(PF_INET, SOCK_STREAM, getprotobyname("IP")->p_proto);
  //printf("server ip %s\nport %d\n",server_ip, atoi(server_port));
  connected = ( ::connect(sound_socket, (sockaddr *)&addr, sizeof(addr)) == 0 );
}

void l3d_sound_client_rplay::send_command(const char *cmd) {
  if ( !connected || send(sound_socket, cmd, strlen(cmd),0) <= 0 ) {
    //- apparently there was an error. try, but just once, to re-establish
    //- contact to server.

    connected = ( ::connect(sound_socket, (sockaddr *)&addr, sizeof(addr)) == 0);
    if(connected) {
      send(sound_socket, cmd, strlen(cmd),0);
    }
  };
}

l3d_sound_client_rplay::~l3d_sound_client_rplay(void) {
  shutdown(sound_socket,2);
}
