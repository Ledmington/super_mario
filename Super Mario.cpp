#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "goomba.h"
#include "mario.h"
#include "block.h"

unsigned int programId;

const float ratio = 16.0 / 9.0;
const unsigned int height = 500;  // put 720 for 720p
const unsigned int width = (unsigned int)((float)height * ratio);
const vec3 center = {(float)width/2, (float)height/2, 0};

const float altezza_prato = height / 2;
const float altezza_pavimento = height / 7;

// Colori utili
const float blu_notte[] = {0, 22.0/255.0, 60.0/255.0, 1};
const float verde_prato[] = {96.0/255.0, 157.0/255.0, 13.0/255.0, 1};
const float marrone_pavimento[] = {170.0/255.0, 68.0/255.0, 0, 1};
const float giallo[] = {1, 1, 0, 1};
const float bordo_stelle[] = {1, 1, 0, 0};
const float bianco[] = { 1, 1, 1, 1 };

mat4 Projection;
mat4 Model;
unsigned int matrixProj;
unsigned int matrixModel;

Figura *cielo = NULL;
Figura *prato = NULL;
Figura *pavimento = NULL;

const unsigned int num_stelle = 20;
const float velocita_stelle = width / (60.0 * 60.0); // ogni stella percorre tutta la larghezza dello schermo in un minuto (a 60fps)
Figura *stelle[num_stelle] = { NULL };

float randab(const float min, const float max) {
	return ((float)rand() / (float)RAND_MAX)*(max - min) + min;
}

void muovi_stelle(void) {
	for (unsigned int i = 0; i < num_stelle; i++) {
		if (stelle[2 * i] == NULL || stelle[2 * i + 1] == NULL) continue;
		// Controllo sia la stella che l'alone luminoso
		bool isoutside = true;
		for (unsigned int j = 0; j < stelle[2*i]->nv; j++) {
			stelle[2*i]->v[j].x -= velocita_stelle;
			// Controllo se tutti i vertici di una stella sono fuori dallo schermo
			if (stelle[2*i]->v[j].x >= 0.0) isoutside = false;
		}
		for (unsigned int j = 0; j < stelle[2*i+1]->nv; j++) {
			stelle[2*i+1]->v[j].x -= velocita_stelle;
			// Controllo se tutti i vertici di una stella sono fuori dallo schermo
			if (stelle[2*i+1]->v[j].x >= 0.0) isoutside = false;
		}
		// Se la stella è fuori dallo schermo, rientra da destra in una posizione casuale
		if (isoutside) {
			destroyFigure(stelle[2 * i]);
			destroyFigure(stelle[2 * i + 1]);
			const float cx = width + 10 + rand()%30;
			const float cy = randab(altezza_prato, height);
			const float angle = randab(0, 2 * PI);
			const float r = randab(2, 3);
			// Alone luminoso
			stelle[2 * i] = stella(cx, cy, r * 4, r * 4, r * 2, r * 2, 5, angle, giallo, bordo_stelle);
			loadFigure(stelle[2 * i]);
			// la stella vera e propria
			stelle[2 * i + 1] = stella(cx, cy, r * 2, r * 2, r, r, 5, angle, bianco, giallo);
			loadFigure(stelle[2 * i + 1]);
		}
		else {
			reloadFigure(stelle[2 * i]);
			reloadFigure(stelle[2 * i + 1]);
		}
	}
}

void update(int value) {
	/*
		This function is called approximately 60 times per second.
		This function updates all objects in the scene.
		The parameter "value" is ignored.
	*/
	cout << "update" << endl;

	muovi_stelle();

	glutTimerFunc(60, update, 0);
	glutPostRedisplay();
}

void INIT_VAOs(void)
{
	cout << "init VAOs" << endl;
	assertNoError();

	char* vertexShader = (char*) "vertexShader.glsl";
	char* fragmentShader = (char*) "fragmentShader.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	// Cielo
	cielo = rettangolo(0, 1, 0, 1, blu_notte);
	loadFigure(cielo);

	// Stelle
	for (unsigned int i = 0; i < num_stelle; i++) {
		const float cx = randab(0, width);
		const float cy = randab(altezza_prato, height);
		const float angle = randab(0, 2 * PI);
		const float raggio = randab(2, 3);
		// Per ogni stella, disegno prima l'alone luminoso
		stelle[2 * i] = stella(cx, cy, raggio*4, raggio*4, raggio*2, raggio*2, 5, angle, giallo, bordo_stelle);
		loadFigure(stelle[2 * i]);
		// Poi la stella vera e propria
		stelle[2 * i + 1] = stella(cx, cy, raggio*2, raggio*2, raggio, raggio, 5, angle, bianco, giallo);
		loadFigure(stelle[2 * i + 1]);
	}
	
	// Prato
	prato = rettangolo(0, 1, 0, 1, verde_prato);
	loadFigure(prato);

	// Pavimento
	pavimento = rettangolo(0, 1, 0, 1, marrone_pavimento);
	loadFigure(pavimento);

	glClearColor(1, 1, 1, 1);

	/*
		Usiamo la matrice ortogonale e non quella "perspective" perchè siamo in 2D
		e non vogliamo che gli oggetti lontani siano renderizzati più piccoli.

		Little help:
		con la matrice ortogonale, la telecamera è un piano;
		con la matrice "perspective", la telecamera è un punto.
	*/
	Projection = ortho(0.0f, (float)width, 0.0f, (float)height);
}

void drawScene(void)
{
	cout << "drawScene" << endl;

	glUniformMatrix4fv((GLint)matrixProj, 1, GL_FALSE, value_ptr(Projection));
	assertNoError();
	glClear(GL_COLOR_BUFFER_BIT);
	assertNoError();

	// Cielo
	Model = mat4(1.0);
	Model = translate(Model, vec3(0, altezza_prato, 0));
	Model = scale(Model, vec3(width, height-altezza_prato, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawFigure(cielo);
	
	// Stelle
	Model = mat4(1.0);
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	for (unsigned int i = 0; i < 2 * num_stelle; i++) {
		drawFigure(stelle[i]);
	}
	
	// Prato
	Model = mat4(1.0);
	Model = translate(Model, vec3(0, 0, 0));
	Model = scale(Model, vec3(width, altezza_prato, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawFigure(prato);

	// Pavimento
	Model = mat4(1.0);
	Model = translate(Model, vec3(0, 0, 0));
	Model = scale(Model, vec3(width, altezza_pavimento, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawFigure(pavimento);
	
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// GLUT_DOUBLE usa un double buffer
	// GLUT_RGBA usa l'encoding dei colori RGBA
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Super Mario");
	glutDisplayFunc(drawScene);

	glutTimerFunc(60, update, 0);
	
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();
	/*
		glewInit apparentemente genera un errore GL_INVALID_ENUM anche se tutti i parametri sono corretti,
		e funziona comunque. Quindi eliminiamo l'errore dalla coda degli errori e lo ignoriamo.
	*/

	INIT_VAOs();
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//Questo identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	matrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//Questo identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	matrixModel = glGetUniformLocation(programId, "Model");
	assertNoError();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	assertNoError();

	glutMainLoop();

	return 0;
}