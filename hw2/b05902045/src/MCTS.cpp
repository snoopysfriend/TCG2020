#include "MCTS.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include "einstein.hpp"
#include "io.cpp"

Color myside;
std::fstream flog;

int ppN = 0;
inline void flip_bit(bool& x) { x= !x; }
Node::Node() {
    depth = 0;
    Ntotal = 0;
    CsqrtlogNtotal = 0;
    CsqrtNtotal = 0;
    scores = 0;
    sum2 = 0;
    average = 0;
    variance = 0;
    Nchild = 0;
}

void Node::update(int deltaS, int deltaS2, int deltaN) {
    Ntotal += deltaN; 
    CsqrtlogNtotal = 1.0 * sqrt(log(double(Ntotal)));
    CsqrtNtotal = sqrt(double(Ntotal));
    scores += deltaS;
    sum2 += deltaS2;
    average = (double)scores / (double)Ntotal;
    //variance = ((double)sum2 - 2 * average * scores)/ Ntotal; 
}

void Node::update2(int deltaS, int deltaS2, int deltaN) {
    Ntotal += deltaN; 
    CsqrtlogNtotal = 1.0 * sqrt(log(double(Ntotal)));
    CsqrtNtotal = sqrt(double(Ntotal));
    scores += deltaS;
    sum2 += deltaS2;
    average = (double)scores / (double)Ntotal;
    variance = ((double)sum2 - average * (2* double(sum2)- average))/ Ntotal; 
}

bool Node::is_terminal() {
    return false;
}


void MCTS::Simulate(Node* node, int deltaN, Board b) {
    int deltaS = 0;
    Board tmpB;
	b.do_move(node->ply);
	b.update_status();
	MoveList mL;
    int deltaS2 = 0; 
    for (int i = 0; i < deltaN; i++) {
        int mL_size;
        Move move;
        tmpB = b;
		tmpB.init2();
        while (!tmpB.is_terminal()) {
            if (tmpB.side_to_move() == RED) {
                mL_size = tmpB.legal_actions<RED>(mL);
            } else {
                mL_size = tmpB.legal_actions<BLUE>(mL);
            }
            move  = mL[rand() % mL_size];
            tmpB.do_move2(move);
            tmpB.update_status2();
        }
        /*
        int delta = (myside == RED)?tmpB.get_num_pieces(RED)-tmpB.get_num_pieces(BLUE):tmpB.get_num_pieces(BLUE)-tmpB.get_num_pieces(RED);
        deltaS2 += delta*delta;
        deltaS += delta;*/
        if ((tmpB.who_won() == myside)) {
            deltaS++;
        }
    }
   
    BackPropagate(node, deltaS, deltaS2, deltaN);
}


void MCTS::init() {
    Node root;
    tree.reserve(2000000);
    tree[0].p_id = 0; // the roots parent is itself
    tree[0].depth = 0;
    tree[0].Nchild = 0;
    nodeId = 1;
}

double MCTS::UCB(int id) {
    return ((tree[id].depth & 1)? (tree[id].average):(1.0-tree[id].average)) + (tree[tree[id].p_id].CsqrtlogNtotal / tree[id].CsqrtNtotal); 
}

double MCTS::UCB2(int id, double Range, double Mins) {
    double SR = (tree[id].average - Mins) / Range;
    return ((tree[id].depth & 1)? (SR):(1.0-SR)) + (tree[tree[id].p_id].CsqrtlogNtotal / tree[id].CsqrtNtotal); 
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
/*
int MCTS::Select(int root, std::vector<Move>& PV) {
    int ptr = root; // for the root
    while (tree[ptr].Nchild > 0) {
        double minS, maxS; 
        minS = maxS = tree[tree[ptr].c_id[0]].average;
        for (int i = 1; i < tree[ptr].Nchild; i++) {
            if (tree[ptr].pp[i]) {
                double tmp = tree[tree[ptr].c_id[i]].average;
                maxS = maxS<tmp?tmp:maxS;
                minS = minS>tmp?tmp:minS;
                int tid = tree[ptr].c_id[i];
                if ((tree[tid].average + 2 * tree[tid].variance < tree[0].average - 2 * tree[0].variance)
                        && tree[tid].variance < 1.5) {
                    tree[ptr].pp[i] = false;
                    ppN++;
                }
            }
        }
        double range = maxS - minS;
        double temp = 0; 
        double maxV = UCB2(tree[ptr].c_id[0], range, minS);
        int maxc = 0;
        for (int i = 1; i < tree[ptr].Nchild; i++) {
            int tid = tree[ptr].c_id[i];
            if (tree[ptr].pp[i]) {
                temp = UCB2(tid, range, minS);
                if (maxV < temp) {
                    maxV = temp;
                    maxc = i;
                } 
            }
        }
        ptr = tree[ptr].c_id[maxc];
        PV.push_back(tree[ptr].ply);
    }
    return ptr;
}*/

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
			if (nodeId >= 2000000) {
				nodeId = 0;
			}
            tree[id].c_id[i] = nodeId;
            tree[id].pp[i] = true;
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
    logger(".log.mcts4");

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
        int count = 0;
        for (myTurn = (start == 'f'); !b.is_terminal(); flip_bit(myTurn)) {
            if (myTurn) { // do the move                    
                myside = b.side_to_move();
                flog << myside << std::endl;
                int N = 4000;
                for (int i = 0; i < N; i++) {
                    std::vector<Move> PV;
                    int node = tree.Select(root, PV); // choosing the PV
                    tree.Expand(node, PV, b);
                }
                //flog << "pp num" << ppN << std::endl;
                flog << "node num " << tree.nodeId << std::endl;
                int node = tree.chooseBest(root);
                root = node;
                tree.tree[root].p_id = root;
                move = tree.tree[node].ply;
                std::string move_str = b.move_to_str(move);
                flog << move_str << std::endl; 
                std::cout << move_str << std::flush; 
                b.do_move(move);
                b.update_status();
                flog << b;
                count = 1;
            } else { // receive the opponents move
                int flag = 1;
                while (flag) {
                    reselect(0, flag);
                    if (!flag) {
                        num = getchar();
                        dir = getchar();
                        flog << myTurn << num << dir << std::endl;
                        Move m = b.str_to_move(num, dir);
                        b.do_move(num, dir);
                        b.update_status();
                        root = tree.findRoot(root, m);
                        flog <<"root" << root << std::endl;
                        tree.tree[root].p_id = root;
                        flog << b;
                    } else if (count) {
                        std::vector<Move> PV;
                        int node = tree.Select(root, PV); // choosing the PV
                        tree.Expand(node, PV, b);
                    }
                }
            }
        }
        flog<< "winner" << b.who_won() << std::endl;

    } while( getchar() == 'y');

    return 0;
}
