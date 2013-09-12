#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lib.h"

WINDOW *disp, *inp1, *inp2;
int mode=1, block_num=70, block_sz=1, seed=20, *ram;
char *mode_str[]={"Best fit","First fit","Worst fit"};

void update_mode_info();
void random_fill_ram();
void print_ram();

void update_mode_info()
{
	mvprintw(LINES-1,COLS-25,"Mode: %s               ",mode_str[mode]);
	mvprintw(LINES-2,COLS-25,"Number of blocks: %d   ",block_num);
	mvprintw(LINES-3,COLS-25,"Block size: %d         ",block_sz);
}

void random_fill_ram()
{
	int i;
	
	srand(time(NULL));
		
	/*randomly fill array*/
	for(i=0;i<block_num;++i)
	{
		ram[i]=(((rand()%100)<seed)?1:0);
	}
	
	print_ram();
}

void print_ram()
{
	int i,x,y;
	
	/*print ram array*/
	wmove(disp,0,2);
	for(i=0;i<block_num;++i)
	{
		if(ram[i])
		{
			wattron(disp,A_STANDOUT);
			wprintw(disp," ");
			wattroff(disp,A_STANDOUT);
			wrefresh(disp);
		}
		else 
		{
			/*if(i==0||i==block_num-1)
				wprintw(disp,"|");
			else*/
				wprintw(disp," ");
		}
	}
	
	/*print ram scale*/	
	for(i=0;i<block_num+1;i+=5)
		mvwprintw(disp,2,1+i,"%02d",i);
	
	mvaddch(2, 2, ACS_ULCORNER);
	mvaddch(2, 73, ACS_URCORNER);
	mvaddch(4, 2, ACS_LLCORNER);
	mvaddch(4, 73, ACS_LRCORNER);
	
	mvhline(2, 3, ACS_HLINE, 70);
	mvhline(4, 3, ACS_HLINE, 70);
	
	/*
	mvvline(3, 2, ACS_VLINE, 1);
	mvvline(3, 73, ACS_VLINE, 1);
		
	mvaddch(3, 2, ACS_VLINE);
	mvaddch(3, 73, ACS_VLINE);
	*/
	
	wrefresh(disp);
}

