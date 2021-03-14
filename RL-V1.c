/*
 * My RougeLike Version 1
 */
// TODO : Clean el code , el spaces, al tabs, naming, commenting,
// TODO : Separate code to files at the end of project
// TODO : comment & describe every function
#include "stdlib.h"
#include "ncurses.h"
#include "time.h"

typedef struct Level
{
	int level;	// bad naming! i'll change this at the end of tut. TODO
	char ** tiles;
	int numberOfRooms;
	struct Room ** rooms;
	struct Monster ** monsters;
	int numberOfMonsters;
	struct Player * user;
} Level;

typedef struct Position
{
	int y;
	int x;
	// TILE_TYLE tile
} Position;

typedef struct Room
{
	Position position;
	int height;
	int width;
	Position ** doors;
} Room;

typedef struct Player
{
	Position * position;
	int health;
	int attack;
	int gold;
	int maxHealth;
	int exp;
	// Room * room;
} Player;

typedef struct Monster
{
	char string[2];
	char symbol;
	int health;
	int attack;
	int speed;
	int defence;
	int pathfinding;
	Position * position;
	int alive;
} Monster;

/* Screen SetUp */
int ScreenSetUp();
int printGameHub(Level * level);

Level * createLevel(int level);

/* Level/map functions */
Room ** roomsSetUp();		// returns pointer to 2D array : master array
char ** saveLevelPositions();

/*  */
Player * PlayerSetUp();
Position * handleInput(int input, Player * user);
int CheckPosition(Position * newPosition, Level * level);
int playerMove(Position * newPosition, Player * user, char ** level);

/* Room functions */
Room * createRoom(int y, int x, int height, int width );
int drawRoom(Room * room);
int connectDoors(Position * doorOne, Position * doorTwo);

/* Monster Functions */
int addMonsters(Level * level);
Monster * selectMonster(int level);
Monster * createMonster(char symbol, int health, int attack, int speed, int defence, int pathfinding);
int killMonster(Monster * monster);
int setStartingPosition(Monster * monster, Room * room);
int moveMonsters(Level * level);
int pathfindingSeek(Position * start, Position * destination);
int pathfindingRandom(Position * position);
Monster * getMonsterAt(Position * position, Monster ** monsters);

int combat(Player * player, Monster * monster, int order);


// -----------------------------------------------------------------------------------
int main(void) {
	int ch;
	Level * level;
	Position * newPosition;

	ScreenSetUp();
	level = createLevel(1);
	printGameHub(level);

	/* Main Game Loop */
	while( (ch = getch()) != 'q' ) {
		printGameHub(level);
		newPosition = handleInput(ch, level->user);
		CheckPosition(newPosition, level);
		moveMonsters(level);
		move(level->user->position->y, level->user->position->x);
	}

	refresh();
	endwin();
	return 0;
}
// -----------------------------------------------------------------------------------

// Start Screen SetUP
// -----------------------------------------------------------------------------------
int ScreenSetUp() {
	initscr();
	srand(time(NULL));		// What this MEAM ?!
	noecho();

	return 1;	// 1 == Succes & 0 == failure !!
}

int printGameHub(Level * level) {
	mvprintw(30, 0, "Level: %d,\t", level->level);
	printw("Gold: %d,\t", level->user->gold);
	printw("Hp: %d(%d),\t", level->user->health, level->user->maxHealth);
	printw("Attack: %d,\t", level->user->attack);
	printw("Exp: %d,\t", level->user->exp);
	return 1;
}

// End Screen SetUP
// -----------------------------------------------------------------------------------

Level * createLevel(int level) {
	Level * newLevel;
	newLevel = malloc(sizeof(Level));

	newLevel->level 			= level;
	newLevel->numberOfRooms 	= 3;	// hardcoded
	newLevel->rooms 			= roomsSetUp();
	newLevel->tiles				= saveLevelPositions();
	newLevel->user 				= PlayerSetUp();
	addMonsters(newLevel);

	return newLevel;
}

Room ** roomsSetUp() {		// Array of structs
	Room ** rooms;			// Create array of room! very simple xD
	rooms	= malloc(sizeof(Room) * 6);

	// --
	rooms[0] 	= createRoom(13, 13, 6, 21);
	drawRoom(rooms[0]);

	rooms[1] 	= createRoom(13, 50, 6, 21);
	drawRoom(rooms[1]);

	rooms[2] 	= createRoom(2, 25, 6, 21);
	drawRoom(rooms[2]);

	// --
	connectDoors(rooms[1]->doors[1], rooms[0]->doors[3]);
	connectDoors(rooms[2]->doors[2], rooms[0]->doors[0]);
	connectDoors(rooms[2]->doors[3], rooms[1]->doors[0]);
	return rooms;
}

