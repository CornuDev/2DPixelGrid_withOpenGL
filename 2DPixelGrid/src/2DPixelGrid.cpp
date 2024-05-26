#pragma once

#include <stdio.h> 
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

/*** freeglut***/
#include "./GL/freeglut.h"
#include "FPSCounter.h"

#define PI 3.14159265

int lineCount = 0;



struct Color {
    float r;
    float g;
    float b;

    Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) : r(r), g(g), b(b) {}
};

struct Point {
    float x;
    float y;
    Color color;

    Point(float x = 0, float y = 0, Color color = Color()) : x(x), y(y), color(color) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

void ChangeSize(int, int);
void RenderScene(void);
void createMenu();
void menu(int index);

void drawGrid(int num);
void drawPoint(int x, int y, Color);
void drawAllVertices(std::vector<Point> vertices);
void drawAllLines(std::vector<Point> lines);

void drawInnerPoints(std::vector<Point> points);
int drawPointCount = 0;

void findBoundingBox(Point p0, Point p1, Point p2, Point& topLeft, Point& bottomRight);

void Line(Point p0, Point p1);

void crow(std::vector<Point> vertices);
void scanY(std::vector<Point> vertices, int n, int i);
void scanX(Point* l, Point* r, int y);
void differenceY(Point* v1, Point* v2, Point* e, Point* de, int y);
void differenceX(Point* v1, Point* v2, Point* e, Point* de, int x);
void difference(Point* v1, Point* v2, Point* e, Point* de, float d, float f);
void increment(Point* edge, Point* delta);

Color generateRandomColor();
Color interpolateColor(const Color& startColor, const Color& endColor, float factor);
bool isLoop = false;

void myKeyboardFunc(unsigned char Key, int x, int y);
void myMouseFunc(int button, int state, int x, int y);
void myTimerFunc(int value);

int refreshMillis = 100;

int gridNum = 10;

float winSizeX = 800;
float winSizeY = 800;

float left = -15;
float right = 15;
float bottom = -15;
float top = 15;
float znear = -15;
float zfar = 15;

float px, py;

std::vector<Point> vertices;
std::vector<Point> lines;
std::vector<Point> innerPoints; 
// std::vector<std::pair<Point, Point>> lines;

enum {
    MODE_GRID_10,
    MODE_GRID_15,
    MODE_GRID_20
};

std::unique_ptr<FPSCounter> fpsCounter(new FPSCounter());

int main(int argc, char** argv)
{
    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(600, 80);
    glutCreateWindow("2DPixelGrid: Crow Algorithm");

    glutKeyboardFunc(myKeyboardFunc);
    glutMouseFunc(myMouseFunc);

    createMenu();

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutTimerFunc(refreshMillis, myTimerFunc, 0);

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
    //drawPoint(px, py, Color{1,0,0});
    
    drawAllVertices(vertices);
    drawAllLines(lines);
    drawInnerPoints(innerPoints);

    fpsCounter->displayAndUpdateFPS(left * 0.9, top * 0.9);
    glutSwapBuffers();
}

void myTimerFunc(int value)
{
    if (drawPointCount < innerPoints.size())
    {
        drawPointCount++;
    }
    glutPostRedisplay();
    glutTimerFunc(refreshMillis, myTimerFunc, 0);
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

void drawAllVertices(std::vector<Point> vertices) {
    //drawLinks
    if (vertices.size() > 1) 
    {
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto& point : vertices) {
            glVertex2f(point.x - 0.5, point.y - 0.5);
        }
        glVertex2f(vertices.front().x - 0.5, vertices.front().y - 0.5);
        glEnd();
    }
    //drawVertices
    for (auto& point : vertices) {
        drawPoint(point.x, point.y, point.color);
    }
}

void drawInnerPoints(std::vector<Point> points) {
    for (int i = 0; i < drawPointCount; i++)
    {
        drawPoint(points[i].x, points[i].y, points[i].color);
    }
}

void drawAllLines(std::vector<Point> lines) {
    Color green { 0, 1, 0};
    Color blue  { 0, 0, 1};
    Point lastPoint = { 0, 0};
    for (auto& point : lines) {
        drawPoint(point.x, point.y, point.color);
        /*
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
        */
    }
}

// Function to calculate the line equation
float lineEq(Point p1, Point p2, int x, int y) {
    return (p2.y - p1.y) * x + (p1.x - p2.x) * y + (p2.x * p1.y - p1.x * p2.y);
}

// Function to find the bounding box of the triangle
void findBoundingBox(Point p0, Point p1, Point p2, Point& topRight, Point& bottomLeft) {
    // Initialize min and max coordinates
    int minX = std::min({ p0.x, p1.x, p2.x });
    int minY = std::min({ p0.y, p1.y, p2.y });
    int maxX = std::max({ p0.x, p1.x, p2.x });
    int maxY = std::max({ p0.y, p1.y, p2.y });

    // Update bounding box coordinates
    topRight.x = maxX;
    topRight.y = maxY;
    bottomLeft.x = minX;
    bottomLeft.y = minY;
}


void Line(Point p0, Point p1) {
    lineCount++;
    float distance = sqrt((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y));

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

    float x0 = p0.x, y0 = p0.y;
    float x1 = p1.x, y1 = p1.y;

    lines.push_back(Point{ x0, y0 ,p0.color});

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

        //interpolateColor Setting
        float factor = sqrt((x0 -p0.x) * (x0 - p0.x) + (y0 - p0.y) * (y0 - p0.y)) / distance;
        Color currentColor = interpolateColor(p0.color, p1.color, factor);
        lines.push_back(Point{ x0, y0 , currentColor});
    }
}

