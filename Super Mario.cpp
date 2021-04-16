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

const float altezza_prato = height / 3;
const float altezza_pavimento = height / 7;

// Colori utili
const float blu_notte[] = {0, 22.0/255.0, 60.0/255.0, 1};
const float verde_prato[] = {96.0/255.0, 157.0/255.0, 13.0/255.0, 1};
const float bordo_colline[] = { 48.0 / 255.0, 89.0 / 255.0, 36.0 / 255.0, 1 };
const float marrone_pavimento[] = {170.0/255.0, 68.0/255.0, 0, 1};
const float giallo[] = {1, 1, 0, 1};
const float bordo_stelle[] = {1, 1, 0, 0};
const float bianco[] = { 1, 1, 1, 1 };
const float grigio_centro_nuvole[] = {0.4, 0.4, 0.4, 0.8};
const float grigio_bordo_nuvole[] = { 0.5, 0.5, 0.5, 0 };

mat4 Projection;
mat4 Model;
unsigned int matrixProj;
unsigned int matrixModel;

Figura *cielo = NULL;
Figura *prato = NULL;
Figura *pavimento = NULL;
Mario *m = NULL;

const float altezza_mario = 70.0;
float larghezza_mario = 60.0;
const float max_velocita_x_mario = 5.0;

const unsigned int num_stelle = 30;
Figura *stelle[num_stelle] = { NULL };

const float altezza_colline = height * 0.1;
const float larghezza_colline = width * 0.1;
const float velocita_colline = width / (60.0 * 20.0);
const unsigned int num_colline = 2 + (unsigned int) (width/larghezza_colline);
Figura **colline = { NULL };

const float altezza_nuvole = height * 0.1;
const float larghezza_nuvole = width * 0.1;
const float velocita_nuvole = width / (60.0 * 40.0);
const unsigned int num_nuvole = 10;
const unsigned int num_cerchi_per_nuvola = 5;
Figura *nuvole[num_nuvole*num_cerchi_per_nuvola] = { NULL };

float randab(const float min, const float max) {
	return ((float)rand() / (float)RAND_MAX)*(max - min) + min;
}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		m->accelerazione.x = -1.0;
		if (!m->going_left) {
			larghezza_mario *= -1;
			m->posizione.x -= larghezza_mario;
		}
		m->going_left = true;
		break;

	case 'd':
		m->accelerazione.x = 1.0;
		if (m->going_left) {
			larghezza_mario *= -1;
			m->posizione.x -= larghezza_mario;
		}
		m->going_left = false;
		break;

	case 27: // tasto 'ESC'
		exit(0);
		break;

	default:
		break;
	}
}

void keyboardReleasedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a': case 'd':
		m->accelerazione.x = 0.0;
		break;

	default:
		break;
	}
}

void muovi_colline(void) {
	for (unsigned int i = 0; i < num_colline; i++) {

		bool isoutside = true;
		for (unsigned int j = 0; j < colline[i]->nv; j++) {
			colline[i]->v[j].x -= velocita_colline;
			// Controllo se tutti i vertici di una collina sono fuori dallo schermo
			if (colline[i]->v[j].x >= 0.0) isoutside = false;
		}

		// Se la collina è fuori dallo schermo, rientra da destra in una posizione casuale
		if (isoutside) {
			destroyFigure(colline[i]);
			// Ridisegno la collina
			colline[i] = ellisse(width + 10 + randab(0, 5), altezza_prato,
				                 larghezza_colline / 2 + randab(1, 20), altezza_colline + randab(1, 20),
				                 30, PI, 0, verde_prato, bordo_colline);
			loadFigure(colline[i]);
		}
		else {
			reloadFigure(colline[i]);
		}
	}
}

void muovi_nuvole(void) {
	for (unsigned int n = 0; n < num_nuvole; n++) {

		bool isoutside = true;
		for (unsigned int i = 0; i < num_cerchi_per_nuvola; i++) {
			for (unsigned int j = 0; j < nuvole[i]->nv; j++) {
				nuvole[n*num_cerchi_per_nuvola+i]->v[j].x -= velocita_nuvole;
				// Controllo se tutti i vertici di una nuvola sono fuori dallo schermo
				if (nuvole[n*num_cerchi_per_nuvola + i]->v[j].x >= 0.0) isoutside = false;
			}
		}
		

		// Se la collina è fuori dallo schermo, rientra da destra in una posizione casuale
		if (isoutside) {
			const float cx = width + randab(50, 100);
			const float cy = randab(altezza_prato + altezza_nuvole * 2, height - altezza_nuvole * 2);
			for (unsigned int i = 0; i < num_cerchi_per_nuvola; i++) {
				destroyFigure(nuvole[n*num_cerchi_per_nuvola + i]);
				nuvole[n*num_cerchi_per_nuvola + i] = ellisse(cx + randab(-20, 20), cy + randab(-20, 20),
					randab(larghezza_nuvole / num_cerchi_per_nuvola, larghezza_nuvole / 2), randab(altezza_nuvole / num_cerchi_per_nuvola, altezza_nuvole / 2),
					30, 2 * PI, 0, grigio_centro_nuvole, grigio_bordo_nuvole);
				loadFigure(nuvole[n*num_cerchi_per_nuvola + i]);
			}
		}
		else {
			for (unsigned int i = 0; i < num_cerchi_per_nuvola; i++) {
				reloadFigure(nuvole[n*num_cerchi_per_nuvola + i]);
			}
		}
	}
}

