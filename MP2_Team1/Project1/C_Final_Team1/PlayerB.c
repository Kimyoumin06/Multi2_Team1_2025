// PlayerB.c
// [통합본] 
// 1. 전투 전략: Kiting Hybrid (거리 유지 + 위협도 분석)
// 2. 스킬 해금: CSV 파싱 및 알고리즘 풀이 (제공해주신 로직 적용)

#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// =================================================================================================
// [PART 1] 데이터 구조 및 CSV 파싱 (문제 풀이를 위한 기반)
// =================================================================================================

#define MAX_Weapon 1000
#define TARGET_CSV "game_puzzle_en.csv"

typedef struct {
    int id;
    char type[20];
    char name[50];
    int ATK;
    int DEF;
    int HP;
    char slot[20];
    char KEY_FRAG[50];
    char CURSE[50];
} Weapon;

Weapon list[MAX_Weapon];
int count = 0;

// CSV 파싱 헬퍼 (쉼표 분리)
void parse_csv_line(char* line, Weapon* out_w) {
    // 포맷: id,type,name,ATK,DEF,HP,slot,KEY_FRAG,CURSE
    // 간단한 파싱 구현 (strtok 등 사용 가능하나 안전하게 직접 파싱 권장)
    // 여기서는 sscanf나 strtok를 사용하여 간략히 처리합니다.

    char* token = NULL;
    char* next_token = NULL;
    char seps[] = ",";

    // ID
    token = strtok_s(line, seps, &next_token);
    if (token) out_w->id = atoi(token);

    // Type
    token = strtok_s(NULL, seps, &next_token);
    if (token) strcpy_s(out_w->type, sizeof(out_w->type), token);

    // Name
    token = strtok_s(NULL, seps, &next_token);
    if (token) strcpy_s(out_w->name, sizeof(out_w->name), token);

    // ATK
    token = strtok_s(NULL, seps, &next_token);
    if (token) out_w->ATK = atoi(token);

    // DEF
    token = strtok_s(NULL, seps, &next_token);
    if (token) out_w->DEF = atoi(token);

    // HP
    token = strtok_s(NULL, seps, &next_token);
    if (token) out_w->HP = atoi(token);

    // Slot
    token = strtok_s(NULL, seps, &next_token);
    if (token) strcpy_s(out_w->slot, sizeof(out_w->slot), token);

    // KEY_FRAG
    token = strtok_s(NULL, seps, &next_token);
    if (token) strcpy_s(out_w->KEY_FRAG, sizeof(out_w->KEY_FRAG), token);

    // CURSE (개행 제거 포함)
    token = strtok_s(NULL, seps, &next_token);
    if (token) {
        // 끝에 \n이 있을 수 있으므로 제거
        size_t len = strlen(token);
        if (len > 0 && token[len - 1] == '\n') token[len - 1] = '\0';
        strcpy_s(out_w->CURSE, sizeof(out_w->CURSE), token);
    }
}

void load_data() {
    FILE* fp = NULL;
    if (fopen_s(&fp, TARGET_CSV, "r") != 0 || fp == NULL) {
        printf("PlayerB: data.csv 파일을 열 수 없습니다.\n");
        return;
    }

    char line[512];
    // 첫 줄(헤더) 스킵
    fgets(line, sizeof(line), fp);

    count = 0;
    while (fgets(line, sizeof(line), fp) && count < MAX_Weapon) {
        parse_csv_line(line, &list[count]);
        count++;
    }
    fclose(fp);
    printf("PlayerB: 데이터 로드 완료 (%d개)\n", count);
}

Weapon* find_item_by_id(int id) {
    for (int i = 0; i < count; i++) {
        if (list[i].id == id) return &list[i];
    }
    return NULL;
}

// =================================================================================================
// [PART 2] 문제 풀이 함수들 (제공해주신 코드)
// =================================================================================================

// [문제 1] CMD_POISON 해금
void solve_problem_1_poison(int my_key) {
    int filtered_indices[MAX_Weapon];
    int filtered_count = 0;

    for (int i = 0; i < count; i++) {
        if (list[i].ATK >= 4 && list[i].DEF <= 5 && list[i].HP <= 100) {
            filtered_indices[filtered_count++] = i;
        }
    }

    char result[500] = "";
    // 파일에 등장하는 역순으로 정렬
    for (int i = filtered_count - 1; i >= 0; i--) {
        int idx = filtered_indices[i];
        if (strlen(result) > 0) {
            strcat_s(result, sizeof(result), "|");
        }
        strcat_s(result, sizeof(result), list[idx].name);
    }

    attempt_skill_unlock(my_key, CMD_POISON, result);
    if (is_skill_unlocked(my_key, CMD_POISON))
        printf("TEAM-1 : CMD_POISON 해금 완료\n");
    else
        printf("TEAM-1 : CMD_POISON 해금 실패\n");
}

