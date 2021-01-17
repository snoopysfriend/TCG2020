#include <assert.h>
#include <cmath>
#include <stdio.h>

#include "board.h"
#include "hash.h"

const int Dir[4] = { -4, 4, -1, 1}; // up down left right

/*
Board::Board(const Board& b) {
    memcpy(board, b.board);
}*/

Pieces::Pieces() {

}
Pieces::Pieces(const Pieces& p) {
    this->empty = p.empty;
    this->color = p.color;
    this->piece = p.piece;
}

int Board::getpiece(int place) {
    return (board[place].piece + board[place].color * 7);
}

Board::Board() {
    turn = false;
}
/*
inline bool Board::is_inside(int index) {
    return board[index].inside;
}*/

int Board::num_piece(bool color) {
    return num_pieces[color];
}

inline bool Board::is_your_cannon(int from, bool color) {
    return (board[from].piece == 1 && board[from].color == color);
}

bool Board::is_empty(int to) {
    return board[to].empty;
}

bool Board::is_legalmove(int from, int to, bool color) {
    return (is_your_piece(from, color) &&
                (is_empty(to) || (is_enemy_piece(to, color) &&
                can_be_eat[board[from].piece][board[to].piece]))
            );
}

inline bool Board::is_enemy_piece(int index, bool color) {
    return (board[index].color != color);
}


int Board::find_legaljump(int from, int idx, bool color) {
    int cover = 0;
    int num = jump_place_num[from][idx];
    for (int i = 0; cover < 2 && i < num; i++) {
        int to = jump_place[from][idx][i];
        //assert(to < 32 && to >= 0);
        if (!is_empty(to)) {
            if (board[to].piece != 7 && is_enemy_piece(to, color) && (cover == 1)) {
                return to;
            } else {
                cover++;
            }
        }
    }
    return -1;
}

void Board::init_board() {
    // init the inside 
    /*for (int i = 0; i < board_size; i++) {
        board[i].inside = false;
    }
    for (int i = 1; i <= ROW; i++){
        for (int j = 1; j <= COL; j++) {
            board[i*COL + j].inside = true;
        }
    }*/
    // init the adjancent
    /*
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            int row1 = i/COL;
            int col1 = i%COL;
            int row2 = j/COL;
            int col2 = j%COL;
            if (abs(row1-row2) == 1 || abs(col1-row2) == 1) {
                adjancent[i][j] = true;
            } else {
                adjancent[i][j] = false;
            }
        }
    }*/
    // init the can_be_eat 
    for (int i = 0; i < piece_type; i++) {
        for (int j = 0; j < piece_type; j++) {
            can_be_eat[i][j] = false; 
        }
    }
    can_be_eat[0][6] = true; // king be eat by little night
    for (int i = 1; i < 7; i++) {
        for (int j = 0; j <= i; j++) {
            can_be_eat[i][j] = true;
        }
    }
    can_be_eat[6][0] = false; // king not eat little night
    // init the same_row_col
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            int row1 = i / COL;
            int col1 = i % COL;
            int row2 = j / COL;
            int col2 = j % COL;
            if (row1 == row2 || col1 == col2) {
                same_row_col[i][j] = true;
            } else {
                same_row_col[i][j] = false;
            }
        }
    }
    // init the move_num and move_place
    for (int from = 0; from < board_size; from++) {
        move_num[from] = 0;
        jump_num[from] = 0;
        for (int j = 0; j < 4; j++) {
            int to = from + Dir[j];
            int row1 = from / COL;
            int col1 = from % COL;
            int row2 = to / COL;
            int col2 = to % COL;
            if (to >= 0 && to < 32 && row2 >=0 && row2 < ROW && col2 >= 0 && col2 < COL && (row1 == row2 || col1 == col2)) {
                move_place[from][move_num[from]++] = to; 
                jump_num[from]++;
                jump_place_num[from][jump_num[from]-1] = 1;
                jump_place[from][jump_num[from]-1][0] = to; 
                to += Dir[j];
                row2 = to / COL;
                col2 = to % COL;
                while (to >= 0 && to < 32 && row2 >=0 && row2 < ROW && col2 >= 0 && col2 < COL && (row1 == row2 || col1 == col2)) {
                    jump_place[from][jump_num[from]-1][jump_place_num[from][jump_num[from]-1]++] = to; 
                    to += Dir[j];
                    row2 = to / COL;
                    col2 = to % COL;
                }
            }
        }
    }
    for (int i = 0; i < 32; i++) {
        printf("move  %d %d\n", i, move_num[i]);
    }
    /*
    for (int from = 0; from < board_size; from++) {
        jump_num[from] = 0;
        for (int j = 0; j < 4; j++) {
            int to = from + Dir[j];
            if (to >= 0 && to < board_size) {
                jump_dir[from][jump_num[from]++] = Dir[j]; 
            }
        }
    }*/
    /*
    int place = 0;
    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 4; j++) {
            board_place[place++] = i * 6 + j;
        }
    }
    // init the value of the board
    // */
    for (int i = 0; i < 32; i++) {
        board[i].empty = false;
        //board[i].dark = true;
        board[i].color = false;
        board[i].piece = 7;
    }
    for(int i = 0; i < 32; i++) {
        for (int j = 0; j < move_num[i]; j++) {
            assert(move_place[i][j] >= 0 && move_place[i][j] < 32);
        }
    }
    this->num_pieces[0] = 0;
    this->num_pieces[1] = 0;

    hash.resetBoard();
    this->hashValue[0] = hash.set_turn(false);
    this->hashValue[1] = hash.set_turn(true);
    this->turn = false;
}


