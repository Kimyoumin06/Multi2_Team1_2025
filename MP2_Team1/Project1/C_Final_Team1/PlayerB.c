/**
 * =================================================================================================
 * [토너먼트용 강화 전략 AI - PlayerB]
 * =================================================================================================
 * 핵심 설계 철학:
 * 1. MP 효율 관리 (Resource Management)
 * 2. 위기 상황 대응 (Crisis Management)
 * 3. 거리별 최적 공격 (Distance-based Attack)
 * 4. 점멸 기동 (Blink Mobility)
 * =================================================================================================
 */

#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_Weapon 100

 // 아이템 정보 구조체
typedef struct {
    int id;
    char name[50];
    char slot[10];
    int ATK;
    int DEF;
    int HP;
    char CURSE[50];
    char KEY_FRAG[50];
} Weapon;

// 전역 변수
Weapon list[MAX_Weapon];
int count = 0;
int my_secret_key;

static const char* TARGET_CSV = "game_puzzle_en.csv";

// =================================================================================================
// [공통 유틸리티 함수]
// =================================================================================================

static int calculate_distance(const Player* p1, const Player* p2) {
    int dx = abs(get_player_x(p1) - get_player_x(p2));
    int dy = abs(get_player_y(p1) - get_player_y(p2));
    return dx + dy;
}

Weapon* find_item_by_id(int id) {
    for (int i = 0; i < count; i++) {
        if (list[i].id == id) return &list[i];
    }
    return NULL;
}

void ReadFile(void) {
    FILE* fp;
    if (fopen_s(&fp, TARGET_CSV, "r") != 0 || fp == NULL) {
        printf("[ReadFile] 오류: CSV 파일을 찾을 수 없습니다. (%s)\n", TARGET_CSV);
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }

    count = 0;
    while (fgets(line, sizeof(line), fp) != NULL && count < MAX_Weapon) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) continue;

        char* context = NULL;
        char* token = strtok_s(line, ",", &context);
        if (!token) continue;
        list[count].id = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[count].name, sizeof(list[count].name), token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[count].slot, sizeof(list[count].slot), token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        list[count].ATK = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        list[count].DEF = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        list[count].HP = atoi(token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[count].CURSE, sizeof(list[count].CURSE), token);

        token = strtok_s(NULL, ",", &context);
        if (!token) continue;
        strcpy_s(list[count].KEY_FRAG, sizeof(list[count].KEY_FRAG), token);

        count++;
    }

    fclose(fp);
    printf("SYSTEM: [PlayerB] Loaded %d items from %s\n", count, TARGET_CSV);
}

// =================================================================================================
// [스킬 해금 함수들]
// =================================================================================================

void solve_problem_1_poison(int my_key) {
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
        if (strlen(result) > 0) strcat_s(result, sizeof(result), "|");
        strcat_s(result, sizeof(result), list[idx].name);
    }
    attempt_skill_unlock(my_key, CMD_POISON, result);
    if (is_skill_unlocked(my_key, CMD_POISON))
        printf("TEAM-1 : CMD_POISON 해금 완료\n");
    else
        printf("TEAM-1 : CMD_POISON 해금 실패 ㅜㅜ\n");
}

void solve_problem_2_strike(int my_key) {
    int total_index = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].slot, "W") == 0) {
            char* pos = strchr(list[i].KEY_FRAG, 'T');
            if (pos != NULL) {
                int index = (int)(pos - list[i].KEY_FRAG);
                total_index += index;
            }
        }
    }
    char result[50];
    sprintf_s(result, sizeof(result), "%dkey", total_index);
    attempt_skill_unlock(my_key, CMD_STRIKE, result);
    if (is_skill_unlocked(my_key, CMD_STRIKE))
        printf("TEAM-1 : CMD_STRIKE 해금 완료\n");
    else
        printf("TEAM-1 : CMD_STRIKE 해금 실패 ㅜㅜ\n");
}

