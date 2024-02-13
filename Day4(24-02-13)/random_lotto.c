#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int random_num() {
    int randomNumber = (rand() % 45)+1;
    return randomNumber;
}

int main() {
    srand(time(NULL));
    bool a[46];
    int i,j, tmp,cnt;

    for(i = 0;i<46;i++){
            a[i]= false;
    }

    for(j=0;j<3;j++){
        
        cnt = 0;
        while(cnt<6){
            tmp = random_num();
            if(a[tmp] == false){
                a[tmp] = true;
                cnt++;
            }
        }
        for(i=1;i<46;i++){
            if(a[i] == true){
                a[i] = false;
                if(i < 10){
                    printf(" ");
                }
                printf("%d ",i);
            }
        }
        printf("\n");
    }
    return 0;
}
