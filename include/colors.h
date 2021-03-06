#ifndef _COLORS_H
#define _COLORS_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define RED(x) ANSI_COLOR_RED x ANSI_COLOR_RESET
#define GREEN(x) ANSI_COLOR_GREEN x ANSI_COLOR_RESET

#endif /* _COLORS_H */
