#ifndef __HASH__
#define __HASH__
#include <random>

class Hash_Function {
public:
    Hash_Function();
    void resetBoard();
    int set_turn(bool turn);
    int do_move(int from, int to, int piece);
    int rev_move(int from, int to, int piece);
    int flip(int place, int piece);
    int rev_flip(int place, int piece);
private:
    int hash;
    int board[32][16]; // 32 place and 14 types of pieces
    int color[2];
};

class Hash_table_entry {
public:
    Hash_table_entry();
    Hash_table_entry(double value, int move, int depth);
    int depth; // the searching depth
    //int board[32]; // store the board status
    double best_value;  // store the best value of the move
    int move; // the best move
};

class Hash_table {
public:
    Hash_table();
    Hash_table_entry* get_value(int hash_key);
    bool insert(int hash_key, double value, int move, int depth);
    ~Hash_table();
private:
    Hash_table_entry** table; 
    int size;
    int mask;
    int num;
};

#endif 
