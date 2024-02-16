#include <stdio.h>

int main(){
int arr[10];

//1. 배열삽입(짝수)
    for(int i=0;i<10;i++)
        arr[i] = (i+1)*2;

//2. 홀수index 값 출력
    for(int i=0;i<10;i+=2)
        printf("%d ",arr[i]);
    printf("\n");

//3. ?
    for(int i=1;i<10;i+=2)
        printf("%d ",arr[i]);
    printf("\n");
return 0;
}