void solve_problem_3_blink(int my_key) {
    char final_key[100] = "";
    Weapon* i202 = find_item_by_id(202);
    Weapon* i208 = find_item_by_id(208);
    int target_hp = (i202 && i208) ? (i202->DEF + i208->DEF) : 0;
    for (int i = 0; i < count; i++) {
        if (list[i].HP == target_hp && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            strcat_s(final_key, sizeof(final_key), list[i].KEY_FRAG);
            break;
        }
    }
    Weapon* i205 = find_item_by_id(205);
    Weapon* i212 = find_item_by_id(212);
    int target_atk = (i205 && i212) ? (i205->ATK * i212->ATK) : 0;
    int last_match_idx = -1;
    for (int i = 0; i < count; i++) {
        if (list[i].ATK == target_atk && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            last_match_idx = i;
        }
    }
    if (last_match_idx != -1) strcat_s(final_key, sizeof(final_key), list[last_match_idx].KEY_FRAG);
    int last_curse_idx = -1;
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].CURSE, "C_01") && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            last_curse_idx = i;
        }
    }
    if (last_curse_idx != -1) strcat_s(final_key, sizeof(final_key), list[last_curse_idx].KEY_FRAG);
    for (int i = 0; i < count; i++) {
        if (list[i].name[0] == 'I' && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            strcat_s(final_key, sizeof(final_key), list[i].KEY_FRAG);
            break;
        }
    }
    attempt_skill_unlock(my_key, CMD_BLINK_UP, final_key);
    if (is_skill_unlocked(my_key, CMD_BLINK_UP))
        printf("TEAM-1 : CMD_BLINK 해금 완료\n");
    else
        printf("TEAM-1 : CMD_BLINK 해금 실패 ㅜㅜ\n");
}

void solve_problem_4_heal_all(int my_key) {
    char final_key[100] = "";
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].name, list[i].slot) >= 0) {
            if (strcmp(list[i].KEY_FRAG, "NIL") != 0) {
                strcpy_s(final_key, sizeof(final_key), list[i].KEY_FRAG);
                break;
            }
        }
    }
    attempt_skill_unlock(my_key, CMD_HEAL_ALL, final_key);
    if (is_skill_unlocked(my_key, CMD_HEAL_ALL))
        printf("TEAM-1 : CMD_HEAL_ALL 해금 완료\n");
    else
        printf("TEAM-1 : CMD_HEAL_ALL 해금 실패 ㅜㅜ\n");
}

void solve_problem_5_range(int my_key) {
    char final_key[100] = "";
    int N = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].KEY_FRAG, "K") != NULL) {
            N = list[i].HP;
            break;
        }
    }
    if (N > 0) {
        FILE* fp = NULL;
        if (fopen_s(&fp, TARGET_CSV, "rb") == 0 && fp != NULL) {
            if (fseek(fp, (long)(N - 1), SEEK_SET) == 0) {
                char t[6] = { 0 };
                size_t r = fread(t, 1, 5, fp);
                t[r] = '\0';
                sprintf_s(final_key, sizeof(final_key), "\"%s\"", t);
            }
            fclose(fp);
        }
    }
    attempt_skill_unlock(my_key, CMD_RANGE_ATTACK, final_key);
    if (is_skill_unlocked(my_key, CMD_RANGE_ATTACK))
        printf("TEAM-1 : CMD_RANGE_ATTACK 해금 완료\n");
    else
        printf("TEAM-1 : CMD_RANGE_ATTACK 해금 실패 ㅜㅜ\n");
}

void solve_problem_6_suicide(int my_key) {
    char final_key[100] = "";
    char combined[2000] = { 0 };
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].name, "Sword") != NULL) {
            strcat_s(combined, sizeof(combined), list[i].KEY_FRAG);
        }
    }
    char temp[2000];
    strcpy_s(temp, sizeof(temp), combined);
    char* next_token = NULL;
    char* tok = strtok_s(temp, "*", &next_token);
    char* best = NULL;
    int max_len = -1;
    while (tok) {
        int len = (int)strlen(tok);
        if (len > max_len) {
            max_len = len;
            best = tok;
        }
        tok = strtok_s(NULL, "*", &next_token);
    }
    if (best) strcpy_s(final_key, sizeof(final_key), best);
    attempt_skill_unlock(my_key, CMD_BLESS, final_key);
    if (is_skill_unlocked(my_key, CMD_BLESS))
        printf("TEAM-1 : CMD_BLESS 해금 완료\n");
    else
        printf("TEAM-1 : CMD_BLESS 해금 실패 ㅜㅜ\n");
}

