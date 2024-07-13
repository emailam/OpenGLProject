#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif
#include<math.h>
#include <stdlib.h>

GLfloat Cx = 0, Cy = 0, Cz = 3.5;
GLfloat x_bycicle = 0;
bool is_door_opened = false;
bool is_windows_opened = false;
const float PI = 3.14159265358979323846f;
int left_wheel_theta = 0;
int right_wheel_theta = 0; // rotate with 45 degrees
bool isAnimating = false;
GLfloat horse_angle = 0;
GLfloat rotation = 0;
GLfloat x_right_wheel = 0.1, y_right_wheel = -0.9, z_right_wheel = 1.6;
GLfloat x_left_wheel = -0.1, y_left_wheel = -0.9, z_left_wheel = 1.6;
void MyInit() {
    glClearColor(0.5, 1, 0.5, 1); // Background color
    glEnable(GL_DEPTH_TEST);      // Enable depth testing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1.4, 100);
    glMatrixMode(GL_MODELVIEW);
}

void Polygon4(GLfloat A[], GLfloat B[], GLfloat C[], GLfloat D[]) {
    glBegin(GL_POLYGON);
        glVertex3fv(A);
        glVertex3fv(B);
        glVertex3fv(C);
        glVertex3fv(D);
    glEnd();
}

void Polygon3(GLfloat A[], GLfloat B[], GLfloat C[]) {
    glBegin(GL_POLYGON);
        glVertex3fv(A);
        glVertex3fv(B);
        glVertex3fv(C);
    glEnd();
}
void Lines(GLfloat A[], GLfloat B[], GLfloat C[], GLfloat D[]) {
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
        glVertex3fv(A);
        glVertex3fv(B);
        glVertex3fv(C);
        glVertex3fv(D);
    glEnd();
}

void DrawBuilding() {
    GLfloat v[10][3] = {
        {-0.5, 1, 0.5}, {0.5, 1, 0.5}, {0.5, -1, 0.5}, {-0.5, -1, 0.5},
        {-0.5, 1, -0.5}, {0.5, 1, -0.5}, {0.5, -1, -0.5}, {-0.5, -1, -0.5},
    };

    GLfloat roof[5][3] = {
        {-0.5, 1, 0.5},
        {-0.5, 1, -0.5},
        {0.5, 1, 0.5},
        {0.5, 1, -0.5},
        {0,1.6,0},
    };

    glColor3f(1, 1, 0.5);
    Polygon4(v[0], v[1], v[2], v[3]); // Front face
    glColor3f(0.4, 0, 0);
    Polygon4(v[4], v[5], v[6], v[7]); // Back face
    glColor3f(1, 1, 0.33);
    Polygon4(v[0], v[3], v[7], v[4]); // Left face
    Polygon4(v[1], v[2], v[6], v[5]); // Right face
    glColor3f(0.75, 0.75, 0.75);
    Polygon4(v[0], v[1], v[5], v[4]); // Top face
    glColor3f(0.4, 0, 0);
    Polygon4(v[3], v[2], v[6], v[7]); // Bottom face

    glColor3f(0.5, 0.3, 0.0);
    Lines(v[0], v[1], v[2], v[3]); // Front lines
    Lines(v[4], v[5], v[6], v[7]); // Back lines
    Lines(v[0], v[3], v[7], v[4]); // Left lines
    Lines(v[1], v[2], v[6], v[5]); // Right lines
    Lines(v[0], v[1], v[5], v[4]); // Top lines
    Lines(v[3], v[2], v[6], v[7]); // Bottom lines
    Polygon3(roof[0],roof[1],roof[4]);
    Polygon3(roof[0],roof[2],roof[4]);
    Polygon3(roof[1],roof[3],roof[4]);
    Polygon3(roof[2],roof[3],roof[4]);
}

