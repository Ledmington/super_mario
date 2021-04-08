#pragma once

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct {
	float x, y, z;
} Point;

typedef struct {
	float x, y, z, r, g, b, a;
} Vertex;

typedef struct {
	unsigned int VAO;
	unsigned int VBO;

	// Numero vertici
	unsigned int nv;
	// Vertici
	Vertex *v;

	// Numero indici
	unsigned int ni;
	// Indici
	unsigned int *i;
} Figura;

// Utility per controllare se avvengono errori
void assertNoError();

void destroyFigure(Figura *f);
void loadFigure(Figura *fig);  // Creates buffers and loads them
void reloadFigure(Figura *fig);  // Loads just the vertices
void drawFigure(Figura *fig);

/*
	Costruisce un'ellisse con i dati parametri.
	(cx, cy) saranno le coordinate del centro;
	(rx, ry) saranno le lunghezze dei raggi rispetto agli assi
	         (dare valori uguali per costruire un cerchio);
	nPunti è il numero di vertici sulla circonferenza che si vogliono calcolare,
	         un numero elevato produce un'ellisse meglio definita ma aumenta
			 il tempo di esecuzione;
	angolo_settore è l'angolo in radianti del settore del cerchio di cui si vogliono
	         calcolare i vertici, passare 2*PI per un cerchio completo, passare
			 PI per un semicerchio;
	rotazione è il valore dell'angolo di rotazione in radianti che si vuole applicare all'ellisse;
	colore_centro è il colore del centro,
	colore_bordo è il colore dei vertici sul bordo.

	Restituisce un puntatore alla struct figura che dovrà essere liberata dal chiamante.
*/
Figura* ellisse(const float cx, const float cy, const float rx, const float ry,
	            const unsigned int nPunti,
			    const float angolo_settore,
	            const float rotazione,
	            const float* colore_centro,
	            const float* colore_bordo);

/*
	Costuisce un rettangolo con i dati parametri.
	points è un vettore di 4 punti,
	colore è un vettore di 4 float: il colore per tutti i vertici.

	Restituisce un puntatore alla struct figura che dovrà essere liberata dal chiamante.
*/
Figura* rettangolo(const Point *points, const float* colore);

/*
	Costruisce un rettangolo allineato con gli assi cartesiani.
	I 4 vertici saranno:
	[xmin; ymin]
	[xmin; ymax]
	[xmax; ymin]
	[xmax; ymax]
	colore è un vettore di 4 float: il colore per tutti i vertici.

	Restituisce un puntatore alla struct figura che dovrà essere liberata dal chiamante.
*/
Figura* rettangolo(const float xmin, const float xmax, const float ymin, const float ymax, const float* colore);

/*
	Costruisce un triangolo con le coordinate date,
	colorando tutti i vertici con lo stesso colore.
*/
Figura* triangolo(const Point* punti, const float* colore);

/*
	Ruota tutte le coordinate x dei vertici della figura
	attorno all'asse cx.
*/
void flipAroundX(Figura *fig, const float cx);

/*
	Disegna una stella all'interno dell'ellisse di centro (cx,cy) e di raggio (Rx,Ry).
	I vertici "concavi", quelli all'interno della stella, si trovano su un'ellisse
	di raggio (rx, ry)
*/
Figura* stella(const float cx, const float cy,
	           const float Rx, const float Ry,
	           const float rx, const float ry,
	           const unsigned int nPunte, const float rotazione, const float* colore_centro, const float* colore_bordo);