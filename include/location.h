#ifndef COMPILER_LOCATION_H
#define COMPILER_LOCATION_H

struct location {
  int line;
  int col;
};

#define YYLLOC_DEFAULT(Cur, Rhs, N)                                            \
  do                                                                           \
    if (N) {                                                                   \
      (Cur).line = YYRHSLOC(Rhs, 1).line;                                      \
      (Cur).col = YYRHSLOC(Rhs, 1).col;                                        \
    } else {                                                                   \
      (Cur).line = YYRHSLOC(Rhs, 0).line;                                      \
      (Cur).col = YYRHSLOC(Rhs, 0).col;                                        \
    }                                                                          \
  while (0)

#endif
