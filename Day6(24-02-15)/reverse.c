#include <stdio.h>

int main(){
char str[10];
int idx=0;
    scanf("%s",str);
while(str[idx]!=NULL) idx++;

for(int i=idx-1;i>=0;i--){
        printf("%c",str[i]);
    }
}
