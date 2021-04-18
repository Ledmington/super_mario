#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <stdbool.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utils.h"

using namespace std;

void assertNoError() {
	GLenum ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		do {
			fprintf(stderr, "\nERROR: %s\n", gluErrorString(ErrorCheckValue));
			ErrorCheckValue = glGetError();
		} while (ErrorCheckValue != GL_NO_ERROR);
		exit(-1);
	}
}

void destroyFigure(Figura *f) {
	if (f != NULL) {
		free(f->v);
		free(f->i);
		free(f);
	}
}

void loadFigure(Figura *fig) {
	assert(fig);
	
	unsigned int EBO;

	glGenVertexArrays(1, &(fig->VAO));
	assertNoError();
	glBindVertexArray(fig->VAO);
	assertNoError();
	glGenBuffers(1, &(fig->VBO));
	assertNoError();
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO);
	assertNoError();
	glBufferData(GL_ARRAY_BUFFER, fig->nv * sizeof(Vertex), &(fig->v[0]), GL_STATIC_DRAW);
	assertNoError();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	assertNoError();
	glEnableVertexAttribArray(0);
	assertNoError();
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	assertNoError();
	glEnableVertexAttribArray(1);
	assertNoError();
	glGenBuffers(1, &EBO);
	assertNoError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	assertNoError();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fig->ni * sizeof(unsigned int), &(fig->i[0]), GL_STATIC_DRAW);
	assertNoError();
	glBindVertexArray(0);
	assertNoError();
}

void reloadFigure(Figura *fig) {
	assert(fig);

	//glBindVertexArray(fig->VAO);
	assertNoError();
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO);
	assertNoError();
	glBufferData(GL_ARRAY_BUFFER, fig->nv * sizeof(Vertex), &(fig->v[0]), GL_STATIC_DRAW);
	assertNoError();
	glBindVertexArray(0);
	assertNoError();
}

void drawFigure(Figura *fig) {
	assert(fig);

	glBindVertexArray(fig->VAO);
	assertNoError();
	//glBindBuffer(GL_ARRAY_BUFFER, fig->VBO);
	glDrawElements(GL_TRIANGLES, fig->ni, GL_UNSIGNED_INT, 0);
	assertNoError();
	//glDrawArrays(GL_POINTS, 0, fig->nv);
	glBindVertexArray(0);
	assertNoError();
}

Figura* ellisse(const float cx, const float cy, const float rx, const float ry,
				const unsigned int nPunti,
				const float angolo_settore,
				const float rotazione,
	            const float* colore_centro,
	            const float* colore_bordo) {

	assert(rx > 0);
	assert(ry > 0);
	assert(nPunti >= 2);
	assert(angolo_settore >= -1e-6 && angolo_settore <= 2*PI+1e-6);
	assert(colore_centro);
	assert(colore_bordo);

	Figura *fig = (Figura*)malloc(sizeof(Figura));
	assert(fig);
	
	/*
		nPunti è il numero di punti sulla circonferenza.
		Il numero di vertici univoci da disegnare è quindi nPunti + 1 perchè
		sono quelli sulla circonferenza più il centro.
		Il numero di indici è il triplo del numero di triangoli, quindi 3*(nPunti-1)
	*/
	fig->nv = nPunti + 2;  // tutti i punti più il centro (più quello finale)
	fig->ni = 3 * nPunti;  // tre per ogni triangolo

	fig->v = (Vertex*)malloc(fig->nv * sizeof(Vertex));
	assert(fig->v);
	fig->i = (unsigned int*)malloc(fig->ni * sizeof(unsigned int));
	assert(fig->i);
	
	float step = angolo_settore / (float)nPunti;

	// Vertici
	fig->v[0] = {cx, cy, 0, colore_centro[0], colore_centro[1], colore_centro[2], colore_centro[3]};
	for (unsigned int i = 1; i < fig->nv; i++) {
		float x = cos((float)(i - 1)*step)*rx;
		float y = sin((float)(i - 1)*step)*ry;
		fig->v[i].x = cx + x*cos(rotazione) - y*sin(rotazione);
		fig->v[i].y = cy + x*sin(rotazione) + y*cos(rotazione);
		fig->v[i].z = 0;
		fig->v[i].r = colore_bordo[0];
		fig->v[i].g = colore_bordo[1];
		fig->v[i].b = colore_bordo[2];
		fig->v[i].a = colore_bordo[3];
	}

	// Indici
	for (unsigned int i = 0; i < fig->ni / 3; i++) {
		fig->i[3 * i] = 0;
		fig->i[3 * i + 1] = i + 1;
		fig->i[3 * i + 2] = i + 2;
	}

	return fig;
}

