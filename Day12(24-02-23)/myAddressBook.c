#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h>

#define NAME_LIMIT 20   //이름 길이 한계
#define PHONE_LIMIT 13  //연락처 길이 한계
#define EMAIL_LIMIT 50  //이메일 길이 한계
#define SEX_LIMIT 4     //성별 길이 한계
#define BIRTH_LIMIT 8   //생년월일 길이 한계
#define GROUP_LIMIT 20  //그룹명 길이 한계
#define MEMO_LIMIT 200  //메모 길이 한계

#define DB_FILE "../adress.db"

#define ADMIN_CODE "iot123"

//학생 구조체
typedef struct {
    int id;             //학번
    char name[50];      //이름
    char phonenum[20];  //연락처
    char email[100];    //이메일 주소
    char sex[10];       //성별
    char birth[10];     //생년월일
    char group[30];     //그룹
    char memo[250];     //메모
} Student;

//그룹명 구조체
typedef struct {
    int id;             //id
    char groupTag[100]; //그룹명
} Groups;

/*
 * --------------------------------------------------------------
 * 'adress.db' DB 정보
 * 테이블명 : STUDENT
 * 컬럼명 : INT id(PRIMARY KEY), TEXT NAME, TEXT PhoneNUM, TEXT EMAIL, INT sex_id, TEXT BIRTH, INT group_id, TEXT MEMO
 * 제약조건 : FOREIGN KEY sex_id REFERENCE SEX(id)
 *          FOREIGN KEY group_id REFERENCE GROUP(id)
 *
 * 컬럼 별 제한
 * id : 숫자만
 * NAME : 문자열(20자 이하)
 * PhoneNum : 문자열(13자)
 * EMAIL : 문자열(50자 이하)
 * SEX(sex_id) : 왜래키 - 문자열(외자 - 남/여)
 * BIRTH : 문자열(8자)
 * GROUPS(group_id) : 왜래키 - 문자열(20자 이하)
 * MEMO : 문자열(200자 이하)
 * --------------------------------------------------------------
 */

/*
 * STUDENT 테이블에 새 컬럼 적용 시 변경해야 할 함수
 * Student(구조체)
 * printStudent
 * returnInputString
 * dbCreateTable
 * dbSelect
 * returnChoiceSQL
 * insertData
 * updateData
 * searchData
 * deleteData
 * 
 * 새 컬럼의 입력 형식에 맞는 새로운 inputChecker_함수 생성 검토
 */

//출력 형식 등 문자열 출력 형식 관리를 위한 함수(~returnInputString)
void printLine()
{
    //점선 출력함수 : 균일한 점선 출력 및 점선 길이 조정을 용이하게 하기 위한 함수
    printf("--------------------------\n");
}

void printStudent(Student temp)
{
    //데이터 레코드 출력함수 : 데이터레코드 출력 형식을 일괄적으로 관리하기 위한 함수
    printf("%d, %s, %s, %s, %s, %s, %s, %s\n",
           temp.id,
           temp.name,
           temp.phonenum,
           temp.email,
           temp.sex,
           temp.birth,
           temp.group,
           temp.memo
           );
}

char* returnInputString(char* msg)
{
    //입력된 문자열에 대응하는 문자열 반환 함수 : 입력포맷 일괄 관리를 위한 함수
    if(strcmp(msg, "id") == 0)
        return "id를 입력하세요.(숫자만) : ";
    else if(strcmp(msg, "NAME") == 0)
        return "이름을 입력하세요.(20자 이하) : ";
    else if(strcmp(msg, "PhoneNUM") == 0)
        return "연락처를 입력하세요.(12~13자, '-'포함) : ";
    else if(strcmp(msg, "EMAIL") == 0)
        return "이메일 주소를 입력하세요.(50자 이하) : ";
    else if(strcmp(msg, "SEX") == 0)
        return "성별을 입력하세요.(남 or 여) : ";
    else if(strcmp(msg, "BIRTH") == 0)
        return "생년월일을 입력하세요.(8자, 예:19221201) : ";
    else if(strcmp(msg, "GROUP") == 0)
        return "그룹 태그를 입력하세요.(20자 이하) : ";
    else if(strcmp(msg, "MEMO") == 0)
        return "메모할 문구를 입력하세요.(200자 이하) : ";
    return "";
}