void solve_problem_7_hv(int my_key) {
    char final_key[16] = "";
    char bestName[50] = "";
    char bestCurse[50] = "";
    int maxNameLen = -1;
    int minCurseLen = 9999;
    for (int i = 0; i < count; i++) {
        const char* name = list[i].name;
        const char* curse = list[i].CURSE;
        if (name[0] != '\0') {
            int len = (int)strlen(name);
            if (len > maxNameLen) {
                maxNameLen = len;
                strcpy_s(bestName, sizeof(bestName), name);
            }
        }
        if (curse[0] != '\0') {
            int len = (int)strlen(curse);
            if (len < minCurseLen) {
                minCurseLen = len;
                strcpy_s(bestCurse, sizeof(bestCurse), curse);
            }
        }
    }
    if (bestName[0] == '\0' || bestCurse[0] == '\0') return;
    char head[4] = { 0 };
    strncpy_s(head, sizeof(head), bestName, 3);
    char tail[4] = { 0 };
    int clen = (int)strlen(bestCurse);
    if (clen >= 3) {
        char* start = (char*)bestCurse + (clen - 3);
        strncpy_s(tail, sizeof(tail), start, 3);
    }
    else {
        strncpy_s(tail, sizeof(tail), bestCurse, sizeof(tail) - 1);
    }
    sprintf_s(final_key, sizeof(final_key), "%s%s", head, tail);
    attempt_skill_unlock(my_key, CMD_H_ATTACK, final_key);
    attempt_skill_unlock(my_key, CMD_V_ATTACK, final_key);
    if (is_skill_unlocked(my_key, CMD_H_ATTACK))
        printf("TEAM-1 : CMD_H_ATTACK/CMD_V_ATTACK 해금 완료\n");
    else
        printf("TEAM-1 : H/V 공격 해금 실패 ㅜㅜ\n");
}

void solve_problem_8_secret(int my_key) {
    char final_key[32] = "";
    char targetName[50] = "";
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].name, "Stone") != NULL) {
            strcpy_s(targetName, sizeof(targetName), list[i].name);
            break;
        }
    }
    if (targetName[0] == '\0') return;
    char buf[50];
    strcpy_s(buf, sizeof(buf), targetName);
    const char* delims = "AEIOUaeiou";
    char* context_vowels = NULL;
    char* tok = strtok_s(buf, delims, &context_vowels);
    char bestToken[50] = "";
    size_t bestLen = 0;
    while (tok != NULL) {
        size_t len = strlen(tok);
        if (len > bestLen) {
            bestLen = len;
            strcpy_s(bestToken, sizeof(bestToken), tok);
        }
        tok = strtok_s(NULL, delims, &context_vowels);
    }
    if (bestToken[0] == '\0') return;
    strncpy_s(final_key, sizeof(final_key), bestToken, _TRUNCATE);
    attempt_skill_unlock(my_key, CMD_SECRETE, final_key);
    if (is_skill_unlocked(my_key, CMD_SECRETE))
        printf("TEAM-1 : CMD_SECRETE 해금 완료\n");
    else
        printf("TEAM-1 : CMD_SECRETE 해금 실패 ㅜㅜ\n");
}

// =================================================================================================
// [토너먼트용 강화 AI 로직]
// =================================================================================================

