#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#define M_PI 3.14159265358979323846

// ------------------ CONFIG --------------------
const int numStars = 1000;
const float radius = 50.0f;

int windowWidth = 800;
int windowHeight = 600;

// Estrelas
float stars[numStars][3];

// Controle da câmera
GLfloat cameraAngleVertical = 45.0f;
GLfloat cameraAngleHorizontal = 0.0f;
float cameraDistance = 30.0f;   // distância da câmera

struct Planet {
    const char* name;
    float orbitRadius;
    float size;
    float angle;
    bool selected;
};

Planet planets[] = {
    {"Mercurio", 2.0f, 0.2f, 0.0f, false},
    {"Venus",    3.0f, 0.3f, 0.0f, false},
    {"Terra",    4.0f, 0.35f, 0.0f, false},
    {"Marte",    5.0f, 0.25f, 0.0f, false},
    {"Jupiter",  7.0f, 0.6f, 0.0f, false},
    {"Saturno",  9.0f, 0.5f, 0.0f, false},
    {"Urano",   11.0f, 0.4f, 0.0f, false},
    {"Netuno",  13.0f, 0.4f, 0.0f, false}
};

int numPlanets = 8;
GLfloat angleMoon = 0.0f;

// ------------------ BLINK CONTROL --------------------
bool blinkVisible[8] = { false };
int blinkCount[8] = { 0 };
bool blinking[8] = { false };

void blinkTimer(int planetIndex);
void startBlink(int planetIndex);

// ------------------ FUNÇÕES AUX --------------------
void initStars() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < numStars; i++) {
        float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
        float phi = static_cast<float>(rand()) / RAND_MAX * M_PI;
        stars[i][0] = radius * sin(phi) * cos(theta);
        stars[i][1] = radius * sin(phi) * sin(theta);
        stars[i][2] = radius * cos(phi);
    }
}

void drawStars() {
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numStars; i++)
        glVertex3f(stars[i][0], stars[i][1], stars[i][2]);
    glEnd();
}

void drawOrbitLine(float radius) {
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_LOOP);
    int numSegments = 100;
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * M_PI * i / numSegments;
        glVertex3f(radius * cos(theta), 0.0f, radius * sin(theta));
    }
    glEnd();
}

// ------------------ PROJEÇÃO --------------------
void projectToScreen(float x, float y, float z, int& sx, int& sy) {
    GLdouble model[16], proj[16];
    GLint viewport[4];
    GLdouble winX, winY, winZ;
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluProject(x, y, z, model, proj, viewport, &winX, &winY, &winZ);
    sx = (int)winX;
    sy = (int)winY;
}

// ------------------ DESENHO MIRA --------------------
void drawSelectionBox(float x, float y, float z) {
    int sx, sy;
    projectToScreen(x, y, z, sx, sy);

    int boxSize = 40; // tamanho fixo
    int gap = boxSize / 4;
    int lineLen = (boxSize - gap) / 2;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // canto superior esquerdo
    glVertex2i(sx - boxSize / 2, sy + boxSize / 2); glVertex2i(sx - boxSize / 2 + lineLen, sy + boxSize / 2);
    glVertex2i(sx - boxSize / 2, sy + boxSize / 2); glVertex2i(sx - boxSize / 2, sy + boxSize / 2 - lineLen);
    // canto superior direito
    glVertex2i(sx + boxSize / 2, sy + boxSize / 2); glVertex2i(sx + boxSize / 2 - lineLen, sy + boxSize / 2);
    glVertex2i(sx + boxSize / 2, sy + boxSize / 2); glVertex2i(sx + boxSize / 2, sy + boxSize / 2 - lineLen);
    // canto inferior esquerdo
    glVertex2i(sx - boxSize / 2, sy - boxSize / 2); glVertex2i(sx - boxSize / 2 + lineLen, sy - boxSize / 2);
    glVertex2i(sx - boxSize / 2, sy - boxSize / 2); glVertex2i(sx - boxSize / 2, sy - boxSize / 2 + lineLen);
    // canto inferior direito
    glVertex2i(sx + boxSize / 2, sy - boxSize / 2); glVertex2i(sx + boxSize / 2 - lineLen, sy - boxSize / 2);
    glVertex2i(sx + boxSize / 2, sy - boxSize / 2); glVertex2i(sx + boxSize / 2, sy - boxSize / 2 + lineLen);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ------------------ BLINK --------------------
void blinkTimer(int planetIndex) {
    if (blinking[planetIndex]) {
        blinkVisible[planetIndex] = !blinkVisible[planetIndex];
        if (!blinkVisible[planetIndex]) blinkCount[planetIndex]++;
        if (blinkCount[planetIndex] >= 3) {
            blinking[planetIndex] = false;
            blinkVisible[planetIndex] = true;
        }
        else {
            glutTimerFunc(333, blinkTimer, planetIndex);
        }
    }
}

void startBlink(int planetIndex) {
    blinking[planetIndex] = true;
    blinkCount[planetIndex] = 0;
    blinkVisible[planetIndex] = true;
    glutTimerFunc(333, blinkTimer, planetIndex);
}

// ------------------ DESENHO INFORMAÇÕES 2D --------------------
void drawTextInfo(const Planet* p) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int x0 = windowWidth - 200;
    int y0 = windowHeight - 20;
    glColor3f(1.0f, 1.0f, 1.0f);

    std::stringstream ss;
    ss << "Nome: " << p->name;
    glRasterPos2i(x0, y0);
    for (char c : ss.str()) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    ss.str(""); ss.clear();
    ss << "Raio: " << p->orbitRadius;
    glRasterPos2i(x0, y0 - 20);
    for (char c : ss.str()) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    ss.str(""); ss.clear();
    ss << "Tamanho: " << p->size;
    glRasterPos2i(x0, y0 - 40);
    for (char c : ss.str()) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ------------------ RENDER --------------------
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // câmera fixa
    float camX = 0.0f, camY = 15.0f, camZ = cameraDistance;
    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);

    glRotatef(cameraAngleVertical, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleHorizontal, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -20.0f);
    drawStars();
    glPopMatrix();

    for (int i = 0; i < numPlanets; i++)
        drawOrbitLine(planets[i].orbitRadius);

    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(1.0f, 50, 50);

    float colors[][3] = {
        {0.8f,0.8f,0.8f}, {0.9f,0.6f,0.2f}, {0.2f,0.4f,0.8f}, {0.8f,0.2f,0.1f},
        {0.8f,0.6f,0.2f}, {0.8f,0.7f,0.2f}, {0.2f,0.8f,0.8f}, {0.1f,0.2f,0.8f}
    };

    Planet* selectedPlanet = nullptr;

    for (int i = 0; i < numPlanets; i++) {
        float angleRad = planets[i].angle * M_PI / 180.0f;
        float x = planets[i].orbitRadius * cos(angleRad);
        float z = planets[i].orbitRadius * sin(angleRad);

        glPushMatrix();
        glTranslatef(x, 0.0f, z);
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glutSolidSphere(planets[i].size, 30, 30);

        if (i == 2) { // Lua da Terra
            drawOrbitLine(0.5f);
            glPushMatrix();
            glRotatef(angleMoon, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.5f, 0.0f, 0.0f);
            glColor3f(0.7f, 0.7f, 0.7f);
            glutSolidSphere(0.1f, 30, 30);
            glPopMatrix();
        }

        if (i == 5) { // Anéis de Saturno
            glPushMatrix();
            glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
            glColor3f(0.6f, 0.6f, 0.6f);
            int numSegments = 100; float innerR = 0.6f; float outerR = 0.8f;
            glBegin(GL_QUAD_STRIP);
            for (int s = 0; s <= numSegments; s++) {
                float theta = 2.0f * M_PI * s / numSegments;
                glVertex3f(innerR * cos(theta), innerR * sin(theta), 0.0f);
                glVertex3f(outerR * cos(theta), outerR * sin(theta), 0.0f);
            }
            glEnd();
            glPopMatrix();
        }

        glPopMatrix();

        if (planets[i].selected && blinkVisible[i]) {
            drawSelectionBox(x, 0.0f, z);
            selectedPlanet = &planets[i];
        }
    }

    if (selectedPlanet) {
        drawTextInfo(selectedPlanet);
    }

    glutSwapBuffers();
}

