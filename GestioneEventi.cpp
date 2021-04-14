#include "Lib.h"
#include "mario.h"

extern Mario *m;

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		m->moving = true;
		m->going_left = true;
		break;

	case 'd':
		m->moving = true;
		m->going_left = false;
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
		m->moving = false;
		break;

	default:
		break;
	}
}