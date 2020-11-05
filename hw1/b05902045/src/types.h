// This file defines the type that used in other files
#include <queue>
#include <unordered_map>

typedef unsigned long long LL;
typedef enum Item : char {
    WALL = '#',
    PLAYER = '@',
    BALL = 'O',
    BOX = '$',
    BALL_BOX = '*',
    FLOOR = '-',
    NOITEM = 'x',
} Item;

typedef enum Direction : int {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
} Dir;

typedef struct BOARD {
    LL box = 0; // the i'th bit represent whether a box on the i'th place of the board or not 
    LL ball = 0; 
    unsigned short player = 0; // 8 bit in short  0~3: x coordinate 4~7: y coordinate 

    BOARD(char Board[16][16], int n, int m) { // init the bit map version of board 
        int pos = 0;  
        ball = 0;
        box = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                //pos = i * m + j;
                switch (Board[i][j]){
                    case PLAYER:
                        player |= i;   
                        player |= j<<4;   
                        break;
                    case BALL:
                        ball |= 1LL<<pos;
                        break;
                    case BOX:
                        box |= 1LL<<pos; 
                        break;
                    case BALL_BOX:
                        ball |= 1LL<<pos;
                        box |= 1LL<<pos; 
                        break;
                    default:
                        break;
                }   
                pos++;
            }   
        }   
    }

    BOARD(){

    }

    BOARD(const BOARD &b){
        box = b.box;
        ball = b.ball;
        player = b.player;
    }
    
    ~BOARD(){
    }
    
    bool operator==(const BOARD& b) const{
        return (box == b.box && ball == b.ball && player == b.player);
    }

    void showBoard() {
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < M; j++){
                int pos = i * M + j;     
                if((ball>>pos) & 1 && (box>>pos) & 1) {
                    printf("*");
                } else if((ball>>pos) & 1) {
                    printf("0");
                } else if((box>>pos) & 1) {
                    printf("$");
                } else{
                    printf("-");
                }
            }
            printf("\n");
        }
    }
} BOARD;
namespace std {
    template <>
    struct hash<BOARD> {
        std::size_t operator()(const BOARD& b) const{
            using std::size_t;
            using std::hash;

            return ((hash<unsigned long long>()(b.ball))
                    ^ (hash<unsigned long long>()(b.box)>>1)
                    ^ (hash<short>()(b.player)<<1));
        }
    };
}
//typedef __int128 BOARD;
typedef struct STATE{ // this is maintain in the queue 
    int step = 0;
    int penalty = 0;
    int heuristic = 0;
    char ans[100]; // if the ans is < 100 this should be set more carefully in the future
    int backward;
    bool ballRes = false;
    bool boxRes = false;
    int dir = 0;
    BOARD board; 

    STATE(){

    }

    STATE(BOARD b){
        board = b;
        dir = 0;
    }

    STATE(const STATE &state){
        step = state.step;
        penalty = state.penalty;
        heuristic = state.heuristic;
        ballRes = state.ballRes;
        boxRes = state.boxRes;
        dir = 0;
        for(int i = 0; i < state.step; i++) {
            ans[i] = state.ans[i];
        }
        board = state.board;
    }
    STATE(char Board[16][16], int n, int m) { // init the bit map version of board 
        board = BOARD(Board, n, m); 
        dir = 0;
        // 0~3 x cor 4~7 y cor 8~59 ball 60~111 box
        /*
        int pos = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                switch (Board[i][j]){
                    case BALL:
                        board |= 1<<(pos+8);
                    case BOX:
                        board |= 1<<(pos+60); 
                    case PLAYER:
                        board |= i;   
                        board |= j<<4;   
                }   
                pos++;
            }   
        }   
        */
    }
    bool operator<(const STATE& r) const{ // because dequeue
        return (penalty+heuristic) > (r.penalty+r.heuristic);
    }
} STATE;

int countBoxNum(char Board[16][16], int nn, int mm) {
    int count = 0;
    for(int i = 0; i < nn; i++) {
        for( int j = 0; j < mm; j++) {
            if(Board[i][j] == BOX || Board[i][j] == BALL_BOX){
                count++; 
            }
        }
    }
    return count;
}

typedef struct GAMESTATE{ // this is for the whole search 
    char boardState[16][16]; // n, m < 15
    STATE state;
    int n, m;
    int boxNum = 0;
    //int box_num = 0;
    GAMESTATE(char Board[16][16], int nn, int mm){
        state = STATE(Board, nn, mm);
        n = nn;
        m = mm;
        boxNum = countBoxNum(Board, nn, mm);
    }


} GAMESTATE;
//typedef std::queue<STATE> QUEUE;
typedef std::priority_queue<STATE> QUEUE;
typedef std::unordered_map<BOARD,bool> HASHMAP;


