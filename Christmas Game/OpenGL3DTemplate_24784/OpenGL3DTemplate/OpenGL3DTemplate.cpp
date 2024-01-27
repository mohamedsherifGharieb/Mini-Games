#include <math.h>
#include <cmath>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 0.0f, float eyeY = 3.0f, float eyeZ = 10.0f,
		float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f,
		float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}


	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};
Camera camera;

float colorPhase = 0.0f;
float colorChangeSpeed = 0.01f;
bool isPresentRotating = false;
float treeRotation = 0.0f;
float presentRotation = 0.0f;
void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}
void print(int x, int y, char* string)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	int len, i;
	glColor3f(1.0f, 1.0f, 1.0f); 
	glRasterPos2i(x, y);
	len = (int)strlen(string);

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}
float wallColor[3] = { 0.1f, 0.1f, 0.1f }; // Initial wall color
void changeWallColor(int value) {
	// Change wall color
	wallColor[0] = static_cast<float>(rand()) / RAND_MAX;
	wallColor[1] = static_cast<float>(rand()) / RAND_MAX;
	wallColor[2] = static_cast<float>(rand()) / RAND_MAX;

	// Redraw the scene
	glutPostRedisplay();

	// Set up the next timer
	glutTimerFunc(1000, changeWallColor, 0);
}
void drawScene() {
	// Draw Ground
	glBegin(GL_QUADS);
	glColor3f(0.8f, 0.8f, 1.0f); // Pale blue (R, G, B)
	glVertex3f(-10.0f, 0.0f, -10.0f); // Bottom-left vertex
	glVertex3f(-10.0f, 0.0f, 10.0f);  // Top-left vertex
	glVertex3f(10.0f, 0.0f, 10.0f);   // Top-right vertex
	glVertex3f(10.0f, 0.0f, -10.0f);  // Bottom-right vertex
	glEnd();

	// Draw Walls (Fences)
	glColor3f(0.5f, 0.35f, 0.05f); // Brown color for the wooden parts

	float fenceHeight = 0.6f; // Height of the fence
	float postDistance = 1.5f; // Distance between posts
	colorPhase += colorChangeSpeed;

	// Calculate RGB components based on the color phase
	float fenceColorR = sin(0.3f * colorPhase + 0.0f) * 0.5f + 0.5f;
	float fenceColorG = sin(0.3f * colorPhase + 2.0f) * 0.5f + 0.5f;
	float fenceColorB = sin(0.3f * colorPhase + 4.0f) * 0.5f + 0.5f;

	glColor3f(fenceColorR, fenceColorG, fenceColorB);
	// Draw fences along the edges
	for (float z = -10.0f; z <= 10.0f; z += 20.0f) {
		for (float x = -10.0f; x <= 10.0f; x += postDistance) {
			glPushMatrix();
			glTranslatef(x, 0.0f, z);
			glScalef(0.1f, fenceHeight, 0.1f);
			glutSolidCube(1.0f);
			glPopMatrix();
		}
	}

	for (float x = -10.0f; x <= 10.0f; x += 20.0f) {
		for (float z = -10.0f; z <= 10.0f; z += postDistance) {
			glPushMatrix();
			glTranslatef(x, 0.0f, z);
			glScalef(0.1f, fenceHeight, 0.1f);
			glutSolidCube(1.0f);
			glPopMatrix();
		}
	}



}
int secondsLeft = 29;
bool gameOver = false;
bool playerWon = false;
float headAngle = 0.0f;
float headSpeed = 1.0f;
float cartPosition = 0.0f;

