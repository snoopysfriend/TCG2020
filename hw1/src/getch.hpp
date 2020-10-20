/*! \file getch.hpp
    \brief gain terminal keystroke in Linux
    \author Yung-Hsien Chung (hiiragi4000)
    \course Theory of Computer Game (TCG)
*/
#ifndef GETCH_HPP
#define GETCH_HPP

#if defined(_WIN32) || defined(WIN32)
    #include <conio.h>
#elif defined __unix__
    // return the character read after a keystroke immediately
    // similar to the 'getch()' defined in conio.h on Windows
    extern char getch();
    extern char getche();
#endif

#endif
