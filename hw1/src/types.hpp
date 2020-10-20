/*
 *  \file types.hpp
 *  \brief game-related types
 *  \author Maria Elsa (elsa)
 *  \course Theory of Computer Game (TCG)
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include "color.hpp"

#define MAX_LENGTH 10
#define MAX_WIDTH 10
#define AREA_MAX 100
#define INF 1000000000

enum Item : char {
    WALL = '#',
    PLAYER = '@',
    BALL = 'O',
    BOX = '$',
    BALL_BOX = '*',
    FLOOR = '-',
    NOITEM = 'x',
};

enum Direction : int {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
};

inline Direction& operator++(Direction& d) {
    return d = Direction(int(d) + 1);
}

inline Direction operator~(const Direction &in) {
    switch (in) {
        case UP:
            return DOWN;
        case DOWN:
            return UP;
        case RIGHT:
            return LEFT;
        case LEFT:
            return RIGHT;
        default:
            return NONE;
    }
}

inline std::ostream& operator<<(std::ostream &os, const Item &i) {
    switch (i) {
        case WALL:
            os << Color::BG_RED << Color::FG_RED;
            break;
        case BALL_BOX:
            os << Color::BG_GREEN << Color::FG_YELLOW;
            break;
        case BALL:
            os << Color::BOLD << Color::FG_PINK;
            break;
        case FLOOR:
            break;
        case BOX:
            os << Color::BOLD << Color::FG_YELLOW;
            break;
        case PLAYER:
            os << Color::BOLD << Color::FG_PINK;
            break;
        case NOITEM:
            break;
    }
    os << static_cast<char>(i) << Color::CLEAR;
    return os;
}

inline char dir_to_char(const Direction dir) {
    char symbol;

    switch (dir) {
        case UP:
            symbol = '^'; break;
        case DOWN:
            symbol = 'v'; break;
        case RIGHT:
            symbol = '>'; break;
        case LEFT:
            symbol = '<'; break;
        default:
            symbol = ' '; break;
    }

    return symbol;
}

inline Direction char_to_dir(const char symbol) {
    Direction dir;

    switch (symbol) {
        case '^':
            dir = UP; break;
        case 'v':
            dir = DOWN; break;
        case '>':
            dir = RIGHT; break;
        case '<':
            dir = LEFT; break;
        default:
            dir = NONE; break;
    }

    return dir;
}

#endif // #ifndef TYPES_HPP
