#ifndef EINSTEIN_HPP
#define EINSTEIN_HPP

#include <sstream>
#include <cassert>

#include "types.hpp"

class Board {
  public:
    
    void init(std::string topLeft, std::string bottomRight) {
        sideToMove = RED;
        status = Status::RedPlay;
        gameLength = 0;
        // Initialize square board
        for (Square s = SQ_A1; s < SQUARE_NB; ++s) {
            board[s] = NO_PIECE;
        }

        hl_move = MOVE_NONE;

        set<RED>(topLeft);
        set<BLUE>(bottomRight);
    }

	void init2() {
		for (int i = 0; i < 12; i++) {
			pieces[i] = SQ_NONE;
		}
        for (Square s = SQ_A1; s < SQUARE_NB; ++s) {
            if ( board[s] != NO_PIECE ) {
				pieces[board[s]] = s;
			}
        }
	}

    template <Color c>
    void set(std::string init_pc_pos) {
        assert(init_pc_pos.size() == PIECE_TYPE_NB);
        for (int i = 0; i < PIECE_TYPE_NB; ++i) {
            put_piece(
                make_piece(c, PieceType(init_pc_pos[i]-'0')),
                init_sq_pos[c][i]
            );
        }
        num_pieces[c] = PIECE_TYPE_NB;
    }

    Move str_to_move(char num_c, char dir_c) const {
        if (num_c == '-' && dir_c == '-') {
            return MOVE_PASS;
        }

        int num, dir, d;
        Piece pc = NO_PIECE;
        Square from;

        num = num_c - '0';
        dir = dir_c - '0';

        for (Square s = SQ_A1; s < SQUARE_NB; ++s) {
            pc = piece_on(s);
            if (color_of(pc) == sideToMove && type_of(pc) == num) {
                from = s;
                break;
            }
        }

        assert(pc != NO_PIECE);

        d = (dir == 0 ? NORTH : (dir == 1) ? EAST : NORTH_EAST);
        if (sideToMove == BLUE) d = -d;

        Move m = make_move(from, from + Direction(d));
        assert(is_move_ok(m));
        return m;
    }

    void do_move(char num_c, char dir_c) {
        bool found = false;

        Move m = str_to_move(num_c, dir_c);

        MoveList mL;
        if (sideToMove == RED) {
            legal_actions<RED>(mL);
        } else if (sideToMove == BLUE) {
            legal_actions<BLUE>(mL);
        }

        for (Move legal_move: mL) {
            if (m == legal_move) {
                found = true;
                break;
            }
        }

        if (!found) {
            std::cerr << num_c << ", " << dir_c << m;
            throw std::runtime_error("Board::do_move: game is over or num/direction is invalid");
        }

        do_move(m);
    }

	void do_move2(int m) {
        if (m != -1) {

            Square from = Square(m>>10);
            Square to = Square(m & 0xFF);
            Piece pc = piece_on(from);
            Piece captured = piece_on(to);

            if (captured != NO_PIECE) {
                remove_piece(pc, from);
				pieces[captured] = SQ_NONE;
                num_pieces[color_of(captured)]--;
            }

            move_piece(pc, from, to);
			pieces[pc] = to;
        }
        sideToMove = ~sideToMove;
        status = (sideToMove == RED) ? Status::RedPlay : Status::BluePlay;
        gameLength++;

	}

    void do_move(Move m) {
        if (m != MOVE_PASS) {

            Square from = from_sq(m);
            Square to = to_sq(m);
            Piece pc = piece_on(from);
            Piece captured = piece_on(to);

            if (captured != NO_PIECE) {
                remove_piece(pc, from);
                num_pieces[color_of(captured)]--;
            }

            move_piece(pc, from, to);
			pieces[pc] = to;
        }
        sideToMove = ~sideToMove;
        status = (sideToMove == RED) ? Status::RedPlay : Status::BluePlay;
        gameLength++;
    }

    std::string move_to_str(Move m) const {
        std::stringstream ss;

        if (m == MOVE_PASS) {
            ss << "--";
        } else {
            Square from = from_sq(m);
            Square to = to_sq(m);
            Piece pc = piece_on(from);
            int dir = abs(to - from) == PIECE_TYPE_NB ? 0 :
                      abs(to - from) == 1 ? 1 : 2;

            ss << type_of(pc) << dir;
            assert(type_of(pc) >= 0 && type_of(pc) < PIECE_TYPE_NB);
            assert(dir >= 0 && dir < 3);
        }

        return ss.str();
    }

