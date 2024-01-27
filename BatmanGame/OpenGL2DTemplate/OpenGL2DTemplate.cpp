#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <cmath>
#include <time.h>


// Game variables
double backRed = 0;
double backGreen = 0;
double backBlue = 0.5;
int redD = 1;
int greenD = 1;
int blueD = 1;
float bgColor[3] = { 0.0, 0.0, 0.0 };
float targetColor[3] = { 0.0, 0.0, 1.0 };
float transitionSpeed = 0.001;
bool gateVisible = false;
bool gameWon = false;
bool gameLost = false;
float batmanX = 0.0, batmanY = 0.0;
float jokerX = 0.5, jokerY = 0.5;
float jokerSpeed = 0.08;
float jokerDX = 0.02, jokerDY = 0.02;
const float speed = 0.05;
int batmanScore = 0, jokerScore = 0;
const int numObstacles = 3;
const int numCoins = 5;
float coins[numCoins][2];
float gateSize = 0.2;
float gateX = 0.5;
float gateY = 0.5;


struct Obstacle {
    float x, y;
    float size;
};

Obstacle obstacles[numObstacles] = {
    {0.3, 0.3, 0.1},
    {-0.5, 0.2, 0.1},
    {0.1, -0.4, 0.1}
};

void drawCircle(float centerX, float centerY, float radius, int numSegments) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= numSegments; i++) {
        float angle = i * 2.0f * 3.14 / numSegments;
        glVertex2f(centerX + cos(angle) * radius, centerY + sin(angle) * radius);
    }
    glEnd();
}
void updateBackgroundColor() {
    for (int i = 0; i < 3; ++i) {
        if (bgColor[i] < targetColor[i]) {
            bgColor[i] += transitionSpeed;
            if (bgColor[i] > targetColor[i]) {
                bgColor[i] = targetColor[i];
            }
        }
        else if (bgColor[i] > targetColor[i]) {
            bgColor[i] -= transitionSpeed;
            if (bgColor[i] < targetColor[i]) {
                bgColor[i] = targetColor[i];
            }
        }
    }
}


void drawBarrel(float x, float y, float size) {
    float radius = size / 2;
    float height = size;

    glColor3f(0.5f, 0.25f, 0.05f);
    drawCircle(x, y + height / 2, radius, 20);

    drawCircle(x, y - height / 2, radius, 20);

    glBegin(GL_QUADS);
    glVertex2f(x - radius, y - height / 2);
    glVertex2f(x + radius, y - height / 2);
    glVertex2f(x + radius, y + height / 2);
    glVertex2f(x - radius, y + height / 2);
    glEnd();
}

void drawObstacle(const Obstacle& obstacle) {
    drawBarrel(obstacle.x, obstacle.y, obstacle.size);
}

void displayLargeText(float x, float y, const char* string) {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x, y);

    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
}

void displayText(float x, float y, const char* string, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);

    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }
}

void displayIconsAndScores() {
    char scoreStr[30];

    sprintf(scoreStr, "Batman: %d", batmanScore);
    displayText(-0.9, 0.9, scoreStr, 0.0, 0.0, 1.0);

    glBegin(GL_POINTS);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(-0.95, 0.92);
    glEnd();

    sprintf(scoreStr, "Joker: %d", jokerScore);
    displayText(0.7, 0.9, scoreStr, 0.0, 1.0, 0.0);

    glBegin(GL_POINTS);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(0.65, 0.92);
    glEnd();
}

void repositionJoker(int value) {
    jokerX = (rand() % 200 - 100) / 100.0;
    jokerY = (rand() % 200 - 100) / 100.0;
    glutPostRedisplay();
    glutTimerFunc(3000, repositionJoker, 0);
}


void RespObj(float& x, float& y) {
    x = ((float)rand() / RAND_MAX) * 1.8 - 0.9;
    y = ((float)rand() / RAND_MAX) * 1.8 - 0.9;
}


void drawCircleHalf(float centerX, float centerY, float radius, int startAngle, int endAngle) {
    glBegin(GL_POLYGON);
    glVertex2f(centerX, centerY);
    for (int i = startAngle; i <= endAngle; i++) {
        float angle = i * 2.0f * 3.14159f / 360.0f;
        glVertex2f(centerX + cos(angle) * radius, centerY + sin(angle) * radius);
    }
    glEnd();
}

void Batman() {
    float size = 0.1;


    glColor3f(0.0, 0.0, 1.0);
    drawCircle(batmanX, batmanY, 0.12, 32);


    glColor3f(1.0, 0.8, 0.6);
    drawCircle(batmanX, batmanY, 0.1, 32);

    glColor3f(0.0, 0.0, 1.0);
    drawCircleHalf(batmanX, batmanY, 0.1, 0, 180);

    glBegin(GL_TRIANGLES);
    glVertex2f(batmanX - size / 2, batmanY + size);
    glVertex2f(batmanX - size / 4, batmanY + size * 1.5);
    glVertex2f(batmanX, batmanY + size);

    glVertex2f(batmanX, batmanY + size);
    glVertex2f(batmanX + size / 4, batmanY + size * 1.5);
    glVertex2f(batmanX + size / 2, batmanY + size);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    drawCircle(batmanX - 0.03, batmanY + 0.05, 0.02, 16);
    drawCircle(batmanX + 0.03, batmanY + 0.05, 0.02, 16);

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(batmanX - 0.05, batmanY - 0.05);
    glVertex2f(batmanX + 0.05, batmanY - 0.05);
    glEnd();
}