Room * createRoom(int y, int x, int height, int width ) {
	Room * newRoom;
	newRoom		= malloc(sizeof(Room));

	newRoom->position.y		= y;
	newRoom->position.x		= x;
	newRoom->height			= height;
	newRoom->width			= width;

	newRoom->doors			= malloc(sizeof(Position) * 4);

	// Doors ..

	/* Top door */
	newRoom->doors[0]			= malloc(sizeof(Position));
	newRoom->doors[0]->x 		= rand() % (width -2) + newRoom->position.x +1;
	newRoom->doors[0]->y 		= newRoom->position.y;

	/* Left door */
	newRoom->doors[1]			= malloc(sizeof(Position));
	newRoom->doors[1]->x 		= newRoom->position.x;
	newRoom->doors[1]->y 		= rand() % (height -2) + newRoom->position.y +1;

	/* Bottom door */
	newRoom->doors[2]			= malloc(sizeof(Position));
	newRoom->doors[2]->x 		= rand() % (width -2) + newRoom->position.x +1;
	newRoom->doors[2]->y 		= newRoom->position.y + newRoom->height;

	/* Right door */
	newRoom->doors[3]			= malloc(sizeof(Position));
	newRoom->doors[3]->x 		= newRoom->position.x + width -1;
	newRoom->doors[3]->y 		= rand() % (height -2) + newRoom->position.y +1;

	return newRoom;
}

Player * PlayerSetUp() {
	Player * newPlayer;
	newPlayer = malloc(sizeof(Player));
	newPlayer->position = malloc(sizeof(Position));

	newPlayer->position->x 	= 20;
	newPlayer->position->y 	= 15;
	newPlayer->attack 		= 1;
	newPlayer->gold 		= 0;
	newPlayer->health 		= 0;
	newPlayer->maxHealth 	= 20;
	newPlayer->exp 			= 0;

	// newPlayer->health		= 20;		// = Useless till now!
	mvprintw(newPlayer->position->y, newPlayer->position->x, "@");	// Update knew
	move(newPlayer->position->y, newPlayer->position->x);

	return newPlayer;
}

Position * handleInput(int input, Player * user) {
	Position * newPosition;
	newPosition = malloc(sizeof(Position));

	if (input == 'w' || input == 'W')	// Move Up
	{
		newPosition->y 	= user->position->y - 1;
		newPosition->x	= user->position->x;
	}
	if (input == 's' || input == 'S')	// Move Down
	{
		newPosition->y 	= user->position->y + 1;
		newPosition->x	= user->position->x;
	}
	if (input == 'a' || input == 'A')	// Move Left
	{
		newPosition->y 	= user->position->y;
		newPosition->x	= user->position->x - 1;
	}
	if (input == 'd' || input == 'D')	// Move Right
	{
		newPosition->y 	= user->position->y;
		newPosition->x	= user->position->x + 1;
	}
	return newPosition;
}

int CheckPosition(Position * newPosition, Level * level) {
	Player * user;
	user = level->user;
	if ( mvinch(newPosition->y, newPosition->x) == '.' 	||
		mvinch(newPosition->y, newPosition->x) == '+' 	||
		mvinch(newPosition->y, newPosition->x) == '#' )
	{
		playerMove(newPosition, user, level->tiles);
	} else if (mvinch(newPosition->y, newPosition->x) == 'X' 	||
		mvinch(newPosition->y, newPosition->x) == 'G' 	||
		mvinch(newPosition->y, newPosition->x) == 'T' ) {
		combat(user, getMonsterAt(newPosition, level->monsters), 1);
	} else {
		move(user->position->y, user->position->x);
	}
	return 1;
}

int playerMove(Position * newPosition,Player * user, char ** level) {
	char buffer[8];
	sprintf(buffer, "%c", level[user->position->y][user->position->x]);	// convert single char to string of char, TODO: Google it!

	mvprintw(user->position->y, user->position->x, buffer);	// Change Prev
	user->position->y 	= newPosition->y;
	user->position->x 	= newPosition->x;
	mvprintw(user->position->y, user->position->x, "@");	// Update knew
	move(user->position->y, user->position->x);
	return 1;
}

