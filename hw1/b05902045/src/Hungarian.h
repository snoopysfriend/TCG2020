/// This file is for the Hungarian Algorithm
//
//
#define size 15

inline int Min(int a1, int a2) {
    return a1<a2?a1:a2;
}
struct HungarianAlgorithm{
    int costMatrix[size][size];
    int dim;
    int labelByWorker[size];
    int labelByJob[size]; // how much cost a job
    int matchJobByWorker[size];
    int matchWorkerByJob[size];
    bool commitWorkers[size];
    int parentWorkerByCommitJob[size];
    
    int minSlackWorkerByJob[size];
    int minSlackValueByJob[size];
    int result[size];
        // minslack[v] = val u1, val is the minium slack lu[u]+lv[v]-w[u][v] u1 is the vertex

    HungarianAlgorithm(int dimSize) {
        dim = dimSize;
        for (int i = 0; i < dim; i++){
            matchWorkerByJob[i] = -1;
            matchJobByWorker[i] = -1;
            parentWorkerByCommitJob[i] = -1;
        }
    }
    // compute an initial by assigning no solution for each job  
    void computeIinitialFeasibleSolution() {
        for (int i = 0; i < dim; i++) {
            labelByJob[i] = 100;
            labelByWorker[i] = 0;
        }
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if (costMatrix[i][j] < labelByJob[j]) {
                    labelByJob[j] = costMatrix[i][j];
                }
            }
        }
    }
    void updateLabeling(int slack) {
        for (int i = 0; i < dim; i++) {
            if (commitWorkers[i]) {
                labelByWorker[i] += slack;
            }
        }
        for (int i = 0; i < dim; i++) {
            if (parentWorkerByCommitJob[i] != -1) {
                labelByJob[i] -= slack;
            } else {
                minSlackValueByJob[i] -= slack;
            }
        }
    }
    void executePhase() {
        while (true) {
            int minSlackWorker = -1, minSlackJob = -1;
            int minSlackValue = 1000;
            for (int i = 0; i < dim; i++) {
                if (parentWorkerByCommitJob[i] == -1) { // no one commit ?
                    if(minSlackValueByJob[i] < minSlackValue) {
                        minSlackWorker = minSlackWorkerByJob[i]; 
                        minSlackValue = minSlackValueByJob[i];
                        minSlackJob = i;
                    }
                }
            }
            if (minSlackValue > 0) {
                updateLabeling(minSlackValue);
            }
            //printf("job %d\n", minSlackJob);
            parentWorkerByCommitJob[minSlackJob] = minSlackWorker;
            if (matchWorkerByJob[minSlackJob] == -1) {
                int committedJob = minSlackJob;
                int parentWorker = parentWorkerByCommitJob[committedJob];
                while (true) {
                    //printf("QQQ\n");
                    int temp = matchJobByWorker[parentWorker];
                    match(parentWorker, committedJob);
                    committedJob = temp;
                    if (committedJob == -1) {
                        break;
                    }
                    parentWorker = parentWorkerByCommitJob[committedJob];
                }

                return;
            } else {
                //printf("QQ\n");
                int worker = matchWorkerByJob[minSlackJob];
                //printf("%d\n", worker);
                commitWorkers[worker] = true;
                for (int j = 0; j < dim; j++) {
                    if (parentWorkerByCommitJob[j] == -1) {
                        int slack = costMatrix[worker][j]
                                - labelByWorker[worker] - labelByJob[j];
                        if (minSlackValueByJob[j] > slack) {
                            minSlackValueByJob[j] = slack;
                            minSlackWorkerByJob[j] = worker;
                        }
                    }
                }
                //printf("QQQQ\n");
            }
        }
    }

    void initialPhase(int index) {
        for(int i = 0; i < dim; i++) {
            commitWorkers[i] = false;
            parentWorkerByCommitJob[i] = -1;
        }
        commitWorkers[index] = true;   
        for (int i = 0; i < dim; i++) {
            minSlackValueByJob[i] = costMatrix[index][i] - labelByWorker[i] - labelByJob[i]; 
            minSlackWorkerByJob[i] = index;
        }
    }
    // return the minimum cost mathching of workers to jobs based on the cost matrix, matching value -1
    // means corresponding worker not assigned
    void execute() {
        reduce();
        computeIinitialFeasibleSolution();
        greedyMatch();
        int index = fetchUnmatchWorker();
        while (index < dim) {
            initialPhase(index);
            executePhase();
            index = fetchUnmatchWorker();
            //printf("%d\n", index);
        }
        for (int i = 0; i < dim; i++) {
            result[i] = matchJobByWorker[i];
        }
        return ;
    }
    // reduce the cost matrix by subtracting the smallest element of each row 
    // from all elements of the row as well as the smallest element from each 
    // column
    
    int fetchUnmatchWorker() {
        for (int i = 0; i < dim; i++) {
            if (matchJobByWorker[i] == -1) {
                return i;
            }
        }
        return dim;
    }
    void reduce() {
        for (int i = 0; i < dim; i++) {
            int minRow = 100;
            for (int j = 0; j < dim; j++) {
                minRow = Min(costMatrix[i][j], minRow);
            }
            for (int j = 0; j < dim; j++) {
                costMatrix[i][j] -= minRow;
            }
        }
        for (int i = 0; i < dim; i++) {
            int minCol = 100;
            for (int j = 0; j < dim; j++) {
                minCol = Min(costMatrix[j][i], minCol);
            }
            for (int j = 0; j < dim; j++) {
                costMatrix[j][i] -= minCol;
            }
        }

    }
    // find the valid matching by greedily selecting among zero-cost match
    void greedyMatch() {
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if(matchJobByWorker[i] == -1 
                        && matchWorkerByJob[j] == -1
                        && costMatrix[i][j] - labelByWorker[i]-labelByJob[j] == 0){
                    match(i, j);
                }
            }
        }
    }
    void match(int i, int j) {
        matchJobByWorker[i] = j;
        matchWorkerByJob[j] = i;
    }
};