    template <Color Us> 
	int legal_actions2(MoveList2 &mL) {
		int idx = 0;
        constexpr Direction Ver = (Us == RED ? NORTH : SOUTH);
        constexpr Direction Hor = (Us == RED ? EAST : WEST);
        constexpr Direction Diag = (Us == RED ? NORTH_EAST : SOUTH_WEST);
        constexpr File Edge = (Us == RED ? FILE_F : FILE_A);

		int start = (Us == RED?0:6);
		int end = (Us == RED?6:12);

		for (int num = start; num < end; ++num) {
			Square s= pieces[num];
			if (s != SQ_NONE) {
                if (is_ok(s + Ver)) {
                    mL[idx++] = make_move2(s, s + Ver);
                }
                if (file_of(s) != Edge && is_ok(s + Hor)) {
                    mL[idx++] = make_move2(s, s + Hor);
                }
                if (file_of(s) != Edge && is_ok(s + Diag)) {
                    mL[idx++] = make_move2(s, s + Diag);
                }
			}
		}
        if (idx == 0) {
            mL[idx++] = -1;
        }

		return idx;
	}

    template <Color Us>
    int legal_actions(MoveList &mL) {
        int idx = 0;

        // Compute some compile time parameters relative to the red side
        constexpr Direction Ver = (Us == RED ? NORTH : SOUTH);
        constexpr Direction Hor = (Us == RED ? EAST : WEST);
        constexpr Direction Diag = (Us == RED ? NORTH_EAST : SOUTH_WEST);
        constexpr File Edge = (Us == RED ? FILE_F : FILE_A);

        for (Square s = SQ_A1; s < SQUARE_NB; ++s) {
            Piece pc = piece_on(s);
            if (color_of(pc) == Us) {
                if (is_ok(s + Ver)) {
                    mL[idx++] = make_move(s, s + Ver);
                }
                if (file_of(s) != Edge && is_ok(s + Hor)) {
                    mL[idx++] = make_move(s, s + Hor);
                }
                if (file_of(s) != Edge && is_ok(s + Diag)) {
                    mL[idx++] = make_move(s, s + Diag);
                }
            }
        }

        if (idx == 0) {
            mL[idx++] = MOVE_PASS;
        }

        return idx;
    }

    MoveType get_move_type(Move m) {
        Square to;
        Piece captured;

        Color op = ~sideToMove;
        to = to_sq(m);
        captured = piece_on(to);

        if (color_of(captured) == op) {
            return CAPTURE_OP;
        } else if (color_of(captured) == sideToMove) {
            return CAPTURE_SELF;
        } else {
            return NORMAL;
        }
    }

    template <Color Us>
    bool movable(Square s) const {
        Color op = ~Us;
        Piece pc = piece_on(s);
        if (pc == NO_PIECE || color_of(pc) == op) {
            return true;
        }
        // can only capture one's stone
        return false;
    }

    template <Color Us>
    bool goal(Square s) const {
        // Compute some compile time parameters relative to the red side
        constexpr Direction Ver = (Us == RED ? NORTH : SOUTH);
        constexpr Direction Hor = (Us == RED ? EAST : WEST);
        constexpr Direction Diag = (Us == RED ? NORTH_EAST : SOUTH_WEST);
        constexpr File Edge = (Us == RED ? FILE_F : FILE_A);

        // Return false if pc can move on square s
        if (is_ok(s + Ver) && movable<Us>(s + Ver)) {
            return false;
        }
        if (is_ok(s + Hor) && file_of(s) != Edge && movable<Us>(s + Hor)) {
            return false;
        }
        if (is_ok(s + Diag) && file_of(s) != Edge && movable<Us>(s + Diag)) {
            return false;
        }

        return true;
    }

    void update_status2() {
        if (num_pieces[RED] == 0) {
            status = Status::BlueWin;
            return;
        }
        if (num_pieces[BLUE] == 0) {
            status = Status::RedWin;
            return;
        }
        bool b_goal = true, r_goal = true;

		for (int i = 0; i < 6; i++) {
			if (pieces[i] != SQ_NONE) {
                if (!goal<RED>(pieces[i])) {
                    r_goal = false;
                }
			}
		}
		for (int i = 6; i < 12; i++) {
			if (pieces[i] != SQ_NONE) {
                if (!goal<BLUE>(pieces[i])) {
                    b_goal = false;
                }
			}
		}

        if (r_goal && !b_goal && num_pieces[RED] == PIECE_TYPE_NB) {
            status = Status::RedWin;
        } else if (b_goal && !r_goal && num_pieces[BLUE] == PIECE_TYPE_NB) {
            status = Status::BlueWin;
        } else if (r_goal && b_goal) {
            Piece top_left = piece_on(SQ_A1);
            Piece bottom_right = piece_on(SQ_F6);
            if (num_pieces[RED] > num_pieces[BLUE]) {
                status = Status::RedWin;
            } else if (num_pieces[BLUE] > num_pieces[RED]) {
                status = Status::BlueWin;
            } else if (type_of(top_left) > type_of(bottom_right)) {
                status = Status::BlueWin;
            } else if (type_of(top_left) < type_of(bottom_right)) {
                status = Status::RedWin;
            } else {
                status = Status::Draw;
            }
        }
    }

