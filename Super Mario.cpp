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

const float ratio = 16.0f / 9.0f;
const unsigned int height = 500;  // put 720 for 720p
const unsigned int width = (unsigned int)((float)height * ratio);
const vec3 center = {(float)width/2, (float)height/2, 0};

const float altezza_prato = height / 3;
const float altezza_pavimento = height / 7;

// Colori utili
const float blu_notte[] = {0, 22.0f/255.0f, 60.0f/255.0f, 1};
const float verde_prato[] = {96.0f/255.0f, 157.0f/255.0f, 13.0f/255.0f, 1};
const float bordo_colline[] = { 48.0f / 255.0f, 89.0f / 255.0f, 36.0f / 255.0f, 1 };
const float marrone_pavimento[] = {170.0f/255.0f, 68.0f/255.0f, 0, 1};
const float giallo[] = {1, 1, 0, 1};
const float bordo_stelle[] = {1, 1, 0, 0};
const float bianco[] = { 1, 1, 1, 1 };
const float grigio_centro_nuvole[] = { 0.4f, 0.4f, 0.4f, 0.8f };
const float grigio_bordo_nuvole[] = { 0.5f, 0.5f, 0.5f, 0 };

mat4 Projection;
mat4 Model;
unsigned int matrixProj;
unsigned int matrixModel;

Figura *cielo = NULL;
Figura *prato = NULL;
Figura *pavimento = NULL;
Mario *m = NULL;
Goomba *g = NULL;
Block *b = NULL;

const float max_velocita_x_mario = 5.0;

float velocita_goomba = 3.0;

const unsigned int num_stelle = 50;
Figura *stelle[2*num_stelle] = { NULL };  // Ogni stella ha il corpo e l'alone luminoso

const float altezza_colline = height * 0.1f;
const float larghezza_colline = width * 0.1f;
const float velocita_colline = width / (60.0f * 20.0f);
const unsigned int num_colline = 2 + (unsigned int) (width/larghezza_colline);
Figura **colline = { NULL };

const float altezza_nuvole = height * 0.1f;
const float larghezza_nuvole = width * 0.1f;
const float velocita_nuvole = width / (60.0f * 40.0f);
const unsigned int num_nuvole = 10;
const unsigned int num_cerchi_per_nuvola = 5;
Figura *nuvole[num_nuvole*num_cerchi_per_nuvola] = { NULL };

float randab(const float min, const float max) {
	return ((float)rand() / (float)RAND_MAX)*(max - min) + min;
}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	if (!m->alive || m->dying) return;

	switch (key)
	{
	case 'a':
		m->accelerazione.x = -1.0;
		if (!m->going_left) {
			m->dimensione.x *= -1;
			m->posizione.x -= m->dimensione.x;
		}
		m->going_left = true;
		break;

	case 'd':
		m->accelerazione.x = 1.0;
		if (m->going_left) {
			m->dimensione.x *= -1;
			m->posizione.x -= m->dimensione.x;
		}
		m->going_left = false;
		break;

	case 'w':
		if (!m->jumping) {
			m->accelerazione.y = 3.0;
		}
		m->jumping = true;
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

	case 'w':
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
				                 30, (float)PI, 0.0f, verde_prato, bordo_colline);
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
					30, 2 * (float)PI, 0, grigio_centro_nuvole, grigio_bordo_nuvole);
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
	if (!m->alive) return;

	if (m->dying) {
		m->dimensione.y -= 1.0;
		if (m->dimensione.y <= 0) m->alive = false;
		return;
	}

	// Updating position
	m->posizione.x += m->velocita.x;
	m->posizione.y += m->velocita.y;

	// Updating velocity
	m->velocita.x = (m->velocita.x + m->accelerazione.x) * 0.9f;
	if (fabs(m->velocita.x) <= 0.1f) m->velocita.x = 0.0f;
	if (m->velocita.x > max_velocita_x_mario) m->velocita.x = max_velocita_x_mario;
	if (m->velocita.x < -max_velocita_x_mario) m->velocita.x = -max_velocita_x_mario;
	m->velocita.y += m->accelerazione.y;

	// We update x-acceleration on the events keyPressed and keyReleased, so no need to do it here
	m->accelerazione.y -= 0.4f;

	if (m->posizione.y < 0.0f) {
		m->posizione.y = 0.0f;
		m->velocita.y = 0.0f;
		m->accelerazione.y = 0.0f;
		m->jumping = false;
	}
	
	if (fminf(m->posizione.x+m->dimensione.x, m->posizione.x) < 0) {
		if (m->dimensione.x < 0) {
			m->posizione.x = m->dimensione.x * -1;
		}
		else {
			m->posizione.x = 0;
		}
		m->velocita.x = fmaxf(0, m->velocita.x);
		m->accelerazione.x = fmaxf(0, m->velocita.x);
	}
	if (fmaxf(m->posizione.x + m->dimensione.x, m->posizione.x) > width) {
		if (m->dimensione.x > 0) {
			m->posizione.x = width - m->dimensione.x;
		}
		else {
			m->posizione.x = width;
		}
		m->velocita.x = fminf(0, m->velocita.x);
		m->accelerazione.x = fminf(0, m->velocita.x);
	}
}

