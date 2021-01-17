#ifndef __BOARD__
#define __BOARD__

#include "hash.h"

#define board_size 32 // (8+2) * (4+2)
#define piece_type 8 // (兵 炮 馬 車 象 士 將) * 2 + cover
#define piece_num 32 // each 16
#define ROW 8
#define COL 4


class Pieces {
public:
    Pieces();
    Pieces(const Pieces& p);
    //bool inside; // 1 if inside the board
    bool empty;
    //bool dark;
    bool color; // 0 for black or 1 for red
    int piece; 
};

struct PL{
    int where;
    int piecetype;
};

class Board{
public:
    Board();
    //Board(const Board& b);
    void init_board();
    int Expand(const bool color, int *result);
    int get_hashValue();
    int Flip(int *result);
    double Evaluate(const bool color);
    double Evaluate2(const bool color);
    int MakeMove(const bool color, const int piece, const int move);
    int move(const int move);
    void reverseMove(const bool color, const int removed_piece, const int move);
    void reverse_move(const bool color, const int removed_piece, const int move);
    void flip(int piece_no, const int move);
    void reverse_flip(int piece_no, int move);
    int getpiece(int place);
    inline bool is_your_piece(int from, bool color);
//    inline bool is_inside(int index);
    inline bool is_your_piece(int index);
    int num_piece(bool color);
    bool is_empty(int to);
    void remove_piece(int i, bool color);
    void do_move();

    int cover_chess[14] = {5, 2, 2, 2, 2, 2, 1, 5, 2, 2, 2, 2, 2, 1}; 
    Pieces board[8*4];
    int hashValue[2];
    bool turn; // 0 not your turn 1 your turn
private:
    Hash_Function hash;
    PL plist[2][16];
    bool your_color; // 
    int num_pieces[2];
    //Piece board[(4+2)*(8+2)];
    bool adjancent[board_size][board_size];
    bool can_be_eat[piece_type][piece_type];
    bool same_row_col[board_size][board_size];
    int move_num[board_size];
    int move_place[board_size][4]; 
    int jump_num[board_size];
    int jump_dir[board_size][4]; 
    int jump_place_num[board_size][4]; 
    int jump_place[board_size][4][10]; 

    inline bool is_your_cannon(int from, bool color);
    inline bool is_enemy_piece(int from, bool color);
    bool is_legalmove(int from, int to, bool color);
    int find_legaljump(int from, int dir, bool color);
};


#endif
