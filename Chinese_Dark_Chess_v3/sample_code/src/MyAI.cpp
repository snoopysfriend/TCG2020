#include "MyAI.h"
#include "float.h"
#include "board.h"
#include <assert.h>

#define DEPTH_LIMIT 3

// TODO the eat color would change
// TODO change the format of the board to as same as the protocal
// TODO set the turn in the board
MyAI::MyAI(void) {}

MyAI::~MyAI(void) {}

bool MyAI::protocol_version(const char *data[], char *response) {
  strcpy(response, "1.0.0");
  return 0;
}

bool MyAI::name(const char *data[], char *response) {
  strcpy(response, "MyAI");
  return 0;
}

bool MyAI::version(const char *data[], char *response) {
  strcpy(response, "1.0.0");
  return 0;
}

bool MyAI::known_command(const char *data[], char *response) {
  for (int i = 0; i < COMMAND_NUM; i++) {
    if (!strcmp(data[0], commands_name[i])) {
      strcpy(response, "true");
      return 0;
    }
  }
  strcpy(response, "false");
  return 0;
}

bool MyAI::list_commands(const char *data[], char *response) {
  for (int i = 0; i < COMMAND_NUM; i++) {
    strcat(response, commands_name[i]);
    if (i < COMMAND_NUM - 1) {
      strcat(response, "\n");
    }
  }
  return 0;
}

bool MyAI::quit(const char *data[], char *response) {
  fprintf(stderr, "Bye\n");
  return 0;
}

bool MyAI::boardsize(const char *data[], char *response) {
  fprintf(stderr, "BoardSize: %s x %s\n", data[0], data[1]);
  return 0;
}

bool MyAI::reset_board(const char *data[], char *response) {
  this->Red_Time = -1;   // unknown
  this->Black_Time = -1; // unknown
  //this->initBoardState();
  return 0;
}

bool MyAI::num_repetition(const char *data[], char *response) { return 0; }

bool MyAI::num_moves_to_draw(const char *data[], char *response) { return 0; }


bool MyAI::flip(const char *data[], char *response) {
  char move[6];
  sprintf(move, "%s(%s)", data[0], data[1]);
  int src = ('8'-move[1])*4+(move[0]-'a');
  if (move[2] == '(') {
      printf("# call flip(): flip(%d,%d) = %d\n", src, src, GetFin(move[3]));
      int p = ConvertChessNo(GetFin(move[3]));
      bool color = p/7;
      int piece = p%7;
      int Move = src * 256 + src;
      printf("%d\n", p);
      gameBoard.MakeMove(color, piece, Move);
      Pirnf_Chessboard();
  }
  return 0;
}

bool MyAI::move(const char* data[], char* response) {
    char move[6];
    sprintf(move, "%s-%s", data[0], data[1]);
    int src = ('8'-move[1])*4+(move[0]-'a');
    int dst = ('8'-move[4])*4+(move[3]-'a');
    int Move = src * 256 + dst;
    gameBoard.MakeMove(0, 0, Move);
    Pirnf_Chessboard();
    return 0;
}
bool MyAI::genmove(const char *data[], char *response) {
  // set color
  if (!strcmp(data[0], "red")) {
    this->Color = RED;
    this->turn = BLACK;
  } else if (!strcmp(data[0], "black")) {
    this->Color = BLACK;
    this->turn = RED;
  } else {
    this->Color = 2;
  }
  // genmove
  char move[6];
  this->generateMove(move);
  sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
  return 0;
}

bool MyAI::game_over(const char *data[], char *response) {
  fprintf(stderr, "Game Result: %s\n", data[0]);
  return 0;
}

bool MyAI::ready(const char *data[], char *response) { return 0; }

bool MyAI::time_settings(const char *data[], char *response) { return 0; }

bool MyAI::time_left(const char *data[], char *response) {
  if (!strcmp(data[0], "red")) {
    sscanf(data[1], "%d", &(this->Red_Time));
  } else {
    sscanf(data[1], "%d", &(this->Black_Time));
  }
  fprintf(stderr, "Time Left(%s): %s\n", data[0], data[1]);
  return 0;
}

bool MyAI::showboard(const char *data[], char *response) {
  Pirnf_Chessboard();
  return 0;
}

// *********************** AI FUNCTION *********************** //

int MyAI::GetFin(char c) {
  static const char skind[] = {'-', 'K', 'G', 'M', 'R', 'N', 'C', 'P',
                               'X', 'k', 'g', 'm', 'r', 'n', 'c', 'p'};
  for (int f = 0; f < 16; f++)
    if (c == skind[f])
      return f;
  return -1;
}

int MyAI::ConvertChessNo(int input) {
  switch (input) {
  case 0:
    return CHESS_EMPTY;
    break;
  case 8:
    return CHESS_COVER;
    break;
  case 1:
    return 6;
    break;
  case 2:
    return 5;
    break;
  case 3:
    return 4;
    break;
  case 4:
    return 3;
    break;
  case 5:
    return 2;
    break;
  case 6:
    return 1;
    break;
  case 7:
    return 0;
    break;
  case 9:
    return 13;
    break;
  case 10:
    return 12;
    break;
  case 11:
    return 11;
    break;
  case 12:
    return 10;
    break;
  case 13:
    return 9;
    break;
  case 14:
    return 8;
    break;
  case 15:
    return 7;
    break;
  }
  return -1;
}


