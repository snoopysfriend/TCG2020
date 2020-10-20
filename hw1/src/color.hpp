/*  
 *  \file color.hpp
 *  \brief ANSI color codes
 *  \author Maria Elsa (elsa)
 *  \course Theory of Computer Game (TCG)
 */

#include <ostream>

namespace Color {

enum Code {
  CLEAR       = 0,
  BOLD        = 1,
  // Foreground color
  FG_RED      = 31,
  FG_GREEN    = 32,
  FG_CYAN     = 36,
  FG_GRAY     = 90,
  FG_YELLOW   = 93,
  FG_PINK     = 95,
  // Background color
  BG_RED      = 41,
  BG_GREEN    = 42,
  BG_BLUE     = 44
};

inline std::ostream& operator<<(std::ostream& os, const Code& code) {
  if (code == CLEAR) {
    os << "\033[m";
  } else {
    os << "\033[" << static_cast<int>(code) << "m";
  }

  return os;
}

} // namespace Color
