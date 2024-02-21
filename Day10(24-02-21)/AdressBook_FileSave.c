#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h>

#define MAX_DATA 100    //최대100명
#define DTYPE_NUM 2     //isUsed를 제외한 Student구조체 내부 변수 개수

//학생 구조체
typedef struct {
    int id;          //학번으로 사용합니다.
    char name[100];  //이름
    int isUsed;     // 1이면 현재값 사용 중 0이면 비어있음으로 표시
} Student;

Student database[MAX_DATA];

void insertData(int id, char* name, bool printCheck) {
    for (int i = 0; i < MAX_DATA; i++) {
        if (!database[i].isUsed) {
            database[i].id = id;
            strcpy(database[i].name, name);
            database[i].isUsed = 1;
            if(printCheck)
                printf("데이터 삽입됨: %d, %s\n", id, name);
            return;
        }
    }
    printf("데이터베이스가 가득 찼습니다.\n");
}

void deleteData(int id) {
    for (int i = 0; i < MAX_DATA; i++) {
        if (database[i].isUsed && database[i].id == id) {
            database[i].isUsed = 0;
            printf("데이터 삭제됨: %d, %s\n", id, database[i].name);
            return;
        }
    }
    printf("데이터를 찾을 수 없습니다.\n");
}

void updateData(int id, char* newName) {
    for (int i = 0; i < MAX_DATA; i++) {
        if (database[i].isUsed && database[i].id == id) {
            strcpy(database[i].name, newName);
            printf("데이터 수정됨: %d -> %s\n", id, newName);
            return;
        }
    }
    printf("데이터를 찾을 수 없습니다.\n");
}

void searchData(int id) {
    for (int i = 0; i < MAX_DATA; i++) {
        if (database[i].isUsed && database[i].id == id) {
            printf("데이터 찾음: %d, %s\n", id, database[i].name);
            return;
        }
    }
    printf("데이터를 찾을 수 없습니다.\n");
}

//file(adressbook.txt)버전 함수
void split_data(char data[], char splitData[10][50])
{

    char temp[50];
    int cur, idx=0;
    for(int i=0;i<DTYPE_NUM;i++){
        for(int j=0;j<50;j++)
            temp[j] = '\0';
        cur = 0;
        while(data[idx]!=' ' && data[idx]!=NULL){
            if(data[idx]!='\n')
                temp[cur] = data[idx];
            idx++;
            cur++;
        }
        if(data[idx] == ' ')
            idx++;
        temp[cur] = '\0';
        strcpy(splitData[i],temp);
    }
}
bool fileDataLoad()
{
    char data[100];
    int LoopChecker = 0;
    FILE * fp = fopen("../adressbook.txt","rt");
    char splitedData[10][50];
    if (fp == NULL)
    {
        printf("주소록 파일 열기 실패");
        return false;
    }
    while(fgets(data, sizeof(data), fp)!=NULL)
    {
        split_data(data, splitedData);
        printf("%d %s\n",atoi(splitedData[0]), splitedData[1]);
        insertData(atoi(splitedData[0]), splitedData[1], false);
        LoopChecker++;
    }
    printf("저장된 주소록 불러오기 완료. 총 %d개 데이터",LoopChecker);
    fclose(fp);
    return true;
}
bool fileSave()
{
    FILE * fp = fopen("../adressbook.txt","wt");
    if (fp == NULL)
    {
        printf("주소록 파일 열기 실패");
        return false;
    }
    char str[100];
    for(int i=0;i<MAX_DATA;i++){
        if(database[i].isUsed){
            sprintf(str,"%d %s\n",database[i].id,database[i].name);
            fputs(str, fp);
        }
    }
    fclose(fp);
    return true;
}

void saveData()
{
    char aggCheck[20];
    while(1){
        printf("주소록 변경사항을 저장할까요?. [y/n]");
        scanf("%s",aggCheck);
        getchar();
        if(strcmp(aggCheck,"y")==0){
            printf("변경사항을 저장합니다.\n");
            if(!fileSave()){
                printf("저장이 실패했습니다.\n");
                continue;
            }
            printf("저장되었습니다.\n");
            break;
        }
        else if(strcmp(aggCheck,"n")==0){
            printf("변경사항 저장이 취소되었습니다.\n");
            break;
        }
        else
            printf("잘못된 입력입니다. %s\n",aggCheck);
    }
}

int main() {
    int choice, id;
    char name[100];

    fileDataLoad();

    while (1) {
        printf("\n메뉴:\n");
        printf("1. 데이터 삽입\n");
        printf("2. 데이터 삭제\n");
        printf("3. 데이터 수정\n");
        printf("4. 데이터 검색\n");
        printf("5. 변경사항 저장\n");
        printf("6. 프로그램 종료\n");
        printf("선택: ");
        scanf("%d", &choice);
        getchar(); // 버퍼에 남은 개행 문자 제거 즉, 엔터제거 효과가 있음 편법

        switch (choice) {
            case 1:
                printf("삽입할 학번을 입력하세요: ");
                scanf("%d", &id);
                getchar(); // 버퍼에 남은 개행 문자 제거
                printf("이름을 입력하세요: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0; // 개행 문자 제거
                insertData(id, name,true);
                break;
            case 2:
                printf("삭제할 학번을 입력하세요: ");
                scanf("%d", &id);
                deleteData(id);
                break;
            case 3:
                printf("수정할 학번을 입력하세요: ");
                scanf("%d", &id);
                getchar(); // 버퍼에 남은 개행 문자 제거
                printf("새 이름을 입력하세요: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0; // 개행 문자 제거
                updateData(id, name);
                break;
            case 4:
                printf("검색할 학번을 입력하세요: ");
                scanf("%d", &id);
                searchData(id);
                break;
            case 5:
                saveData();
                break;
            case 6:
                printf("경고 : 저장하지 않고 종료할 시 작업한 내용이 소실됩니다.\n");
                saveData();
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 선택입니다. 다시 시도해 주세요.\n");
        }
    }

    return 0;
}
