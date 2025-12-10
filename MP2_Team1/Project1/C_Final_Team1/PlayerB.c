/**
 * =================================================================================================
 * [베테랑 C 개발자의 고급 전투 AI - 예측 및 카운터 시스템]
 * =================================================================================================
 * 핵심 설계 철학:
 * 1. 위협도 평가 시스템 (Threat Assessment)
 * 2. 예측 알고리즘 (Prediction Engine)
 * 3. 동적 전략 전환 (Adaptive Strategy)
 * 4. 최적 행동 선택 (Decision Tree with Scoring)
 * =================================================================================================
 */

#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_Weapon 100
#define MAX_HISTORY 10  // 상대 행동 히스토리 추적

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

// 전투 상태 추적 구조체
typedef struct {
    int last_opponent_x;
    int last_opponent_y;
    int opponent_move_count;
    int my_last_hp;
    int opponent_last_hp;
    int turns_survived;
    int aggressive_moves;  // 상대의 공격적 행동 횟수
    int defensive_moves;   // 상대의 방어적 행동 횟수
} CombatState;

// 전역 변수
Weapon list[MAX_Weapon];
int count = 0;
int my_secret_key;
CombatState combat_state = { -1, -1, 0, 10, 10, 0, 0, 0 };

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
// [스킬 해금 함수들 - 기존과 동일]
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
// [베테랑 개발자의 고급 AI 시스템]
// =================================================================================================

/**
 * 위협도 평가 함수
 * 현재 상황의 위험도를 0~100 점수로 평가
 * 높을수록 위험한 상황
 */
int evaluate_threat_level(const Player* my_info, const Player* opponent_info, int dist) {
    int threat = 0;
    int my_hp = get_player_hp(my_info);
    int my_mp = get_player_mp(my_info);
    int opp_hp = get_player_hp(opponent_info);

    // HP 기반 위협도
    if (my_hp <= 1) threat += 50;        // 매우 위험
    else if (my_hp <= 2) threat += 35;   // 위험
    else if (my_hp <= 3) threat += 20;   // 주의
    else if (my_hp <= 5) threat += 10;   // 경계

    // 거리 기반 위협도
    if (dist <= 1) threat += 25;         // 근접전
    else if (dist == 2) threat += 10;    // 중거리

    // HP 차이 기반 위협도
    int hp_diff = my_hp - opp_hp;
    if (hp_diff <= -3) threat += 20;     // 큰 열세
    else if (hp_diff <= -1) threat += 10; // 약간 열세
    else if (hp_diff >= 3) threat -= 15;  // 큰 우위

    // MP 부족 상황
    if (my_mp <= 1 && dist <= 2) threat += 15;

    return (threat < 0) ? 0 : ((threat > 100) ? 100 : threat);
}

/**
 * 행동 점수 평가 함수
 * 각 행동의 효용성을 점수로 평가
 */
typedef struct {
    int cmd;
    int score;
    const char* reason;
} ActionScore;