    void update_status() {
        if (num_pieces[RED] == 0) {
            status = Status::BlueWin;
            return;
        }
        if (num_pieces[BLUE] == 0) {
            status = Status::RedWin;
            return;
        }
        bool b_goal = true, r_goal = true;

        for (Square s = SQ_A1; s < SQUARE_NB; ++s) {
            Piece pc = piece_on(s);

            if (color_of(pc) == RED) {
                if (!goal<RED>(s)) {
                    r_goal = false;
                }
            } else if (color_of(pc) == BLUE) {
                if (!goal<BLUE>(s)) {
                    b_goal = false;
                }
            }
        }

        if (r_goal && !b_goal && num_pieces[RED] == PIECE_TYPE_NB) {
            status = Status::RedWin;
        } else if (b_goal && !r_goal && num_pieces[BLUE] == PIECE_TYPE_NB) {
            status = Status::BlueWin;
        } else if (r_goal && b_goal) {
            Piece top_left = piece_on(SQ_A1);
            Piece bottom_right = piece_on(SQ_F6);
            if (num_pieces[RED] > num_pieces[BLUE]) {
                status = Status::RedWin;
            } else if (num_pieces[BLUE] > num_pieces[RED]) {
                status = Status::BlueWin;
            } else if (type_of(top_left) > type_of(bottom_right)) {
                status = Status::BlueWin;
            } else if (type_of(top_left) < type_of(bottom_right)) {
                status = Status::RedWin;
            } else {
                status = Status::Draw;
            }
		}
	}
    bool is_terminal() const {
        if (status == Status::RedWin || status == Status::BlueWin || status == Status::Draw) {
            return true;
        }
        return false;
    }

    Color who_won() const {
        if (status == Status::RedWin) return RED;
        else if (status == Status::BlueWin) return BLUE;
        return COLOR_NONE;
    }

    Color side_to_move() const { return sideToMove; }

    int get_gameLength() const { return gameLength; }

    void set_hl(Move m) { hl_move = m; }

    int get_num_pieces(Color c) const { return num_pieces[c]; }

    std::string print_board() const {
        std::stringstream ss;
        ss << " a | b | c | d | e | f |\n";
        for (Rank r = RANK_1; r < RANK_NB; ++r) {
            for (File f = FILE_A; f < FILE_NB; ++f) {
                Piece pc = piece_on(make_square(f, r));
                if (color_of(pc) == RED) {
                    ss << "R" << int(type_of(pc)) << " |";
                } else if (color_of(pc) == BLUE) {
                    ss << "B" << int(type_of(pc)) << " |";
                } else {
                    ss << "   |";
                }
            }
            ss << "\n";
        }
        ss << "\n";

        return ss.str();
    }

  protected:
    friend std::ostream &operator<<(std::ostream &os, Board const &b);

    inline Piece piece_on(Square s) const {
        return board[s];
    }

    inline void move_piece(Piece pc, Square from, Square to) {
        board[from] = NO_PIECE;
        board[to] = pc;
    }

    inline void put_piece(Piece pc, Square to) {
        board[to] = pc;
    }

    inline void remove_piece(Piece pc, Square from) {
        board[from] = NO_PIECE;
    }

    Color sideToMove;
    Status status;
    int gameLength;
    Move hl_move; // highlighted move
    Piece board[SQUARE_NB];
	Square pieces[12];


    //Piece initial[SQUARE_NB];
    int num_pieces[COLOR_NB];
};

std::ostream &operator<<(std::ostream &os, Board const &b) {
    Square hl_from = from_sq(b.hl_move);
    Square hl_to = to_sq(b.hl_move);
    os << Ansi::BOLD << Ansi::FG_CYAN << "Turn " << b.gameLength + 1 << Ansi::CLEAR << "\n\n";

    for (Rank r = RANK_1; r < RANK_NB; ++r) {
        for (File f = FILE_A; f < FILE_NB; ++f) {
            Square s = make_square(f, r);
            Piece pc = b.piece_on(s);
            if (is_move_ok(b.hl_move) && s == hl_from) {
                os << Ansi::BG_GREY << Ansi::BOLD << Icon[type_of(pc)] << Ansi::CLEAR;
            } else if (is_move_ok(b.hl_move) && s == hl_to) {
                os << Ansi::BG_GREEN << Ansi::FG_YELLOW << "✘ " << Ansi::CLEAR;
            } else {
                os << pc;
            }
        }
        os << "\n";
    }
    os << "\n";
    os << Ansi::FG_RED << "R pieces: " << b.num_pieces[RED] << "\n";
    os << Ansi::FG_BLUE << "B pieces: " << b.num_pieces[BLUE] << Ansi::CLEAR << "\n";
    return os;
}

#endif // #ifndef EINSTEIN_HPP
