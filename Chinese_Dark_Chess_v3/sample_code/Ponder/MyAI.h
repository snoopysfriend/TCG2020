#ifndef MYAI_INCLUDED
#define MYAI_INCLUDED 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RED 0
#define BLACK 1
#define CHESS_COVER -1
#define CHESS_EMPTY -2
class MyAI  
{
enum { 
	EMPTY=0,
	RK, RG, RM, RR, RN, RC, RP,
	CLOSE,
	BK, BG, BM, BR, BN, BC, BP,
	NUM_KIND
};
public:
void flip(const int src,const int pce_type);
void move(const int src,const int dst);
void generateMove(char move[6]);
void MakeMove(const char move[6]);
public:
	MyAI(void);
	~MyAI(void);
	void initBoardState(int iPieceCount[], char iCurrentPosition[]);
	void color(int c);
	void turn(int t);
	void time(int ms);
	void Pirnf_Chessboard();
private:
	int Color;
	int Time;
	int Board[32];
	int CloseChess[14];
	int ConvertChessNo(int input);
	void Pirnf_Chess(int chess_no,char *Result);
	bool Referee(int* Board,int Startoint,int EndPoint,int color);
	int Expand(int* Board,int color,int *Result);

};

#endif