void DrawWindows(GLfloat windows[16][3]) {
    for (int i = 0; i < 16; i += 4) {
        glPushMatrix(); // Push the current matrix
        if (is_windows_opened) {
            glTranslatef(windows[i][0], windows[i][1], windows[i][2]);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f); // Rotate around the y-axis in the opposite direction
            glTranslatef(-windows[i][0], -windows[i][1], -windows[i][2]);
        }
        Polygon4(windows[i], windows[i + 1], windows[i + 2], windows[i + 3]);
        Lines(windows[i], windows[i + 1], windows[i + 2], windows[i + 3]);
        glPopMatrix(); // Restore the previous matrix
    }
}

void DrawBackWindows(GLfloat windows[16][3]) {
    for (int i = 0; i < 16; i += 4) {
        Polygon4(windows[i], windows[i + 1], windows[i + 2], windows[i + 3]);
        Lines(windows[i], windows[i + 1], windows[i + 2], windows[i + 3]);
    }
}

void DrawDoor(GLfloat door[4][3]) {
    if(is_door_opened){
        glPushMatrix();
        glTranslatef(-0.4f, 0, 0.6f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    }

    Polygon4(door[0], door[1], door[2], door[3]);
    Lines(door[0], door[1], door[2], door[3]);

    if(is_door_opened){
        glPopMatrix();
    }
}
void DrawBackDoor(GLfloat backDoor[4][3]){
    Polygon4(backDoor[0], backDoor[1], backDoor[2], backDoor[3]);
}
void DrawCircle(GLfloat xc,GLfloat yc,GLfloat zc, GLfloat numberOfSegments, GLfloat radius){
    glColor3f(0.85, 0.85, 0.85);
    glLineWidth(30);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<=numberOfSegments;i++){
        GLfloat theta = 2 * PI * i / numberOfSegments;
        GLfloat new_x = xc + radius * cos(theta);
        GLfloat new_z = zc + radius * sin(theta);
        GLfloat new_y = yc;
        GLfloat v[3] = {new_x,new_y,new_z};
        glVertex3fv(v);
    }
    glEnd();
}
void RightWheel(GLfloat xc,GLfloat yc,GLfloat zc, GLfloat numberOfSegments, GLfloat radius){
    glPushMatrix();
    glTranslatef(xc , yc, zc);
    glRotatef(right_wheel_theta, 0, 1, 0);
    glTranslatef(-xc, -yc, -zc);


    glColor3f(0, 0, 0);
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<=numberOfSegments;i++){
        GLfloat theta = 2 * PI * i / numberOfSegments;
        GLfloat new_x = xc + radius * cos(theta);
        GLfloat new_y = yc + radius * sin(theta);
        GLfloat new_z = zc;
        GLfloat v[3] = {new_x,new_y,new_z};
        glVertex3fv(v);
    }
    glEnd();

    glPopMatrix();
}
void LeftWheel(GLfloat xc,GLfloat yc,GLfloat zc, GLfloat numberOfSegments, GLfloat radius){
    glPushMatrix();
    glTranslatef(xc, yc, zc);
    glRotatef(left_wheel_theta, 0, 1, 0);
    glTranslatef(-xc, -yc, -zc);


    glColor3f(0, 0, 0);
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<=numberOfSegments;i++){
        GLfloat theta = 2 * PI * i / numberOfSegments;
        GLfloat new_x = xc + radius * cos(theta);
        GLfloat new_y = yc + radius * sin(theta);
        GLfloat new_z = zc;
        GLfloat v[3] = {new_x,new_y,new_z};
        glVertex3fv(v);
    }
    glEnd();

    glPopMatrix();
}
void DrawBicycle() {

    // Draw right wheel
    RightWheel(x_bycicle + x_right_wheel, y_right_wheel, z_right_wheel, 50, 0.08);

    // Draw left wheel
    LeftWheel(x_bycicle + x_left_wheel, y_left_wheel, z_left_wheel, 50, 0.08);

    // Draw body and handles
    GLfloat Body[2][3] = {{x_bycicle + x_left_wheel - 0.03, y_left_wheel + 0.08, z_left_wheel},
                          {x_bycicle + x_right_wheel + 0.03, y_right_wheel + 0.08, z_right_wheel}};
    GLfloat Handles[2][3] = {{x_bycicle + x_right_wheel, y_right_wheel + 0.1, z_right_wheel + 0.05},
                             {x_bycicle + x_right_wheel, y_right_wheel + 0.1, z_right_wheel - 0.05}};

    glLineWidth(9);

    glPushMatrix(); // Push the current matrix

    glBegin(GL_LINES);
    glVertex3fv(Body[0]);
    glVertex3fv(Body[1]);
    glEnd();

    glLineWidth(20);
    glBegin(GL_LINES);
    glVertex3fv(Handles[0]);
    glVertex3fv(Handles[1]);
    glEnd();

    glPopMatrix(); // Restore the previous matrix
}

