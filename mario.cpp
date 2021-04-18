#include "mario.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

static float rosso_maglia[] = { 211.0f / 255.0f, 0.0f, 11.0f / 255.0f, 1.0f };
static float blu_tuta[] = { 0.0f, 102.0f / 255.0f, 211.0f / 255.0f, 1.0f };
static float rosa_carne[] = { 1.0f, 227.0f / 255.0f, 200.0f / 255.0f, 1.0f };
static float marrone_scarpe[] = { 138.0f / 255.0f, 45.0f / 255.0f, 0.0f, 1.0f };
static float giallo[] = { 1, 1, 0, 1 };
static float nero[] = { 0, 0, 0, 1 };
static float bianco[] = { 1, 1, 1, 1 };

Mario* createMario(const float width, const float height) {
	assert(width > 0);
	assert(height > 0);

	Mario *m = (Mario*)malloc(sizeof(Mario));
	assert(m);

	const float cx = width / 2;
	const float cy = 0;

	// Parametri cappello
	const float altezza_cappello = height * 0.15f;
	const float larghezza_cappello = width * 0.3f;
	const float lunghezza_visiera = width * 0.2f;

	// Parametri testa
	const float altezza_testa = height * 0.2f;
	const float larghezza_testa = larghezza_cappello;
	const Point centro_testa = {cx, cy + height - altezza_cappello - altezza_testa/2, 0 };
	const float diag_testa = sqrtf(altezza_testa*altezza_testa + larghezza_testa*larghezza_testa) / 2;
	const float larghezza_capelli = larghezza_testa * 0.2f;
	const float lunghezza_naso = larghezza_testa * 0.4f;
	const float altezza_naso = lunghezza_naso * 0.8f;
	const float altezza_baffi = altezza_testa * 0.15f;
	const float altezza_occhi = altezza_testa * 0.4f;
	const float larghezza_occhi = larghezza_testa * 0.2f;
	const float altezza_pupilla = altezza_occhi * 0.7f;
	const float larghezza_pupilla = larghezza_occhi * 0.7f;

	// Parametri corpo
	const float altezza_corpo = height * 0.28f;
	const float larghezza_corpo = width * 0.5f;
	const Point centro_corpo = {cx, cy+height-altezza_cappello-altezza_testa-altezza_corpo/2, 0};
	const float larghezza_braccio = width * 0.2f;
	const float distanza_bretelle = width * 0.1f;
	const float larghezza_bretelle = width * 0.06f;
	const float altezza_fibbie = larghezza_bretelle; // facciamo le fibbie quadrate
	const float altezza_bacino = altezza_corpo / 2;

	// Parametri gambe
	const float altezza_gambe = height * 0.15f;
	const float distanza_gambe = width * 0.05f;
	const float lunghezza_piedi = (width - distanza_gambe) * 0.4f;
	const float larghezza_gambe = width * 0.3f;
	const float altezza_piedi = height * 0.1f;

	// Cappello
	m->cappello[0] = ellisse(cx, cy+height-altezza_cappello, larghezza_cappello/2+larghezza_capelli, altezza_cappello, 30, (float)PI, 0, rosso_maglia, rosso_maglia);
	m->cappello[1] = rettangolo(cx + larghezza_cappello / 2,
		                        cx + larghezza_cappello / 2 + lunghezza_visiera,
		                        cy + height - altezza_cappello,
		                        cy + height - altezza_cappello / 2, rosso_maglia);

	// Testa
	m->testa[0] = rettangolo(cx-larghezza_testa/2,
		                     cx+larghezza_testa/2,
		                     cy+height-altezza_cappello-altezza_testa,
		                     cy+height-altezza_cappello, rosa_carne);
	m->testa[1] = ellisse(centro_testa.x, centro_testa.y, diag_testa, diag_testa, 30, (float)PI/2, -(float)PI/4, rosa_carne, rosa_carne);
	m->testa[2] = rettangolo(cx-larghezza_testa/2-larghezza_capelli,
		                     cx-larghezza_testa/2,
		                     cy+height-altezza_cappello-altezza_testa,
		                     cy+height-altezza_cappello, nero);
	const Point capelli[] = { {cx - larghezza_testa / 2 - larghezza_capelli, cy + height - altezza_cappello, 0 },
							 {cx - larghezza_testa / 2 - larghezza_capelli, cy + height - altezza_cappello - altezza_testa / 2, 0},
							 {cx, cy + height - altezza_cappello, 0} };
	m->testa[3] = triangolo(capelli, nero);
	const Point naso[] = { {centro_testa.x+larghezza_testa/2, centro_testa.y, 0}, 
	                       {centro_testa.x+larghezza_testa/2+lunghezza_naso, centro_testa.y, 0}, 
	                       {centro_testa.x+larghezza_testa/2, centro_testa.y+altezza_naso, 0} };
	m->testa[4] = triangolo(naso, rosa_carne);
	m->testa[5] = rettangolo(centro_testa.x, centro_testa.x+diag_testa,
		                     centro_testa.y-altezza_baffi, centro_testa.y, nero);
	m->testa[6] = ellisse(centro_testa.x+larghezza_occhi/2, centro_testa.y+altezza_occhi/2,
		                  larghezza_occhi/2, altezza_occhi/2, 30, (float)PI*2, 0, bianco, bianco);
	m->testa[7] = ellisse(centro_testa.x + larghezza_occhi / 2, centro_testa.y + altezza_occhi / 2,
		                  larghezza_pupilla / 2, altezza_pupilla / 2, 30, (float)PI * 2, 0, nero, nero);

	// Corpo
	m->corpo[0] = rettangolo(centro_corpo.x-larghezza_corpo/2, centro_corpo.x+larghezza_corpo/2,
		                     centro_corpo.y-altezza_corpo/2, centro_corpo.y+altezza_corpo/2, rosso_maglia);
	m->corpo[1] = ellisse(centro_corpo.x-larghezza_corpo/2, centro_corpo.y-altezza_corpo/2,
		                  altezza_corpo, larghezza_braccio,
		                  30, (float)PI/2, (float)PI/2, rosso_maglia, rosso_maglia); // braccio sx
	m->corpo[2] = ellisse(centro_corpo.x + larghezza_corpo / 2, centro_corpo.y - altezza_corpo / 2,
		                  larghezza_braccio, altezza_corpo,
		                  30, (float)PI / 2, 0, rosso_maglia, rosso_maglia); // braccio dx
	m->corpo[3] = rettangolo(centro_corpo.x-distanza_bretelle/2-larghezza_bretelle, centro_corpo.x-distanza_bretelle/2,
		                     centro_corpo.y-altezza_corpo/2, centro_corpo.y+altezza_corpo/2, blu_tuta); // bretella sx
	m->corpo[4] = rettangolo(centro_corpo.x + distanza_bretelle / 2, centro_corpo.x + distanza_bretelle / 2 + larghezza_bretelle,
		                     centro_corpo.y - altezza_corpo / 2, centro_corpo.y + altezza_corpo / 2, blu_tuta); // bretella dx
	m->corpo[5] = rettangolo(centro_corpo.x-larghezza_corpo/2, centro_corpo.x+larghezza_corpo/2,
		                     centro_corpo.y-altezza_corpo/2-altezza_bacino, centro_corpo.y-altezza_corpo/2, blu_tuta); // bacino
	m->corpo[6] = rettangolo(centro_corpo.x-distanza_bretelle/2-larghezza_bretelle, centro_corpo.x-distanza_bretelle/2,
		                     centro_corpo.y-altezza_corpo/2-altezza_fibbie, centro_corpo.y-altezza_corpo/2, giallo); // fibbia sx
	m->corpo[7] = rettangolo(centro_corpo.x+distanza_bretelle/2, centro_corpo.x+distanza_bretelle/2+larghezza_bretelle,
		                     centro_corpo.y-altezza_corpo/2-altezza_fibbie, centro_corpo.y-altezza_corpo/2, giallo); // fibbia dx
	m->corpo[8] = rettangolo(centro_corpo.x-larghezza_corpo/2-larghezza_braccio, centro_corpo.x-larghezza_corpo/2,
		                     centro_corpo.y-altezza_corpo/2-altezza_bacino, centro_corpo.y-altezza_corpo/2, rosa_carne); // mano sx
	m->corpo[9] = rettangolo(centro_corpo.x+larghezza_corpo/2, centro_corpo.x+larghezza_corpo/2+larghezza_braccio,
		                     centro_corpo.y-altezza_corpo/2-altezza_bacino, centro_corpo.y-altezza_corpo/2, rosa_carne); // mano dx

	// Gambe
	Point gamba[4] = { {centro_corpo.x-distanza_gambe/2, centro_corpo.y-altezza_corpo/2-altezza_bacino, 0},
					   {centro_corpo.x-distanza_gambe/2-larghezza_gambe*3/4, centro_corpo.y-altezza_corpo/2-altezza_bacino, 0},
					   {centro_corpo.x-distanza_gambe/2-larghezza_gambe/4, centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe, 0},
					   {centro_corpo.x-distanza_gambe/2-larghezza_gambe, centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe, 0} };
	m->gambe[0] = rettangolo(gamba, blu_tuta); // gamba sx
	m->gambe[1] = rettangolo(gamba, blu_tuta); // gamba dx
	flipAroundX(m->gambe[1], centro_corpo.x);
	m->gambe[2] = rettangolo(centro_corpo.x-distanza_gambe/2-larghezza_gambe, centro_corpo.x-distanza_gambe/2-larghezza_gambe/4,
		                     centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe-altezza_piedi, centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe, nero); // piede sx
	m->gambe[3] = rettangolo(centro_corpo.x-distanza_gambe/2-larghezza_gambe, centro_corpo.x-distanza_gambe/2-larghezza_gambe/4,
		                     centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe-altezza_piedi, centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe, nero); // piede dx
	flipAroundX(m->gambe[3], centro_corpo.x);
	m->gambe[4] = ellisse(centro_corpo.x-distanza_gambe/2-larghezza_gambe*1.1f,
		                  centro_corpo.y-altezza_corpo/2-altezza_bacino-altezza_gambe-altezza_piedi,
		                  altezza_piedi*0.9f, altezza_piedi*1.2f, 30, (float)PI, 0, nero, nero); // punta scarpa sx
	m->gambe[5] = ellisse(centro_corpo.x - distanza_gambe / 2 - larghezza_gambe * 1.1f,
		                  centro_corpo.y - altezza_corpo / 2 - altezza_bacino - altezza_gambe - altezza_piedi,
		                  altezza_piedi*0.9f, altezza_piedi*1.2f, 30, (float)PI, 0, nero, nero); // punta scarpa sx
	flipAroundX(m->gambe[5], centro_corpo.x);

	return m;
}

