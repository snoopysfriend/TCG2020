#include <random>

#include "hash.h"

#define table_size 28

Hash_Function::Hash_Function() {
    //srand(time(NULL));
    srand(0);
    this->hash = 0;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 15; j++) {
            this->board[i][j] = rand();
        }
    }
    this->color[0] = rand();
    this->color[1] = rand();
}

int Hash_Function::set_turn(bool turn) {
    return this->hash ^ this->color[turn];
}

void Hash_Function::resetBoard() {
    for (int i = 0; i < 32; i++) {
        this->hash ^= board[i][14]; 
        // 15 is the cover
    }
}

int Hash_Function::do_move(int from, int to, int piece) {
    this->hash ^= board[from][piece]; 
    this->hash ^= board[to][piece]; 
    this->hash ^= board[from][15]; // empty
    return this->hash;
}

int Hash_Function::rev_move(int from, int to, int piece) {
    this->hash ^= board[from][piece]; 
    this->hash ^= board[to][piece]; 
    this->hash ^= board[from][15]; // empty
    return this->hash;
}

int Hash_Function::flip(int place, int piece) {
    this->hash ^= board[place][14]; // cover
    this->hash ^= board[place][piece]; // cover
    return this->hash;
}

int Hash_Function::rev_flip(int place, int piece) {
    this->hash ^= board[place][piece]; // cover
    this->hash ^= board[place][14]; // cover
    return this->hash;
}

Hash_table_entry::Hash_table_entry() {

}

Hash_table_entry::Hash_table_entry(double value, int move, int depth) {
    this->best_value = value;
    this->move = move;
    this->depth = depth;
}

Hash_table::~Hash_table() {
    delete table;
}

bool Hash_table::insert(int hash_key, double value, int move, int depth) {
    int index = hash_key & this->mask; 
    if (table[index] != NULL) {
        return false;
    }
    table[index] = new Hash_table_entry(value, move, depth);
    return true;
}

Hash_table_entry* Hash_table::get_value(int hash_key) {
    int index = hash_key & this->mask; 
    return table[index];
}

Hash_table::Hash_table() {
    this->mask = 0;
    for (int i = 0; i < table_size; i++) {
        this->mask |= 1<<i;
    }
    this->size = 1<<table_size;
    this->table = new Hash_table_entry*[size];
    this->num = 0;
}