// [문제 2] CMD_STRIKE 해금
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
    sprintf_s(result, sizeof(result), "%d", total_index);
    strcat_s(result, sizeof(result), "key");

    attempt_skill_unlock(my_key, CMD_STRIKE, result);
    if (is_skill_unlocked(my_key, CMD_STRIKE))
        printf("TEAM-1 : CMD_STRIKE 해금 완료\n");
    else
        printf("TEAM-1 : CMD_STRIKE 해금 실패\n");
}

// [문제 3] CMD_BLINK_UP 해금
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
    if (last_match_idx != -1) {
        strcat_s(final_key, sizeof(final_key), list[last_match_idx].KEY_FRAG);
    }

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
    // 다른 방향 블링크도 동일 키 사용
    attempt_skill_unlock(my_key, CMD_BLINK_DOWN, final_key);
    attempt_skill_unlock(my_key, CMD_BLINK_LEFT, final_key);
    attempt_skill_unlock(my_key, CMD_BLINK_RIGHT, final_key);

    if (is_skill_unlocked(my_key, CMD_BLINK_UP))
        printf("TEAM-1 : CMD_BLINK 해금 완료\n");
    else
        printf("TEAM-1 : CMD_BLINK 해금 실패\n");
}

// [문제 4] CMD_HEAL_ALL 해금
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
        printf("TEAM-1 : CMD_HEAL_ALL 해금 실패\n");
}

// [문제 5] CMD_RANGE_ATTACK 해금
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
        printf("TEAM-1 : CMD_RANGE_ATTACK 해금 실패\n");
}

// [문제 6] CMD_SUICIDE(BLESS) 해금
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
        printf("TEAM-1 : CMD_SUICIDE(BLESS) 해금 완료\n");
    else
        printf("TEAM-1 : CMD_SUICIDE(BLESS) 해금 실패\n");
}

// [문제 7] CMD_H_ATTACK / CMD_V_ATTACK 해금
void solve_problem_7_hv(int my_key) {
    char final_key[16] = "";
    char bestName[50] = "";
    char bestCurse[50] = "";
    int  maxNameLen = -1;
    int  minCurseLen = 9999;

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

    if (bestName[0] == '\0' || bestCurse[0] == '\0') {
        printf("TEAM-1 (문제 7) : 데이터 부족으로 해금 실패.\n");
        return;
    }

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
        printf("TEAM-1 : H/V 공격 해금 실패\n");
}

// [문제 8] CMD_SECRETE 해금 (선택사항)
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

    if (bestToken[0] != '\0') {
        strncpy_s(final_key, sizeof(final_key), bestToken, _TRUNCATE);
        attempt_skill_unlock(my_key, CMD_SECRETE, final_key);
    }
}


// =================================================================================================
// [PART 3] 전투 AI 로직 (Kiting Fixed Strategy)
// =================================================================================================

static int my_secret_key = 0;
#define ACTION_HISTORY 32
static int opp_history[ACTION_HISTORY];
static int opp_hist_size = 0;

#define FEAT_COUNT 7
#define ACTION_COUNT 10

static int CANDIDATE_ACTIONS[ACTION_COUNT] = {
    CMD_ATTACK, CMD_STRIKE, CMD_RANGE_ATTACK, CMD_HEAL, CMD_HEAL_ALL,
    CMD_V_ATTACK, CMD_H_ATTACK, CMD_POISON, CMD_BLESS, CMD_REST
};

