//  mpic++ circuit1.cpp
//  Compiling with Rokkodai cluster: mpirun -hostfile ~/hostfile -np 4 ./a.out 


#include <stdio.h>
#include <string.h>
#include "mpi.h"

//プロセス0から、他のプロセスへデータを送る
/*void Get_data(float* a_ptr, float* b_ptr, int* n_ptr, int my_rank, int p)
{
    int source = 0;
    int dest;
    int tag;
    MPI_Status status;

    if(my_rank==0)
    {
        printf("Enter a, b, and n\n");
        scanf("%f %f %d", a_ptr, b_ptr, n_ptr);

        for(dest = 1; dest<p; dest++)
        {
            tag = 0;
            MPI_Send(a_ptr, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
            tag = 1;
            MPI_Send(b_ptr, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
            tag = 2;
            MPI_Send(n_ptr, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
        }
    }
    else
    {
        tag = 0;
        MPI_Recv(a_ptr, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
        tag = 1;
        MPI_Recv(b_ptr, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
        tag = 2;
        MPI_Recv(n_ptr, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
    }
}*/

//プロセス0から、他のプロセスへデータを集団通信で送る
/*
* BCast を全プロセスで呼び出す
* 第4引数と合致する rank のプロセスに於いては bcast は送信処理をする。
* それ以外の rank のプロセスでは bcast は受信処理をする。
*/
void Get_data2(float* a_ptr, float* b_ptr, int* n_ptr, int my_rank)
{
    if(my_rank==0)
    {
        printf("Enter a, b, and n\n");
        scanf("%f %f %d",a_ptr, b_ptr, n_ptr);
    }
    //MPI_Bcast (void* message, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
    MPI_Bcast(a_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(b_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(n_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
}


int main(int argc, char* argv[]){
    int   my_rank;  //カレントプロセスのランク
    int   p;        //プロセス数
    float a = 0.0;  //全積分区間の左端
    float b = 1.0;  //全積分区間の右端
    int   n = 1024; //台形の全個数
    float h;        //台形の底辺の長さ
    float local_a;  //本プロセスの積分区間の左端
    float local_b;  //本プロセスの積分区間の右端
    int   local_n;  //本プロセスの台形の数
    float integral; //本プロセスの積分
    float total;    //全積分値
    int   source;   //送信プロセスのランク;
    int   dest=0;   //受信プロセスのランク (ここでは、メッセージを全て プロセス 0 へ送る)
    int   tag=0;    //メッセージタグ
    char  message[100];  //メッセージの保管場所
    MPI_Status status;  //受信の戻りステータス


    float Trap(float local_a, float local_b, int local_n, float h); //ローカルの積分値を計算


    MPI_Init(&argc,&argv);   //MPIのスタートアップ


    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   //カレントプロセスのランクを求める
    MPI_Comm_size (MPI_COMM_WORLD, &p);   //全プロセス数を求める

    Get_data2(&a, &b, &n, my_rank);

    h = (b-a)/n;    //hは全てのプロセスで同じとする
    local_n = n/p;  //台形の数も同じ


    //各プロセスの積分区間の指定
    local_a = a + my_rank*local_n*h; 
    local_b = local_a+local_n*h;
    integral = Trap(local_a, local_b, local_n, h); //カレントプロセスにおける積分値の計算


    //各プロセスの積分を加算する
    /*if(my_rank==0)
    {
        total = integral; //自分のプロセスの積分値をまずは全積分値へ格納
        for(source = 1; source<p; source++)
        {
            MPI_Recv(&integral, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status); 
            total = total+integral; //各プロセスから送られてきた積分値をtotalに足し込む
        }
    }
    else
    {
        MPI_Send(&integral, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
    }*/

    //MPI_Reduceを使って、各プロセスの要素をプロセス0で足しあげる
    /*
    * &integralで参照されるメモリ中の複数のオペランド (ここでは、積分値)を、
    * 演算 (MPI_SUM)を用いて結合して、結果をルートプロセス (6番目の引数:0)へ
    * の&totalへ書き込む。
    * 引数は、左から オペランド (計算される要素)のアドレス、結果をストアする変数アドレス、データサイズ、データ型、演算の種類、結果をストアするプロセス番号、コミュニケータ
    */
    MPI_Reduce(&integral, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);


    //結果の出力
    if(my_rank==0)
    {
        printf("With n=%d trapezoids, our estimate\n",n);
        printf("of the integral from %f to %f = %f\n",a, b, total);
    }
    MPI_Finalize();   //MPIのシャットダウン
    return 0;
}


//台形公式による積分
float Trap(float local_a, float local_b, int local_n, float h)
{
    float integral=0.0;
    float x=0.0;
    int i=0;


    float f (float x); //被積分関数


    integral = (f(local_a)+f(local_b))/2.0;
    x = local_a;
    for(i=1; i<= local_n-1; i++)
    {
        x = x+h;
        integral = integral+f(x);
    }
    integral = integral*h;
        return integral;
}


float f (float x)
{
    float return_val;
    return_val = x*x;
    return return_val;
}