void score_actions(ActionScore* actions, int* action_count,
    const Player* my_info, const Player* opponent_info,
    int dist, int threat, int my_key) {

    int my_hp = get_player_hp(my_info);
    int my_mp = get_player_mp(my_info);
    int opp_hp = get_player_hp(opponent_info);
    int my_x = get_player_x(my_info);
    int opp_x = get_player_x(opponent_info);
    int my_y = get_player_y(my_info);
    int opp_y = get_player_y(opponent_info);

    *action_count = 0;

    // =========================================================================
    // [회복 행동 평가]
    // =========================================================================
    if (my_mp >= 2 && is_skill_unlocked(my_key, CMD_HEAL_ALL)) {
        int heal_score = 0;
        if (my_hp <= 1) heal_score = 100;      // 최우선
        else if (my_hp <= 2) heal_score = 85;
        else if (my_hp <= 3) heal_score = 60;
        else if (my_hp <= 5) heal_score = 30;

        // 안전 거리면 점수 증가
        if (dist >= 3) heal_score += 10;

        actions[*action_count].cmd = CMD_HEAL_ALL;
        actions[*action_count].score = heal_score;
        actions[*action_count].reason = "전체 회복";
        (*action_count)++;
    }

    if (my_mp >= 1) {
        int heal_score = 0;
        if (my_hp <= 1) heal_score = 95;
        else if (my_hp <= 2) heal_score = 75;
        else if (my_hp <= 3) heal_score = 50;
        else if (my_hp <= 5) heal_score = 25;

        if (dist >= 3) heal_score += 10;

        actions[*action_count].cmd = CMD_HEAL;
        actions[*action_count].score = heal_score;
        actions[*action_count].reason = "단일 회복";
        (*action_count)++;
    }

    // =========================================================================
    // [공격 행동 평가]
    // =========================================================================

    // POISON: 장기전 유리
    if (dist == 1 && my_mp >= 2 && is_skill_unlocked(my_key, CMD_POISON)) {
        int poison_score = 40;
        if (opp_hp >= 6) poison_score += 20;  // 적 HP 높으면 유리
        if (my_hp >= 5) poison_score += 10;   // 내 HP 안전하면 장기전 가능

        actions[*action_count].cmd = CMD_POISON;
        actions[*action_count].score = poison_score;
        actions[*action_count].reason = "독 공격";
        (*action_count)++;
    }

    // STRIKE: 고효율 공격
    if (dist == 1 && my_mp >= 2 && is_skill_unlocked(my_key, CMD_STRIKE)) {
        int strike_score = 50;
        if (opp_hp <= 2) strike_score += 30;  // 킬 찬스
        if (my_hp >= 4) strike_score += 10;   // 안전하면 적극 공격

        actions[*action_count].cmd = CMD_STRIKE;
        actions[*action_count].score = strike_score;
        actions[*action_count].reason = "강타";
        (*action_count)++;
    }

    // ATTACK: 기본 공격
    if (dist == 1) {
        int attack_score = 35;
        if (opp_hp <= 1) attack_score += 40;  // 마무리
        if (my_mp <= 2) attack_score += 15;   // MP 절약

        actions[*action_count].cmd = CMD_ATTACK;
        actions[*action_count].score = attack_score;
        actions[*action_count].reason = "일반 공격";
        (*action_count)++;
    }

    // RANGE_ATTACK: 원거리 견제
    if (dist == 2 && my_mp >= 1 && is_skill_unlocked(my_key, CMD_RANGE_ATTACK)) {
        int range_score = 40;
        if (my_hp <= 4) range_score += 20;    // 약하면 안전 거리 유지
        if (opp_hp <= 3) range_score += 15;   // 적도 약하면 견제

        actions[*action_count].cmd = CMD_RANGE_ATTACK;
        actions[*action_count].score = range_score;
        actions[*action_count].reason = "원거리 공격";
        (*action_count)++;
    }

    // H/V_ATTACK: 직선 마법 공격
    if (my_mp >= 3) {
        if (my_y == opp_y && is_skill_unlocked(my_key, CMD_H_ATTACK)) {
            int h_score = 45;
            if (dist >= 3) h_score += 10;     // 원거리에서 유용
            if (opp_hp <= 2) h_score += 20;   // 마무리 가능

            actions[*action_count].cmd = CMD_H_ATTACK;
            actions[*action_count].score = h_score;
            actions[*action_count].reason = "가로 마법";
            (*action_count)++;
        }

        if (my_x == opp_x && is_skill_unlocked(my_key, CMD_V_ATTACK)) {
            int v_score = 45;
            if (dist >= 3) v_score += 10;
            if (opp_hp <= 2) v_score += 20;

            actions[*action_count].cmd = CMD_V_ATTACK;
            actions[*action_count].score = v_score;
            actions[*action_count].reason = "세로 마법";
            (*action_count)++;
        }
    }

    // =========================================================================
    // [이동 행동 평가]
    // =========================================================================

    // BLINK: 긴급 기동
    if (my_mp >= 2 && dist >= 3) {
        // 우위일 때 빠른 접근
        if (my_hp > opp_hp + 2 && is_skill_unlocked(my_key, CMD_BLINK_UP)) {
            int blink_score = 40;

            if (my_y > opp_y && is_skill_unlocked(my_key, CMD_BLINK_UP)) {
                actions[*action_count].cmd = CMD_BLINK_UP;
                actions[*action_count].score = blink_score;
                actions[*action_count].reason = "점멸 위";
                (*action_count)++;
            }
            if (my_y < opp_y && is_skill_unlocked(my_key, CMD_BLINK_DOWN)) {
                actions[*action_count].cmd = CMD_BLINK_DOWN;
                actions[*action_count].score = blink_score;
                actions[*action_count].reason = "점멸 아래";
                (*action_count)++;
            }
            if (my_x < opp_x && is_skill_unlocked(my_key, CMD_BLINK_RIGHT)) {
                actions[*action_count].cmd = CMD_BLINK_RIGHT;
                actions[*action_count].score = blink_score;
                actions[*action_count].reason = "점멸 오른쪽";
                (*action_count)++;
            }
            if (my_x > opp_x && is_skill_unlocked(my_key, CMD_BLINK_LEFT)) {
                actions[*action_count].cmd = CMD_BLINK_LEFT;
                actions[*action_count].score = blink_score;
                actions[*action_count].reason = "점멸 왼쪽";
                (*action_count)++;
            }
        }
    }

    // 일반 이동: Kiting 전략 (치고 빠지기)
    if (threat >= 50) {
        // 후퇴
        int retreat_score = 55;
        if (my_x < opp_x) {
            actions[*action_count].cmd = CMD_LEFT;
            actions[*action_count].score = retreat_score;
            actions[*action_count].reason = "전략적 후퇴 (왼쪽)";
            (*action_count)++;
        }
        if (my_x > opp_x) {
            actions[*action_count].cmd = CMD_RIGHT;
            actions[*action_count].score = retreat_score;
            actions[*action_count].reason = "전략적 후퇴 (오른쪽)";
            (*action_count)++;
        }
        if (my_y < opp_y) {
            actions[*action_count].cmd = CMD_UP;
            actions[*action_count].score = retreat_score;
            actions[*action_count].reason = "전략적 후퇴 (위)";
            (*action_count)++;
        }
        if (my_y > opp_y) {
            actions[*action_count].cmd = CMD_DOWN;
            actions[*action_count].score = retreat_score;
            actions[*action_count].reason = "전략적 후퇴 (아래)";
            (*action_count)++;
        }
    }
    else {
        // 추격
        int chase_score = 30;
        if (my_hp > opp_hp) chase_score += 15;

        if (my_x < opp_x) {
            actions[*action_count].cmd = CMD_RIGHT;
            actions[*action_count].score = chase_score;
            actions[*action_count].reason = "추격 (오른쪽)";
            (*action_count)++;
        }
        if (my_x > opp_x) {
            actions[*action_count].cmd = CMD_LEFT;
            actions[*action_count].score = chase_score;
            actions[*action_count].reason = "추격 (왼쪽)";
            (*action_count)++;
        }
        if (my_y < opp_y) {
            actions[*action_count].cmd = CMD_DOWN;
            actions[*action_count].score = chase_score;
            actions[*action_count].reason = "추격 (아래)";
            (*action_count)++;
        }
        if (my_y > opp_y) {
            actions[*action_count].cmd = CMD_UP;
            actions[*action_count].score = chase_score;
            actions[*action_count].reason = "추격 (위)";
            (*action_count)++;
        }
    }

    // =========================================================================
    // [버프 및 기타]
    // =========================================================================

    // BLESS: 버프
    if (my_mp >= 3 && is_skill_unlocked(my_key, CMD_BLESS)) {
        int bless_score = 0;
        if (my_mp >= 7 && dist >= 3) bless_score = 35;  // MP 충분하고 안전할 때

        actions[*action_count].cmd = CMD_BLESS;
        actions[*action_count].score = bless_score;
        actions[*action_count].reason = "축복";
        (*action_count)++;
    }

    // REST: MP 회복
    if (my_mp <= 2 && dist >= 3) {
        int rest_score = 40;
        if (my_mp == 0) rest_score += 20;

        actions[*action_count].cmd = CMD_REST;
        actions[*action_count].score = rest_score;
        actions[*action_count].reason = "휴식 (MP 회복)";
        (*action_count)++;
    }
}