void myKeyboardFunc(unsigned char Key, int x, int y) {
    switch (Key) {
    case 'b':
        std::cout << "clean!" << std::endl;
        drawPointCount = 0;
        lines.clear();
        vertices.clear();
        innerPoints.clear();
        isLoop = false;
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


        //如果 要新增的點 和 首項 相同 代表迴圈
        if (vertices.size() >= 2 && vertices.front().x == px && vertices.front().y == py)
        {
            isLoop = true;
            Line(vertices.back(), vertices.front());
            crow(vertices);
        }
        
        if (!isLoop)
        {
            vertices.push_back(Point{ px, py ,generateRandomColor() });

            printf("Point V%d : (%f, %f)\n", vertices.size(), px, py);

            if (vertices.size() >= 2)
            {
                Line(vertices[vertices.size() - 2], vertices[vertices.size() - 1]);
            }
        } 
    }
    else if (state == GLUT_UP) {

    }
}

void crow(std::vector<Point> vertices) 
{
    //Step1 : Find the vertex with the smallest y value to start
    int iMin = 0;
    int n = vertices.size();
    for (int i = 1; i < n; i++) 
    {
        if (vertices[i].y < vertices[iMin].y)
            iMin = i;
    }

    scanY(vertices, n, iMin);
}

void scanY(std::vector<Point> vertices, int n, int i)
{
    // Step2 : Scan upward maintaining the active edge list

    int li, ri; // left & right upper endpoint indices
    int ly, ry; // left & right upper endpoint y values
    Point l, dl; // current left edge and delta
    Point r, dr; // current right edge and delta

    int rem; //number of remaining vertices: n
    int y; //current scanline

    li = ri = i;
    ly = ry = y = std::ceil(vertices[i].y);

    for (rem = n; rem > 0;)
    {
        // 2.1 find appropriate left edge
        while (ly <= y && rem > 0)
        {
            rem--;
            i = li - 1;
            if (i < 0)
                i = n - 1; // go clockwise

            ly = std::ceil(vertices[i].y);
            if (ly > y) //replace left edge
                differenceY(&vertices[li], &vertices[i], &l, &dl, y);
            li = i; // index of the left endpoint
        }

        // 2.2 find appropriate right edge
        while (ry <= y && rem > 0)
        {
            rem--;
            i = ri + 1;
            if (i >= n)
                i = 0; // go counterclockwise

            ry = std::ceil(vertices[i].y);
            if (ry > y) //replace right edge
                differenceY(&vertices[ri], &vertices[i], &r, &dr, y);
            ri = i; // index of the right endpoint
        }

        // 2.3 while l&r span y(the current scanline)
        for (; y < ly && y < ry; y++)
        {
            scanX(&l, &r, y);//draw the span
            increment(&l, &dl);
            increment(&r, &dr);
        }
    }
}

void differenceY(Point* v1, Point* v2, Point* e, Point * de, int y)
{
    difference(v1, v2, e, de, float(v2->y - v1->y), float(y - v1->y));
}

void differenceX(Point* v1, Point* v2, Point* e, Point* de, int x)
{
    difference(v1, v2, e, de, float(v2->x - v1->x), float(x - v1->x));
}

void difference(Point* v1, Point* v2, Point* e, Point* de, float d, float f)
{
    de->x = (v2->x - v1->x) / d;
    e->x = v1->x + f * de->x;

    de->color.r = (v2->color.r - v1->color.r) / d;
    e->color.r = v1->color.r + f * de->color.r;

    de->color.g = (v2->color.g - v1->color.g) / d;
    e->color.g = v1->color.g + f * de->color.g;

    de->color.b = (v2->color.b - v1->color.b) / d;
    e->color.b = v1->color.b + f * de->color.b;
    
}

void scanX(Point* l, Point* r, int y)
{
    float x, lx, rx;
    Point s, ds;

    lx = ceil(l->x);
    rx = ceil(r->x);
    if (lx < rx)
    {
        differenceX(l, r, &s, &ds, lx);
        for (x = lx; x < rx; x++)
        {
            //float factor = (x - lx) / (rx - lx);
            //Color currentColor = interpolateColor(l->color, r->color, factor);
            //innerPoints.push_back(Point{ x, float(y), currentColor});
            innerPoints.push_back(Point{ x, float(y), Color{s.color.r, s.color.g, s.color.b}});
            increment(&s, &ds);
        }
    }
}

void increment(Point* edge, Point* delta) 
{
    edge->x += delta->x;
    edge->color.r += delta->color.r;
    edge->color.g += delta->color.g;
    edge->color.b += delta->color.b;
}

Color generateRandomColor() 
{
    return Color(static_cast<float>(rand()) / RAND_MAX,
        static_cast<float>(rand()) / RAND_MAX,
        static_cast<float>(rand()) / RAND_MAX);
}


Color interpolateColor(const Color& startColor, const Color& endColor, float factor)
{
    return Color{
        startColor.r + factor * (endColor.r - startColor.r),
        startColor.g + factor * (endColor.g - startColor.g),
        startColor.b + factor * (endColor.b - startColor.b)
    };
}