inline bool Board::is_your_piece(int index, bool color) {
    return (board[index].color == color);
}


void Board::flip(int piece_no, const int move) {
    //if (num_pieces[color] < 0) {
    //printf("%d %d %d\n", color, num_pieces[0], num_pieces[1]); assert(false);}
    int place = move / 256;
    int to = move % 256;
    assert(place == to);
    //int piece = new_piece + 7*color;
    int piece = piece_no%7;
    bool color = piece_no/7;
    cover_chess[piece_no]--;
    this->hashValue[this->turn] = hash.flip(place, piece_no);
    board[place].piece = piece;
    board[place].color = color;
    //assert(num_pieces[color] >= 0);
    plist[color][num_pieces[color]].where = place;
    plist[color][num_pieces[color]].piecetype = piece;
    num_pieces[color]++;
    this->turn = !this->turn;
}

void Board::reverse_flip(int piece_no, int move) {
    //int piece = piece_no % 7;
    int place = move / 256;
    //int to = move % 256;
    int color = piece_no / 7;
    board[place].piece = 7;
    board[place].empty = false;
    cover_chess[piece_no]++;
    this->hashValue[this->turn] = hash.flip(place, piece_no);
    num_pieces[color]--;
    this->turn = !this->turn;
    //assert(num_pieces[color] >= 0);
}

void Board::remove_piece(int to, bool color) {

    //if (num_pieces[color] < 0) {
    //printf("%d %d %d\n", color, num_pieces[0], num_pieces[1]); assert(false);}
    num_pieces[color]--;
    for (int i = 0; i <= num_pieces[color]; i++) {
        if (plist[color][i].where == to) {
            plist[color][i] = plist[color][num_pieces[color]];
            break;
        }
    }
    return ;
}

int Board::Expand(const bool color, int *result) {
    int resultCount = 0;

    for (int i = 0; i < num_pieces[color]; i++) {
        int from = plist[color][i].where;
        //assert(from >= 0 && from < 32);
        for (int j = 0; j < move_num[from]; j++) {
            const int to =  move_place[from][j];
            //assert(to < 32 && to >= 0);
            if (is_legalmove(from, to, color)) {
                result[resultCount++] = from * 256 + to;
            } 
        } if (is_your_cannon(from, color)) {
            for (int j = 0; j < jump_num[from]; j++) {
                int to;
                if ((to = find_legaljump(from, j, color)) >= 0) {
                    result[resultCount++] = from * 256 + to;
                }
            }
        }
    }
    return resultCount;
}

int Board::Flip(int result[]) {
    int resultCount = 0;
    for (int i = 0; i < 32; i++) {
        if(board[i].piece == 7) {
            result[resultCount++] = (i * 256 + i);
        }
    }
    return resultCount;
}

int Board::move(const int move) {
    int from = move /256;
    int to = move % 256;
    int removed = -1;
    if (!board[to].empty) {
        removed = board[to].piece;
        remove_piece(to, board[to].color);
    }
    int piece = board[from].piece + 7*board[from].color;
    this->hashValue[this->turn] = hash.do_move(from, to, piece);
    board[to] = board[from];
    board[from].empty = true;
    this->turn = !this->turn;
    return removed; 
}
int Board::MakeMove(const bool color, const int new_piece, const int move) {
    int from = move / 256;
    int to = move % 256;
    int removed = -1;
    //int piece = board[from].piece;
    if (from == to) { // flip the chess
        //flip(from, new_piece, color);
        //assert((new_piece + 7*color) >= 0 && (new_piece + 7*color)< 14);
        int piece = new_piece + 7*color;
        cover_chess[piece]--;
        this->hashValue[this->turn] = hash.flip(from, piece);
    } else {
        if (!board[to].empty) {
            removed = board[to].piece;
            //assert(removed != 7);
            //assert(board[to].color != color);
            remove_piece(to, board[to].color);
        }
        int piece = board[from].piece + 7*board[from].color;
        this->hashValue[this->turn] = hash.do_move(from, to, piece);
        board[to] = board[from];
        board[from].empty = true;
    }
    return removed; 
}

int Board::get_hashValue() {
   return this->hashValue[this->turn]; 
}

void Board::reverse_move(const bool color, const int removed_piece, const int move) {
    int from = move / 256;
    int to = move % 256;
    board[from] = board[to];
    if (removed_piece != -1) {
        board[to].empty = false;
        board[to].color = !color;
        board[to].piece = removed_piece;
        num_pieces[!color]++;
    } else {
        board[to].empty = true;
    }
    this->turn = !this->turn;
}
void Board::reverseMove(const bool color, const int removed_piece, const int move) {
    int from = move / 256;
    int to = move % 256;
    if (from == to) {
        //reverse_flip(from, removed_piece, color);
    } else {
        board[from] = board[to];
        if (removed_piece != -1) {
            board[to].empty = false;
            board[to].color = !color;
            board[to].piece = removed_piece;
            num_pieces[!color]++;
        } else {
            board[to].empty = true;
        }
    }
}

double Board::Evaluate(const bool color) {
  // total score
  double score = 1943; // 1*5+180*2+6*2+18*2+90*2+270*2+810*1
  // static material values
  // cover and empty are both zero
  static const double values[14] = {1, 180, 6, 18, 90, 270, 810,
                                    1, 180, 6, 18, 90, 270, 810};
  for (int i = 0; i < 32; i++) {
    if (!(board[i].empty || (board[i].piece == 7))) {
      if (board[i].color == color) {
        score += values[board[i].piece];
      } else {
        score -= values[board[i].piece];
      }
    }
  }
  return score;
}

double Board::Evaluate2(const bool color) {
    // TODO
    return 0;
}