int drawRoom(Room * room) {

	int x;
	int y;
	/* Draw top & bottom */
	for (x = room->position.x; x < room->position.x + room->width; ++x )
	{
		mvprintw(room->position.y, x, "-");	// the top
		mvprintw(room->position.y + room->height, x, "-");	// the bottom
	}

	for (y = room->position.y + 1; y < room->position.y + room->height; ++y)
	{
		/* draw side walls */
		mvprintw(y, room->position.x, "|");	// Left side
		mvprintw(y, room->position.x + room->width -1, "|");	// Right side
		for ( x = room->position.x +1; x < room->position.x + room->width -1; ++x )
		{
			mvprintw(y, x, ".");
		}
	}
	/* Draw doors */
	mvprintw(room->doors[0]->y, room->doors[0]->x, "+");
	mvprintw(room->doors[1]->y, room->doors[1]->x, "+");
	mvprintw(room->doors[2]->y, room->doors[2]->x, "+");
	mvprintw(room->doors[3]->y, room->doors[3]->x, "+");
	return 1;	// 1 == Success
}

int connectDoors(Position * doorOne, Position * doorTwo) {

	Position temp;
	Position prev;

	temp.x 		= doorOne->x;
	temp.y 		= doorOne->y;

	int count 	= 0;

	prev 		= temp;

	while(1) {
		// Move Left
		if (abs( (temp.x -1) - doorTwo->x) < abs( ( temp.x) - doorTwo->x) &&
			mvinch(temp.y, temp.x -1) == ' ')
		{
			prev.x 	= temp.x;
			temp.x 	= temp.x -1;
		}
		// Move Right
		else if (abs( temp.x +1 - doorTwo->x) < abs( ( temp.x) - doorTwo->x) &&
			mvinch(temp.y, temp.x +1) == ' ')
		{
			prev.x 	= temp.x;
			temp.x 	= temp.x +1;
		}
		// Move down
		else if (abs( (temp.y +1) - doorTwo->y) < abs( ( temp.y) - doorTwo->y) &&
			mvinch(temp.y +1, temp.x) == ' ')
		{
			prev.x 	= temp.x;
			temp.y 	= temp.y +1;
		}
		// Move up
		else if (abs( (temp.y +1) - doorTwo->y) < abs( ( temp.y) - doorTwo->y) &&
			mvinch(temp.y -1, temp.x) == ' ')
		{
			prev.x 	= temp.x;
			temp.y 	= temp.y +1;
		}
		else
		{
			if( count == 0) {
				temp = prev;
				count++;
				continue;
			} else {
				return 0;
			}
		}
		mvprintw(temp.y, temp.x, "#");
		// getch();			// REMOVE ME !!!!!!!!
	}

	return 1;	// 1 == Success
}

/**
Master Function!
This func saves every single locaiton in 2D array
*/
char ** saveLevelPositions() {
	int y, x;
	char ** positions;
	positions = malloc(sizeof(char *) * 25);

	for (int y = 0; y < 25; ++y)
	{
		positions[y] = malloc(sizeof(char)*100);
		for (x = 0; x < 100; ++x)
		{
			positions[y][x] = mvinch(y, x);
		}
	}
	return positions;
}

// Start Monsters
// -----------------------------------------------------------------------------------
/*
1 Spider
	Symbol: X
	levels: 1 to 3
	health: 2
	Attacks: 1
	defence: 1
	pathfinding: 1 (1 == random)

2 Goblin
	Symbol: G
	levels: 1 to 5
	health: 5
	Attacks: 3
	defence: 1
	pathfinding: 2 (2 == Move closer to the player) ("Seeking")

3 Troll
	Symbol: T
	levels: 4 to 6
	health: 15
	Attacks: 1
	defence: 1
	pathfinding: 1 (random)
*/

int addMonsters(Level * level) {
	int x;
	level->numberOfMonsters = 0;

	level->monsters = malloc(sizeof(Monster *) * 6);	// max num of rooms is 6, so ..
	for (x = 0; x < level->numberOfRooms; ++x)
	{
		if ( (rand() % 2) == 0 )
		{
			level->monsters[level->numberOfMonsters] = selectMonster(level->level);
			setStartingPosition(level->monsters[level->numberOfMonsters], level->rooms[x]);
			++level->numberOfMonsters;
		}
	}
	return 1;
}

Monster * selectMonster(int level) {
	int monster;

	switch(level) {
		case 1:
		case 2:
		case 3:
			monster = (rand() % 2) + 1;
			break;
		case 4:
		case 5:
			monster = (rand() % 2) + 2;
			break;
		case 6:
			monster = 3;
			break;
	}
	// ---------------
	switch(monster) {
		case 1: /* Spider */
			return createMonster('X', 2, 1, 1, 1, 1);
		case 2: /* Goblin */
			return createMonster('G', 5, 3, 1, 1, 2);
		case 3: /* Troll */
			return createMonster('T', 15, 5, 1, 1, 1);
	}
}

