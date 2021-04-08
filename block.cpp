#include "block.h"

#include "utils.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <iostream>

using namespace std;

static float arancione_sfondo[] = { 1, 166.0/255.0, 66.0/255.0, 1 };
static float arancione_punto_int[] = { 1, 91.0 / 255.0, 16.0 / 255.0, 1 };
static float rosso_mattone[] = {204.0/255.0, 34.0/255.0, 0, 1};
static float nero[] = {0, 0, 0, 1};

Block* createBlock(const float width, const float height, const bool isspecial, const bool isbroken) {
	assert(width > 0);
	assert(height > 0);
	if (!isspecial && isbroken) {
		cout << "ERRORE: un blocco normale non rotto non va disegnato." << endl;
		return NULL;
	}

	Block* b = (Block*)malloc(sizeof(Block));
	assert(b);

	// Il blocco � 12x12 pixel
	const float px_side_x = width / 12.0;
	const float px_side_y = height / 12.0;

	if (isspecial && !isbroken) {
		b->fig[0] = rettangolo(0, width, 0, height, arancione_sfondo);
		b->fig[1] = rettangolo(px_side_x, px_side_x * 2, px_side_y * 10, px_side_y * 11, nero);
		b->fig[2] = rettangolo(px_side_x * 10, px_side_x * 11, px_side_y * 10, px_side_y * 11, nero);
		b->fig[3] = rettangolo(px_side_x, px_side_x * 2, px_side_y, px_side_y * 2, nero);
		b->fig[4] = rettangolo(px_side_x * 10, px_side_x * 11, px_side_y, px_side_y * 2, nero);
		b->fig[5] = ellisse(width/2, height*2/3, width*0.3, height*0.2, 30, (float)PI*3/2, -(float)PI/2, arancione_punto_int, arancione_punto_int);
		b->fig[6] = ellisse(width/2, height*2/3, width*0.2, height*0.1, 30, (float)PI*3/2, -(float)PI/2, arancione_sfondo, arancione_sfondo);
		b->fig[7] = rettangolo(width*0.45, width*0.55, height*0.25, height/2, arancione_punto_int);
		b->fig[8] = rettangolo(width*0.45, width*0.55, height*0.1, height*0.2, arancione_punto_int);
		for (int i = 9; i < 12; i++) b->fig[i] = NULL;
	}
	else if (isspecial && isbroken) {
		b->fig[0] = rettangolo(0, width, 0, height, rosso_mattone);
		b->fig[1] = rettangolo(px_side_x, px_side_x * 2, px_side_y * 10, px_side_y * 11, nero);
		b->fig[2] = rettangolo(px_side_x * 10, px_side_x * 11, px_side_y * 10, px_side_y * 11, nero);
		b->fig[3] = rettangolo(px_side_x, px_side_x * 2, px_side_y, px_side_y * 2, nero);
		b->fig[4] = rettangolo(px_side_x * 10, px_side_x * 11, px_side_y, px_side_y * 2, nero);
		for (int i = 5; i < 12; i++) b->fig[i] = NULL;
	}
	else if (!isspecial && !isbroken) {
		// Ogni mattone � 5x2 pixel, separati da un pixel nero

		b->fig[0] = rettangolo(0, width, 0, height, nero);
		b->fig[1] = rettangolo(0, px_side_x * 5, px_side_y * 10, height, rosso_mattone);
		b->fig[2] = rettangolo(px_side_x*6, px_side_x * 11, px_side_y * 10, height, rosso_mattone);
		b->fig[3] = rettangolo(0, px_side_x * 2, px_side_y * 7, px_side_y * 9, rosso_mattone);
		b->fig[4] = rettangolo(px_side_x * 3, px_side_x * 8, px_side_y * 7, px_side_y * 9, rosso_mattone);
		b->fig[5] = rettangolo(px_side_x * 9, width, px_side_y * 7, px_side_y * 9, rosso_mattone);
		b->fig[6] = rettangolo(0, px_side_x * 5, px_side_y * 4, px_side_y * 6, rosso_mattone);
		b->fig[7] = rettangolo(px_side_x * 6, px_side_x * 11, px_side_y * 4, px_side_y * 6, rosso_mattone);
		b->fig[8] = rettangolo(0, px_side_x * 2, px_side_y, px_side_y * 3, rosso_mattone);
		b->fig[9] = rettangolo(px_side_x * 3, px_side_x * 8, px_side_y, px_side_y * 3, rosso_mattone);
		b->fig[10] = rettangolo(px_side_x * 9, width, px_side_y, px_side_y * 3, rosso_mattone);
		b->fig[11] = NULL;
	}

	return b;
}

void loadBlock(Block* b) {
	for (int i = 0; i < 12; i++) {
		loadFigure(b->fig[i]);
	}
}

void drawBlock(Block* b) {
	for (int i = 0; i < 12; i++) {
		drawFigure(b->fig[i]);
	}
}

void destroyBlock(Block* b) {
	for (int i = 0; i < 12; i++) {
		destroyFigure(b->fig[i]);
	}
	free(b);
}