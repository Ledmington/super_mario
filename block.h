#pragma once

#include "utils.h"
#include <stdbool.h>

typedef struct {
	/*
		Il blocco speciale ha:
		sfondo giallino/arancione,
		4 puntini neri vicino agli angoli,
		il punto interrogativo al centro (due trequarti di cerchio e un punto)

		Il blocco normale ha:
		sfondo nero,
		10 mattoni rossicci.

		Il bloco speciale rotto ha:
		sfondo rossiccio,
		4 punti neri vicino agli angoli.
	*/
	Figura *fig[12];

	bool special;
	bool broken;
} Block;

Block* createBlock(const float width, const float height, const bool isspecial, const bool isbroken);
void loadBlock(Block* b);
void drawBlock(Block* b);
void destroyBlock(Block* b);