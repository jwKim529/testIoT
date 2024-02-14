#include <stdio.h>

int perfect(int num){
    int sum=0;
    for(int i=1;i<num;i++){
        if(num%i == 0)
            sum+=i;
    }
    return sum;
}

int main() {
    int num;
    while(1) {
        scanf("%d", &num);
        if(num == -1) {
            printf("EXIT\n");
            break;
        }
        if (num == perfect(num)) {
            printf("yes\n");
        }
        else{
            printf("no\n");
        }
    }

    return 0;
}
