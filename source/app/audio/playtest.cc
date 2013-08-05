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

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

main() {
  int mysocket;

  struct sockaddr_in addr;
  struct sockaddr_in boundaddr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(5556);
  inet_aton("127.0.0.1", &addr.sin_addr);
  mysocket = socket(PF_INET, SOCK_STREAM, getprotobyname("IP")->p_proto);
  connect(mysocket, (sockaddr *)&addr, sizeof(addr));

  while(1) {
    char s[256];
    gets(s);
    char cmd[] = "play count=1 volume=64 sound=crash.au\n";
    printf("sending following command to server: %s\n",cmd);
    send(mysocket, cmd, strlen(cmd),0);
  }
  shutdown(mysocket,2);
}
