#pragma once

#include "utils.h"
#include <stdbool.h>

typedef struct {
	Figura* testa[4]; // l'esagono e i 3 semicerchi
	Figura* corpo; // il semicerchio
	Figura* piedi[6]; // un rettangolo e due semicerchi per ogni piede
	Figura* occhi[6]; // due ellissi (e un rettangolo per il sopracciglio) per ogni occhio

	bool alive;
	bool dying;

	Point posizione;
} Goomba;

/*
	Costruisce un Goomba all'interno del rettangolo
	[0; width] ; [0; height]
*/
Goomba* createGoomba(const float width, const float height);
void loadGoomba(Goomba *g);
void drawGoomba(Goomba *g);
void destroyGoomba(Goomba *g);