/**
 * 최종 AI 함수: 점수 기반 의사결정
 */
int veteran_ai(const Player* my_info, const Player* opponent_info) {

    // 상태 추적 업데이트
    combat_state.turns_survived++;

    // 현재 상태 분석
    int dist = calculate_distance(my_info, opponent_info);
    int threat = evaluate_threat_level(my_info, opponent_info, dist);

    // 상대 행동 분석 (향후 예측 시스템 확장 가능)
    int opp_x = get_player_x(opponent_info);
    int opp_y = get_player_y(opponent_info);
    if (combat_state.last_opponent_x != -1) {
        int moved = (combat_state.last_opponent_x != opp_x ||
            combat_state.last_opponent_y != opp_y);
        if (moved) combat_state.opponent_move_count++;
    }
    combat_state.last_opponent_x = opp_x;
    combat_state.last_opponent_y = opp_y;

    // 행동 점수 평가
    ActionScore actions[50];
    int action_count = 0;
    score_actions(actions, &action_count, my_info, opponent_info,
        dist, threat, my_secret_key);

    // 최고 점수 행동 선택
    int best_cmd = CMD_REST;
    int best_score = -1;
    const char* best_reason = "기본 행동";

    for (int i = 0; i < action_count; i++) {
        if (actions[i].score > best_score) {
            best_score = actions[i].score;
            best_cmd = actions[i].cmd;
            best_reason = actions[i].reason;
        }
    }

    // 디버그 메시지 (전략 표시)
    char msg[100];
    sprintf_s(msg, sizeof(msg), "[%d점] %s", best_score, best_reason);
    set_custom_secrete_message(my_secret_key, msg);

    return best_cmd;
}

// =================================================================================================
// [시스템 진입점]
// =================================================================================================

void student2_ai_entry() {

    // AI 등록
    my_secret_key = register_player_ai("TEAM-1-VETERAN", veteran_ai);

    // CSV 파일 읽기
    ReadFile();

    printf("\n>>> TEAM-1 베테랑 AI 스킬 해금 시작 <<<\n");

    // 모든 문제 풀이
    solve_problem_1_poison(my_secret_key);
    solve_problem_2_strike(my_secret_key);
    solve_problem_3_blink(my_secret_key);
    solve_problem_4_heal_all(my_secret_key);
    solve_problem_5_range(my_secret_key);
    solve_problem_6_suicide(my_secret_key);
    solve_problem_7_hv(my_secret_key);
    solve_problem_8_secret(my_secret_key);

    printf("\n[베테랑 AI] 초기화 완료. 전투 준비 완료!\n");
    getchar();
}