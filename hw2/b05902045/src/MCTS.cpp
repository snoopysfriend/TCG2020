#include "MCTS.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include "einstein.hpp"
//#include "types.hpp"
//#include "einstein.hpp"

Color myside;
std::fstream flog;
int prev = 0;

inline void flip_bit(bool& x) { x= !x; }
Node::Node() {
    depth = 0;
    Ntotal = 0;
    CsqrtlogNtotal = 0;
    scores = 0;
    sum2 = 0;
    average = 0;
    variance = 0;
    Nchild = 0;
}

void Node::update(int deltaS, int deltaS2, int deltaN) {
    Ntotal += deltaN; 
    CsqrtlogNtotal = 1.0 * sqrt(log(double(Ntotal)));
    scores += deltaS;
    sum2 += deltaS2;
    average = (double)scores / (double)Ntotal;
    variance = ((double)sum2 - 2 * average * scores)/ Ntotal; 
}

bool Node::is_terminal() {
    return false;
}


void MCTS::Simulate(Node* node, int deltaN, Board b) {
    int deltaS = 0;
    Board tmpB;
    for (int i = 0; i < deltaN; i++) {
        MoveList mL;
        int mL_size;
        Move move;
        tmpB = b;
        while (!tmpB.is_terminal()) {
            if (tmpB.side_to_move() == RED) {
                mL_size = tmpB.legal_actions<RED>(mL);
            } else {
                mL_size = tmpB.legal_actions<BLUE>(mL);
            }
            move  = mL[rand() % mL_size];
            tmpB.do_move(move);
            tmpB.update_status();
        }
        if ((tmpB.who_won() == myside)) {
            deltaS++;
        }
    }
    int deltaS2 = 0; // TODO 
    BackPropagate(node, deltaS, deltaS2, deltaN);
}


void MCTS::init() {
    Node root;
    tree.reserve(500000);
    //tree.push_back(root);
    tree[0].p_id = 0; // the roots parent is itself
    tree[0].depth = 0;
    tree[0].Nchild = 0;
    nodeId = 1;
}

double MCTS::UCB(int id) {
    return ((tree[id].depth & 1)? (tree[id].average):(1.0-tree[id].average)) + (tree[tree[id].p_id].CsqrtlogNtotal / tree[id].CsqrtlogNtotal); 
}

int MCTS::Select(int root, std::vector<Move>& PV) {
    int ptr = root; // for the root
    while (tree[ptr].Nchild > 0) {
        double temp = 0; 
        double maxV = UCB(tree[ptr].c_id[0]);
        int maxc = 0;
        for (int i = 1; i < tree[ptr].Nchild; i++) {
            temp = UCB(tree[ptr].c_id[i]);
            if (maxV < temp) {
                maxV = temp;
                maxc = i;
            }
        }
        ptr = tree[ptr].c_id[maxc];
        PV.push_back(tree[ptr].ply);
    }
    return ptr;
}

void MCTS::Expand(int id, std::vector<Move> PV, Board b) {
    for (auto move: PV) {
        b.do_move(move);
        b.update_status();
    }
    if (!b.is_terminal()) {
        MoveList mL;
        int mL_size;
        if (b.side_to_move() == RED) {
            mL_size = b.legal_actions<RED>(mL);
        } else {
            mL_size = b.legal_actions<BLUE>(mL);
        }
        tree[id].Nchild = mL_size;
        for (int i = 0; i < mL_size; i++) {
            tree[id].c_id[i] = nodeId;
            tree[nodeId].p_id = id;
            tree[nodeId].Nchild = 0;
            tree[nodeId].depth = tree[id].depth+1;
            tree[nodeId].ply = mL[i];
            Simulate(&tree[nodeId], 15, b);
            nodeId++;
        }
    }
}

void MCTS::BackPropagate(Node* node, int deltaS, int deltaS2, int deltaN) {
    int cur_id = -1;
    while (node->p_id != cur_id) {
        node->update(deltaS, deltaS2, deltaN);
        cur_id = node->p_id;
        node = &tree[cur_id];
    }
    node = &tree[cur_id];
    node->update(deltaS, deltaS2, deltaN);
}
int MCTS::chooseBest(int root) {
    double maxV = tree[tree[root].c_id[0]].average;
    int maxi = tree[root].c_id[0];
    for (int i = 1; i < tree[root].Nchild; i++) {
        if (maxV < tree[tree[root].c_id[i]].average) {
            maxV = tree[tree[root].c_id[i]].average;
            maxi = tree[root].c_id[i];
        }
    }
    return maxi;
}

int MCTS::findRoot(int root, Move m) {
    for (int i = 0; i < tree[root].Nchild; i++) {
        if ( tree[tree[root].c_id[i]].ply == m) {
            return tree[root].c_id[i];
        }
    }
    return 0;
}

void logger(std::string logfile) {
    flog.open(logfile, std::fstream::out);
    if (!flog.is_open()) {
        std::runtime_error("error opeing file\n");
    }
}

int main() {
    logger(".log.mcts3");

    Board b;
    Move move;
    bool myTurn;
    char start, num, dir;
    std::string move_str;
    char init[2][PIECE_TYPE_NB + 1];
    do {
        for (int i = 0; i < COLOR_NB; i++) {
            for (int j = 0; j < PIECE_TYPE_NB; j++) {
                init[i][j] = getchar();
            }
        }

        init[0][PIECE_TYPE_NB] = init[1][PIECE_TYPE_NB] = '\0';

        start = getchar();
        flog << init[0]<< " " << init[1] << std::endl; 
        flog << start << std::endl;
        srand(0);
        b.init(init[0], init[1]);
        MCTS tree;
        tree.init();
        int root = 0;
        for (myTurn = (start == 'f'); !b.is_terminal(); flip_bit(myTurn)) {
            if (myTurn) { // do the move                    
                myside = b.side_to_move();
                flog << myside << std::endl;
                int N = 1000;
                for (int i = 0; i < N; i++) {
                    std::vector<Move> PV;
                    int node = tree.Select(root, PV); // choosing the PV
                    tree.Expand(node, PV, b);
                }
                flog << "node num " << tree.nodeId << std::endl;
                int node = tree.chooseBest(root);
                root = node;
                move = tree.tree[node].ply;
                std::string move_str = b.move_to_str(move);
                flog << move_str << std::endl; 
                std::cout << move_str << std::flush; 
                b.do_move(move);
                b.update_status();
                flog << b;
            } else { // receive the opponents move
                num = getchar();
                dir = getchar();
                flog << myTurn << num << dir << std::endl;
                Move m = b.str_to_move(num, dir);
                b.do_move(num, dir);
                b.update_status();
                root = tree.findRoot(root, m);
                flog <<"root" << root << std::endl;
                tree.tree[root].p_id = root;
                tree.tree[root].depth = 0;
                // TODO need to accelerate the simulation?
                flog << b;
            }
        }
        flog<< "winner" << b.who_won() << std::endl;

    } while( getchar() == 'y');

    return 0;
}
