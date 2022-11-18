#ifndef LYN_LOCATION_H
#define LYN_LOCATION_H

#include <ostream>

namespace lyn {

struct location {
  int line;
  int col;
};

inline std::ostream &operator<<(std::ostream &stream, const location &loc) {
  return stream << "<" << loc.line << " " << loc.col << ">";
}

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

} // namespace lyn

#endif
