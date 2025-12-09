#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    char slot[10];
    int ATK;
    int DEF;
    int HP;
    char CURSE[20];
    char KEY_FRAG[20];
}Weapon;

#define MAX_Weapon 100

// 거리 계산 함수
static int calculate_distance(const Player* p1, const Player* p2) {
    int dx = abs(get_player_x(p1) - get_player_x(p2));
    int dy = abs(get_player_y(p1) - get_player_y(p2));
    return dx + dy;
}

// 간단한 AI 함수
int simple_killer_ai2(const Player* my_info, const Player* opponent_info) {
    int distance = calculate_distance(my_info, opponent_info);
    int my_x = get_player_x(my_info);
    int my_y = get_player_y(my_info);
    int opp_x = get_player_x(opponent_info);
    int opp_y = get_player_y(opponent_info);

    if (distance <= 1) {
        return CMD_ATTACK;
    }

    if (my_x != opp_x) {
        return (my_x < opp_x) ? CMD_RIGHT : CMD_LEFT;
    }

    if (my_y != opp_y) {
        return (my_y < opp_y) ? CMD_DOWN : CMD_UP;
    }

    return CMD_ATTACK;
}

// CSV 파일 읽기
int ReadFile(Weapon list[], int* count) {
    FILE* fp;
    *count = 0;

    if (fopen_s(&fp, "AI1-2_C_Final.csv", "r") != 0 || fp == NULL) {
        return 0;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL && *count < MAX_Weapon) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) continue;

        char* context = NULL;
        char* token = strtok_s(line, ",", &context);
        if (!token) continue;
        list[*count].id = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[*count].name, sizeof(list[*count].name), token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[*count].slot, sizeof(list[*count].slot), token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        list[*count].ATK = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        list[*count].DEF = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        list[*count].HP = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[*count].CURSE, sizeof(list[*count].CURSE), token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[*count].KEY_FRAG, sizeof(list[*count].KEY_FRAG), token);

        (*count)++;
    }

    fclose(fp);
    return 1;
}

// P2 등록 함수
void student2_ai_entry() {

    // PlayerA와 동일한 방식으로 AI 등록
    int my_secret_key = register_player_ai("TEAM-1", simple_killer_ai2);

    // CSV 파일 읽기
    Weapon list[MAX_Weapon];
    int count = 0;

    if (!ReadFile(list, &count)) {
        printf("TEAM-1 : CSV 파일 읽기 실패\n");
        return;
    }

    // ------------------------------------------------------------------
    // [COMMAND UNLOCK SECTION] - PlayerA.c와 동일한 구조
    // ------------------------------------------------------------------

    // 1번 문제: CMD_POISON 해금
    int filtered_indices[MAX_Weapon];
    int filtered_count = 0;

    for (int i = 0; i < count; i++) {
        if (list[i].ATK >= 4 && list[i].DEF <= 5 && list[i].HP <= 100) {
            filtered_indices[filtered_count++] = i;
        }
    }

    char result[500] = "";
    for (int i = filtered_count - 1; i >= 0; i--) {
        int idx = filtered_indices[i];
        if (strlen(result) > 0) {
            strcat_s(result, sizeof(result), "|");
        }
        strcat_s(result, sizeof(result), list[idx].name);
    }

    attempt_skill_unlock(my_secret_key, CMD_POISON, result);
    if (is_skill_unlocked(my_secret_key, CMD_POISON))
        printf("TEAM-1 : CMD_POISON 해금 완료\n");
    else
        printf("TEAM-1 : CMD_POISON 해금 실패 ㅜㅜ\n");

    // ------------------------------------------------------------------

    // 2번 문제: CMD_STRIKE 해금
    int total_index = 0;

    // SLOT이 "W"인 아이템 찾기
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].slot, "W") == 0) {
            // KEY_FRAG에서 'T' 찾기
            char* pos = strchr(list[i].KEY_FRAG, 'T');

            if (pos != NULL) {
                // 'T'가 있으면 인덱스 계산 (포인터 차이)
                int index = (int)(pos - list[i].KEY_FRAG);
                total_index += index;
            }
            else {
                // 'T'가 없으면 0 추가
                total_index += 0;
            }
        }
    }

    // 정수를 문자열로 변환 후 "key" 붙이기
    char result2[50];
    sprintf_s(result2, sizeof(result2), "%d", total_index);
    strcat_s(result2, sizeof(result2), "key");

    attempt_skill_unlock(my_secret_key, CMD_STRIKE, result2);
    if (is_skill_unlocked(my_secret_key, CMD_STRIKE))
        printf("TEAM-1 : CMD_STRIKE 해금 완료\n");
    else
        printf("TEAM-1 : CMD_STRIKE 해금 실패 ㅜㅜ\n");

    // ------------------------------------------------------------------

    printf("TEAM-1 : 플레이어 초기화 완료. 아무키나 누르시오.\n");

    // PlayerA.c와 동일하게 getchar() 호출
    getchar();
}