int main(int argc, char *argv[])
{
	initscr();
	
	char title[]="Memory Allocation Simulator",ch;
	int i;
	
	/*print the title*/
	attron(A_STANDOUT);
	printw("%s",title);
	for(i=strlen(title);i<COLS;++i)
		printw(" ");
	printw("\n");
	attroff(A_STANDOUT);
	
	/*print mode info*/
	update_mode_info();
	
	/*print lower panel*/
	attron(A_STANDOUT);
	mvprintw(LINES-2,0," A ");
	attroff(A_STANDOUT);
	printw(" Request memory\t");
	
	attron(A_STANDOUT);
	mvprintw(LINES-1,0," M ");
	attroff(A_STANDOUT);
	printw(" Change mode\t\t");
	
	attron(A_STANDOUT);
	printw(" R ");
	attroff(A_STANDOUT);
	printw(" Reset\t");
	
	attron(A_STANDOUT);
	printw(" Q ");
	attroff(A_STANDOUT);
	printw(" Quit");
	
	refresh();
		
	/*create disp window and randomly fill ram*/
	disp=newwin(6,COLS-2,3,1);	
	ram=malloc(block_num*sizeof(int));
	random_fill_ram();
		
	/*main event loop*/
	curs_set(0);
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	while((ch=getch())!='Q' && ch!='q')
	{
		switch(ch)
		{
			/*change mode*/
			case 'm':
			case 'M':
			{
				inp1=newwin(5,COLS/4,LINES/3,(COLS-COLS/4)/2);
				inp2=newwin(3,(COLS/4)-2,(LINES/3)+1,((COLS-COLS/4)/2)+1);
				box(inp1,0,0);
				wrefresh(inp1);
				
				mode=menu_control(inp2,mode_str,3,mode);
				mode--;
				
				wborder(inp1, ' ', ' ', ' ',' ',' ',' ',' ',' ');
				wrefresh(inp1);
				wrefresh(inp2);
				delwin(inp1);
				delwin(inp2);
								
				update_mode_info();
				
				mvprintw(LINES-5,0,"MESSAGE: Memory allocation mode changed to %s                                  ",mode_str[mode]);
				refresh();
				
				break;
			}
			
			/*request memory space*/
			case 'a':
			case 'A':
			{
				int request=0;
				
				inp1=newwin(5,COLS/4,LINES/3,(COLS-COLS/4)/2);
				inp2=newwin(3,(COLS/4)-2,(LINES/3)+1,((COLS-COLS/4)/2)+1);
				box(inp1,0,0);
				wrefresh(inp1);
				
				echo();
				curs_set(1);
				cbreak();
				
				wprintw(inp2,"Enter number of \nblocks requested:\n");
				wscanw(inp2,"%d",&request);
				
				noecho();
				curs_set(0);
				
				wborder(inp1, ' ', ' ', ' ',' ',' ',' ',' ',' ');
				wrefresh(inp1);
				wrefresh(inp2);
				delwin(inp1);
				delwin(inp2);
				
				/*make a pass throught the ram to find available space*/
				/*store it's size and address in cont_ */
				int abc_sz=0,abc_addr=-1;
				int cont_sz=0,cont_addr=-1;
					
				/*default case for WORST FIT*/
				for(i=0;i<block_num;++i)
				{
					if(ram[i]==0)
					{
						if(abc_addr<0)
							abc_addr=i;
						abc_sz++;
						
						if(mode==1 && request<=abc_sz)/*break out of the loop on encounter of first free space */
						{/*FIRST FIT*/
							cont_addr=abc_addr;
							cont_sz=abc_sz;
							break;
						}
					}
					else
					{
						if(mode==0 && abc_sz==request)
						{/*BEST FIT*/
							cont_sz=abc_sz;
							cont_addr=abc_addr;
							break;
						}
						if(abc_sz>cont_sz)
						{/*WORST FIT??? OR DEFAULT??*/
							cont_sz=abc_sz;
							cont_addr=abc_addr;
						}
						abc_sz=0;
						abc_addr=-1;
					}
				}
				
				/*display result*/
				if(cont_addr==-1 || cont_sz<request)
				{/*print failure message*/
					mvprintw(LINES-5,0,"FAILURE: No continuous array of blocks were found suitable. [%d{%d}]           ",cont_sz,cont_addr);
					refresh();
				}
				else
				{
					if(mode==2)
					{/*make worst fit worse*/
						int tmp;
						tmp=cont_sz-request;
						tmp/=2;
						cont_addr+=tmp;
					}
					
					/*update the ram*/
					for(i=cont_addr;i<request+cont_addr;++i)
						ram[i]=1;
					print_ram();
					
					mvprintw(LINES-5,0,"SUCCESS: %d blocks starting at {%d} were found and allocated                   ",request,cont_addr);
					refresh();
				}
				break;
			}	
			
			/*reset*/
			case 'r':
			case 'R':
			{
				inp1=newwin(5,COLS/4,LINES/3,(COLS-COLS/4)/2);
				inp2=newwin(3,(COLS/4)-2,(LINES/3)+1,((COLS-COLS/4)/2)+1);
				box(inp1,0,0);
				wrefresh(inp1);
				
				echo();
				curs_set(1);
				cbreak();
				
				wprintw(inp2,"Enter seed value \nbetween 0 to 100\n(current %d): ",seed);
				wscanw(inp2,"%d",&seed);
				
				noecho();
				curs_set(0);
				
				wborder(inp1, ' ', ' ', ' ',' ',' ',' ',' ',' ');
				wrefresh(inp1);
				wrefresh(inp2);
				delwin(inp1);
				delwin(inp2);
				
				random_fill_ram();
				
				mvprintw(LINES-5,0,"MESSAGE: The memory was reset [seed=%d]                                        ",seed);
				break;
			}
			default:
				break;
		}
	}
	endwin();
	return (EXIT_SUCCESS);
}