Figura* rettangolo(const Point *points, const float* colore) {
	assert(points);
	assert(colore);

	Figura *fig = (Figura*)malloc(sizeof(Figura));
	assert(fig);

	fig->nv = 4;
	fig->ni = 6;

	fig->v = (Vertex*)malloc(fig->nv * sizeof(Vertex));
	assert(fig->nv);
	fig->i = (unsigned int*)malloc(fig->ni * sizeof(unsigned int));
	assert(fig->i);

	for (int i = 0; i < 4; i++) {
		fig->v[i].x = points[i].x;
		fig->v[i].y = points[i].y;
		fig->v[i].z = points[i].z;
		fig->v[i].r = colore[0];
		fig->v[i].g = colore[1];
		fig->v[i].b = colore[2];
		fig->v[i].a = colore[3];
	}

	fig->i[0] = 0;
	fig->i[1] = 1;
	fig->i[2] = 2;
	fig->i[3] = 1;
	fig->i[4] = 2;
	fig->i[5] = 3;

	return fig;
}

Figura* rettangolo(const float xmin, const float xmax, const float ymin, const float ymax, const float* colore) {
	assert(xmin <= xmax);
	assert(ymin <= ymax);
	assert(colore);

	Figura *fig = (Figura*)malloc(sizeof(Figura));
	assert(fig);

	fig->nv = 4;
	fig->ni = 6;

	fig->v = (Vertex*)malloc(fig->nv * sizeof(Vertex));
	assert(fig->nv);
	fig->i = (unsigned int*)malloc(fig->ni * sizeof(unsigned int));
	assert(fig->i);

	fig->v[0] = { xmin, ymin, 0 };
	fig->v[1] = { xmin, ymax, 0 };
	fig->v[2] = { xmax, ymin, 0 };
	fig->v[3] = { xmax, ymax, 0 };

	for (unsigned int i = 0; i < fig->nv; i++) {
		fig->v[i].r = colore[0];
		fig->v[i].g = colore[1];
		fig->v[i].b = colore[2];
		fig->v[i].a = colore[3];
	}

	fig->i[0] = 0;
	fig->i[1] = 1;
	fig->i[2] = 2;
	fig->i[3] = 1;
	fig->i[4] = 2;
	fig->i[5] = 3;

	return fig;
}

Figura* triangolo(const Point* punti, const float* colore) {
	assert(punti);

	Figura *fig = (Figura*)malloc(sizeof(Figura));
	assert(fig);

	fig->nv = 3;
	fig->ni = 3;

	fig->v = (Vertex*)malloc(fig->nv * sizeof(Vertex));
	assert(fig->nv);
	fig->i = (unsigned int*)malloc(fig->ni * sizeof(unsigned int));
	assert(fig->i);

	for (unsigned int i = 0; i < fig->nv; i++) {
		fig->v[i].x = punti[i].x;
		fig->v[i].y = punti[i].y;
		fig->v[i].z = punti[i].z;
		fig->v[i].r = colore[0];
		fig->v[i].g = colore[1];
		fig->v[i].b = colore[2];
		fig->v[i].a = colore[3];
		
		fig->i[i] = i;
	}

	return fig;
}

void flipAroundX(Figura *fig, const float cx) {
	for (unsigned int i = 0; i < fig->nv; i++) {
		float dist = fig->v[i].x - cx;
		fig->v[i].x = cx - dist;
	}
}

Figura* stella(const float cx, const float cy,
	           const float Rx, const float Ry,
	           const float rx, const float ry,
	           const unsigned int nPunte, const float rotazione, const float* colore_centro, const float* colore_bordo) {
	assert(Rx > 0); assert(Ry > 0);
	assert(rx > 0); assert(ry > 0);
	assert(nPunte > 4);
	assert(colore_centro);
	assert(colore_bordo);

	Figura *fig = (Figura*)malloc(sizeof(Figura));
	assert(fig);

	fig->nv = nPunte * 2 + 1;  // il doppio delle punte della stella più il centro
	fig->ni = 3 * (fig->nv - 1);  // tre per ogni vertice esterno (quindi escluso il centro)

	fig->v = (Vertex*) malloc(fig->nv * sizeof(Vertex));
	assert(fig->v);
	fig->i = (unsigned int*) malloc(fig->ni * sizeof(unsigned int));
	assert(fig->i);

	float step = 2 * (float)PI / (float)(fig->nv - 1);

	// Vertici
	fig->v[0] = { cx, cy, 0, colore_centro[0], colore_centro[1], colore_centro[2], colore_centro[3]};
	for (unsigned int i = 1; i < fig->nv; i++) {
		float x, y;
		if (i % 2 == 0) {
			x = cos((float)(i - 1)*step)*Rx;
			y = sin((float)(i - 1)*step)*Ry;
		}
		else {
			x = cos((float)(i - 1)*step)*rx;
			y = sin((float)(i - 1)*step)*ry;
		}
		fig->v[i].x = cx + x*cos(rotazione) - y*sin(rotazione);
		fig->v[i].y = cy + x*sin(rotazione) + y*cos(rotazione);
		fig->v[i].z = 0;
		fig->v[i].r = colore_bordo[0];
		fig->v[i].g = colore_bordo[1];
		fig->v[i].b = colore_bordo[2];
		fig->v[i].a = colore_bordo[3];
	}

	// Indici
	for (unsigned int i = 0; i < fig->ni / 3; i++) {
		fig->i[3 * i] = 0;
		fig->i[3 * i + 1] = i + 1;
		fig->i[3 * i + 2] = (i+2>=fig->nv) ? ((i+2)%(fig->nv-1)) : i+2;
	}

	return fig;
}