void DrawTime() {
	char timeString[20];
	sprintf(timeString, "Time: %02d", secondsLeft);
	print(5, 455, timeString);
}
void Timer(int value) {
	if (!gameOver) {
		if (secondsLeft == 0) {
			gameOver = true;
		}
		else {
			secondsLeft--;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(1000, Timer, 0);
}
bool HP1 = false;
bool HP2 = false;
bool HP3 = false;
class Player {
public:
	float x, y, z;
	float size;
	int playerDirection;

	Player(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _size = 0.5f) {
		x = _x;
		y = _y;
		z = _z;
		size = _size;
		playerDirection = 0;
	}

	void draw() {
		// Body
		glColor3f(0.8f, 0.2f, 0.2f); // Red color 
		glPushMatrix();
		glTranslatef(x, size, z);
		glRotatef(playerDirection, 0, 1, 0);
		glScalef(1.0f, 1.2f, 0.5f);
		glutSolidCube(size * 0.7); // Body
		glPopMatrix();

		// Head
		glColor3f(1.0f, 0.8f, 0.6f); // Light brown color for the head
		glPushMatrix();
		glTranslatef(x, size * 1.6, z);
		glRotatef(playerDirection, 0, 1, 0);
		glutSolidSphere(size * 0.3, 20, 20); // Head

		// Hat
		glPushMatrix();
		glColor3f(0.95f, 0.95f, 1.0f); // Blue color
		glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate cylinder to stand upright
		GLUquadricObj* quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluCylinder(quadric, size * 0.3, 0.0, 0.4, 32, 32);
		gluDeleteQuadric(quadric);
		glTranslatef(x, size * 1.7, z); // Place the hat relative to the head
		glPopMatrix();

		// Eyes
		glColor3f(0.0f, 0.0f, 0.0f); // Black color for the eyes
		glPushMatrix();
		glTranslatef(-size * 0.1, size * 0.1, size * 0.25);
		glutSolidSphere(size * 0.05, 10, 10); // Left Eye
		glTranslatef(size * 0.2, 0.0f, 0.0f);
		glutSolidSphere(size * 0.05, 10, 10); // Right Eye
		glPopMatrix();


		// Left arm
		glColor3f(0.0f, 0.0f, 0.0f); // Dark gray color for the arms
		glPushMatrix();
		glTranslatef(0 - size * 0.4, -0.1f, 0);
		glRotatef(45, 0.0f, 0.0f, 1.0f); // Rotate cylinder to stand upright
		glScalef(0.2f, 0.7f, 0.2f);
		glutSolidCube(0.5); // Left arm
		glPopMatrix();

		// Right arm
		glPushMatrix();
		glTranslatef(0 + size * 0.4, -0.1f, 0);
		glRotatef(-45, 0.0f, 0.0f, 1.0f); // Rotate cylinder to stand upright
		glScalef(0.2f, 0.7f, 0.2f);
		glutSolidCube(0.5); // Right arm
		glPopMatrix();


		glPopMatrix();
	}

	void move(float dx, float dz) {
		// Check if the new position is within the boundaries
		if (x + dx >= -10.0f && x + dx <= 10.0f && z + dz >= -10.0f && z + dz <= 10.0f) {
			x += dx;
			z += dz;


			if (x <= -7.5f && x >= -8.5f && z >= 0.5f && z <= 1.5f) {
				HP1 = true;
				glutPostRedisplay();
			}
			if (x >= 0.5f && x <= 1.5f && z >= 3.5f && z <= 4.5f) {
				HP2 = true;
				glutPostRedisplay();
			}
			if (x >= 5.5f && x <= 6.5f && z >= 2.5f && z <= 3.5f) {
				HP3 = true;
				glutPostRedisplay();
			}
			if (HP1 == true && HP2 == true && HP3 == true) {
				playerWon = true;
				glutPostRedisplay();
			}
		}
	}
};
Player player; // Global player instance
//for in place animation
float scaleFactor = 1.0f;
bool scalingUp = true;
bool animate = false;
void update(int value) {
	if (scalingUp) {
		scaleFactor += 0.005f;
		if (scaleFactor >= 1.1f) {
			scalingUp = false;
		}
	}
	else {
		scaleFactor -= 0.005f;
		if (scaleFactor <= 0.9f) {
			scalingUp = true;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}
void drawSnowflakes() {
	glColor3f(1.0f, 1.0f, 1.0f);
	glPointSize(7.0f);
	int numFlakes = 1000; // Number of snowflakes
	glBegin(GL_POINTS); // Begin drawing points
	for (int i = 0; i < numFlakes; ++i) {
		float x = ((float)rand() / RAND_MAX) * 100.0f - 50.0f; // Random x-coordinate in [-50, 50]
		float y = ((float)rand() / RAND_MAX) * 100.0f - 50.0f; // Random y-coordinate in [-50, 50]
		float z = ((float)rand() / RAND_MAX) * 100.0f - 50.0f; // Random z-coordinate in [-50, 50]
		glVertex3f(x, y, z); // Draw snowflakes at random positions
	}
	glEnd(); // End drawing points
}

void drawSantaCart(float x, float y, float z) {
	static float cartAnimationX = 0.0f;

	float CartX = x;
	float CartY = y;
	float CartZ = z;

	if (animate) {
		float moveRangeX = 0.5f;
		float moveSpeedX = 0.05f;
		CartX += moveRangeX * sin(cartAnimationX);
		cartAnimationX += moveSpeedX;
	}
	glColor3f(0.8f, 0.2f, 0.2f); // Red color for the base
	glPushMatrix();
	glTranslatef(CartX, CartY, CartZ);
	glScalef(1.5f, 0.2f, 0.8f);
	glutSolidCube(1.0f); // Draw the base of the cart
	glPopMatrix();

	// Sides of the cart
	glColor3f(0.5f, 0.3f, 0.1f); // Brown color for the sides
	glPushMatrix();
	glTranslatef(CartX, CartY + 0.15f, CartZ + 0.35f);
	glScalef(1.5f, 0.3f, 0.1f);
	glutSolidCube(1.0f); // Draw one side of the cart
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CartX, CartY + 0.15f, CartZ - 0.35f);
	glScalef(1.5f, 0.3f, 0.1f);
	glutSolidCube(1.0f); // Draw the other side of the cart
	glPopMatrix();

	// Wheels of the cart
	glColor3f(0.1f, 0.1f, 0.1f); // Black color for the wheels
	glPushMatrix();
	glTranslatef(CartX + 0.6f, CartY - 0.1f, CartZ + 0.3f);
	glutSolidTorus(0.05f, 0.1f, 10, 10); // Draw one wheel
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CartX + 0.6f, CartY - 0.1f, CartZ - 0.3f);
	glutSolidTorus(0.05f, 0.1f, 10, 10); // Draw the other wheel
	glPopMatrix();
}

void drawTree(float x, float y, float z) {
	static float treeAnimationZ = 0.0f; // Animation variable for Z-axis movement

	float treex = x;
	float treey = y;
	float treez = z;

	if (animate) {
		float moveRangeZ = 2.0f; // Set the range of movement along Z-axis
		float moveSpeedZ = 0.05f; // Set the speed of movement along Z-axis
		treez += moveRangeZ * sin(treeAnimationZ); // Update Z-coordinate based on sine function
		treeAnimationZ += moveSpeedZ; // Incremen
	}

	// Trunk of the tree (cylinder)
	glColor3f(0.5f, 0.35f, 0.05f); // Brown color for the trunk
	glPushMatrix();
	glRotatef(treeRotation, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
	glTranslatef(treex, treey, treez); // Position the trunk at the specified coordinates
	glScalef(0.1f, 1.0f, 0.1f); // Adjust the size of the trunk
	glutSolidCube(1.0f); // Draw the trunk of the tree
	glPopMatrix();

	// Foliage of the tree (cones)
	glColor3f(0.0f, 0.5f, 0.0f); // Green color for foliage

	glPushMatrix();
	glRotatef(treeRotation, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
	glTranslatef(treex, treey + 0.3f, treez); // Position the first cone at the top
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate to align with Y-axis
	glutSolidCone(0.4f, 1.0f, 10, 3); // Draw the first cone
	glPopMatrix();

	glPushMatrix();
	glRotatef(treeRotation, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis

	glTranslatef(treex, treey + 0.6f, treez); // Position the second cone above the first
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate to align with Y-axis
	glutSolidCone(0.3f, 0.8f, 10, 3); // Draw the second cone
	glPopMatrix();

	glPushMatrix();
	glRotatef(treeRotation, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
	glTranslatef(treex, treey + 0.9f, treez); // Position the third cone above the second
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate to align with Y-axis
	glutSolidCone(0.2f, 0.6f, 10, 3); // Draw the third cone
	glPopMatrix();
}
void present1(float x, float y, float z) {
	float presentSpeed = 0.05f; // Speed of the floating effect
	float floatRange = 0.5f; // Range of the floating effect
	float yOffset = 0.0;

	if (animate) {
		presentRotation += 1.0f; // Increment rotation angle over time
		yOffset = floatRange * sin(presentSpeed * glutGet(GLUT_ELAPSED_TIME) * 0.001f);

		}
	if (HP1) {
		return;
	}
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for the box
	glPushMatrix();
	glTranslatef(x, y + yOffset, z); // Position the present at the specified coordinates
	glRotatef(presentRotation, 0.0f, 1.0f, 0.0f); // Apply rotation around the y-axis
	glScalef(0.3f, 0.3f, 0.3f); // Adjust the size of the present box
	glutSolidCube(1.0f); // Draw the present box
	glPopMatrix();

	// Ribbon on top of the present
	glColor3f(0.0f, 1.0f, 0.0f); // Green color for the ribbon
	glPushMatrix();
	glTranslatef(x, y + 0.15f + yOffset, z); // Position the ribbon slightly above the box
	glRotatef(presentRotation, 0.0f, 1.0f, 0.0f); // Apply the same rotation
	glScalef(0.4f, 0.05f, 0.4f); // Adjust the size of the ribbon
	glutSolidCube(1.0f); // Draw the ribbon
	glPopMatrix();
}
void present2(float x, float y, float z) {
	float presentSpeed = 0.05f; // Speed of the floating effect
	float floatRange = 0.5f; // Range of the floating effect
	float yOffset = 0.0;

	if (animate) {
		presentRotation += 1.0f; // Increment rotation angle over time
		yOffset = floatRange * sin(presentSpeed * glutGet(GLUT_ELAPSED_TIME) * 0.001f);
	}
	if (HP2) {
		return;
	}
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for the box
	glPushMatrix();
	glTranslatef(x, y + yOffset, z); // Position the present at the specified coordinates
	glRotatef(presentRotation, 0.0f, 1.0f, 0.0f); // Apply rotation around the y-axis
	glScalef(0.3f, 0.3f, 0.3f); // Adjust the size of the present box
	glutSolidCube(1.0f); // Draw the present box
	glPopMatrix();

	// Ribbon on top of the present
	glColor3f(0.0f, 1.0f, 0.0f); // Green color for the ribbon
	glPushMatrix();
	glTranslatef(x, y + 0.15f + yOffset, z); // Position the ribbon slightly above the box
	glRotatef(presentRotation, 0.0f, 1.0f, 0.0f); // Apply the same rotation
	glScalef(0.4f, 0.05f, 0.4f); // Adjust the size of the ribbon
	glutSolidCube(1.0f); // Draw the ribbon
	glPopMatrix();
}
void present3(float x, float y, float z) {
	float presentSpeed = 0.05f; // Speed of the floating effect
	float floatRange = 0.5f; // Range of the floating effect
	float yOffset = 0.0;
	if (animate) {
		presentRotation += 1.0f; // Increment rotation angle over time
		 yOffset = floatRange * sin(presentSpeed * glutGet(GLUT_ELAPSED_TIME) * 0.001f);

	}
	if (HP3) {
		return;
	}

	glColor3f(1.0f, 0.0f, 0.0f); // Red color for the box
	glPushMatrix();
	glTranslatef(x, y + yOffset, z); // Position the present at the specified coordinates
	glRotatef(presentRotation, 0.0f, 1.0f, 0.0f); // Apply rotation around the y-axis
	glScalef(0.3f, 0.3f, 0.3f); // Adjust the size of the present box
	glutSolidCube(1.0f); // Draw the present box
	glPopMatrix();

	// Ribbon on top of the present
	glColor3f(0.0f, 1.0f, 0.0f); // Green color for the ribbon
	glPushMatrix();
	glTranslatef(x, y + 0.15f + yOffset, z); // Position the ribbon slightly above the box
	glRotatef(presentRotation, 0.0f, 1.0f, 0.0f); // Apply the same rotation
	glScalef(0.4f, 0.05f, 0.4f); // Adjust the size of the ribbon
	glutSolidCube(1.0f); // Draw the ribbon
	glPopMatrix();
}

void drawReindeer() {
	// Body of the reindeer
	glColor3f(0.6f, 0.3f, 0.0f); // Brown color for the body
	glPushMatrix();
	glTranslatef(5.0f, 0.23f, -7.0f); // Adjusted position in front of the cart
	glScalef(0.6f, 0.3f, 0.3f);
	glutSolidCube(1.0f); // Draw the body of the reindeer
	glPopMatrix();

	// Legs of the reindeer
	glColor3f(0.4f, 0.2f, 0.0f); // Darker brown color for the legs

	// Front left leg
	glPushMatrix();
	glTranslatef(4.8f, 0.06f, -6.92f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Front right leg
	glPushMatrix();
	glTranslatef(5.2f, 0.06f, -6.92f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Back left leg
	glPushMatrix();
	glTranslatef(4.8f, 0.06f, -7.08f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Back right leg
	glPushMatrix();
	glTranslatef(5.2f, 0.06f, -7.08f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Head of the reindeer
	glColor3f(0.6f, 0.3f, 0.0f); // Brown color for the head
	glPushMatrix();
	glTranslatef(5.4f, 0.33f, -7.0f);
	glRotatef(headAngle, 1.0f, 0.0f, 0.0f); // Apply nodding motion to the head
	glutSolidCube(0.2f); // Use a cube for simplicity, replace with desired head shape
	glPopMatrix();

	// Eyes
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for the eyes
	glPushMatrix();
	glTranslatef(5.32f, 0.38f, -6.85f);
	glutSolidSphere(0.025f, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5.32f, 0.38f, -7.15f);
	glutSolidSphere(0.025f, 10, 10);
	glPopMatrix();

	// Horns (sticks)
	glColor3f(0.5f, 0.3f, 0.0f); // Horn color
	glPushMatrix();
	glTranslatef(5.35f, 0.4f, -6.95f);
	glRotatef(45.0f, 1.0f, 1.0f, 0.0f); // Adjust rotation for one horn
	glScalef(0.025f, 0.25f, 0.025f);
	glutSolidCube(1.0f); // Left horn
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5.35f, 0.4f, -7.05f);
	glRotatef(-45.0f, 1.0f, 1.0f, 0.0f); // Adjust rotation for the other horn
	glScalef(0.025f, 0.25f, 0.025f);
	glutSolidCube(1.0f); // Right horn
	glPopMatrix();
}
void drawSnowman() {

	// Draw body parts of the snowman
	glColor3f(1.0f, 1.0f, 1.0f); // Snowman color (white)

	// Bottom sphere (body)
	glPushMatrix();
	glTranslatef(0.0f, 0.15f, 7.0f); // Position above the ground
	glutSolidSphere(0.25f, 20, 20); // Radius, slices, stacks
	glPopMatrix();

	// Middle sphere (torso)
	glPushMatrix();
	glTranslatef(0.0f, 0.40f, 7.0f); // Position above the bottom sphere
	glutSolidSphere(0.2f, 20, 20); // Radius, slices, stacks
	glPopMatrix();

	// Top sphere (head)
	glPushMatrix();
	glTranslatef(0.0f, 0.60f, 7.0f); // Position above the middle sphere
	glutSolidSphere(0.15f, 20, 20); // Radius, slices, stacks
	glPopMatrix();



	// Draw eyes with wooden stick hands
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for eyes and stick hands

	// Eyes
	glPushMatrix();
	glTranslatef(-0.05f, 0.65f, 7.12f); // Position of left eye
	glutSolidSphere(0.015f, 10, 10); // Left eye
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.05f, 0.65f, 7.12f); // Position of right eye
	glutSolidSphere(0.015f, 10, 10); // Right eye
	glPopMatrix();

	// Stick hands
	glPushMatrix();
	glTranslatef(-0.20f, 0.45f, 7.0f); // Position of left stick hand
	glRotatef(-45.0f, 0.0f, 0.0f, 1.0f); // Rotate for a diagonal stick hand
	glScalef(0.03f, 0.3f, 0.03f); // Lengthen and thin the stick
	glutSolidCube(1.0f); // Left stick hand
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.20f, 0.45f, 7.0f); // Position of right stick hand
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f); // Rotate for a diagonal stick hand
	glScalef(0.03f, 0.3f, 0.03f); // Lengthen and thin the stick
	glutSolidCube(1.0f); // Right stick hand
	glPopMatrix();

}

void DrawGameOver(char* string) {
	print(290, 240, string);
}
void Display() {
	setupCamera();
	setupLights();
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
	player.draw();
	drawSnowman();
	drawReindeer();
	drawSnowflakes();
	present1(-8, 0.1, 1.0);//
	present2(1.0, 0.1, 4.0);
	present3(6.0, 0.1, 3.0);
	drawTree(-2, 0.15, -4);
	drawTree(-7, 0.15, 1);
	drawTree(9, 0.15, 3);

	drawSantaCart(3.5, 0, -7);
	DrawTime();
	if (gameOver) {
		if (playerWon) {
			DrawGameOver("You Won");
		}
		else {
			DrawGameOver("You Lost");
		}
	}


	glFlush();
}
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.5;
	float movement = 0.2;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

		// Player movement
	case '8':
		if (gameOver) break;
		player.playerDirection = 180;
		player.move(0.0, -movement);
		break;
	case '5':
		if (gameOver) break;
		player.playerDirection = 0;
		player.move(0.0, movement);
		break;
	case '4':
		if (gameOver) break;
		player.playerDirection = 270;
		player.move(-movement, 0.0);
		break;
	case '6':
		if (gameOver) break;
		player.playerDirection = 90;
		player.move(movement, 0.0);
		break;
	case '1':
		camera = Camera(0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
		break;
	case '2':
		camera = Camera(0.0f, 1.5f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		break;
	case '3':
		camera = Camera(15.0f, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		break;
	case 'y':
		animate = !animate;
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}
void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Christmas_Game");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutTimerFunc(1000, Timer, 0); // Start the timer
	glutTimerFunc(0, changeWallColor, 0); // Start the timer immediately
	glutTimerFunc(25, update, 0); // Start the animation of objects
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glutMainLoop();
}