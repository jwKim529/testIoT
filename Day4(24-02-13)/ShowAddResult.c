#include <stdio.h>

void ShowAddResult(int num){
    printf("덧셈결과 출력 :%d\n",num);

}

int main()
{
    int num1, num2;
    printf("Number1 :");
    scanf("%d",&num1);

    printf("Number1 :");
    scanf("%d",&num2);

    ShowAddResult(num1+num2);

    return 0;
}
