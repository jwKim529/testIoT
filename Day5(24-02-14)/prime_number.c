#include <stdio.h>
//#include <math.h>

int res(int num){
  int ckr=0;
  for(int i=2;i<num/2;i++){
    if(num%i == 0)
      ckr = 1;
  }
  if(ckr == 0)
    return 1;
  return 0;
}
int main(){
  for(int i=2;i<=100;i++){
    if(res(i) == 1)
      printf("%d ",i);
  }
  return 0;
}
