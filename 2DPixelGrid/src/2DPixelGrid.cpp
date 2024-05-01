#pragma once

#include <stdio.h> 
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>

/*** freeglut***/
#include ".\GL\freeglut.h"
#include "FPSCounter.h"

#define PI 3.14159265
int lineCount = 0;

struct Point {
    int x;
    int y;
};

struct Color {
    float r;
    float g;
    float b;
};

void ChangeSize(int, int);
void RenderScene(void);
void createMenu();
void menu(int index);

void drawGrid(int num);
void drawPoint(int x, int y, Color);
void drawAllPoints(std::vector<Point>);
void drawAllLines(std::vector<Point>);

void Line(Point p0, Point p1);

void myKeyboardFunc(unsigned char Key, int x, int y);
void myMouseFunc(int button, int state, int x, int y);


int gridNum = 10;

float winSizeX = 800;
float winSizeY = 800;

float left = -15;
float right = 15;
float bottom = -15;
float top = 15;
float znear = -15;
float zfar = 15;

int px, py;

std::vector<Point> points;
std::vector<Point> lines;
// std::vector<std::pair<Point, Point>> lines;

enum {
    MODE_GRID_10,
    MODE_GRID_15,
    MODE_GRID_20
};

std::unique_ptr<FPSCounter> fpsCounter(new FPSCounter());

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 80);
    glutCreateWindow("2DPixelGrid");

    glutKeyboardFunc(myKeyboardFunc);
    glutMouseFunc(myMouseFunc);

    createMenu();

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

    glutMainLoop();
    return 0;
}

void ChangeSize(int w, int h)
{
    printf("Window Size= %d X %d\n", w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, znear, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderScene(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, znear, zfar);

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    gluLookAt(0, 0, 10.0f, 0, 0, 0, 0, 1, 0);

    drawGrid(gridNum);
    drawPoint(px, py, Color{1,0,0});
    drawAllPoints(points);
    drawAllLines(lines);

    fpsCounter->displayAndUpdateFPS(left * 0.9, top * 0.9);
    glutSwapBuffers();
}

// Create Menu
void createMenu() {
    glutCreateMenu(menu);

    glutAddMenuEntry("Grid:10", MODE_GRID_10);
    glutAddMenuEntry("Grid:15", MODE_GRID_15);
    glutAddMenuEntry("Grid:20", MODE_GRID_20);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Menu Callback function
void menu(int index) {
    switch (index)
    {
    case MODE_GRID_10:
        gridNum = 10;
        left = -15; right = 15;
        bottom = -15; top = 15;
        znear = -15; zfar = 15;
        glutPostRedisplay();
        break;

    case MODE_GRID_15:
        gridNum = 15;
        left = -22.5; right = 22.5;
        bottom = -22.5; top = 22.5;
        znear = -22.5; zfar = 22.5;
        glutPostRedisplay();
        break;

    case MODE_GRID_20:
        gridNum = 20;
        left = -30; right = 30;
        bottom = -30; top = 30;
        znear = -30; zfar = 30;
        glutPostRedisplay();
        break;
    }
}

void drawGrid(int num) {
    glBegin(GL_LINES);

    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(num, 0);
    glVertex2f(-num, 0);

    glVertex2f(0, num);
    glVertex2f(0, -num);

    glColor3f(0.5f, 0.5f, 0.5f);

    for (int i = -num; i <= num; i++)
    {
        glVertex2f( num, i);
        glVertex2f(-num, i);

        glVertex2f(i,  num);
        glVertex2f(i, -num);
    }
    glEnd();
}


void drawPoint(int x, int y, Color color) {
    if (x <= gridNum && x > -gridNum && y > -gridNum && y <= gridNum)
    {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_QUADS);
        glVertex2f(x - 1, y - 1);
        glVertex2f(x, y - 1);
        glVertex2f(x, y);
        glVertex2f(x - 1, y);
        glEnd();
    }
}

void drawAllPoints(std::vector<Point>) {
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (auto& point : points) {
        glVertex2f(point.x - 0.5, point.y - 0.5);
    }
    glEnd();

    Color red{ 1, 0, 0 };
    for (auto& point : points) {
        drawPoint(point.x, point.y, red);
    }
}

void drawAllLines(std::vector<Point>) {
    Color green { 0, 1, 0};
    Color blue  { 0, 0, 1};
    Point lastPoint = { 0, 0};
    for (auto& point : lines) {
        if (lastPoint.x == point.x || lastPoint.y == point.y)
        {
            //MidPoint:E
            drawPoint(point.x, point.y, green); 
        }
        else 
        {
            //MidPoint:NE
            drawPoint(point.x, point.y, blue);
        }
        lastPoint = point;
    }
}


void Line(Point p0, Point p1) {
    lineCount++;

    int dx = abs(p1.x - p0.x);
    int dy = abs(p1.y - p0.y);

    // Angle in radians
    double angle = atan2(p1.y - p0.y, p1.x - p0.x);

    // Determine octant
    float octant = (int)(((angle) / (PI / 4)) + 8) % 8 + 1;
    std::cout << "Line V"<< lineCount << "V" << lineCount+1 << ": region" << octant << std::endl;

    int sx = (p0.x < p1.x) ? 1 : -1; //stride: 如果 "起點" 小於 "終點" 則為 -1
    int sy = (p0.y < p1.y) ? 1 : -1; //stride: 如果 "起點" 小於 "終點" 則為 -1

    int err = dx - dy;
    int err2;

    int x0 = p0.x, y0 = p0.y;
    int x1 = p1.x, y1 = p1.y;

    lines.push_back(Point{ x0, y0 });

    while (x0 != x1 || y0 != y1) {

        // Draw the pixel at (x0, y0)
        //std::cout << "(" << x0 << ", " << y0 << ")" << std::endl;

        err2 = 2 * err; //loop 1: 2dx - 2dy

        if (err2 > -dy) { //loop 1: 2dx - dy > 0
            err -= dy; // next loop : err2 -= 2dy 
            x0 += sx;
        }

        if (err2 < dx) { //loop 1: dx - 2dy < 0 (2dy - dx > 0)
            err += dx; // next loop : err2 += 2dx 
            y0 += sy;
        }

        lines.push_back(Point{ x0, y0 });
    }
}

void myKeyboardFunc(unsigned char Key, int x, int y) {
    switch (Key) {
    case 'b':
        std::cout << "clean!" << std::endl;
        lines.clear();
        points.clear();
        break;
    }
}

void myMouseFunc(int button, int state, int x, int y) {

    if (state == GLUT_DOWN) {
        //printf("Screen Coordinates: (%d, %d)\n", x, y);
        /* Screen to NDC*/
        float xn = (x - winSizeX / 2) / (winSizeX / 2);
        float yn = (winSizeY / 2 - y) / (winSizeY / 2);
        float zn = -1;

        //printf("NDC Coordinates: (%f, %f, %f)\n", xn, yn, zn);
        /* NDC to Atho*/
        float sx = (xn + 1) * (right - left) / 2 + left;
        float sy = (yn + 1) * (top - bottom) / 2 + bottom;
        float sz = -(zn + 1) * (zfar - znear) / 2 - znear;

        //printf("Space Coordinates: (%f, %f, %f)\n", sx, sy, sz);

        px = round(sx + 0.5);
        py = round(sy + 0.5);

        points.push_back(Point{ px, py });

        printf("Point V%d : (%d, %d)\n", points.size(), px, py);

        if (points.size() >= 2)
        {
            Line(points[points.size() - 2], points[points.size() - 1]);
        }
    }
    else if (state == GLUT_UP) {

    }
}