#include<stdio.h>
#define INF 99999 

//multplie modify 

int dist[4][4] = { {0,  INF,  INF, INF}, 
                        {INF, 0,  INF, INF}, 
                        {INF, INF, 0,   INF}, 
                        {INF, INF, INF,0} 
                      }; 


int graph[4][4] = {     {0, 6, 5, 7}, 
                        {6, 0, INF, 2}, 
                        {5, INF, 0, INF}, 
                        {7, 2, INF, 0} 
                      }; 


// int graph[4][4] =  { {0,   -5,  INF, 10}, 
//                         {INF, 0,   3, INF}, 
//                         {INF, INF, 0,   1}, 
//                         {INF, INF, INF, 0} 
//                       }; 


int minElement(int a,int b){

  return a<b?a:b;

}

int dpc(){

  for (int k = 0;k<4;k++){
    for(int j = k-1;j>=0;j--){
      for(int i = j-1;i>=0;i--){
        graph[i][j] = minElement(graph[i][j],graph[i][k]+graph[k][j]);
        graph[j][i] = minElement(graph[j][i],graph[j][k]+graph[k][i]);
        if(graph[i][j]+graph[j][i]<0)
          return 0;
      }
    }
  }

  return 1;
}


void snowball(){

    for(int k = 0;k<4;k++){

        for(int j = k-1;j>=0;j--){

          for(int i = 0;i<k;i++){

              dist[i][k] = minElement(dist[i][k],dist[i][j]+graph[j][k]);
              dist[k][i] = minElement(dist[k][i],dist[j][i]+graph[k][j]);
          }

        }

    }

}


int main(){

    if(dpc()==0)
      return 0;

    snowball();

    for(int i=0;i<4;i++){
      for(int j=0;j<4;j++){
        printf("%d ", dist[i][j]);
      }
      printf("\n");
    }


    for(int i=0;i<4;i++){
      for(int j=0;j<4;j++){
        printf("%d ", graph[i][j]);
      }
      printf("\n");
    }

    return 0; 
}