//테이블 생성 또는 작업 시작 준비를 위한 함수(~dbOpen)
sqlite3* dbCreateTable(sqlite3* db, char* err_msg)
{
    //SQLITE3 기반 테이블 생성 함수 : STUDENT 테이블을 초기화하여 생성하는 함수.
    char *sql = "DROP TABLE IF EXISTS SEX;"
                "CREATE TABLE SEX("
                "id INT PRIMARY KEY,"
                "GEN_NAME TEXT"
                ");"
                "INSERT INTO SEX VALUES (1, '남');"
                "INSERT INTO SEX VALUES (2, '여');"
                "DROP TABLE IF EXISTS GROUPS;"
                "CREATE TABLE GROUPS("
                "id INT PRIMARY KEY,"
                "GROUP_TAG TEXT"
                ");"
                "INSERT INTO GROUPS VALUES (999,'없음');"
                "DROP TABLE IF EXISTS STUDENT;"
                "CREATE TABLE STUDENT("
                "id INT PRIMARY KEY, "
                "NAME TEXT, "
                "PhoneNUM TEXT, "
                "EMAIL TEXT, "
                "sex_id INT, "
                "BIRTH TEXT, "
                "group_id INT, "
                "MEMO TEXT, "
                "FOREIGN KEY (sex_id) REFERENCES SEX(id),"
                "FOREIGN KEY (group_id) REFERENCES GROUPS(id) ON DELETE CASCADE"
                ");"
                ;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if(rc != SQLITE_OK){
        printf("%d\n",rc);
        printf("Fail to create table\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }
    return db;
}

int dbTableExistCheck(sqlite3* db, char* err_msg)
{
    //테이블 존재 여부 체크 함수 : SQL 쿼리문을 통해 테이블이 존재하는 지 여부를 판단하고 반환하는 함
    //반환값 1:있음, 0:없음
    char* sql = "SELECT COUNT(*) AS \"Exists\" FROM sqlite_master WHERE type = \"table\" AND name = \"STUDENT\";";

    char checker[10];
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db,sql, -1, &res,0);
    if(rc != SQLITE_OK){
        printf("Fail to select data\n");
        sqlite3_finalize(res);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    while(sqlite3_step(res) == SQLITE_ROW){
        if(sqlite3_column_text(res,0)!=NULL)
            strcpy(checker,sqlite3_column_text(res,0));
    }
    sqlite3_finalize(res);

    return atoi(checker);
}

sqlite3* dbOpen(sqlite3 *db, char*err_msg)
{
    //SQLite3 기반의 DB파일을 열고 해당 데이터베이스의 핸들 변수를 리턴하는 함수
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }
    if(dbTableExistCheck(db, err_msg))
        return db;

    db = dbCreateTable(db, err_msg);
    if(sqlite3_limit(db, SQLITE_LIMIT_LENGTH,100) == SQLITE_ERROR){
        printf("Cannot set column limit\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }
    return db;
}

//터미널 입력값 오류 관리를 위한 함수(~inputChecker_Birth)
int inputChecker_Num(char* msg)
{
    //문자열->숫자 변환 함수 : 문자열이 숫자로 변환 가능한지 판별하여 가능하다면 숫자로 변환하여 리턴하는 함수
    char inputData[100];
    int idx;
    while(1) {
        printf(msg);
        scanf("%s", inputData);
        getchar();
        idx = 0;
        while (inputData[idx] >= '0' && inputData[idx] <= '9')
            idx++;
        if (strlen(inputData) == idx)
            return atoi(inputData);
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 숫자가 아닌 데이터\n\n", inputData);
    }
}

int inputChecker_Menu(char* msg, int maxNum)
{
    //메뉴입력 판별 함수 : 입력의 숫자 판별 기능을 이용하여 처리가능한 메뉴값이 입력되었는 지 판단하는 함수
    int num;
    while(1) {
        num = inputChecker_Num(msg);
        if (num > 0 && num <= maxNum)
            return num;
        else
            printf("잘못된 입력입니다.\n입력된 데이터 : %d\n위반사항 : 입력 범위(1~%d) 벗어남\n\n", num,maxNum);
    }

}

char* inputChecker_Char(char* msg, int lenLimit)
{
    //입력된 문자열이 지정된 길이 이하로 입력되었는지 판별하는 함수
    static char inputData[100];
    while(1) {
        printf(msg);
        scanf("%s", inputData);
        getchar();
        if(strlen(inputData) <= lenLimit)
            return inputData;
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 데이터 길이 한계(%d)\n\n", inputData,lenLimit);
    }
}

char* inputChecker_Phone(char* msg, int lenLimit)
{
    //입력된 문자열이 연락처 형식에 맞는 지 판별하는 함수
    static char inputData[20];
    int slen, cnt;
    while(1){
        cnt=0;
        printf(msg);
        scanf("%s", inputData);
        getchar();
        slen = strlen(inputData);
        if(slen <= lenLimit && slen>=12) {
            for(int i=0;i<slen;i++){
                if(
                        ( slen == 12 && (i == 3||i == 7)  && inputData[i]!='-' )||
                        ( slen == 12 && !(i == 3||i == 7) && (inputData[i]<'0' || inputData[i]>'9') )||
                        ( slen == 13 && (i == 3||i == 8)  && inputData[i]!='-' )||
                        ( slen == 13 && !(i == 3||i == 8) && (inputData[i]<'0' || inputData[i]>'9') )
                        )// 12 또는 13자리, 하이픈(-)의 위치, 하이픈 제외 데이터가 숫자
                    break;
                cnt++;
            }
            if(cnt == slen)
                return inputData;
            printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 연락처(전화번호) 형식에 맞지 않는 입력\n\n", inputData);
        }
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 데이터 길이 제한(12~%d)\n\n", inputData,lenLimit);
    }

}

char* inputChecker_yn(char* msg, int lenLimit)
{
    //y또는 n 입력이 정확하게 들어왔는 지 판별하는 함수
    char* ynChecker;
    while(1)
    {
        ynChecker = inputChecker_Char(msg, lenLimit);
        if(strcmp(ynChecker,"y") == 0 || strcmp(ynChecker,"n")==0)
            return ynChecker;
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 입력 범위(y or n) 벗어남\n\n", ynChecker);
    }
}

char* inputChecker_Sex(char* msg, int lenLimit)
{
    static char inputData[100];
    while(1) {
        printf(msg);
        scanf("%s", inputData);
        getchar();
        if(strlen(inputData) <= lenLimit ) {
            if (strcmp(inputData, "남") == 0 || strcmp(inputData, "여") == 0)
                return inputData;
            else
                printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 입력 범위(남 or 여) 벗어남\n\n", inputData);
        }
        else
            printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 데이터 길이 한계(%d)\n\n", inputData,lenLimit);
    }
}

char* inputChecker_Birth(char* msg, int lenLimit)
{
    static char inputData[100];
    int cnt, slen;
    while(1) {
        printf(msg);
        scanf("%s", inputData);
        getchar();
        cnt = 0;
        slen = strlen(inputData);
        if(slen != lenLimit){
            printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 데이터 길이 불일치(%d)\n\n", inputData,lenLimit);
            continue;
        }
        while(cnt < slen){
            if(inputData[cnt]<'0' && inputData[cnt]>'9')
                break;
            cnt++;
        }
        if(cnt==slen)
            return inputData;
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 적절하지 않은 입력\n\n", inputData);
    }
}

//작업 종료 표시를 출력하는 함수
void noneTypeInput()
{
    //작업 완료 문구를 표기하는 함수
    printf("\n작업이 완료되었습니다. 아무 키나 입력하면 메인 메뉴로 돌아갑니다.");
    getchar();
}

//SQL구문을 입력받거나 지정된 SQL구문을 직접 처리하는 함수(~dbRecordCount)
void dbQueryExec(sqlite3* db, char* err_msg, char* sql)
{
    //SQL 쿼리문 처리 함수 : SQL 쿼리문의 리턴값이 없거나 필요없는 경우에 사용하는 함수
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }
}

int dbSelect(sqlite3* db, char* err_msg, char* sql, Student* temp)
{
    //SQL 쿼리 처리 함수 : SELECT 구문의 리턴값을 반환해주는 함수
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db,sql, -1, &res,0);
    if(rc != SQLITE_OK){
        printf("Fail to select data\n");
        sqlite3_finalize(res);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    int step, idx=0;
    while(1){
        step = sqlite3_step(res);
        if(step == SQLITE_ROW){
            temp[idx].id = atoi(sqlite3_column_text(res,0));
            strcpy(temp[idx].name,sqlite3_column_text(res,1));
            strcpy(temp[idx].phonenum,sqlite3_column_text(res,2));
            strcpy(temp[idx].email,sqlite3_column_text(res,3));
            strcpy(temp[idx].sex,sqlite3_column_text(res,4));
            strcpy(temp[idx].birth,sqlite3_column_text(res,5));
            strcpy(temp[idx].group,sqlite3_column_text(res,6));
            strcpy(temp[idx].memo,sqlite3_column_text(res,7));
            idx++;
        }
        else
            break;
    }
    sqlite3_finalize(res);
    return idx;
}

int dbSearch(sqlite3* db, char* err_msg, char* sql, Student* students)
{
    //SELECT 구문처리 결과물인 데이터 레코드를 출력하는 함수
    int idx = dbSelect(db, err_msg, sql, students);
    if(idx == 0) {
        printf("데이터를 찾을 수 없습니다.\n");
        return idx;
    }
    printf("검색된 데이터 : \n");
    printLine();
    for(int i=0;i<idx;i++) {
        printf("[%d] ",i+1);
        printStudent(students[i]);
    }
    printLine();
    return idx;
}

int dbRecordCount(sqlite3* db, char* err_msg)
{
    //STUDENT 테이블 내 데이터 레코드의 수를 반환하는 함수
    char sql[200];
    sprintf(sql,"SELECT COUNT(*) FROM STUDENT");
    char counter[10];
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(db,sql, -1, &res,0);
    if(rc != SQLITE_OK){
        printf("Fail to select data\n");
        sqlite3_finalize(res);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    while(sqlite3_step(res) == SQLITE_ROW){
        if(sqlite3_column_text(res,0)!=NULL)
            strcpy(counter,sqlite3_column_text(res,0));
    }
    sqlite3_finalize(res);

    return atoi(counter);
}

//그룹 취급 함수(~groupMenu)
int dbSelectToGroups(sqlite3* db, char* err_msg, char* sql, Groups* groups)
{
    sqlite3_stmt *res;

    int rc = sqlite3_prepare_v2(db,sql, -1, &res,0);
    if(rc != SQLITE_OK){
        printf("Fail to select data\n");
        sqlite3_finalize(res);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    int step, idx = 0;
    while(1){
        step = sqlite3_step(res);
        if(step == SQLITE_ROW){
            groups[idx].id = atoi(sqlite3_column_text(res,0));
            strcpy(groups[idx].groupTag,sqlite3_column_text(res,1));
            idx++;
        }
        else
            break;
    }
    sqlite3_finalize(res);
    return idx;
}

int dbSearchToGroups(sqlite3* db, char* err_msg, char* sql, Groups* groups)
{

    //SELECT 구문처리 결과물인 데이터 레코드를 출력하는 함수
    int idx = dbSelectToGroups(db, err_msg, sql, groups);

    if(idx == 0) {
        printf("데이터를 찾을 수 없습니다.\n");
        return idx;
    }
    printf("검색된 데이터 : \n");
    printLine();
    for(int i=0;i<idx;i++)
        printf("[%d] %d, %s\n",i+1, groups[i].id, groups[i].groupTag);
    printLine();
    return idx;
}

char* inputChecker_Group(sqlite3* db, char* err_msg, char* msg, int lenLimit, int checkMode)
{
    static char inputData[100];
    char sql[200];
    Groups groups[100];
    int idx;

    while(checkMode == 1) {
        printf(msg);
        scanf("%s", inputData);
        getchar();
        if(strlen(inputData) <= lenLimit){
            sprintf(sql, "SELECT * FROM GROUPS WHERE GROUP_TAG = '%s'", inputData);
            idx = dbSelectToGroups(db, err_msg, sql, groups);
            if(idx == 0) {
                printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 존재하지 않는 그룹명\n\n", inputData);
                continue;
            }
            else if(idx>0)
                return inputData;
        }
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 데이터 길이 한계(%d)\n\n", inputData,lenLimit);
    }
    while(checkMode == 2){
        printf(msg);
        scanf("%s", inputData);
        getchar();
        if(strlen(inputData) <= lenLimit){
            sprintf(sql, "SELECT * FROM GROUPS WHERE GROUP_TAG = '%s'", inputData);
            idx = dbSelectToGroups(db, err_msg, sql, groups);
            if(idx > 0) {
                printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 이미 존재하는 그룹명\n\n", inputData);
                continue;
            }
            else if(idx == 0)
                return inputData;
        }
        printf("잘못된 입력입니다.\n입력된 데이터 : %s\n위반사항 : 데이터 길이 한계(%d)\n\n", inputData,lenLimit);
    }
    return "";
}

void insertGroupTag(sqlite3* db, char* err_msg)
{
    char sql[200];
    Groups groups[100];
    sprintf(sql,"SELECT * FROM GROUPS");
    int idx = dbSearchToGroups(db, err_msg, sql, groups);
    if(idx == 0)
        return;

    int id=1,cur;
    char name[100];
    printf("추가할 그룹명을 입력하세요 : ");
    scanf("%s",name);
    getchar();
    while(id < 100) {
        cur = 0;
        while(cur<idx){
            if(groups[cur].id == id)
                break;
            cur++;
        }
        if(cur == idx)
            break;
        id++;
    }
    if(id == 100){
        printf("더 이상 그룹명을 추가할 수 없습니다.");
        return;
    }
    sprintf(sql, "INSERT INTO GROUPS VALUES( %d,'%s' )", id, name);
    dbQueryExec(db, err_msg,sql);
    noneTypeInput();
}

void deleteGroupTag(sqlite3* db, char* err_msg)
{
    char sql[200];
    Groups groups[100];
    sprintf(sql,"SELECT * FROM GROUPS");
    int idx = dbSearchToGroups(db, err_msg, sql, groups);
    if(idx == 1){
        printf("제거 가능한 항목이 존재하지 않습니다.\n");
        return;
    }
    int inputData_Num, cur;
    while(1){
        inputData_Num= inputChecker_Num("제거할 그룹의 id(그룹명 앞 숫자)를 입력하세요 : ");
        if(inputData_Num!=999) {
            cur = 0;
            while(cur<idx) {
                if (groups[cur].id == inputData_Num)
                    break;
                cur++;
            }
            if(cur < idx)
                break;
            printf("존재하지 않는 id입니다. 다시 입력해주세요.\n");
            continue;
        }
        printf("999번 항목은 제거할 수 없습니다. 다시 입력해주세요\n");
    }
    sprintf(sql, "DELETE FROM GROUPS WHERE id = %d", inputData_Num);
    dbQueryExec(db, err_msg,sql);
    noneTypeInput();
}

void updateGroupTag(sqlite3* db, char* err_msg)
{
    char sql[200];
    Groups groups[100];
    sprintf(sql,"SELECT * FROM GROUPS");
    int idx = dbSearchToGroups(db, err_msg, sql, groups);
    if(idx == 1){
        printf("수정 가능한 항목이 존재하지 않습니다.\n");
        return;
    }
    int inputData_Num, cur;
    while(1){
        inputData_Num = inputChecker_Num("수정할 그룹의 id(그룹명 앞 숫자)를 입력하세요 : ");
        if(inputData_Num!=999) {
            cur = 0;
            while(cur<idx) {
                if (groups[cur].id == inputData_Num)
                    break;
                cur++;
            }
            if(cur < idx)
                break;
            printf("존재하지 않는 id입니다. 다시 입력해주세요.\n");
            continue;
        }
        printf("999번 항목은 수정할 수 없습니다. 다시 입력해주세요\n");
    }
    char* inputData = inputChecker_Group(db, err_msg, "새로운 그룹명을 입력하세요 : ", GROUP_LIMIT, 2);
    sprintf(sql, "UPDATE GROUPS SET GROUP_TAG = '%s' WHERE id = %d", inputData, inputData_Num);
    dbQueryExec(db, err_msg,sql);
    noneTypeInput();
}

void groupMenu(sqlite3* db, char* err_msg)
{
    int choice;
    while(1) {
        choice = inputChecker_Menu("\n그룹 메뉴:\n"
                                   "1. 되돌아가기\n"
                                   "2. 데이터 삽입\n"
                                   "3. 데이터 삭제\n"
                                   "4. 데이터 수정\n"
                                   "선택: ", 4);
        printf("\n");
        switch(choice){
            case 1:
                return;
            case 2:
                insertGroupTag(db, err_msg);
                break;
            case 3:
                deleteGroupTag(db, err_msg);
                break;
            case 4:
                updateGroupTag(db, err_msg);
                break;

        }
    }


}

//메인메뉴 처리 함수(~deleteData)
char* returnChoiceSQL(sqlite3* db, char* err_msg, int choice)
{
    static char sql[200];
    char* inputData;
    int inputData_Num;
    switch(choice)
    {
        case 1:
            sprintf(sql, "");
            break;
        case 2:
            inputData_Num = inputChecker_Num(returnInputString("id"));
            sprintf(sql,"SELECT * FROM STUDENT WHERE id=%d",inputData_Num);
            break;
        case 3:
            inputData = inputChecker_Char(returnInputString("NAME"),NAME_LIMIT);
            sprintf(sql,"SELECT * FROM STUDENT WHERE NAME='%s'",inputData);
            break;
        case 4:
            inputData = inputChecker_Phone(returnInputString("PhoneNUM"),PHONE_LIMIT);
            sprintf(sql,"SELECT * FROM STUDENT WHERE PhoneNUM='%s'",inputData);
            break;
        case 5:
            inputData = inputChecker_Char(returnInputString("EMAIL"),EMAIL_LIMIT);
            sprintf(sql,"SELECT * FROM STUDENT WHERE EMAIL='%s'",inputData);
            break;
        case 6:
            inputData = inputChecker_Sex(returnInputString("SEX"),SEX_LIMIT);
            sprintf(sql,"SELECT * FROM STUDENT WHERE sex_id=(SELECT id FROM SEX WHERE GEN_NAME = '%s')",inputData);
            break;
        case 7:
            inputData = inputChecker_Birth(returnInputString("BIRTH"),BIRTH_LIMIT);
            sprintf(sql,"SELECT * FROM STUDENT WHERE BIRTH='%s'",inputData);
            break;
        case 8:
            inputData = inputChecker_Group(db, err_msg, returnInputString("GROUP"),GROUP_LIMIT,1);
            sprintf(sql,"SELECT * FROM STUDENT WHERE group_id=(SELECT id FROM GROUPS WHERE GROUP_TAG = '%s')",inputData);
            break;
        case 9:
            sprintf(sql,"SELECT * FROM STUDENT");
            break;
    }
    return sql;
}

void insertData(sqlite3* db, char* err_msg)
{
    //데이터 삽입 함수
    printf("데이터 삽입\n");

    //현재 주소록 개수 점검
    int rc = dbRecordCount(db, err_msg);
    if(rc == 100) {
        printf("주소록이 가득 차 더 이상 추가할 수 없습니다.\n다른 메뉴를 통해 주소록을 정리한 후에 다시 시도하십시오.\n");
        return;
    }

    int id;
    char sql[200];

    id = inputChecker_Num(returnInputString("id"));
    sprintf(sql,"SELECT * FROM STUDENT WHERE id=%d",id);

    Student students[100];
    int idx = dbSelect(db, err_msg, sql, students);
    if(idx>0) {
        printf("삽입에 실패했습니다. 이미 존재하는 id입니다.\n작업을 종료하고 메인 메뉴로 돌아갑니다.\n");
        return;
    }

    Student temp;
    temp.id = id;

    strcpy(temp.name,inputChecker_Char(returnInputString("NAME"), NAME_LIMIT));
    strcpy(temp.phonenum,inputChecker_Phone(returnInputString("PhoneNUM"), PHONE_LIMIT));
    strcpy(temp.email,inputChecker_Char(returnInputString("EMAIL"), EMAIL_LIMIT));
    strcpy(temp.sex,inputChecker_Sex(returnInputString("SEX"), SEX_LIMIT));
    strcpy(temp.birth,inputChecker_Birth(returnInputString("BIRTH"), BIRTH_LIMIT));
    strcpy(temp.group,inputChecker_Group(db,err_msg,returnInputString("GROUP"), GROUP_LIMIT,1));
    strcpy(temp.memo,inputChecker_Char(returnInputString("MEMO"), MEMO_LIMIT));

    printf("현재 주소록 내 데이터 개수 : %d개\n",rc);
    sprintf(sql,"INSERT INTO STUDENT VALUES ("
                "%d, '%s', '%s',"
                "'%s',(SELECT id FROM SEX WHERE GEN_NAME = '%s'), '%s',"
                "(SELECT id FROM GROUPS WHERE GROUP_TAG='%s'), '%s'  )",
                temp.id,temp.name, temp.phonenum,
                temp.email, temp.sex, temp.birth,
                temp.group, temp.memo);
    dbQueryExec(db,err_msg, sql);
    printf("해당 데이터 삽입이 완료되었습니다. : ");
    printStudent(temp);

    noneTypeInput();
}

void updateData(sqlite3* db, char* err_msg)
{
    //데이터 변경 을 위한 SQL 쿼리문 생성 함수
    printf("주의 : 해당 작업은 id를 통해서만 조회 후 수정이 가능합니다. 검색 기능으로 id 조회 후 이용해주세요.\n");
    char* printText = "데이터 수정\n"
                      "1. 되돌아가기\n"
                      "2. 이름 수정\n"
                      "3. 연락처 수정\n"
                      "4. 이메일 수정\n"
                      "5. 성별 수정\n"
                      "6. 생년월일 수정\n"
                      "7. 그룹 수정\n"
                      "8. 메모 변경\n"
                      "선택: ";
    int menuNum = 8;
    int choice = inputChecker_Menu(printText, menuNum), inputData_Num;
    char* inputData;
    char sql[200];
    Student students[100];
    int idx;

    inputData_Num = inputChecker_Num(returnInputString("id"));
    sprintf(sql,"SELECT * FROM STUDENT WHERE id=%d",inputData_Num);
    idx = dbSearch(db, err_msg, sql, students);
    if(idx<=0) return;

    switch(choice)
    {
        case 1:
            return;
        case 2:
            printf("해당 데이터의 이름을 수정합니다.\n");
            inputData = inputChecker_Char(returnInputString("NAME"),NAME_LIMIT);
            sprintf(sql,"UPDATE STUDENT SET NAME = '%s' WHERE id=%d",inputData, inputData_Num);
            break;
        case 3:
            printf("해당 데이터의 연락처를 수정합니다.\n");
            inputData = inputChecker_Phone(returnInputString("PhoneNUM"),PHONE_LIMIT);
            sprintf(sql,"UPDATE STUDENT SET PhoneNUM = '%s' WHERE id=%d",inputData, inputData_Num);
            break;
        case 4:
            printf("해당 데이터의 이메일을 수정합니다.\n");
            inputData = inputChecker_Char(returnInputString("EMAIL"),EMAIL_LIMIT);
            sprintf(sql,"UPDATE STUDENT SET EMAIL = '%s' WHERE id=%d",inputData, inputData_Num);
            break;
        case 5:
            printf("해당 데이터의 성별을 수정합니다.\n");
            inputData = inputChecker_Sex(returnInputString("SEX"),SEX_LIMIT);
            sprintf(sql,"UPDATE STUDENT SET sex_id = (SELECT id FROM SEX WHERE GEN_NAME = '%s') WHERE id=%d",inputData, inputData_Num);
            break;
        case 6:
            printf("해당 데이터의 생년월일을 수정합니다.\n");
            inputData = inputChecker_Birth(returnInputString("BIRTH"),BIRTH_LIMIT);
            sprintf(sql,"UPDATE STUDENT SET BIRTH = '%s' WHERE id=%d",inputData, inputData_Num);
            break;
        case 7:
            printf("해당 데이터의 그룹명을 수정합니다.\n");
            inputData = inputChecker_Group(db, err_msg, returnInputString("GROUP"),GROUP_LIMIT,1);
            sprintf(sql,"UPDATE STUDENT SET group_id = (SELECT id FROM GROUPS WHERE GROUP_TAG = '%s') WHERE id=%d",inputData, inputData_Num);
            break;
        case 8:
            printf("해당 데이터의 메모를 수정합니다.\n");
            inputData = inputChecker_Char(returnInputString("MEMO"),MEMO_LIMIT);
            sprintf(sql,"UPDATE STUDENT SET MEMO = '%s' WHERE id=%d",inputData, inputData_Num);
            break;
    }
    dbQueryExec(db, err_msg, sql);
    noneTypeInput();
}

void searchData(sqlite3* db, char* err_msg)
{
    //데이터 검색 함수
    char* printText = "데이터 검색\n"
                      "1. 되돌아가기\n"
                      "2. id로 조회\n"
                      "3. 이름으로 조회\n"
                      "4. 연락처로 조회\n"
                      "5. 이메일로 조회\n"
                      "6. 성별로 조회\n"
                      "7. 생년월일로 조회\n"
                      "8. 그룹명으로 조회\n"
                      "9. 전체 데이터 조회\n"
                      "선택: ";
    int menuNum = 9;
    Student students[100];
    char sql[200];

    strcpy(sql,returnChoiceSQL(db, err_msg, inputChecker_Menu(printText, menuNum)));

    if(strcmp(sql,"")==0)
        return;

    int idx = dbSearch(db, err_msg, sql, students);
    if(idx > 0)
        noneTypeInput();
}

void deleteData(sqlite3* db, char* err_msg)
{
    //데이터 삭제 함수
    char* printText = "데이터 삭제 (주의:다시 복구할 수 없는 작업입니다)\n"
                      "1. 되돌아가기\n"
                      "2. id로 조회 및 제거\n"
                      "3. 이름으로 조회 및 제거\n"
                      "4. 연락처로 조회 및 제거\n"
                      "5. 이메일로 조회 및 제거\n"
                      "6. 성별로 조회 및 제거\n"
                      "7. 생년월일로 조회 및 제거\n"
                      "8. 그룹으로 조회 및 제거\n"
                      "9. 모든 데이터 제거\n"
                      "선택: ";
    int menuNum = 9;
    Student students[100];
    char sql[200];

    strcpy(sql,returnChoiceSQL(db, err_msg, inputChecker_Menu(printText, menuNum)));

    if(strcmp(sql,"")==0)
        return;

    int idx = dbSearch(db, err_msg, sql ,students);
    if(idx == -1 || idx == 0) return;
    while(1) {
        if (strcmp(inputChecker_yn("위 데이터를 삭제할까요?(되돌릴 수 없습니다)[y/n] ", 1), "y") != 0) {
            printf("데이터 삭제를 중지합니다.\n");
            return;
        }
        else
            break;
    }
    for(int i=0;i<idx;i++) {
        sprintf(sql, "DELETE FROM STUDENT WHERE id = %d",students[i].id);
        dbQueryExec(db, err_msg, sql);
        printf("대상 : ");
        printStudent(students[i]);
        printf("해당 데이터를 삭제하였습니다.\n");

    }

    noneTypeInput();
}

//(특수)테이블 전체초기화 전용 함수
sqlite3* enterAdminMODE(sqlite3* db, char* err_msg)
{
    //관리자 권한 함수
    //현재 기능 : 관리자모드 시 자동으로 STUDENT테이블 초기화
    char admin[100];
    scanf("%s",admin);
    getchar();
    if(strcmp(admin,ADMIN_CODE)==0){
        if (sqlite3_open(DB_FILE, &db) != SQLITE_OK)
        {
            printf("Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_free(err_msg);
            sqlite3_close(db);
            exit(0);
        }
        dbCreateTable(db, err_msg);
    }
    return db;
}

int main()
{

    int choice;
    sqlite3 *db;
    char* err_msg=0;

    //db = enterAdminMODE(db, err_msg); //테이블 전체초기화가 필요할 시 활성화 및 실행

    db = dbOpen(db, err_msg);

    while (1) {

        choice = inputChecker_Menu("\n주소록 관리 프로그램\n"
                                   "메인 메뉴:\n"
                                   "1. 데이터 삽입\n"
                                   "2. 데이터 삭제\n"
                                   "3. 데이터 수정\n"
                                   "4. 데이터 검색\n"
                                   "5. 그룹 메뉴\n"
                                   "6. 프로그램 종료\n"
                                   "선택: ",6);

        printf("\n");
        printLine();
        switch (choice) {
            case 1:
                insertData(db, err_msg);
                break;
            case 2:
                deleteData(db, err_msg);
                break;
            case 3:
                updateData(db, err_msg);
                break;
            case 4:
                searchData(db, err_msg);
                break;
            case 5:
                groupMenu(db, err_msg);
                break;
            case 6:
                printf("프로그램을 종료합니다.\n");
                sqlite3_free(err_msg);
                sqlite3_close(db);
                return 0;
        }
    }
}
