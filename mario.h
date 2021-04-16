#pragma once

#include "utils.h"
#include <stdbool.h>

typedef struct {
	Figura* cappello[2]; // semicerchio e visiera
	
	/*
		rettangolo e quarto di cerchio per la faccia,
		triangolo e rettangolo per capelli,
		triangolo per naso,
		rettangolo per baffi,
		due ellissi per occhio e pupilla.
	*/
	Figura* testa[8];

	/*
		un rettangolo per corpo,
		due quarti di cerchio per le spalle,
		due rettangoli per le bretelle,
		un rettangolo per il bacino,
		due rettangoli per le fibbie,
		due quadrati per le mani.
	*/
	Figura* corpo[10];

	/*
		un rettangolo per ogni gamba,
		un rettangolo e un semicerchio per ogni piede.
	*/
	Figura* gambe[6];

	Point posizione;
	Point velocita;
	Point accelerazione;

	bool going_left;
	bool jumping;
} Mario;

/*
	Costruisce il modello di Mario nel rettangolo
	[0;width] ; [0;height]
*/
Mario* createMario(const float width, const float height);
void flipMario(Mario *m, const float cx);
void loadMario(Mario *m);
void reloadMario(Mario *m);
void drawMario(Mario *m);
void destroyMario(Mario *m);