void MyAI::Init() {
    gameBoard.init_board();
    assert(gameBoard.num_piece(true) >= 0 );
    assert(gameBoard.num_piece(false) >= 0 );
}

void MyAI::generateMove(char move[6]) {
  /* generateMove Call by reference: change src,dst */
  this->node = 0;
  int startPoint = 0;
  int EndPoint = 0;

  Pirnf_Chessboard();
  int best_move = 0;
  double alpha = -DBL_MAX;
  double beta = DBL_MAX;
  int Result[2048];
  int flip = gameBoard.Flip(Result);
  //int count = gameBoard.Expand(this->Color, Result);
  printf("flip %d \n", flip);
  double t = Nega_Scout(&best_move, this->Color, 0, DEPTH_LIMIT, alpha, beta);
    
  startPoint = best_move / 256;
  EndPoint = best_move % 256;
  printf("%d\n", startPoint);
  int row = (startPoint/4);
  int col = (startPoint%4);
  int row2 = (EndPoint/4);
  int col2 = (EndPoint%4);
  //printf("flip %d %d\n", flip, count);
  sprintf(move, "%c%c-%c%c", 'a' + row, '1' + (7 -col ),
          'a' + row2, '1' + (7 - col2));

  char chess_Start[4] = "";
  char chess_End[4] = "";
  Pirnf_Chess(gameBoard.getpiece(startPoint), chess_Start);
  Pirnf_Chess(gameBoard.getpiece(EndPoint), chess_End);
  printf("My result: \n--------------------------\n");
  printf("Nega Scout: %lf (node: %d)\n", t, this->node);
  printf("(%d) -> (%d)\n", startPoint, EndPoint);
  printf("<%s> -> <%s>\n", chess_Start, chess_End);
  printf("move:%s\n", move);
  printf("--------------------------\n");
  this->Pirnf_Chessboard();
}

/*
 *
void MyAI::MakeMove(int *board, int *red_chess_num, int *black_chess_num,
                    int *cover_chess, const char move[6]) {
  int src, dst;
  src = ('8' - move[1]) * 4 + (move[0] - 'a');
  if (move[2] == '(') {
    printf("# call flip(): flip(%d,%d) = %d\n", src, src, GetFin(move[3]));
    board[src] = ConvertChessNo(GetFin(move[3]));
    cover_chess[ConvertChessNo(GetFin(move[3]))]--;
    Pirnf_Chessboard();
  } else {
    dst = ('8' - move[4]) * 4 + (move[3] - 'a');
    printf("# call move(): move : %d-%d \n", src, dst);
    if (board[dst] != CHESS_EMPTY) {
      if (board[dst] / 7 == 0) {
        (*red_chess_num)--;
      } else {
        (*black_chess_num)--;
      }
    }
    board[dst] = board[src];
    board[src] = CHESS_EMPTY;
    Pirnf_Chessboard();
  }
}*/

// Display chess board
/*
void MyAI::Pirnf_Chessboard()
{
        char Mes[1024]="";

}*/

// Display chess board
void MyAI::Pirnf_Chessboard() {
  char Mes[1024] = "";
  char temp[1024];
  char myColor[10] = "";
  if (Color == -99)
    strcpy(myColor, "Unknown");
  else if (this->Color == RED)
    strcpy(myColor, "Red");
  else
    strcpy(myColor, "Black");
  sprintf(temp, "------------%s-------------\n", myColor);
  strcat(Mes, temp);
  strcat(Mes, "<8> ");
  for (int i = 0; i < 32; i++) {
    if (i != 0 && i % 4 == 0) {
      sprintf(temp, "\n<%d> ", 8 - (i / 4));
      strcat(Mes, temp);
    }
    char chess_name[4] = "";
    //Pirnf_Chess(this->Board[i], chess_name);
    //printf("%d\n", gameBoard.getpiece(i));
    if (this->gameBoard.is_empty(i)) {
        Pirnf_Chess(-2, chess_name);
    } else if (this->gameBoard.board[i].piece == 7) {
        Pirnf_Chess(-1, chess_name);
        //printf("good\n");
    } else {
        Pirnf_Chess(this->gameBoard.getpiece(i), chess_name);
    }
    sprintf(temp, "%5s", chess_name);
    strcat(Mes, temp);
  }
  strcat(Mes, "\n\n     ");
  for (int i = 0; i < 4; i++) {
    sprintf(temp, " <%c> ", 'a' + i);
    strcat(Mes, temp);
  }
  strcat(Mes, "\n\n");
  printf("%s", Mes);
}