void muovi_goomba(void) {
	if (!g->alive) return;

	if (g->dying) {
		g->dimensione.y -= 1.0;
		g->posizione.y -= 1.0;

		if (g->dimensione.y <= 0.0) g->alive = false;
	}
	else {
		g->posizione.x += velocita_goomba;

		if (g->posizione.x <= 10.0 || g->posizione.x >= width - 30.0) velocita_goomba *= -1;
	}
}

void check_mario_goomba(void) {
	if (!m->alive || m->dying || !g->alive || g->dying) return;

	const Point centro_mario = { (2 * m->posizione.x + m->dimensione.x) / 2,
								 (2 * m->posizione.y + m->dimensione.y) / 2,
								 0 };
	const float destra_mario = fmaxf(m->posizione.x, m->posizione.x + m->dimensione.x);
	const float sinistra_mario = fminf(m->posizione.x, m->posizione.x + m->dimensione.x);

	const float top_goomba = g->posizione.y + g->dimensione.y;
	const float destra_goomba = g->posizione.x + g->dimensione.x / 2;
	const float sinistra_goomba = g->posizione.x - g->dimensione.x / 2;

	const float dist_x = fabs(centro_mario.x - g->posizione.x);
	const float dist_y = fabs(centro_mario.y - g->posizione.y);

	// Se si toccano
	if (destra_mario >= sinistra_goomba &&
		sinistra_mario <= destra_goomba &&
		dist_x <= (fabs(m->dimensione.x)+g->dimensione.x)/2 &&
		dist_y <= (m->dimensione.y+g->dimensione.y)/2) {

		/*
			Se Mario è sopra il Goomba,
			Mario salta e il Goomba si schiaccia.
		*/
		if (dist_y > dist_x) {
			m->accelerazione.y = fmaxf(3.0, m->accelerazione.y + 3.0);
			m->velocita.y = fmaxf(0, m->velocita.y);
			g->dying = true;
		}
		// Altrimenti Mario si schiaccia e muore
		else {
			m->accelerazione.x = m->accelerazione.y = 0;
			m->velocita.x = m->velocita.y = 0;
			m->dying = true;
			velocita_goomba *= -1;
		}
	}
}