static float WEIGHTS[ACTION_COUNT][FEAT_COUNT] = {
    {  0.0f, -0.2f, -0.2f, 0.0f, -1.0f, 0.0f, -0.4f }, // ATTACK
    {  0.0f,  1.1f, -1.2f, 0.0f, -2.0f, 0.0f, -0.6f }, // STRIKE
    {  0.0f,  0.9f, -0.8f, 0.0f, -0.8f, 0.1f,  0.2f }, // RANGE
    { -1.6f,  0.6f,  0.0f, 0.0f,  0.0f, 0.0f, -0.1f }, // HEAL
    { -2.2f,  1.0f,  0.0f, 0.0f,  0.0f, 0.0f, -0.1f }, // HEAL_ALL
    {  0.0f,  1.0f,  0.0f, 0.0f, -0.4f, 2.2f,  0.2f }, // V_ATTACK
    {  0.0f,  1.0f,  0.0f, 0.0f, -0.4f, 2.2f,  0.2f }, // H_ATTACK
    {  0.0f,  0.5f, -0.7f, 0.0f,  0.0f, 0.0f,  0.3f }, // POISON
    {  0.0f,  0.4f,  0.0f, 0.0f,  0.0f, 0.0f, -0.1f }, // BLESS
    {  0.5f, -1.0f,  0.0f, 0.0f,  0.2f, 0.0f, -0.3f }  // REST
};

static void record_opponent_action(int act) {
    if (opp_hist_size < ACTION_HISTORY) {
        opp_history[opp_hist_size++] = act;
    }
    else {
        for (int i = 1; i < ACTION_HISTORY; ++i) opp_history[i - 1] = opp_history[i];
        opp_history[ACTION_HISTORY - 1] = act;
    }
}

static float estimate_opponent_threat(const Player* opp) {
    if (!opp || opp_hist_size <= 0) return 0.0f;
    int cnt_attack = 0, cnt_strike = 0, cnt_range = 0;
    for (int i = 0; i < opp_hist_size; i++) {
        int a = opp_history[i];
        if (a == CMD_ATTACK) cnt_attack++;
        else if (a == CMD_STRIKE) cnt_strike++;
        else if (a == CMD_RANGE_ATTACK) cnt_range++;
    }
    float total = (float)opp_hist_size;
    float threat = (cnt_attack * 2.0f + cnt_strike * 3.5f + cnt_range * 2.5f) / total;
    if (threat > 6.0f) threat = 6.0f;
    return threat;
}

static void extract_features(const Player* me, const Player* opp, float feat[FEAT_COUNT]) {
    int mx = get_player_x(me); int my = get_player_y(me);
    int ox = get_player_x(opp); int oy = get_player_y(opp);
    int dist = abs(mx - ox) + abs(my - oy);
    int is_line = (mx == ox || my == oy) ? 1 : 0;

    feat[0] = (float)get_player_hp(me);
    feat[1] = (float)get_player_mp(me);
    feat[2] = (float)get_player_hp(opp);
    feat[3] = (float)get_player_mp(opp);
    feat[4] = (float)dist;
    feat[5] = (float)is_line;
    feat[6] = estimate_opponent_threat(opp);
}

static int is_action_feasible(int cmd, const Player* me, const Player* opp) {
    int my_mp = get_player_mp(me);
    int my_hp = get_player_hp(me);
    int mx = get_player_x(me); int my = get_player_y(me);
    int ox = get_player_x(opp); int oy = get_player_y(opp);
    int dist = abs(mx - ox) + abs(my - oy);
    int is_line = (mx == ox || my == oy);

    switch (cmd) {
    case CMD_ATTACK:       return (dist <= 1);
    case CMD_STRIKE:       return (dist <= 1 && my_mp >= 2);
    case CMD_RANGE_ATTACK: return (dist == 2 && my_mp >= 1);
    case CMD_POISON:       return (my_mp >= 5);
    case CMD_HEAL:         return (my_mp >= 1 && my_hp < 5);
    case CMD_HEAL_ALL:     return (my_mp >= 2 && my_hp < 5);
    case CMD_REST:         return (my_mp < 5);
    case CMD_BLESS:        return (my_mp >= 2);
    case CMD_H_ATTACK:     return (is_line && my_mp >= 3 && my == oy);
    case CMD_V_ATTACK:     return (is_line && my_mp >= 3 && mx == ox);
    case CMD_BLINK_UP:
    case CMD_BLINK_DOWN:
    case CMD_BLINK_LEFT:
    case CMD_BLINK_RIGHT:  return (my_mp >= 1);
    default:               return 1;
    }
}

static int escape_from_enemy(const Player* me, const Player* opp) {
    int dx = get_player_x(me) - get_player_x(opp);
    int dy = get_player_y(me) - get_player_y(opp);
    if (abs(dx) >= abs(dy)) return (dx >= 0) ? CMD_RIGHT : CMD_LEFT;
    else return (dy >= 0) ? CMD_DOWN : CMD_UP;
}