// ------------------ UPDATE --------------------
void update(int value) {
    for (int i = 0; i < numPlanets; i++)
        planets[i].angle += (i == 0 ? 0.8f : i == 1 ? 0.4f : i == 2 ? 0.2f : i == 3 ? 0.1f :
            i == 4 ? 0.05f : i == 5 ? 0.03f : i == 6 ? 0.015f : 0.01f);
    angleMoon += 2.0f;

    glutPostRedisplay();
    glutTimerFunc(50, update, 0);
}

// ------------------ RESHAPE --------------------
void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// ------------------ CONTROLE CÂMERA --------------------
void specialFunc(int key, int, int) {
    switch (key) {
    case GLUT_KEY_UP: cameraAngleVertical += 1.0f; break;
    case GLUT_KEY_DOWN: cameraAngleVertical -= 1.0f; break;
    case GLUT_KEY_LEFT: cameraAngleHorizontal -= 1.0f; break;
    case GLUT_KEY_RIGHT: cameraAngleHorizontal += 1.0f; break;
    }
    glutPostRedisplay();
}

// ------------------ CLIQUE --------------------
void mouseClick(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int selectedIndex = -1;
        float minDist = 1e6;

        for (int i = 0; i < numPlanets; i++) {
            float angleRad = planets[i].angle * M_PI / 180.0f;
            float x = planets[i].orbitRadius * cos(angleRad);
            float z = planets[i].orbitRadius * sin(angleRad);

            int sx, sy;
            projectToScreen(x, 0.0f, z, sx, sy);

            int dx = mx - sx;
            int dy = (windowHeight - my) - sy;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist < 50 && dist < minDist) {
                selectedIndex = i;
                minDist = dist;
            }
        }

        for (int i = 0; i < numPlanets; i++) {
            planets[i].selected = (i == selectedIndex);
            blinking[i] = false;
            blinkVisible[i] = false;
            blinkCount[i] = 0;
        }

        if (selectedIndex != -1)
            startBlink(selectedIndex);
    }
}

// ------------------ CONTROLE TECLADO NORMAL --------------------
void keyboardFunc(unsigned char key, int, int) {
    switch (key) {
    case 'z': // Aproximar a câmera
        cameraDistance -= 1.0f;
        if (cameraDistance < 5.0f) cameraDistance = 5.0f; // limite mínimo
        break;
    case 'x': // Afastar a câmera
        cameraDistance += 1.0f;
        if (cameraDistance > 100.0f) cameraDistance = 100.0f; // limite máximo
        break;
    }
    glutPostRedisplay();
}

// ------------------ MAIN --------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Sistema Solar");

    initStars();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialFunc);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseClick);
    glutTimerFunc(50, update, 0);

    glutMainLoop();
    return 0;
}
