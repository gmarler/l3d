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

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

main() {
  int mysocket;

  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(5559);
  inet_aton("127.0.0.1", &addr.sin_addr);
  mysocket = socket(PF_INET, SOCK_STREAM, getprotobyname("IP")->p_proto);
  bind(mysocket, (sockaddr *)&addr, sizeof(addr));
  listen(mysocket, 10);

  struct sockaddr_in newaddr;
  while(1) {
    int newsock;
    socklen_t addrlen;
    newsock = accept(mysocket, (sockaddr *)&newaddr, &addrlen);

    if(fork()==0) {
      char msg[512];
      sprintf(msg,"Welcome to SimonServer v0.0\n");
      send(newsock, msg, strlen(msg), 0);

      while(1) {
        char buf[256];
        int size;
        size = recv(newsock, buf, sizeof(buf), 0);
        buf[size] = 0;
        char buf2[512];
        sprintf(buf2,"Simon says: %s\n", buf);
        send(newsock, buf2, strlen(buf2), 0);
        if(strncmp(buf,"quit",4)==0) {
          send(newsock, "bye\n", 4, 0);
          shutdown(newsock,2);
          exit(0);
        }
      }
    }
  }
  close(mysocket);
}
