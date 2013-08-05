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

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

void display(void) {
  int i;
  int x,y;

  for(i=0; i<7 ; i++) {
    x = rand() % 540;
    y = rand() % 380;

    glBegin(GL_POLYGON);
    glColor3i(rand(), rand(), rand());
    glVertex2i(x,y);
    glVertex2i(x+100,y+10);
    glVertex2i(x+70,y+100);
    glEnd();
  }
  glutSwapBuffers();

  glFlush();
}

void init(void) {
  glClearColor(0.0,0.0,0.0,0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 640, 480, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375, 0.375, 0.0);
  glViewport(0,0,640,480);
}

void update(void) {
  glutPostRedisplay();
}

void key(unsigned char c, int x, int y) {
  switch(c) {
    case 'q': exit(0);
  }
}

main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
  glutInitWindowSize(640,480);
  glutInitWindowPosition(100,100);
  glutCreateWindow("hello");
  init();
  glutDisplayFunc(display);
  glutIdleFunc(update);
  glutKeyboardFunc(key);
  glutMainLoop();
  return 0;
}
