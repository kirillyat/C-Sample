//
//  main.cpp
//
//  Created by Кирилл Яценко on 02.11.2019.
//  Copyright © 2019 Кирилл Яценко. All rights reserved.
//


#include <iostream>
#include <time.h>
#include <random>
#include <stdlib.h>
#include <fcntl.h>



using namespace std;


void printMatrix(int* MxN, int M, int N)
{
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << MxN[i*N + j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    
}


void readMatrix(int* MxN, int M, int N)
{
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cin >> MxN[i*N + j];
        }
    }
}


void initMatrix(int* MxN, int M, int N)
{
    for (int i = 0; i < M; i++) {
       // srand((unsigned int)time(NULL));
        int R = (int)rand();
        for (int j = 0; j < N; j++) {
            MxN[i*N + j] = R%2;
            R /= 2;
        }
    }
}
void copyMatrix(int* copy, int* original, int M, int N)
{
   for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            copy[i*N + j] = original[i*N + j];
        }
    }
}

int countOnes(int* MxN, int M, int N, int i, int j)
{
    int c = 0;
    if ((i*N+j+1 < M*N) && (i*N+j+1 >= 0) && (j != N-1)){
        c+=MxN[i*N+j+1];
    }
    if ((i*N+j-1 < M*N) && (i*N+j-1 >= 0) && (j != 0)){
        c+=MxN[i*N+j-1];
    }
    if (((i+1)*N+j < M*N) && ((i+1)*N+j >= 0) && (i != M)){
        c+=MxN[(i+1)*N+j];
    }
    if (((i+1)*N+j+1 < M*N) && ((i+1)*N+j+1 >= 0) && (j != N-1) && (i != M)){
        c+=MxN[(i+1)*N+j+1];
    }
    if (((i+1)*N+j-1 < M*N) && ((i+1)*N+j-1 >= 0) && (j != 0) && (i != M)){
        c+=MxN[(i+1)*N+j-1];
    }
    if (((i-1)*N+j < M*N) && ((i-1)*N+j >= 0) && (i != 0)){
        c+=MxN[(i-1)*N+j];
    }
    if (((i-1)*N+j+1 < M*N) && ((i-1)*N+j+1 >= 0) && (j != N-1) && (i != 0)){
        c+=MxN[(i-1)*N+j+1];
    }
    if (((i-1)*N+j-1 < M*N) && ((i-1)*N+j-1 >= 0) && (j != 0) && (i != 0)){
        c+=MxN[(i-1)*N+j-1];
    }

    return c;
            
}

int sumMatrix(int* MxN, int M, int N)
{
    int sum = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            sum += MxN[i*N + j];
        }
    }
    return sum;
}



void cycle(int* MxN, int M, int N)
{
    int new_MxN[M][N];
    copyMatrix(*new_MxN, MxN, M, N);
    printMatrix(MxN, M, N);
    time_t start, end;
    time(&start);
    while (true) {
        
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                int count = countOnes(MxN, M, N, i, j);
                if (MxN[i*N + j] == 1) {
                    if (count < 2)
                        new_MxN[i][j] = 0;
                    else if (count > 3)
                        new_MxN[i][j] = 0;
                } else {
                    if (count == 3)
                        new_MxN[i][j] = 1;
                }
            }
        }
        copyMatrix(MxN, *new_MxN, M, N);
        time(&end);
        if (end - start > 0) {
            printMatrix(MxN, M, N);
            time(&start);
            if (sumMatrix(MxN, M, N) == 0){
                cout << endl << "THE END !!! \n(ALL ZERO)"<< endl;
                break;
            }
            
        }
    }
}

int changeStdin(int argc, const char * argv[])
{
    int fd = open(argv[1], O_RDONLY);
    return (fd == -1) ? 0 : 1;
}


int main(int argc, const char * argv[]) {
    int M, N;
    int tmp = changeStdin(argc, argv);
    cin >> M >> N;
    int MxN[M][N];
    
    if ((argc > 1) && (tmp))
        readMatrix(*MxN, M, N);
    else
        initMatrix(*MxN, M, N);
    
     printMatrix(*MxN, M, N);
     
     cycle(*MxN, M, N);
    
    return 0;
}
