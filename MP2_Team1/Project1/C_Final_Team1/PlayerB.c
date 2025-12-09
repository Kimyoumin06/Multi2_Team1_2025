#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_Weapon 100

// 아이템 정보를 저장할 구조체 (PlayerB.c 원본 사용)
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

// PlayerB.c 원본의 거리 계산 함수
static int calculate_distance(const Player* p1, const Player* p2) {
    int dx = abs(get_player_x(p1) - get_player_x(p2));
    int dy = abs(get_player_y(p1) - get_player_y(p2));
    return dx + dy;
}

// PlayerB.c 원본의 간단한 AI 함수
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

// PlayerB.c 원본의 CSV 파일 읽기 함수
int ReadFile(Weapon list[], int* count) {
    FILE* fp;
    *count = 0;

    if (fopen_s(&fp, "game_puzzle_en.csv", "r") != 0 || fp == NULL) {
        printf("[ReadFile] 오류: AI1-2_C_Final.csv 파일을 찾을 수 없습니다.\n");
        return 0;
    }

    char line[256];
    // 헤더 건너뛰기
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

// ID로 아이템을 찾아주는 도우미 함수 (20251413.txt 기반)
Weapon* find_item_by_id(int id, Weapon list[], int count) {
    for (int i = 0; i < count; i++) {
        if (list[i].id == id) return &list[i];
    }
    return NULL;
}


// P2 등록 함수 (TEAM-BRAVO의 문제 풀이 로직 통합)
void student2_ai_entry() {

    // 1. 내 캐릭터 등록 (PlayerB.c 원본은 "TEAM-1"이었으나, 20251413.txt에 따라 "TEAM-BRAVO"로 변경)
    int my_secret_key = register_player_ai("TEAM-BRAVO", simple_killer_ai2);

    // 2. CSV 파일 읽기
    Weapon list[MAX_Weapon];
    int count = 0;

    if (!ReadFile(list, &count)) {
        printf("TEAM-BRAVO : CSV 파일 읽기 실패\n");
        return;
    }

    // ------------------------------------------------------------------
    // [COMMAND UNLOCK SECTION] - 문제 1, 2, 3, 4 통합
    // ------------------------------------------------------------------

    // ******************************************************************
    // PlayerB.c 원본 - 1번 문제: CMD_POISON 해금 (20251402 김유민)
    // ******************************************************************
    int filtered_indices_1[MAX_Weapon];
    int filtered_count_1 = 0;

    for (int i = 0; i < count; i++) {
        if (list[i].ATK >= 4 && list[i].DEF <= 5 && list[i].HP <= 100) {
            filtered_indices_1[filtered_count_1++] = i;
        }
    }

    char result_1[500] = "";
    for (int i = filtered_count_1 - 1; i >= 0; i--) {
        int idx = filtered_indices_1[i];
        if (strlen(result_1) > 0) {
            strcat_s(result_1, sizeof(result_1), "|");
        }
        strcat_s(result_1, sizeof(result_1), list[idx].name);
    }

    attempt_skill_unlock(my_secret_key, CMD_POISON, result_1);
    if (is_skill_unlocked(my_secret_key, CMD_POISON))
        printf("TEAM-1 : CMD_POISON 해금 완료\n");
    else
        printf("TEAM-1 : CMD_POISON 해금 실패 ㅜㅜ\n");

    // ******************************************************************
    // PlayerB.c 원본 - 2번 문제: CMD_STRIKE 해금 (20251402 김유민)
    // ******************************************************************
    int total_index = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].slot, "W") == 0) {
            char* pos = strchr(list[i].KEY_FRAG, 'T');

            if (pos != NULL) {
                int index = (int)(pos - list[i].KEY_FRAG);
                total_index += index;
            }
            else {
                total_index += 0;
            }
        }
    }

    char result_2[50];
    sprintf_s(result_2, sizeof(result_2), "%d", total_index);
    strcat_s(result_2, sizeof(result_2), "key");

    attempt_skill_unlock(my_secret_key, CMD_STRIKE, result_2);
    if (is_skill_unlocked(my_secret_key, CMD_STRIKE))
        printf("TEAM-1 : CMD_STRIKE 해금 완료\n");
    else
        printf("TEAM-1 : CMD_STRIKE 해금 실패 ㅜㅜ\n");

    // ******************************************************************
    // 20251413.txt - 3번 문제: CMD_BLINK_UP (점멸) 해금 (*A**C**F**T* 만들기)
    // ******************************************************************
    char final_key_3[100] = "";

    // 조건 1: (202번 방어력 + 208번 방어력)과 같은 HP를 가진 아이템 -> 'A'
    Weapon* i202 = find_item_by_id(202, list, count);
    Weapon* i208 = find_item_by_id(208, list, count);
    int target_hp = (i202 && i208) ? (i202->DEF + i208->DEF) : 0;
    for (int i = 0; i < count; i++) {
        if (list[i].HP == target_hp && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            strcat_s(final_key_3, sizeof(final_key_3), list[i].KEY_FRAG);
            break;
        }
    }

    // 조건 2: (205번 공격력 * 212번 공격력)과 같은 ATK를 가진 아이템 중 마지막 -> 'C'
    Weapon* i205 = find_item_by_id(205, list, count);
    Weapon* i212 = find_item_by_id(212, list, count);
    int target_atk = (i205 && i212) ? (i205->ATK * i212->ATK) : 0;
    int last_match_idx = -1;
    for (int i = 0; i < count; i++) {
        if (list[i].ATK == target_atk && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            last_match_idx = i;
        }
    }
    if (last_match_idx != -1) {
        strcat_s(final_key_3, sizeof(final_key_3), list[last_match_idx].KEY_FRAG);
    }

    // 조건 3: CURSE에 "C_01"이 포함된 아이템 중 마지막 -> 'F'
    int last_curse_idx = -1;
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].CURSE, "C_01") && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            last_curse_idx = i;
        }
    }
    if (last_curse_idx != -1) strcat_s(final_key_3, sizeof(final_key_3), list[last_curse_idx].KEY_FRAG);

    // 조건 4: 이름(NAME)이 'I'로 시작하는 아이템 중 처음 -> 'T'
    for (int i = 0; i < count; i++) {
        if (list[i].name[0] == 'I' && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            strcat_s(final_key_3, sizeof(final_key_3), list[i].KEY_FRAG);
            break;
        }
    }

    attempt_skill_unlock(my_secret_key, CMD_BLINK_UP, final_key_3);
    if (is_skill_unlocked(my_secret_key, CMD_BLINK_UP))
        printf("TEAM-1 : CMD_BLINK 해금 성공!\n");
    else
        printf("TEAM-1 : CMD_BLINK 해금 실패 ㅜㅜ\n");


    // ******************************************************************
    // 20251413.txt - 4번 문제: CMD_HEAL_ALL (회복2) 해금 (*H* 만들기)
    // ******************************************************************
    char final_key_4[100] = "";
    // 조건: 이름(NAME)이 슬롯(SLOT)보다 사전순으로 뒤거나 같은(>=0) 첫 번째 아이템 -> 'H'
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].name, list[i].slot) >= 0) {
            if (strcmp(list[i].KEY_FRAG, "NIL") != 0) {
                strcpy_s(final_key_4, sizeof(final_key_4), list[i].KEY_FRAG);
                break;
            }
        }
    }

    attempt_skill_unlock(my_secret_key, CMD_HEAL_ALL, final_key_4);
    if (is_skill_unlocked(my_secret_key, CMD_HEAL_ALL))
        printf("TEAM-1 : CMD_HEAL_ALL 해금 성공!\n");
    else
        printf("TEAM-1 : CMD_HEAL_ALL 해금 실패 ㅜㅜ\n");

    // ------------------------------------------------------------------

    printf("TEAM-1 : 플레이어 초기화 완료. 아무키나 누르시오.\n");
    // PlayerB.c와 동일하게 getchar() 호출
    getchar();
}