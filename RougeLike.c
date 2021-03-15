// FIXME all function returns int should return something int!
// FIXME clean all alloc spaces
// FIXME make a wrapper to ncurses and reverse y <-> x postions
/*
 * My RougeLike version 2
 *
 * hjkl  for move around because now I use vim!
 *    and I think this more logical to me! and more cute!
 */
#include <ncurses.h>
#include <stdlib.h>

typedef struct Position_ {
   int x;
   int y;
} Position;

typedef struct Room_ {
   Position position;
   int height;
   int width;
   //monster** monster;
   //Item** items;
} Room;

typedef struct Player_ {
   Position position;
   int health;
} Player;

int screenSetUp(); // ncurses setup
Room** mapSetUp(); //
Player* playerSetUp();
int handleInput(int input, Player* user);
int playerMove(int y, int x, Player* user);
int checkPostion(int newY, int newX, Player* unit);

/* room functions */
Room* createRoom(int x, int y, int height, int width);
int drawRoom(Room* room);

int main()
{
   Player* user;
   screenSetUp();
   int ch;

   mapSetUp();

   user = playerSetUp();

   /* main game loop */
   while( (ch = getch()) != 'q')
   {
      handleInput(ch, user);
   }

   endwin();

   return 0;
}

int screenSetUp()
{
   initscr();
   printw("Hello world");
   noecho();
   refresh();

   return 0;
}

Room** mapSetUp()
{
   Room** rooms = malloc(sizeof(Room)*6);

   rooms[0] = createRoom(13, 13, 6, 8);
   drawRoom(rooms[0]);
   rooms[1] = createRoom(30, 13, 10, 20);
   drawRoom(rooms[1]);
   rooms[2] = createRoom(13, 25, 10, 15);
   drawRoom(rooms[2]);
}

Player* playerSetUp()
{
   Player* newPlayer;
   newPlayer = malloc(sizeof(Player));
   newPlayer->position.x = 14;
   newPlayer->position.y = 14;
   newPlayer->health = 14;

   playerMove(newPlayer->position.y, newPlayer->position.x, newPlayer);

   return newPlayer;
}

int handleInput(int input, Player* user)
{
   int newY;
   int newX;
   switch (input) {
      /* move up */
   case 'k':
   case 'K':
      newY  = user->position.y -1;
      newX  = user->position.x;
      break;

      /* move down */
   case 'j':
   case 'J':
      newY  = user->position.y +1;
      newX  = user->position.x;
      break;

      /* move left */
   case 'h':
   case 'H':
      newY  = user->position.y;
      newX  = user->position.x -1;
      break;

      /* move right */
   case 'l':
   case 'L':
      newY  = user->position.y;
      newX  = user->position.x +1;
      break;

   default:
      break;

   }

   checkPostion(newY, newX, user);

}

int checkPostion(int newY, int newX, Player* unit)
{
   int space;
   switch(mvinch(newY, newX))
   {
      case '.':
         playerMove(newY, newX, unit);
         break;
      default:
         move(unit->position.y, unit->position.x);
         break;
   }
}

int playerMove(int y, int x, Player* user)
{
   mvprintw(user->position.y, user->position.x, ".");

   user->position.y   = y;
   user->position.x   = x;

   mvprintw(user->position.y, user->position.x, "@");
   move(user->position.y, user->position.x);


}

// room info
Room* createRoom(int x, int y, int height, int width)
{
   Room* newRoom;
   newRoom = malloc(sizeof(Room));

   newRoom->position.x   = x;
   newRoom->position.y   = y;
   newRoom->height      = height;
   newRoom->width       = width;

   return newRoom;
}


/*
   ------------
   |..........|
   |..........|
   |..........|
   |..........|
   ------------
*/
int drawRoom(Room* room)
{
   int x;
   int y;

   /* draw top and bottom */
   for(x = room->position.x; x < room->position.x+ room->width; ++x)
   {
      mvprintw(room->position.y, x, "-"); /* top */
      mvprintw(room->position.y + room->height -1, x, "-"); /* bottom */
   }

   /* draw floors and side walls */
   for(y = room->position.y +1; y < room->position.y + room->height -1; ++y)
   {
      /* draw side walls */
      mvprintw(y, room->position.x, "|");
      mvprintw(y, room->position.x + room->width -1, "|");
      /* draw floors */
      for(x = room->position.x +1; x < room->position.x + room->width -1; ++x)
      {
         mvprintw(y, x, ".");
      }
   }

}