void Draw() {
    GLfloat windows[16][3] = {
        {-0.4, 0.8, 0.5001}, {-0.4, 0.4, 0.5001}, {-0.1, 0.4, 0.5001}, {-0.1, 0.8, 0.5001},
        {0.1, 0.8, 0.5001}, {0.1, 0.4, 0.5001}, {0.4, 0.4, 0.5001}, {0.4, 0.8, 0.5001},
        {-0.4, 0, 0.5001}, {-0.4, -0.4, 0.5001}, {-0.1, -0.4, 0.5001}, {-0.1, 0, 0.5001},
        {0.1, 0, 0.5001}, {0.1, -0.4, 0.5001}, {0.4, -0.4, 0.5001}, {0.4, 0, 0.5001}
    };

    GLfloat door[4][3] = {
        {-0.1, -1, 0.5001}, {-0.1, -0.6, 0.5001}, {0.1, -0.6, 0.5001}, {0.1, -1, 0.5001}
    };

    GLfloat secondFloor[4][3] = {
        {-0.5,0.1,0.5},{0.5,0.1,0.5},{0.5,0.1,-0.5},{-0.5,0.1,-0.5},
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(Cx, Cy, Cz, 0, 0, 0, 0, 1, 0);


    glColor3f(0.5, 0.3, 0.0);
    Lines(secondFloor[0],secondFloor[1],secondFloor[2],secondFloor[3]);
    DrawDoor(door);
    glColor3f(0.9, 0.9, 0.9);
    DrawWindows(windows);
    glColor3f(0, 0, 0);
    DrawBackDoor(door);
    DrawBackWindows(windows);
    DrawBuilding();
    DrawCircle(0,-1,0,500,1.6);

    glRotatef(rotation,0,1,0);
    glRotatef(horse_angle,0,0,1);
    DrawBicycle();
    glutSwapBuffers();
}

void Key(unsigned char ch, int x, int y) {
    switch (ch) {
        case 'x': Cx -= 0.5; break; // x to decrease the x-axis.
        case 'X': Cx += 0.5; break; // X to increase the x-axis. and so on..
        case 'y': Cy -= 0.5; break;
        case 'Y': Cy += 0.5; break;
        case 'z': Cz -= 0.5; break;
        case 'Z': Cz += 0.5; break;
        case 'o':
            if(!is_door_opened){
                is_door_opened = true;
            }
            break;
        case 'c':
            if(is_door_opened){
                is_door_opened = false;
            }
            break;
        case 'O':
            if(!is_windows_opened){
                is_windows_opened = true;
            }
            break;
        case 'C':
            if(is_windows_opened){
                is_windows_opened = false;
            }
            break;
        case 'f':
            x_bycicle += 0.02;
            break;
        case 'b':
            x_bycicle -= 0.02;
            break;
        case 'r':
            right_wheel_theta += 45;
            break;
        case 'l':
            left_wheel_theta -= 45;
            break;
        case 'h':
            horse_angle += 30;
            break;
        case 'H':
            horse_angle -= 30;
    }
    glutPostRedisplay();
}
void Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isAnimating = true;
    }
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        isAnimating = false;
    }
}
void animate(){
    if(isAnimating){
        rotation += 0.05;
        glutPostRedisplay();
    }
}
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(500, 500);
    glutInitWindowSize(500, 500);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Simplified Building with Camera");
    MyInit();
    glutDisplayFunc(Draw);
    glutKeyboardFunc(Key);
    glutMouseFunc(Mouse);
    glutIdleFunc(animate);
    glutMainLoop();
    return EXIT_SUCCESS;
}