void loadMario(Mario *m) {
	for (int i = 0; i < 2; i++) {
		loadFigure(m->cappello[i]);
	}
	for (int i = 0; i < 8; i++) {
		loadFigure(m->testa[i]);
	}
	for (int i = 0; i < 10; i++) {
		loadFigure(m->corpo[i]);
	}
	for (int i = 0; i < 6; i++) {
		loadFigure(m->gambe[i]);
	}
}

void drawMario(Mario *m) {
	for (int i = 0; i < 2; i++) {
		drawFigure(m->cappello[i]);
	}
	for (int i = 0; i < 8; i++) {
		drawFigure(m->testa[i]);
	}
	for (int i = 0; i < 10; i++) {
		drawFigure(m->corpo[i]);
	}
	for (int i = 0; i < 6; i++) {
		drawFigure(m->gambe[i]);
	}
}

void destroyMario(Mario *m) {
	for (int i = 0; i < 2; i++) {
		destroyFigure(m->cappello[i]);
	}
	for (int i = 0; i < 8; i++) {
		destroyFigure(m->testa[i]);
	}
	for (int i = 0; i < 10; i++) {
		destroyFigure(m->corpo[i]);
	}
	for (int i = 0; i < 6; i++) {
		destroyFigure(m->gambe[i]);
	}
	free(m);
}

void flipMario(Mario *m, const float cx) {
	for (int i = 0; i < 2; i++) {
		flipAroundX(m->cappello[i], cx);
	}
	for (int i = 0; i < 8; i++) {
		flipAroundX(m->testa[i], cx);
	}
	for (int i = 0; i < 10; i++) {
		flipAroundX(m->corpo[i], cx);
	}
	for (int i = 0; i < 6; i++) {
		flipAroundX(m->gambe[i], cx);
	}
}

void reloadMario(Mario *m) {
	for (int i = 0; i < 2; i++) {
		reloadFigure(m->cappello[i]);
	}
	for (int i = 0; i < 8; i++) {
		reloadFigure(m->testa[i]);
	}
	for (int i = 0; i < 10; i++) {
		reloadFigure(m->corpo[i]);
	}
	for (int i = 0; i < 6; i++) {
		reloadFigure(m->gambe[i]);
	}
}