#ifndef CONSTANTS_h
#define CONSTANTS_h

#define SW_VER_MAJ 0
#define SW_VER_MIN 1

enum States
{
	Alive,
	Damaged,
	Stunned,
	Healed,
	Dead,
};

enum HitDirection
{
	Front,
	Back,
	Left,
	Right,
	NoDirection = 0xFF,
};

#endif
