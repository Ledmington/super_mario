#include "utils.h"
#include "goomba.h"

#include <stdlib.h>
#include <assert.h>
#include <iostream>

using namespace std;

static float marrone[] = { 169.0/255.0, 85.0/255.0, 0, 1 };
static float rosa[] = { 254.0 / 255.0, 186.0 / 255.0, 128.0 / 255.0, 1 };
static float nero[] = { 0, 0, 0, 1 };
static float bianco[] = { 1, 1, 1, 1 };

Goomba* createGoomba(const float width, const float height) {
	Goomba *g = (Goomba*)malloc(sizeof(Goomba));
	assert(g);

	const float cx = width / 2;
	const float cy = height / 2;

	// Testa
	const float r = width * 0.8;
	g->testa[0] = ellisse(cx, cy, r, r, 6, 2 * (float)PI, 0, marrone, marrone);
	g->testa[1] = ellisse(cx, cy+r*sqrt(3)/2, r/2, r/2, 30, (float)PI, 0, marrone, marrone);
	g->testa[2] = ellisse(cx+r*3/4, cy-r*sqrt(3)/4, r/2, r/2, 30, (float)PI, -(float)PI / 3 * 2, marrone, marrone);
	g->testa[3] = ellisse(cx-r*3/4, cy-r*sqrt(3)/4, r/2, r/2, 30, (float)PI, (float)PI / 3 * 2, marrone, marrone);

	// Corpo
	g->corpo = ellisse(cx, cy-r*sqrt(3)/2, r*0.6, r*0.6, 30, (float)PI, (float)PI, rosa, rosa);

	// Piedi
	float rpiccolo = height * 0.2;
	float rgrande = height * 0.3;
	float ymin = cy - height * 1.6;
	float xmin = cx + rgrande - rpiccolo;
	float lunghezza = rpiccolo + rgrande + width * 0.5;
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
	float larghezza_occhi = width * 0.3;
	float altezza_occhi = height * 0.7;
	float larghezza_pupille = width * 0.2;
	float altezza_pupille = height * 0.5;
	float distanza_occhi = width * 0.1;
	float centro_occhio_dx = cx + distanza_occhi + larghezza_occhi / 2;
	float centro_occhio_sx = cx - distanza_occhi - larghezza_occhi / 2;
	g->occhi[0] = ellisse(centro_occhio_dx, cy, larghezza_occhi/2, altezza_occhi/2, 30, (float)PI*2, 0, bianco, bianco);
	g->occhi[1] = ellisse(centro_occhio_sx, cy, larghezza_occhi/2, altezza_occhi/2, 30, (float)PI * 2, 0, bianco, bianco);
	g->occhi[2] = ellisse(centro_occhio_dx, cy, larghezza_pupille/2, altezza_pupille/2, 30, (float)PI * 2, 0, nero, nero);
	g->occhi[3] = ellisse(centro_occhio_sx, cy, larghezza_pupille/2, altezza_pupille/2, 30, (float)PI * 2, 0, nero, nero);
	Point sopracciglio[] = {cx, cy+altezza_pupille*0.8, 0,
	                        cx+distanza_occhi, cy+altezza_pupille/2, 0,
	                        cx+2*larghezza_occhi, cy+altezza_pupille, 0,
							cx+distanza_occhi+2*larghezza_occhi, cy+altezza_pupille*0.8, 0};
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