void Coins() {
    glColor3f(1.0, 1.0, 0.0);
    for (int i = 0; i < numCoins; ++i) {
        glPushMatrix();
        glTranslatef(coins[i][0], coins[i][1], 0);
        glutSolidSphere(0.05, 20, 20);
        glPopMatrix();
    }
}



void Joker() {
    glColor3f(0.0, 1.0, 0.0);
    drawCircle(jokerX, jokerY, 0.12, 32);
    glColor3f(1.0, 1.0, 1.0);
    drawCircle(jokerX, jokerY, 0.1, 32);
    glColor3f(0.0, 0.0, 0.0);
    drawCircle(jokerX - 0.03, jokerY + 0.05, 0.02, 16); // Left eye
    drawCircle(jokerX + 0.03, jokerY + 0.05, 0.02, 16); // Right eye

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(jokerX - 0.05, jokerY - 0.05);
    glVertex2f(jokerX + 0.05, jokerY - 0.05);
    glEnd();
}
void drawGate() {
    if (!gateVisible) return;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(gateX - gateSize, gateY - gateSize);
    glVertex2f(gateX + gateSize, gateY - gateSize);
    glVertex2f(gateX + gateSize, gateY + gateSize);
    glVertex2f(gateX - gateSize, gateY + gateSize);
    glEnd();
}



bool CheckC(float x1, float y1, float x2, float y2, float threshold) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt(dx * dx + dy * dy) < threshold;
}
void initGame() {
    srand(time(0));
    for (int i = 0; i < numCoins; ++i) {
        RespObj(coins[i][0], coins[i][1]);
    }
    RespObj(jokerX, jokerY);
}

void display() {
    glClearColor(backRed, backGreen, backBlue, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameLost) {
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        displayLargeText(-0.4, 0.0, "You Lost!");
    }
    else if (gameWon) {
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        displayLargeText(-0.4, 0.0, "Congratulations, You Won!");
    }
    else {
        glClearColor(backRed, backGreen, backBlue, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawGate();
        Batman();
        Coins();
        Joker();
        for (int i = 0; i < numObstacles; i++) {
            drawObstacle(obstacles[i]);
        }
        displayIconsAndScores();
    }
    glFlush();
}
bool CheckColwithObs(float batmanX, float batmanY, float batmanSize, const Obstacle& obstacle) {
    return batmanX + batmanSize > obstacle.x - obstacle.size &&
        batmanX - batmanSize < obstacle.x + obstacle.size &&
        batmanY + batmanSize > obstacle.y - obstacle.size &&
        batmanY - batmanSize < obstacle.y + obstacle.size;
}


void keyboard(unsigned char key, int x, int y) {
    float nextX = batmanX, nextY = batmanY;
    float Cx = batmanX, Cy = batmanY;
    switch (key) {
    case 'w': nextY += speed; break;
    case 's': nextY -= speed; break;
    case 'a': nextX -= speed; break;
    case 'd': nextX += speed; break;
    }
    float halfSize = 0.1;
    if (nextX + halfSize > 1.0) nextX = 1.0 - halfSize;
    if (nextX - halfSize < -1.0) nextX = -1.0 + halfSize;
    if (nextY + halfSize > 1.0) nextY = 1.0 - halfSize;
    if (nextY - halfSize < -1.0) nextY = -1.0 + halfSize;

    for (int i = 0; i < numCoins; ++i) {
        if (CheckC(batmanX, batmanY, coins[i][0], coins[i][1], 0.15)) {
            batmanScore += 1;
            if (batmanScore == 5) {
                gateVisible = true;
            }
            RespObj(coins[i][0], coins[i][1]);
            glutPostRedisplay();
        }
    }
    batmanX = nextX;
    batmanY = nextY;
    if (CheckC(batmanX, batmanY, jokerX, jokerY, 0.2)) {
        batmanX = Cx;
        batmanY = Cy;
        batmanScore -= 5;
        jokerScore += 5;
        if (batmanScore < 0) {
            gameLost = true;
        }
        glutPostRedisplay();
    }
    if (gateVisible && CheckC(batmanX, batmanY, gateX, gateY, gateSize)) {
        gameWon = true;
    }
    for (int i = 0; i < numObstacles; i++) {
        if (CheckColwithObs(nextX, nextY, 0.1, obstacles[i])) {
            batmanX = Cx;
            batmanY = Cy;
            break;
        }
    }

    glutPostRedisplay();
}
void timer(int val)
{
    backRed += 0.1 * redD;
    if (backRed < 0 || backRed>1)
        redD *= -1;

    backGreen += 0.1 * greenD;
    if (backGreen < 0 || backGreen>0.5)
        greenD *= -1;

    backBlue += 0.1 * blueD;
    if (backBlue < 0.5 || backBlue>0.75)
        blueD *= -1;

    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Batman Game");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glLoadIdentity();
    initGame();
    glutTimerFunc(0, timer, 0);
    glutTimerFunc(3000, repositionJoker, 0);
    glutMainLoop();
    return 0;
}

