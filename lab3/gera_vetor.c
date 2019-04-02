#include<stdio.h>

#define N 7000


int main() {
   int i;
   printf("%d %d\n", N, N);
   //printf("%d \n", N);
   for(i=0; i<N*N; i++) {
      printf("%.1f ", 3.0);
   }
   printf("\n");
   return 0;
}
