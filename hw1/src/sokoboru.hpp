/*
 *  \file sokoboru.hpp
 *  \brief game-related structure
 *  \author Maria Elsa (elsa)
 *  \course Theory of Computer Game (TCG)
 */

#ifndef SOKOBORU_HPP
#define SOKOBORU_HPP

#include <vector>
#include <tuple>

#include "types.hpp"

struct Move {
    // Direction symbol ^v><
    char symbol;
    // Whether this is a push move
    bool push;
    // Whether this is a sliding move
    bool slide;
    // Whether this is a merging move
    bool merge;
    // Original position of ball, if sliding move
    int ball_x, ball_y;

    Move() : symbol(NOITEM), push(false), slide(false), merge(false), ball_x(-1), ball_y(-1) { }
};

using History = std::vector<Move>;

constexpr const int dx[4] = {0, 0, 1, -1};
constexpr const int dy[4] = {-1, 1, 0, 0};

class Board {
  public:
    explicit Board();
    bool init_board(std::ifstream &infile);
    void reset();
    bool do_move(Direction dir);
    void undo_move();
    int move_count() const;
    bool is_terminal() const;
    int get_penalty() const;
    std::string display_board() const;
    std::string display_moves() const;

  protected:
    bool is_valid(int x, int y) const;
    virtual Item sourceItem(Item moving, int from_x, int from_y, bool split);
    virtual Item destItem(Item moving, int to_x, int to_y, bool split, bool &merge);
    virtual void move_player(int new_px, int new_py);
    virtual std::tuple<bool, Move> move_box(int new_px, int new_py, int box_x, int box_y, bool split);
    virtual std::tuple<bool, Move> move_ball(int from_x, int from_y, Direction dir);
    std::tuple<int, int> ball_destination(int from_x, int from_y, Direction dir);

    int length, width;
    // Initial game state
    char initial[MAX_LENGTH][MAX_WIDTH];
    // Current game state
    char state[MAX_LENGTH][MAX_WIDTH];
    // Player position
    int px, py;
    // Moves history
    History moves;
    int penalty;
};

#endif // #ifndef SOKOBORU_HPP