Monster * createMonster(char symbol, int health, int attack, int speed, int defence, int pathfinding) {
	Monster * newMonster;
	newMonster = malloc(sizeof(Monster));

	newMonster->symbol 		= symbol;
	newMonster->health 		= health;
	newMonster->attack 		= attack;
	newMonster->speed 		= speed;
	newMonster->defence 	= defence;
	newMonster->symbol 		= pathfinding;
	newMonster->alive 		= 1;

	sprintf(newMonster->string, "%c", symbol);

	return newMonster;
}

int killMonster(Monster * monster) {
	mvprintw(monster->position->y, monster->position->x, ".");
	monster->alive 		= 0;
	return 1;
}

/*
	---------
	|.......|
	|.......|
	|.......|
	---------

*/

int setStartingPosition(Monster * monster, Room * room) {
	monster->position 		= malloc(sizeof(Position));

	monster->position->y 	= (rand() % (room->height -2)) + room->position.y +1;
	monster->position->x 	= (rand() % (room->width -2)) + room->position.x +1;

	mvprintw(monster->position->y, monster->position->x, monster->string);
	return 1;
}

/** Loop through all the monsters in the level
*/
int moveMonsters(Level * level) {
	int i;
	for (i = 0; i < level->numberOfMonsters; ++i)
	{
		if(level->monsters[i]->alive == 0) { // monister is alive ?
			continue;
		}

		mvprintw(level->monsters[i]->position->y, level->monsters[i]->position->x, ".");
		if (level->monsters[i]->pathfinding == 2)
		{
			/* seek pathfinding */
			pathfindingSeek(level->monsters[i]->position, level->user->position);
		} else {
			/* random pathfinding */
			pathfindingRandom(level->monsters[i]->position);
		}
		mvprintw(level->monsters[i]->position->y, level->monsters[i]->position->x, level->monsters[i]->string);
	}
}

/*
pathfinding types:
	Seeking
	Random
*/

/**
Random pathfinding
*/
int pathfindingRandom(Position * position) {
	int random;

	random = rand() % 5;
	switch(random) {
		/* Step UP */
		case 0:
			if ( mvinch(position->y -1, position->x) == '.' )
			{
				position->y 	= position->y -1;
			}
			break;
		/* Step Down */
		case 1:
			if ( mvinch(position->y +1, position->x) == '.' )
			{
				position->y 	= position->y +1;
			}
			break;
		/* Step Left */
		case 2:
			if ( mvinch(position->y, position->x -1) == '.' )
			{
				position->x 	= position->x -1;
			}
			break;
		/* Step Right */
		case 3:
			if ( mvinch(position->y, position->x +1) == '.' )
			{
				position->x 	= position->x +1;
			}
			break;
		/* Do Nothing */
		case 4:
			break;
	}
	return 1;
}

/**
Seek pathfinding
*/
int pathfindingSeek(Position * start, Position * destination) {
	// Move Left
	if (abs( (start->x -1) - destination->x) < abs( ( start->x) - destination->x) &&
		mvinch(start->y, start->x -1) == '.')
	{
		start->x 	= start->x -1;
	}
	// Move Right
	else if (abs( start->x +1 - destination->x) < abs( ( start->x) - destination->x) &&
		mvinch(start->y, start->x +1) == '.')
	{
		start->x 	= start->x +1;
	}
	// Move down
	else if (abs( (start->y +1) - destination->y) < abs( ( start->y) - destination->y) &&
		mvinch(start->y +1, start->x) == '.')
	{
		start->y 	= start->y +1;
	}
	// Move up
	else if (abs( (start->y +1) - destination->y) < abs( ( start->y) - destination->y) &&
		mvinch(start->y -1, start->x) == '.')
	{
		start->y 	= start->y +1;
	}
	else {
		/* Do nothing */
	}

	return 1;
}
// End Monsters
// -----------------------------------------------------------------------------------

// Start Combat
// -----------------------------------------------------------------------------------
int combat(Player * player, Monster * monster, int order) {
	/* Player attacking */
	if ( order == 1 )
	{
		monster->health -= player->attack;
		if ( monster->health > 0 )
		{
			player->health -= monster->attack;
		} else {
			killMonster(monster);
			++player->exp;		// the beauty of syntax
		}
	}
	/* Monster attacking */
	else {
		player->health -= monster->attack;
		if ( player->health > 0 )
		{
			monster->health -= player->attack;
		}
	}

	return 1;
}

Monster * getMonsterAt(Position * position, Monster ** monsters) {
	int i;
	for (i = 0; i < 6; ++i)
	{
		if ( (position->y == monsters[i]->position->y) && (position->x == monsters[i]->position->x) )
		{
			return monsters[i];
		}
	}
	return NULL;
}

// End Combat
// -----------------------------------------------------------------------------------