// Print chess
void MyAI::Pirnf_Chess(int chess_no, char *Result) {
  // XX -> Empty
  if (chess_no == CHESS_EMPTY) {
    strcat(Result, " - ");
    return;
  }
  // OO -> DarkChess
  else if (chess_no == CHESS_COVER) {
    strcat(Result, " X ");
    return;
  }
  /*if(Color == RED )
                  strcat(Result, "R");
  else
                  strcat(Result, "B");*/
  switch (chess_no) {
  case 0:
    strcat(Result, " P ");
    break;
  case 1:
    strcat(Result, " C ");
    break;
  case 2:
    strcat(Result, " N ");
    break;
  case 3:
    strcat(Result, " R ");
    break;
  case 4:
    strcat(Result, " M ");
    break;
  case 5:
    strcat(Result, " G ");
    break;
  case 6:
    strcat(Result, " K ");
    break;
  case 7:
    strcat(Result, " p ");
    break;
  case 8:
    strcat(Result, " c ");
    break;
  case 9:
    strcat(Result, " n ");
    break;
  case 10:
    strcat(Result, " r ");
    break;
  case 11:
    strcat(Result, " m ");
    break;
  case 12:
    strcat(Result, " g ");
    break;
  case 13:
    strcat(Result, " k ");
    break;
  }
}

inline double Min(double m1, double m2) {
    return m1<m2?m1:m2;
}

inline double Max(double m1, double m2) {
    return m1>m2?m1:m2;
}

double MyAI::Nega_Scout(int* move, const int color, const int depth, const int remain_depth, double Alpha, double Beta) {
    //assert(gameBoard.num_piece(true) >= 0 );
    //assert(gameBoard.num_piece(false) >= 0 );
    Hash_table_entry* before;
    before = table.get_value(gameBoard.get_hashValue());
    if (before != NULL ) {
        if (before->depth >= remain_depth) {
            return before->best_value;    
        } else {
            // TODO use this for the 
        }
    }
    if (remain_depth == 0) {
        this->node++;
        return gameBoard.Evaluate(color) * (2*((depth&1)^1)-1);
    } /*else if (gameBoard.num_piece(true) == 0 || gameBoard.num_piece(false) == 0) {
        this->node++;
        return gameBoard.Evaluate(color) * (2*((depth&1)^1)-1);
    }*/

    int Moves[2048], Chess[2048];
    int flip_count = 0;
    int remain_count = 0;
    int remain_total = 0;
    int move_count = 0;
    // gen move
    move_count = gameBoard.Expand(color, Moves);
    //move_count = 0;
    flip_count = gameBoard.Flip(Moves + move_count);
    // TODO move this to the board
    for (int i = 0; i < 14; i++) {
        if (gameBoard.cover_chess[i] > 0) {
            Chess[remain_count++] = i;
            remain_total += gameBoard.cover_chess[i];
        }
    }
    int new_move;
    if ((move_count + flip_count) == 0) {
        this->node++;
        return gameBoard.Evaluate(color) * (2*((depth&1)^1)-1);
    } else {
        double m = -DBL_MAX;
        double n = Beta;
        //Pieces tmp[32];
        for (int i = 0; i < move_count; i++) {
            int removed = gameBoard.move(Moves[i]);
            double t = -Nega_Scout(&new_move, !color, depth+1, remain_depth-1, -n, -Alpha);
            if (t > m) { // failed high
                if (n == Beta || remain_depth < 3 || t >= Beta) { // to shallow not good for scout
                    m = t;
                } else {
                    m = -Nega_Scout(&new_move, !color, depth+1, remain_depth-1, -Beta, -t);
                    // re-search
                }
            }

            // TODO check the alpha beta
            gameBoard.reverse_move(color, removed, Moves[i]);
            if (m >= Beta) { // cut-off
                printf("cut off\n");
                return m;
            } 
            if (m > Alpha) {
                *move = Moves[i]; // better node
                Alpha = m;
            }
            n = Alpha + 1;
            // set up null window search
        }
        // chance node search 
        for (int i = move_count; i < (move_count + flip_count); i++) {
            double A = remain_total * (Alpha - DBL_MAX) + DBL_MAX;
            double B = remain_total * (Beta + DBL_MAX) - DBL_MAX;
            double m0 = -DBL_MAX; // current lower bound
            double M0 = DBL_MAX; // current upper bound
            double total = 0; // current sum of expected value
            for (int k = 0; k < remain_count; k++) {
                //Pirnf_Chessboard();
                gameBoard.flip(Chess[k], Moves[i]);
                double t = -Nega_Scout(&new_move, !color, depth+1, remain_depth-1, -B, -A);
                //printf("do move %d %d\n", Moves[i], depth);
                gameBoard.reverse_flip(Chess[k], Moves[i]);
                int chess_num = gameBoard.cover_chess[Chess[k]];
                m0 += ((t + DBL_MAX) * chess_num)/ remain_total; 
                M0 += ((t - DBL_MAX) * chess_num)/ remain_total; 
                if (t > B) {
                    return m0;
                } else if (t < A) {
                    return M0;
                }
                total += chess_num * t;
                A += DBL_MAX - t;
                B += (-DBL_MAX - t);
            }
            double E_score = (total / remain_total); // the expected value
            if (E_score > m) {
                m = E_score;
                *move = Moves[i];
            }
        }
        table.insert(gameBoard.get_hashValue(), m, *move, remain_depth);
        return m;
    }
}
