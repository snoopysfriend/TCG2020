/*
 *  \file sokoboru.cpp
 *  \brief game logic
 *  \author Maria Elsa (elsa)
 *  \course Theory of Computer Game (TCG)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>

#include "sokoboru.hpp"

Board::Board() = default;

bool Board::init_board(std::ifstream &infile) {
    std::string line;

    if (infile.is_open()) {
        infile >> length >> width;
        std::getline(infile, line);
        if (infile.eof()) return false;

        for (int i = 0; i < length; ++i) {
            std::getline(infile, line);
            strcpy(initial[i], line.c_str());
        }
    }

    reset();
    return true;
}

void Board::reset() {
    assert(length <= MAX_LENGTH && width <= MAX_WIDTH);

    for (int y = 0; y < length; ++y) {
        for (int x = 0; x < width; ++x) {
            state[y][x] = initial[y][x];
            if (state[y][x] == PLAYER) {
                px = x, py = y;
            }
        }
    }

    moves.clear();
    penalty = 0;
}

bool Board::is_valid(int new_px, int new_py) const {
    if (new_px < 0 || new_py < 0 || new_px >= width || new_py >= length) {
        return false;
    }

    if (state[new_py][new_px] == WALL) {
        return false;
    }

    return true;
}

bool Board::do_move(Direction dir) {
    Move m;
    bool push = false;
    bool slide = false;
    bool ball_box = false;
    int new_px = px + dx[dir];
    int new_py = py + dy[dir];

    if (is_valid(new_px, new_py)) {
        if (state[new_py][new_px] == BOX or state[new_py][new_px] == BALL_BOX) {
            push = true;
        } else if (state[new_py][new_px] == BALL) {
            push = true;
            slide = true;
        }
        if (push) {
            int box_x = new_px + dx[dir];
            int box_y = new_py + dy[dir];
            if (slide) {
                bool ball;
                std::tie(ball, m) = move_ball(new_px, new_py, dir);
                if (!ball) return false;
            } else {
                bool box;
                if (state[new_py][new_px] == BALL_BOX) ball_box = true;
                std::tie(box, m) = move_box(new_px, new_py, box_x, box_y, false);
                if (!box) return false;
                if (ball_box) penalty += 1;
            }
        }
        move_player(new_px, new_py);
        m.symbol = dir_to_char(dir);
        m.push = push;
        m.slide = slide;
        moves.push_back(m);
        penalty += 1;
        return true;
    }
    return false;
}

void Board::undo_move() {
    if (move_count() == 0) {
        return;
    }
    Move m = moves.back();
    moves.pop_back();
    Direction dir = char_to_dir(m.symbol);
    int old_px = px - dx[dir];
    int old_py = py - dy[dir];
    int old_box_x = px, old_box_y = py;
    int box_x = px + dx[dir];
    int box_y = py + dy[dir];
    move_player(old_px, old_py);

    if (m.push && !m.slide) {
        move_box(box_x, box_y, old_box_x, old_box_y, m.merge);
        if (state[old_box_y][old_box_x] == BALL_BOX) penalty -= 1;
    } else if (m.push && m.slide) {
        move_ball(m.ball_x, m.ball_y, ~dir);
    }
    penalty -= 1;
}

Item Board::sourceItem(Item moving, int from_x, int from_y, bool split) {
    if (moving == PLAYER) {
        return FLOOR;
    } else if (moving == BOX && state[from_y][from_x] == BOX) {
        return FLOOR;
    } else if (!split && moving == BALL_BOX) {
        return FLOOR;
    } else if (split && moving == BALL_BOX ) {
        return BALL;
    } else if (moving == BALL && state[from_y][from_x] == BALL_BOX) {
        return BOX;
    } else if (moving == BALL) {
        return FLOOR;
    } else {
        return NOITEM;
    } 
}

Item Board::destItem(Item moving, int to_x, int to_y, bool split, bool &merge) {
    if (moving == PLAYER && state[to_y][to_x] != WALL) {
        return PLAYER;
    } else if (moving == BOX && state[to_y][to_x] == FLOOR) {
        return BOX;
    } else if (moving == BOX && state[to_y][to_x] == BALL) {
        merge = true;
        return BALL_BOX;
    } else if (!split && moving == BALL_BOX && state[to_y][to_x] == FLOOR) {
        return BALL_BOX;
    } else if (split && moving == BALL_BOX && state[to_y][to_x] == FLOOR) {
        return BOX;
    } else if (moving == BALL && state[to_y][to_x] == FLOOR) {
        return BALL;
    } else if (moving == BALL && state[to_y][to_x] == BOX) {
        merge = true;
        return BALL_BOX;
    } else {
        return NOITEM;
    }
}

void Board::move_player(int to_x, int to_y) {
    bool merge = false;
    state[py][px] = sourceItem(PLAYER, px, py, false);
    state[to_y][to_x] = destItem(PLAYER, to_x, to_y, false, merge);
    px = to_x, py = to_y;
}

std::tuple<bool, Move> Board::move_box(int from_x, int from_y, int to_x, int to_y, bool split) {
    Item source, destination;
    Move m;

    if (!is_valid(to_x, to_y)) return std::make_tuple(false, m);

    source = sourceItem(Item(state[from_y][from_x]), from_x, from_y, split);
    destination = destItem(Item(state[from_y][from_x]), to_x, to_y, split, m.merge);

    if (source == NOITEM || destination == NOITEM)
        return std::make_tuple(false, m);
    
    state[from_y][from_x] = source;
    state[to_y][to_x] = destination;
    return std::make_tuple(true, m);
}

std::tuple<int, int> Board::ball_destination(int from_x, int from_y, Direction dir) {
    int to_x = from_x;
    int to_y = from_y;
    while (true) {
        to_x += dx[dir];
        to_y += dy[dir];
        if (state[to_y][to_x] == BALL_BOX || state[to_y][to_x] == BALL || state[to_y][to_x] == WALL || state[to_y][to_x] == PLAYER || !is_valid(to_x, to_y)) {
            to_x -= dx[dir];
            to_y -= dy[dir];
            break;
        } else if (state[to_y][to_x] == BOX) break;
    }
    return std::make_tuple(to_x, to_y);
}

std::tuple<bool, Move> Board::move_ball(int from_x, int from_y, Direction dir) {
    Move m;
    Item source, destination;
    int to_x = from_x + dx[dir];
    int to_y = from_y + dy[dir];
    
    if (!is_valid(to_x, to_y)) return std::make_tuple(false, m);

    std::tie(to_x, to_y) = ball_destination(from_x, from_y, dir);
    source = sourceItem(BALL, from_x, from_y, false);
    destination = destItem(BALL, to_x, to_y, false, m.merge);

    if (source == NOITEM || destination == NOITEM)
        return std::make_tuple(false, m);
    
    state[from_y][from_x] = source;
    state[to_y][to_x] = destination;
    m.ball_x = to_x;
    m.ball_y = to_y;
    return std::make_tuple(true, m);
}

int Board::move_count() const { return moves.size(); }

bool Board::is_terminal() const {
    for (int y = 0; y < length; ++y) {
        for (int x = 0; x < width; ++x) {
            if (state[y][x] == BOX) {
                return false;
            }
        }
    }
    return true;
}

int Board::get_penalty() const { return penalty; }

std::string Board::display_board() const {
    std::stringstream ss;
    for (int y = 0; y < length; ++y) {
        for (int x = 0; x < width; ++x) {
            ss << Item(state[y][x]);
        }
        ss << std::endl;
    }

    return ss.str();
}

std::string Board::display_moves() const {
    std::stringstream ss;
    
    if (moves.size() == 0) {
        ss << "No history yet ~ OAO\n";
    }

    for (Move move: moves) {
        ss << move.symbol;
    }

    return ss.str();
}