static int playerB_decision_maker(const Player* me, const Player* opp) {
    if (!me || !opp) return CMD_REST;

    int opp_last = get_player_last_command(opp);
    record_opponent_action(opp_last);

    float feat[FEAT_COUNT];
    extract_features(me, opp, feat);

    int dist = (int)feat[4];
    int is_line = (int)feat[5];
    int my_hp = (int)feat[0];
    int my_mp = (int)feat[1];

    // [전략] 거리 1이면 무조건 회피 (Blink 우선)
    if (dist == 1) {
        if (is_skill_unlocked(my_secret_key, CMD_BLINK_UP) && is_action_feasible(CMD_BLINK_UP, me, opp))    return CMD_BLINK_UP;
        if (is_skill_unlocked(my_secret_key, CMD_BLINK_DOWN) && is_action_feasible(CMD_BLINK_DOWN, me, opp))  return CMD_BLINK_DOWN;
        if (is_skill_unlocked(my_secret_key, CMD_BLINK_LEFT) && is_action_feasible(CMD_BLINK_LEFT, me, opp))  return CMD_BLINK_LEFT;
        if (is_skill_unlocked(my_secret_key, CMD_BLINK_RIGHT) && is_action_feasible(CMD_BLINK_RIGHT, me, opp)) return CMD_BLINK_RIGHT;
        return escape_from_enemy(me, opp);
    }

    // [전략] 거리 유지 공격
    if (is_line) {
        if (is_skill_unlocked(my_secret_key, CMD_V_ATTACK) && is_action_feasible(CMD_V_ATTACK, me, opp)) return CMD_V_ATTACK;
        if (is_skill_unlocked(my_secret_key, CMD_H_ATTACK) && is_action_feasible(CMD_H_ATTACK, me, opp)) return CMD_H_ATTACK;
    }
    if (dist == 2) {
        if (is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK) && is_action_feasible(CMD_RANGE_ATTACK, me, opp)) return CMD_RANGE_ATTACK;
    }
    if (is_skill_unlocked(my_secret_key, CMD_POISON) && is_action_feasible(CMD_POISON, me, opp)) {
        if (my_mp >= 5) return CMD_POISON;
    }
    if (my_hp <= 3) {
        if (is_skill_unlocked(my_secret_key, CMD_HEAL_ALL) && is_action_feasible(CMD_HEAL_ALL, me, opp) && my_mp >= 2) return CMD_HEAL_ALL;
        if (is_skill_unlocked(my_secret_key, CMD_HEAL) && is_action_feasible(CMD_HEAL, me, opp)) return CMD_HEAL;
    }

    // [전략] Perceptron Fallback
    float best_score = -99999.0f;
    int best_cmd = CMD_REST;

    for (int i = 0; i < ACTION_COUNT; ++i) {
        int cmd = CANDIDATE_ACTIONS[i];
        if (!is_skill_unlocked(my_secret_key, cmd)) continue;
        if (!is_action_feasible(cmd, me, opp)) continue;

        float score = 0.0f;
        for (int f = 0; f < FEAT_COUNT; ++f) score += feat[f] * WEIGHTS[i][f];
        score += ((rand() % 21) - 10) * 0.01f;

        if (my_hp <= 2 && (cmd == CMD_HEAL || cmd == CMD_HEAL_ALL)) score += 1.5f;
        if (score > best_score) {
            best_score = score;
            best_cmd = cmd;
        }
    }

    if (!is_action_feasible(best_cmd, me, opp)) return CMD_REST;
    return best_cmd;
}

// =================================================================================================
// [ENTRY] 메인 진입점
// =================================================================================================
void student2_ai_entry(void) {
    // 1. 플레이어 등록
    my_secret_key = register_player_ai("PLAYER-B-FINAL", playerB_decision_maker);

    // 2. 데이터 로드 (문제 풀이를 위해 필수)
    load_data();

    // 3. 문제 풀이 함수 호출 (실시간 연산 후 스킬 해금)
    if (count > 0) {
        solve_problem_1_poison(my_secret_key);
        solve_problem_2_strike(my_secret_key);
        solve_problem_3_blink(my_secret_key);
        solve_problem_4_heal_all(my_secret_key);
        solve_problem_5_range(my_secret_key);
        solve_problem_6_suicide(my_secret_key);
        solve_problem_7_hv(my_secret_key);
        solve_problem_8_secret(my_secret_key);
    }
    else {
        printf("PlayerB: 데이터가 없어 스킬 해금을 건너뜁니다.\n");
    }
}