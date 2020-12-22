#ifndef MCTS_H
#define MCTS_H
#include <vector>
#include "time.h"
#include "cstdlib"
#include "math.h"
#include "einstein.hpp"
# define MaxChild 18 // because 6 peices and 3 of each
# define MaxNode 55000 // maximum nodes in a tree

class Node {
public:
    Move ply; // the ply from parent to here 
    int p_id; // parent id 
    int c_id[MaxChild]; // child id
    int depth; // depth
    int Nchild; // number of children
    int Ntotal; // total number of simulation
    double CsqrtlogNtotal; 
    double CsqrtNtotal; 
    //int wins; // number of winning
    int scores; // sum1 sum of scores
    int sum2; // sum2 sum of each square
    double average; // win rate
    double variance; // variance of score
    //char init[2][7]; // to init the board
    void update(int, int, int);
    void addChild();
    bool is_terminal();
    Node* next_step();
    Node();
    Node(const Node &n) {
        p_id = n.p_id;
        ply = n.ply;
        Nchild = n.Nchild;
        depth = n.depth;
        Ntotal = 0;
        CsqrtlogNtotal = 0;
        scores = 0;
        sum2 = 0;
        average = 0;
    }

};

struct MCTS {
private:
public:
    std::vector<Node> tree;
    int nodeId;
    int Select(int, std::vector<Move>&);
    void init();
    void Expand(int, std::vector<Move>, Board);
    void Simulate(Node*, int, Board); // DFS random simulate the node with budget times 
    void BackPropagate(Node*, int, int, int);
    double UCB(int);
    int chooseBest(int );
    int findRoot(int, Move);

};
#endif
