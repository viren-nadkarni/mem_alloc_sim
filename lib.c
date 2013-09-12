#include <curses.h>
#include "lib.h"

void print_menu(WINDOW *w, char * menu[], int num, int highlight)
{
	werase(w);
	int i;
	for(i=0;i<num;++i)
	{
		if(i==highlight)
		{
			wattron(w,A_STANDOUT);
			wprintw(w,"%s\n",(menu[i]));
			wattroff(w,A_STANDOUT);
		}
		else
			wprintw(w,"%s\n",(menu[i]));
	}
	wrefresh(w);
}

int menu_control(WINDOW *w, char **menu, int num_options, int current)
{
	char ch;
	
	keypad(w, TRUE);
	noecho();
	cbreak();
	
	while(1)
	{
		print_menu(w, menu, num_options, current);
		ch=getch();
		switch(ch)
		{
			case 3:				
				if(current!=0)
					print_menu(w,menu, num_options, --current);
				break;
				
			case 2:
				if(current+1!=num_options)
					print_menu(w, menu, num_options, ++current);
				break;
				
			case 10:
				return (current+1);
				
			default:
				break;
		}
		wrefresh(w);
		werase(w);
	}
}
