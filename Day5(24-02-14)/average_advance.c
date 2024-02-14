#include <stdio.h>

float sum_array(float arr[],int count)
{
    float sum=0;
    for(int i=0;i<count;i++) {
        sum += arr[i];
    }
    return sum;
}


int main()
{
    float dataset[100000], tmp, new_data1[100000], new_data2[100000];
    int n, k;
    //입력
    scanf("%d %d",&n,&k);

    for(int i=0;i<n;i++){
        scanf("%f",&dataset[i]);
    }

    //정렬
    for(int i=0;i<n;i++) {
        for (int j = i + 1; j < n; j++) {
            if (dataset[j] < dataset[i]) {
                tmp = dataset[j];
                dataset[j] = dataset[i];
                dataset[i] = tmp;
            }
        }
    }

    //절사평균 : 일정수 제외 후 값 계산
    for(int i=k;i<n-k;i++){
        new_data1[i-k] = dataset[i];
    }

    //보정평균 : 일정수 교체(가장 가까운 값) 후 값 계산
    for(int i=0;i<n;i++){
        if(i<k){
            new_data2[i] = dataset[k];
        }
        else if(i>=n-k){
            new_data2[i] = dataset[n-k-1];
        }
        else{
            new_data2[i] = dataset[i];
        }

    }
    //출력 : 소숫점 셋째자리에서 반올림하여 둘째 자리까지 출력(고정)
    printf("%.2f\n%.2f\n", sum_array(new_data1, n-k*2)/(float)(n-k*2), sum_array(new_data2,n)/(float)n);
    return 0;
}
