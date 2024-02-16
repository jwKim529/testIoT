#include <stdio.h>

int main(){
    int arr[5];
    int* ptr;
    printf("arr : %x\n\n",arr);
    //문제 1
    for(int i=0;i<5;i++)
        arr[i] = i+1;

    for(int i=0;i<5;i++){
        ptr = &arr[i];
        *ptr += 2;
    }
    printf("ptr : %x\n",ptr);
    for(int i=0;i<5;i++)
        printf("%d ",arr[i]);
    printf("\n\n");

    //문제2
    ptr = &arr[0];
    for(int i=0;i<5;i++)
        arr[i] = i+1;

    for(int i=0;i<5;i++){
        *(ptr+i) += 2;
    }
    printf("ptr : %x\n",ptr);
    for(int i=0;i<5;i++)
        printf("%d ",arr[i]);
    printf("\n");
    return 0;
}