void check_mario_blocco(void) {
	if (!m->alive || m->dying) return;

	const Point centro_mario = { (2 * m->posizione.x + m->dimensione.x) / 2,
								 (2 * m->posizione.y + m->dimensione.y) / 2,
								 0 };

	const float top_mario = m->posizione.y + m->dimensione.y;
	const float bot_mario = m->posizione.y;
	const float destra_mario = fmaxf(m->posizione.x, m->posizione.x + m->dimensione.x);
	const float sinistra_mario = fminf(m->posizione.x, m->posizione.x + m->dimensione.x);

	const float top_blocco = b->posizione.y + b->dimensione.y;
	const float bot_blocco = b->posizione.y;
	const float destra_blocco = b->posizione.x + b->dimensione.x;
	const float sinistra_blocco = b->posizione.x;

	// Se si toccano da sotto
	if (top_mario >= bot_blocco &&
		bot_mario <= bot_blocco &&
		destra_mario >= sinistra_blocco &&
		sinistra_mario <= destra_blocco) {
		m->posizione.y = bot_blocco - m->dimensione.y;
		m->velocita.y = fminf(0, m->velocita.y);
		m->accelerazione.y = fminf(0, m->accelerazione.y);

		if (!b->broken) {
			destroyBlock(b);
			b = createBlock(1, 1, true, true);
			loadBlock(b);
			b->posizione = { 400, 100, 0 };
			b->dimensione = { 50, 50, 0 };
		}
	}
	// Se si toccano da sinistra
	else if (destra_mario >= sinistra_blocco &&
		sinistra_mario <= sinistra_blocco && 
		bot_mario <= top_blocco &&
		top_mario >= bot_blocco) {
		m->posizione.x = (m->dimensione.x < 0) ? sinistra_blocco : sinistra_blocco - m->dimensione.x;
		m->velocita.x = fminf(0, m->velocita.x);
		m->accelerazione.x = fminf(0, m->accelerazione.x);
	}
	// Se si toccano da destra
	else if (sinistra_mario <= destra_blocco &&
		     destra_mario >= destra_blocco &&
		     bot_mario <= top_blocco &&
		     top_mario >= bot_blocco) {
		m->posizione.x = (m->dimensione.x < 0) ? destra_blocco - m->dimensione.x : destra_blocco;
		m->velocita.x = fmaxf(0, m->velocita.x);
		m->accelerazione.x = fmaxf(0, m->accelerazione.x);
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
	muovi_goomba();

	check_mario_goomba();
	check_mario_blocco();

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

	// Nuvole
	for (unsigned int n = 0; n < num_nuvole; n++) {
		const float cx = randab(larghezza_nuvole, width - larghezza_nuvole);
		const float cy = randab(altezza_prato + altezza_nuvole * 2, height - altezza_nuvole * 2);

		for (unsigned int i = 0; i < num_cerchi_per_nuvola; i++) {
			nuvole[n*num_cerchi_per_nuvola + i] = ellisse(cx + randab(-20, 20), cy + randab(-20, 20),
				randab(larghezza_nuvole / num_cerchi_per_nuvola, larghezza_nuvole / 2), randab(altezza_nuvole / num_cerchi_per_nuvola, altezza_nuvole / 2),
				30, 2 * (float)PI, 0, grigio_centro_nuvole, grigio_bordo_nuvole);
			loadFigure(nuvole[n*num_cerchi_per_nuvola + i]);
		}
	}

	// Stelle
	for (unsigned int i = 0; i < num_stelle; i++) {
		const float cx = randab(0, width);
		const float cy = randab(altezza_prato, height);
		const float angle = randab(0, 2 * (float)PI);
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

	// Colline
	colline = (Figura**)malloc(num_colline * sizeof(Figura*));
	assert(colline);
	for (unsigned int i = 0; i < num_colline; i++) {
		colline[i] = ellisse(larghezza_colline * (i-1+0.5f) + randab(0, 5), altezza_prato,
			                 larghezza_colline/2 + randab(8, 20), altezza_colline + randab(1, 15),
			                 30, (float)PI, 0, verde_prato, bordo_colline);
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
	m->dimensione = {60, 70, 0};
	m->going_left = false;
	m->jumping = false;
	m->alive = true;
	m->dying = false;
	
	// Goomba
	g = createGoomba(1, 1); // il Goomba viene scalato dopo
	loadGoomba(g);
	g->posizione = { width*3/4, 20, 0 };
	g->dimensione = { 20, 20, 0 };
	g->alive = true;
	g->dying = false;

	// Blocco
	b = createBlock(1, 1, true, false);
	loadBlock(b);
	b->posizione = { 400, 100, 0 };
	b->dimensione = { 50, 50, 0 };
	
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

	// Nuvole
	Model = mat4(1.0);
	Model = scale(Model, vec3(5, 5, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	for (unsigned int i = 0; i < num_nuvole*num_cerchi_per_nuvola; i++) {
		drawFigure(nuvole[i]);
	}

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
	Model = scale(Model, vec3(m->dimensione.x, m->dimensione.y, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawMario(m);
	
	// Goomba
	Model = mat4(1.0);
	Model = translate(Model, vec3(g->posizione.x, g->posizione.y + altezza_pavimento, 0));
	Model = scale(Model, vec3(g->dimensione.x, g->dimensione.y, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawGoomba(g);

	// Blocco
	Model = mat4(1.0);
	Model = translate(Model, vec3(b->posizione.x, b->posizione.y + altezza_pavimento, 0));
	Model = scale(Model, vec3(b->dimensione.y, b->dimensione.y, 0));
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, value_ptr(Model));
	drawBlock(b);
	
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));
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