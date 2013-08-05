// my first GL program from scratch!
// Sat Apr 10 01:21:48 MEST 1999

#include <GL/glut.h>
#define NUMPOLY 1500

float x[NUMPOLY], y[NUMPOLY], z[NUMPOLY];

void display(void) {
  int i;
  glClear(GL_COLOR_BUFFER_BIT);


//   gluLookAt(0.0,0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  for(i=0; i<NUMPOLY; i++) {
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3i(x[i]+70,y[i]+100,z[i]);
    glVertex3i(x[i]+100,y[i]+10,z[i]);
    glVertex3i(x[i],y[i],z[i]);
    glEnd();
  }
  glutSwapBuffers();

  glFlush();
}

void init(void) {
  int i;
  glShadeModel(GL_FLAT);
  glClearColor(0.0,0.0,0.0,0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, 1.0, 1.0, 150000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // glTranslatef(0.375, 0.375, 0.0);  // CGDC notes 1998: do this to use int coords
  //glViewport(0,0,640,480);


  for(i=0; i<NUMPOLY; i++) {
    x[i] = rand() % 6400 - 3200;
    y[i] = rand() % 6400 - 3200;
    z[i] = -rand() % 10000;
  }
}

void update(void) {
  int i;

  for(i=0; i<NUMPOLY; i++) {
    z[i]+= 100;
    if(z[i]>-10.0) {
      x[i] = rand() % 6400 - 3200;
      y[i] = rand() % 6400 - 3200;
      z[i] = -rand() % 10000;
    }
  }

  glutPostRedisplay();
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
  glutMainLoop();
  return 0;
}

