#include "utils.h"
#include "goomba.h"

#include <stdlib.h>
#include <assert.h>
#include <iostream>

using namespace std;

static float marrone[] = { 169.0f/255.0f, 85.0f/255.0f, 0.0f, 1.0f };
static float rosa[] = { 254.0f / 255.0f, 186.0f / 255.0f, 128.0f / 255.0f, 1.0f };
static float nero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
static float bianco[] = { 1.0f, 1.0f, 1.0f, 1.0f };

Goomba* createGoomba(const float width, const float height) {
	Goomba *g = (Goomba*)malloc(sizeof(Goomba));
	assert(g);

	const float cx = width / 2;
	const float cy = height / 2;

	// Testa
	const float r = width * 0.8f;
	g->testa[0] = ellisse(cx, cy, r, r, 6, 2 * (float)PI, 0, marrone, marrone);
	g->testa[1] = ellisse(cx, cy+r*sqrtf(3.0f)/2.0f, r/2.0f, r/2.0f, 30, (float)PI, 0, marrone, marrone);
	g->testa[2] = ellisse(cx+r*3/4, cy-r*sqrtf(3.0f)/4.0f, r/2, r/2, 30, (float)PI, -(float)PI / 3 * 2, marrone, marrone);
	g->testa[3] = ellisse(cx-r*3/4, cy-r*sqrtf(3.0f)/4.0f, r/2, r/2, 30, (float)PI, (float)PI / 3 * 2, marrone, marrone);

	// Corpo
	g->corpo = ellisse(cx, cy-r*sqrtf(3.0f)/2.0f, r*0.6f, r*0.6f, 30, (float)PI, (float)PI, rosa, rosa);

	// Piedi
	float rpiccolo = height * 0.2f;
	float rgrande = height * 0.3f;
	float ymin = cy - height * 1.6f;
	float xmin = cx + rgrande - rpiccolo;
	float lunghezza = rpiccolo + rgrande + width * 0.5f;
	g->piedi[0] = ellisse(xmin+rpiccolo, ymin+rpiccolo, rpiccolo, rpiccolo, 30, (float)PI, (float)PI/2, nero, nero);
	g->piedi[1] = ellisse(xmin+lunghezza-rgrande, ymin+rgrande, rgrande, rgrande, 30, (float)PI, -(float)PI/2, nero, nero);
	Point p[] = {xmin+rpiccolo, ymin, 0,
	             xmin+rpiccolo, ymin+2*rpiccolo, 0,
	             xmin+lunghezza-rgrande, ymin, 0,
				 xmin + lunghezza - rgrande, ymin + 2 * rgrande, 0};
	g->piedi[2] = rettangolo(p, nero);
	xmin = cx - rgrande + rpiccolo;
	g->piedi[3] = ellisse(xmin-rpiccolo, ymin+rpiccolo, rpiccolo, rpiccolo, 30, (float)PI, -(float)PI / 2, nero, nero);
	g->piedi[4] = ellisse(xmin-lunghezza+rgrande, ymin+rgrande, rgrande, rgrande, 30, (float)PI, (float)PI / 2, nero, nero);
	p[0].x = p[1].x = xmin - rpiccolo;
	p[2].x = p[3].x = xmin - lunghezza + rgrande;
	g->piedi[5] = rettangolo(p, nero);

	// Occhi
	float larghezza_occhi = width * 0.3f;
	float altezza_occhi = height * 0.7f;
	float larghezza_pupille = width * 0.2f;
	float altezza_pupille = height * 0.5f;
	float distanza_occhi = width * 0.1f;
	float centro_occhio_dx = cx + distanza_occhi + larghezza_occhi / 2.0f;
	float centro_occhio_sx = cx - distanza_occhi - larghezza_occhi / 2.0f;
	g->occhi[0] = ellisse(centro_occhio_dx, cy, larghezza_occhi/2, altezza_occhi/2, 30, (float)PI*2, 0, bianco, bianco);
	g->occhi[1] = ellisse(centro_occhio_sx, cy, larghezza_occhi/2, altezza_occhi/2, 30, (float)PI * 2, 0, bianco, bianco);
	g->occhi[2] = ellisse(centro_occhio_dx, cy, larghezza_pupille/2, altezza_pupille/2, 30, (float)PI * 2, 0, nero, nero);
	g->occhi[3] = ellisse(centro_occhio_sx, cy, larghezza_pupille/2, altezza_pupille/2, 30, (float)PI * 2, 0, nero, nero);
	Point sopracciglio[] = {cx, cy+altezza_pupille*0.8f, 0,
	                        cx+distanza_occhi, cy+altezza_pupille/2, 0,
	                        cx+2*larghezza_occhi, cy+altezza_pupille, 0,
							cx+distanza_occhi+2*larghezza_occhi, cy+altezza_pupille*0.8f, 0};
	g->occhi[4] = rettangolo(sopracciglio, nero);
	// Inverto x rispetto a cx
	g->occhi[5] = rettangolo(sopracciglio, nero);
	flipAroundX(g->occhi[5], cx);

	return g;
}

void loadGoomba(Goomba *g) {
	for (int i = 0; i < 4; i++) {
		loadFigure(g->testa[i]);
	}
	loadFigure(g->corpo);
	for (int i = 0; i < 6; i++) {
		loadFigure(g->piedi[i]);
	}
	for (int i = 0; i < 6; i++) {
		loadFigure(g->occhi[i]);
	}
}

void drawGoomba(Goomba *g) {
	for (int i = 0; i < 4; i++) {
		drawFigure(g->testa[i]);
	}
	drawFigure(g->corpo);
	for (int i = 0; i < 6; i++) {
		drawFigure(g->piedi[i]);
	}
	for (int i = 0; i < 6; i++) {
		drawFigure(g->occhi[i]);
	}
}

void destroyGoomba(Goomba *g) {
	for (int i = 0; i < 4; i++) {
		destroyFigure(g->testa[i]);
	}
	destroyFigure(g->corpo);
	for (int i = 0; i < 6; i++) {
		destroyFigure(g->piedi[i]);
	}
	for (int i = 0; i < 6; i++) {
		destroyFigure(g->occhi[i]);
	}
	free(g);
}