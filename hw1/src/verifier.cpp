// by Yunghsien Chung (hiiragi4000)
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

// by elsa
#include "sokoboru.hpp"

#define MAX_STEPS 1000

// If the format of the input file (puzzle) is invalid, the program terminates.
void Input_verifier(FILE *fin){
#define INVALID_INPUT do{\
    puts("The input file is invalid.");\
    exit(1);\
}while(0)
    char s[MAX_LENGTH][MAX_WIDTH+2];
    int count = 0;
    while(fgets(s[0], sizeof(s[0]), fin)){
        int len = strlen(s[0]);
        if(len>5 || s[0][len-1]!='\n'){
            INVALID_INPUT;
        }
        s[0][--len] = 0;
        char *sp = strchr(s[0], ' ');
        if(!sp || strchr(sp+1, ' ')) {
            INVALID_INPUT;
        }
        int n = 0;
        for(char *it=s[0]; it!=sp; ++it){
            if(!isdigit(*it) || (it==s[0]&&*it=='0')) {
                INVALID_INPUT;
            }
            n = 10*n+(*it)-48;
        }
        int m = 0;
        for(char *it=sp+1; *it; ++it){
            if(!isdigit(*it) || (it==sp+1&&*it=='0')) {
                INVALID_INPUT;
            }
            m = 10*m+(*it)-48;
        }
        if(n > MAX_LENGTH || m > MAX_WIDTH || n*m > AREA_MAX) {
            printf("%d > MAX_LENGTH or %d > MAX_WIDTH\n", n, m);
            INVALID_INPUT;
        }
        for(int i=0; i<=n-1; ++i){
            if(!fgets(s[i], sizeof(s[i]), fin)) INVALID_INPUT;
            if((int)strlen(s[i])!=m+1 || s[i][m]!='\n') {
                printf("Invalid height\n");
                INVALID_INPUT;
            }
            for(int j=0; j<=m-1; ++j){
                if(!strchr("#@O$*-", s[i][j])) {
                    printf("Invalid char %c\n", s[i][j]);
                    INVALID_INPUT;
                }
            }
        }
        int boxcnt=0, ballcnt=0, pcnt=0;
        for(int i=0; i<=n-1; ++i) for(int j=0; j<=m-1; ++j){
            boxcnt += (s[i][j] == '$');
            ballcnt += (s[i][j] == 'O');
            pcnt += (s[i][j] == '@');
        }
        if(!boxcnt || boxcnt!=ballcnt || pcnt!=1) {
            printf("%d, %d\n", boxcnt, ballcnt);
            INVALID_INPUT;
        }
    }
}

// If the format of the output file (solution) is invalid, the program terminates.
void Output_verifier(FILE *fin){
#define INVALID_OUTPUT do{\
    puts("The output file is invalid.");\
    exit(1);\
}while(0)
    char line1[20];
    for(int stage=1; fgets(line1, 20, fin); ++stage){
        int len = strlen(line1);
        if(len<2 || len>9 || line1[len-1]!='\n') INVALID_OUTPUT;
        line1[--len] = 0;
        int n = 0;
        for(int i=0; i<=len-1; ++i){
            if(!isdigit(line1[i]) || (len>1&&i==0&&line1[i]=='0')) INVALID_OUTPUT;
            n = 10*n+line1[i]-48;
        }
        char buf[MAX_STEPS];
        if(!fgets(buf, MAX_STEPS, fin)) INVALID_OUTPUT;

        if(buf[strlen(buf)-1]!='\n') INVALID_OUTPUT;

        for(int i=0; i<(strlen(buf) - 1); ++i){
            if(!strchr("^v<>", buf[i])) INVALID_OUTPUT;
        }
    }
}

int main(int argc, char **argv){
    char *input=0, *output=0;
#define WRONG_ARG do{\
    puts("Usage: verifier [-i infile] [-o outfile]");\
    puts("-i infile: if specified, check if the file \'infile\' is a valid input");\
    puts("-o outfile: if specified, check if the file \'outfile\' is a valid output");\
    puts("if both -i and -o are specified, also check if \'outfile\' solves the puzzle \'infile\'");\
    exit(1);\
}while(0)
    for(int i=1; i<argc; ++i){
        if(!strcmp(argv[i], "-i")){
            if(++i==argc || input) WRONG_ARG;
            input = argv[i];
        }else if(!strcmp(argv[i], "-o")){
            if(++i==argc || output) WRONG_ARG;
            output = argv[i];
        }else WRONG_ARG;
    }
    if(!input && !output) WRONG_ARG;
    FILE *fpi=0, *fpo=0;
    if(input){
        fpi = fopen(input, "r");
        if(!fpi){
            printf("Error in opening the file \'%s\'\n", input);
            return 1;
        }
        Input_verifier(fpi);
    }
    if(output){
        fpo = fopen(output, "r");
        if(!fpo){
            printf("Error in opening the file \'%s\'\n", output);
            return 1;
        }
        Output_verifier(fpo);
    }
    std::ifstream infile;
    infile.open(input);
    if(input && output){
        rewind(fpi);
        rewind(fpo);
        Board b;

        for(int stage=1; b.init_board(infile); ++stage){
            printf("Stage #%d: ", stage);
            int n;
            if(fscanf(fpo, "%d ", &n)!=1 || n==0){
                puts("Wrong Answer");
                continue;
            }
            char sol[MAX_STEPS];
            if(!fgets(sol, MAX_STEPS, fpo)){
                // never happens
                assert(0);
            }
            bool ac = true;
            for(int i=0; i<(strlen(sol)-1); ++i){
                Direction dir = char_to_dir(sol[i]);
                if ( !b.do_move(dir) ) {
                	ac = false; break;
                }
            }
            if ( !b.is_terminal() ) {
            	ac = false;
            }
            if ( b.get_penalty() != n) {
                ac = false;
            }

            if(ac){
                printf("Accepted: %d\n", n);
            }else puts("Wrong Answer");
        }
        char tail[2];
        if(fgets(tail, 2, fpo)){
            puts("Output Limit Exceeded");
        }
    }
    if(fpi) fclose(fpi);
    if(fpo) fclose(fpo);
    return 0;
}
