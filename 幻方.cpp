#include <stdio.h>
#include <iostream>
#define N 15
using namespace std;
int main(){
    int i,j,k,a[N][N];
    for(i=0;i<N;i++)
    for(j=0;j<N;j++)
    a[i][j]=0; /*初始值为零*/
    j=N/2;
    a[0][j]=1; /*确定1的位置*/ 
    for(k=2;k<=N*N;k++){
                        i--;
                        j++;
                        if(i<0)
                        i=N-1; /*行出界*/
                        else if(j>N-1)
                        j=0; /*列出界*/ 
                        if(a[i][j]==0)
                        a[i][j]=k; /*按规律顺序确定数值位置*/
                        else{i=(i+2)%N;
                        j=(j-1+N)%N;
                        a[i][j]=k; /*已有数字时数字的位置*/}}
                        printf("\n");
                        for(i=0;i<N;i++){
                                         printf("  ");
                                         for(j=0;j<N;j++)
                                         printf("%3d",a[i][j]); /*显示幻方阵*/
                                         printf("\n");}
                                         system("pause");
                                         return 0;
                                         }
