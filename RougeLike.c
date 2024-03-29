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
#include <time.h>

typedef struct Position_ {
   int x;
   int y;
} Position;

typedef struct Room_ {
   Position position;
   int height;
   int width;
   Position doors[4];
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
int connectDoors(Position* doorOne, Position* doorTwo);

/* room functions */
Room* createRoom(int x, int y, int height, int width);
int drawRoom(Room* room);

int main()
{
   Player* user;
   screenSetUp();
   int ch;

   srand(time(NULL));      // don't but this at any fun called more than once..

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
   noecho();
   start_color();
   refresh();

   return 0;
}

Room** mapSetUp()
{
   Room** rooms = malloc(sizeof(Room)*6);
   init_pair(1, COLOR_RED, COLOR_BLACK);
   init_pair(2, COLOR_WHITE, COLOR_BLACK);

   rooms[0] = createRoom(13, 13, 6, 8);
   drawRoom(rooms[0]);
   rooms[1] = createRoom(30, 13, 10, 20);
   drawRoom(rooms[1]);
   rooms[2] = createRoom(13, 25, 10, 10);
   drawRoom(rooms[2]);

   connectDoors(&rooms[0]->doors[2], &rooms[2]->doors[0]);
   connectDoors(&rooms[1]->doors[2], &rooms[2]->doors[3]);
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
   // Check with color of floor,
   // white == allow to move over
   switch(mvinch(newY, newX) & A_COLOR)
   {
      case COLOR_PAIR(2):
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


   newRoom->position.x  = x;
   newRoom->position.y  = y;
   newRoom->height      = height;
   newRoom->width       = width;

   /* top door */
   newRoom->doors[0].x  = rand() % (width -2) + newRoom->position.x +1;
   newRoom->doors[0].y  = newRoom->position.y;

   /* left door */
   newRoom->doors[1].y  = rand() % (height -2) + newRoom->position.y +1;
   newRoom->doors[1].x  = newRoom->position.x;

   /* bottom door */
   newRoom->doors[2].x  = rand() % (width -2) + newRoom->position.x +1;
   newRoom->doors[2].y  = newRoom->position.y + newRoom->height -1;

   /* right door */
   newRoom->doors[3].y  = rand() % (height -2) + newRoom->position.y +1;
   newRoom->doors[3].x  = newRoom->position.x + width -1;

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
      attron(COLOR_PAIR(1));
      mvprintw(room->position.y, x, "-"); /* top */
      mvprintw(room->position.y + room->height -1, x, "-"); /* bottom */
   }

   /* draw floors and side walls */
   for(y = room->position.y +1; y < room->position.y + room->height -1; ++y)
   {
      /* draw side walls */
      attron(COLOR_PAIR(1));
      mvprintw(y, room->position.x, "|");
      mvprintw(y, room->position.x + room->width -1, "|");
      attron(COLOR_PAIR(2));
      /* draw floors */
      for(x = room->position.x +1; x < room->position.x + room->width -1; ++x)
      {
         mvprintw(y, x, ".");
      }
   }

   /* draw doors */
   mvprintw(room->doors[0].y, room->doors[0].x, "+");
   mvprintw(room->doors[1].y, room->doors[1].x, "+");
   mvprintw(room->doors[2].y, room->doors[2].x, "+");
   mvprintw(room->doors[3].y, room->doors[3].x, "+");

}

int connectDoors(Position* doorOne, Position* doorTwo)
{
   Position temp;
   Position prev;
   int count = 0;

   temp.x   = doorOne->x;
   temp.y   = doorOne->y;

   prev     = temp;

   while(1)
   {
      /* step left */
      if( (abs((temp.x - 1) - doorTwo->x) < abs(temp.x - doorTwo->x)) && (mvinch(temp.y, temp.x -1) == ' ') )
      {
         prev.x   = temp.x;
         temp.x = temp.x -1;
      }

      /* step right */
      else if( (abs((temp.x + 1) - doorTwo->x) < abs(temp.x - doorTwo->x)) && (mvinch(temp.y, temp.x +1) == ' ') )
      {
         prev.x   = temp.x;
         temp.x = temp.x +1;
      }

      /* step down */
      else if( (abs((temp.y + 1) - doorTwo->y) < abs(temp.y - doorTwo->y)) && (mvinch(temp.y +1, temp.y) == ' ') )
      {
         prev.y   = temp.y;
         temp.y = temp.y +1;
      }

      /* step up */
      else if( (abs((temp.y - 1) - doorTwo->y) < abs(temp.y - doorTwo->y)) && (mvinch(temp.y -1, temp.y) == ' ') )
      {
         prev.y   = temp.y;
         temp.y = temp.y -1;
      }

      else {
         if(count == 0) {
            temp  = prev;
            ++count;
            continue;
         } else {
            return 0;
         }
      }

      mvprintw(temp.y, temp.x, "#");

   }

   return 1; // Success
}
