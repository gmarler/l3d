// my first GL program from scratch!
// Sat Apr 10 01:21:48 MEST 1999

#include <GL/glut.h>

void display(void) {
//  glClear(GL_COLOR_BUFFER_BIT);

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
  glTranslatef(0.375, 0.375, 0.0);  // CGDC notes 1998: do this to use int coords
  glViewport(0,0,640,480);
}

void update(void) {
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

