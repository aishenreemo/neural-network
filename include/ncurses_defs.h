#ifndef __NCURSES_DEFS_H__
#define __NCURSES_DEFS_H__

#include <ncurses.h>

// man curs_initscr
#define ncurses_init_screen() initscr()
#define ncurses_endwin()      endwin()

// man curs_kernel
#define ncurses_cursor_visibility(b) curs_set(b)

// man curs_inopts
#define ncurses_keypad(win, bf) keypad(win, bf)
#define ncurses_raw_mode()      raw()
#define ncurses_no_echo()       noecho()

// man curs_refresh
#define ncurses_refresh() refresh()

// man curs_clear
#define ncurses_clear_nr()          erase()
#define ncurses_clear()             clear()

// man curs_printw
#define ncurses_move_printw(column,line,fmt_args...) mvprintw(line,column,fmt_args)

// man curs_color
#define ncurses_has_colors()         has_colors()
#define ncurses_use_default_colors() use_default_colors()
#define ncurses_start_color()	     start_color()
#define ncurses_init_pair(e,fg,bg)   init_pair(e,fg,bg)

// other
#define ncurses_move_hline(column,line,chtype,length) mvhline(line,column,chtype,length)
#define ncurses_move_vline(column,line,chtype,length) mvvline(line,column,chtype,length)
#define ncurses_move_addch(column,line,ch)            mvaddch(line,column,ch);

#endif // __NCURSES_DEFS_H__
