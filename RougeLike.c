/*
 * My RougeLike version 2
 */
#include <ncurses.h>
#include <stdlib.h>

typedef struct Player_ {
   int xPosition;
   int yPosition;
   int health;
} Player;

int screenSetUp(); // ncurses setup
int mapSetUp(); //
Player* playerSetUp();
int handleInput(int input, Player* user);
int playerMove(int y, int x, Player* user);

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

int mapSetUp()
{
   mvprintw(13, 13, "------------");
   mvprintw(14, 13, "|..........|");
   mvprintw(15, 13, "|..........|");
   mvprintw(16, 13, "|..........|");
   mvprintw(17, 13, "|..........|");
   mvprintw(18, 13, "------------");


   mvprintw(13, 40, "------------");
   mvprintw(14, 40, "|..........|");
   mvprintw(15, 40, "|..........|");
   mvprintw(16, 40, "|..........|");
   mvprintw(17, 40, "|..........|");
   mvprintw(18, 40, "------------");


   return 1; // 1 == Succes
}

Player* playerSetUp()
{
   Player* newPlayer;
   newPlayer = malloc(sizeof(Player));
   newPlayer->xPosition = 14;
   newPlayer->yPosition = 14;
   newPlayer->health = 14;

   playerMove(newPlayer->yPosition, newPlayer->xPosition, newPlayer);

   return newPlayer;
}

int handleInput(int input, Player* user)
{
   switch (input) {
      /* move up */
   case 'k':
   case 'K':
      playerMove(user->yPosition - 1, user->xPosition, user);
      break;

      /* move down */
   case 'j':
   case 'J':
      playerMove(user->yPosition + 1, user->xPosition, user);
      break;

      /* move left */
   case 'h':
   case 'H':
      playerMove(user->yPosition, user->xPosition - 1, user);
      break;

      /* move right */
   case 'l':
   case 'L':
      playerMove(user->yPosition, user->xPosition + 1, user);
      break;

   default:
      break;

   }

}

int playerMove(int y, int x, Player* user)
{
   mvprintw(user->yPosition, user->xPosition, ".");

   user->yPosition   = y;
   user->xPosition   = x;

   mvprintw(user->yPosition, user->xPosition, "@");
   move(user->yPosition, user->xPosition);


}