int player_b_strategy(const Player* my_info, const Player* opponent_info)
{
    int dist = calculate_distance(my_info, opponent_info);
    int mp = get_player_mp(my_info);
    int hp = get_player_hp(my_info);
    int opp_hp = get_player_hp(opponent_info);
    int my_x = get_player_x(my_info);
    int my_y = get_player_y(my_info);
    int opp_x = get_player_x(opponent_info);
    int opp_y = get_player_y(opponent_info);
    int my_key = my_secret_key;

    int dx = opp_x - my_x;
    int dy = opp_y - my_y;

    // =========================
    // 0. MP 없고 멀리 있으면 휴식으로 MP 땡기기
    // =========================
    if (mp <= 1 && dist > 1) {
        // 휴식: MP 2 회복 (패치된 룰 기준 이득 큼)
        set_custom_secrete_message(my_key, "MP 충전");
        return CMD_REST;
    }

    // =========================
    // 1. 위기 관리 (체력 관리)
    // =========================

    // 완전 위험: HP 1 이하 → 무조건 회복
    if (hp <= 1 && mp >= 1) {
        // MP 충분하면 HEAL_ALL로 크게 회복
        if (mp >= 3 && is_skill_unlocked(my_key, CMD_HEAL_ALL)) {
            set_custom_secrete_message(my_key, "긴급 전체회복!");
            return CMD_HEAL_ALL;
        }
        // 아니면 그냥 기본 회복
        set_custom_secrete_message(my_key, "긴급 회복!");
        return CMD_HEAL;
    }

    // 중위험: HP 2~3인데 상대 체력이 더 높고, MP 여유 많으면 선제 HEAL_ALL
    if (hp <= 3 && hp < opp_hp && mp >= 4 && is_skill_unlocked(my_key, CMD_HEAL_ALL)) {
        set_custom_secrete_message(my_key, "전략적 회복");
        return CMD_HEAL_ALL;
    }

    // =========================
    // 2. 근접 전투 (거리 1 이하)
    // =========================
    if (dist <= 1) {
        // (1) 강타: 2 데미지, 효율 좋음 → 최우선
        if (mp >= 2 && is_skill_unlocked(my_key, CMD_STRIKE)) {
            set_custom_secrete_message(my_key, "강타!");
            return CMD_STRIKE;
        }

        // (2) 독: 효율은 좀 안 좋지만, 내가 피/MP 여유 있고 상대 HP가 높을 때 한 번 써볼만
        if (mp >= 5 && is_skill_unlocked(my_key, CMD_POISON) && hp > 3 && opp_hp >= 4) {
            set_custom_secrete_message(my_key, "독 공격");
            return CMD_POISON;
        }

        // (3) 그냥 기본 공격
        set_custom_secrete_message(my_key, "일반 공격");
        return CMD_ATTACK;
    }

    // =========================
    // 3. 거리 2 → 원거리 공격
    // =========================
    if (dist == 2 && mp >= 1 && is_skill_unlocked(my_key, CMD_RANGE_ATTACK)) {
        set_custom_secrete_message(my_key, "원거리 공격");
        return CMD_RANGE_ATTACK;
    }

    // =========================
    // 4. 가로/세로 광역 공격 (라인 맞으면 박기)
    // =========================
    if (mp >= 3 && is_skill_unlocked(my_key, CMD_H_ATTACK)) {
        // 너무 가까울 때는 STRIKE가 더 좋은데,
        // 여기까지 왔다는 건 dist > 1이라 STRIKE 사정거리가 아님
        if (my_y == opp_y) {
            // 같은 줄이면 가로 공격
            set_custom_secrete_message(my_key, "가로 마법!");
            return CMD_H_ATTACK;
        }
        if (my_x == opp_x && is_skill_unlocked(my_key, CMD_V_ATTACK)) {
            // 같은 열이면 세로 공격 (열려있을 때만)
            set_custom_secrete_message(my_key, "세로 마법!");
            return CMD_V_ATTACK;
        }
    }

    // =========================
    // 5. 멀리서 점멸(BLINK)로 접근
    // =========================
    if (dist >= 3 && mp >= 1 && is_skill_unlocked(my_key, CMD_BLINK_UP)) {
        // x, y 중 차이가 더 큰 방향으로 점멸
        if (abs(dx) >= abs(dy)) {
            if (dx > 0 && is_skill_unlocked(my_key, CMD_BLINK_RIGHT)) {
                set_custom_secrete_message(my_key, "점멸 돌진 →");
                return CMD_BLINK_RIGHT;
            }
            else if (dx < 0 && is_skill_unlocked(my_key, CMD_BLINK_LEFT)) {
                set_custom_secrete_message(my_key, "점멸 돌진 ←");
                return CMD_BLINK_LEFT;
            }
        }
        else {
            if (dy > 0 && is_skill_unlocked(my_key, CMD_BLINK_DOWN)) {
                set_custom_secrete_message(my_key, "점멸 돌진 ↓");
                return CMD_BLINK_DOWN;
            }
            else if (dy < 0 && is_skill_unlocked(my_key, CMD_BLINK_UP)) {
                set_custom_secrete_message(my_key, "점멸 돌진 ↑");
                return CMD_BLINK_UP;
            }
        }
    }

    // =========================
    // 6. 그 외 : 기본 추격 로직
    // =========================
    set_custom_secrete_message(my_key, "추격");
    if (my_x < opp_x) return CMD_RIGHT;
    if (my_x > opp_x) return CMD_LEFT;
    if (my_y < opp_y) return CMD_DOWN;
    if (my_y > opp_y) return CMD_UP;

    // 혹시라도 완전 정지 상태면 쉰다
    set_custom_secrete_message(my_key, "대기");
    return CMD_REST;
}

// =================================================================================================
// [시스템 진입점]
// =================================================================================================

void student2_ai_entry() {

    // AI 등록
    my_secret_key = register_player_ai("TEAM-1-TOURNAMENT", player_b_strategy);

    // CSV 파일 읽기
    ReadFile();

    printf("\n>>> TEAM-1 토너먼트 AI 스킬 해금 시작 <<<\n");

    // 모든 문제 풀이
    solve_problem_1_poison(my_secret_key);
    solve_problem_2_strike(my_secret_key);
    solve_problem_3_blink(my_secret_key);
    solve_problem_4_heal_all(my_secret_key);
    solve_problem_5_range(my_secret_key);
    solve_problem_6_suicide(my_secret_key);
    solve_problem_7_hv(my_secret_key);
    solve_problem_8_secret(my_secret_key);

    printf("\n[토너먼트 AI] 초기화 완료. 전투 준비 완료!\n");
    getchar();
}