#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int randNumber(){
    int randomNumber = (rand() % 45) + 1;
    return randomNumber;
}

void init_main(){
    srand(time(NULL));
}

int main() {
    init_main();

    int i,j, cnt=0,tmp;
    bool lotto[46];
    for(i=0;i<46;i++) lotto[i] = false;

    printf("--------------------------------\n");
    printf("로또번호 : ");
    while(cnt<6){
        tmp = randNumber();
        if(lotto[tmp] == false){
            lotto[tmp] = true;
            cnt++;
        }
    }
    for(i=1;i<46;i++) {
        if(lotto[i] == true) {
            printf("%d ",i);
        }
    }
    cnt = 0;
    while(cnt<1){
        tmp = randNumber();
        if(lotto[tmp] == false){
            break;
        }
    }
    printf("\n보너스번호 : %d\n",tmp);
    printf("--------------------------------\n");
    return 0;
}


