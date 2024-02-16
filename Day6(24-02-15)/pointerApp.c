#include <stdio.h>

int main(){
    int arr[6] = {1,3,5,7,9,11};
    int* ptr = &arr[0];

    for(int i=0;i<6;i++)
        printf("%d ",arr[i]);
    printf("\n");

    for(int i=0;i<6;i++)
        *(ptr+i) += 3;

    for(int i=0;i<6;i++)
        printf("%d ",arr[i]);

    return 0;
}
