// This file deine the util functions

#include <cstdio>
#include "global.h"

int readFile() {
    if(scanf("%d %d", &N, &M) == EOF){
        return -1;
    }
    for(int i = 0; i < N; i++){
        scanf("%s", globalBoard[i]);
    }
    return 0;
}
