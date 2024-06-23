//  mpic++ MPI_intro.cpp
//  mpirun -hostfile ~/hostfile -np 4 ./a.out
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <iostream>

int main(int argc, char* argv[]){
    int my_rank;  //カレントプロセスのランク
    int p;  //プロセス数
    int source;   //送信プロセスのランク;
    int dest;   //受信プロセスのランク
    int tag=0;   //メッセージタグ
    char message[100];  //メッセージの保管場所
    MPI_Status status;  //受信の戻りステータス

    MPI_Init(&argc,&argv);   //MPIのスタートアップ

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   //カレントプロセスのランクを求める
    MPI_Comm_size (MPI_COMM_WORLD, &p);   //全プロセス数を求める

    if(my_rank!=0){
        sprintf(message, "greeting from process %d!",my_rank);//メッセージの生成
        dest=0;
        MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);   //char型最後に付く0も送るので、strlen+1を使う
    } else {
         for(source = 1; source < p; source++){
             MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
             //printf("%s n",message);  //nにバックスラッシュを後で入れる
             std::cout<<message<<std::endl;
        }
    }
    MPI_Finalize();   //MPIのシャットダウン
    return 0;
}