void muovi_mario(void) {
	printf("Pos: (%f, %f)\n", m->posizione.x, m->posizione.y);
	printf("Vel: (%f, %f)\n", m->velocita.x, m->velocita.y);
	printf("Acc: (%f, %f)\n", m->accelerazione.x, m->accelerazione.y);
	printf("======\n");

	// Updating position
	m->posizione.x += m->velocita.x;
	m->posizione.y += m->velocita.y;

	// Updating velocity
	m->velocita.x = (m->velocita.x + m->accelerazione.x) * 0.9;
	if (fabs(m->velocita.x) <= 0.1) m->velocita.x = 0.0;
	if (m->velocita.x > max_velocita_x_mario) m->velocita.x = max_velocita_x_mario;
	if (m->velocita.x < -max_velocita_x_mario) m->velocita.x = -max_velocita_x_mario;
	m->velocita.y += m->accelerazione.y;

	// We update acceleration on the events keyPressed and keyReleased, so no need to do it here

	if (m->posizione.y < altezza_pavimento) {
		m->posizione.y = 0.0;
		m->velocita.y = 0.0;
		m->accelerazione.y = 0.0;
	}
}

void update(int value) {
	/*
		This function is called approximately 60 times per second.
		This function updates all objects in the scene.
		The parameter "value" is ignored.
	*/
	//cout << "update" << endl;

	muovi_colline();
	muovi_nuvole();

	muovi_mario();

	glutTimerFunc(60, update, 0);
	glutPostRedisplay();
}

void INIT_VAOs(void)
{
	//cout << "init VAOs" << endl;
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

	// Nuvole
	for (unsigned int n = 0; n < num_nuvole; n++) {
		const float cx = randab(larghezza_nuvole, width-larghezza_nuvole);
		const float cy = randab(altezza_prato+altezza_nuvole*2, height-altezza_nuvole*2);
		
		for (unsigned int i = 0; i < num_cerchi_per_nuvola; i++) {
			nuvole[n*num_cerchi_per_nuvola + i] = ellisse(cx+randab(-20, 20), cy+randab(-20, 20),
				                                          randab(larghezza_nuvole/num_cerchi_per_nuvola, larghezza_nuvole/2), randab(altezza_nuvole / num_cerchi_per_nuvola, altezza_nuvole / 2), 
				                                          30, 2*PI, 0, grigio_centro_nuvole, grigio_bordo_nuvole);
			loadFigure(nuvole[n*num_cerchi_per_nuvola + i]);
		}
	}
	
	// Prato
	prato = rettangolo(0, 1, 0, 1, verde_prato);
	loadFigure(prato);

	// Colline
	colline = (Figura**)malloc(num_colline * sizeof(Figura*));
	assert(colline);
	for (unsigned int i = 0; i < num_colline; i++) {
		colline[i] = ellisse(larghezza_colline * (i-1+0.5) + randab(0, 5), altezza_prato,
			                 larghezza_colline/2 + randab(8, 20), altezza_colline + randab(1, 15),
			                 30, PI, 0, verde_prato, bordo_colline);
		loadFigure(colline[i]);
	}

	// Pavimento
	pavimento = rettangolo(0, 1, 0, 1, marrone_pavimento);
	loadFigure(pavimento);

	// Mario
	m = createMario(1, 1);  // Mario viene scalato dopo
	loadMario(m);
	m->accelerazione = { 0, 0, 0 };
	m->posizione = {0, 0, 0};
	m->velocita = {0, 0, 0};
	m->going_left = false;

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
	//cout << "drawScene" << endl;

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

	// Nuvole
	Model = mat4(1.0);
	Model = scale(Model, vec3(5, 5, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	for (unsigned int i = 0; i < num_nuvole*num_cerchi_per_nuvola; i++) {
		drawFigure(nuvole[i]);
	}
	
	// Prato
	Model = mat4(1.0);
	Model = translate(Model, vec3(0, 0, 0));
	Model = scale(Model, vec3(width, altezza_prato, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawFigure(prato);

	Model = mat4(1.0);
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	for (unsigned int i = 0; i < num_colline; i++) {
		drawFigure(colline[i]);
	}

	// Pavimento
	Model = mat4(1.0);
	Model = translate(Model, vec3(0, 0, 0));
	Model = scale(Model, vec3(width, altezza_pavimento, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawFigure(pavimento);

	// Mario
	Model = mat4(1.0);
	Model = translate(Model, vec3(m->posizione.x, altezza_pavimento + m->posizione.y, 0));
	Model = scale(Model, vec3(larghezza_mario, altezza_mario, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawMario(m);
	
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
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);

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