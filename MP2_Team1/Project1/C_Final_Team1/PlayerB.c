#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_Weapon 100

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
int my_secret_key; // PlayerB의 비밀 키

// CSV 파일명
static const char* TARGET_CSV = "game_puzzle_en.csv";

// =================================================================================================
// [PART 1] 공통 함수 (CSV 로드 및 거리 계산)
// =================================================================================================

// 맨해튼 거리 계산 함수
static int calculate_distance(const Player* p1, const Player* p2) {
    int dx = abs(get_player_x(p1) - get_player_x(p2));
    int dy = abs(get_player_y(p1) - get_player_y(p2));
    return dx + dy;
}

// ID로 아이템을 찾아주는 도우미 함수
Weapon* find_item_by_id(int id) {
    for (int i = 0; i < count; i++) {
        if (list[i].id == id) return &list[i];
    }
    return NULL;
}

// CSV 파일 읽기 함수
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
// [PART 2] 문제 풀이 함수들 (1~8번) - 기존 로직 유지
// =================================================================================================

// [문제 1] CMD_POISON 해금 (20251402 김유민)
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
        if (strlen(result) > 0) {
            strcat_s(result, sizeof(result), "|");
        }
        strcat_s(result, sizeof(result), list[idx].name);
    }

    attempt_skill_unlock(my_key, CMD_POISON, result);
    if (is_skill_unlocked(my_key, CMD_POISON))
        printf("TEAM-1 : CMD_POISON 해금 완료\n");
    else
        printf("TEAM-1 : CMD_POISON 해금 실패 ㅜㅜ\n");
}

// [문제 2] CMD_STRIKE 해금 (20251402 김유민)
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
        printf("TEAM-1 : CMD_STRIKE 해금 실패 ㅜㅜ\n");
}

// [문제 3] CMD_BLINK 4종 해금 (20251413 이종석)
void solve_problem_3_blink(int my_key) {
    char final_key[100] = "";

    // 조건 1: HP = DEF(202) + DEF(208)
    Weapon* i202 = find_item_by_id(202);
    Weapon* i208 = find_item_by_id(208);
    int target_hp = (i202 && i208) ? (i202->DEF + i208->DEF) : 0;
    for (int i = 0; i < count; i++) {
        if (list[i].HP == target_hp && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            strcat_s(final_key, sizeof(final_key), list[i].KEY_FRAG);
            break;
        }
    }

    // 조건 2: ATK = ATK(205) * ATK(212)
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

    // 조건 3: CURSE에 "C_01" 포함, 마지막
    int last_curse_idx = -1;
    for (int i = 0; i < count; i++) {
        if (strstr(list[i].CURSE, "C_01") && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            last_curse_idx = i;
        }
    }
    if (last_curse_idx != -1) strcat_s(final_key, sizeof(final_key), list[last_curse_idx].KEY_FRAG);

    // 조건 4: NAME이 'I'로 시작, 첫 번째
    for (int i = 0; i < count; i++) {
        if (list[i].name[0] == 'I' && strcmp(list[i].KEY_FRAG, "NIL") != 0) {
            strcat_s(final_key, sizeof(final_key), list[i].KEY_FRAG);
            break;
        }
    }

    // BLINK 4종 모두 해금
    attempt_skill_unlock(my_key, CMD_BLINK_UP, final_key);
    attempt_skill_unlock(my_key, CMD_BLINK_DOWN, final_key);
    attempt_skill_unlock(my_key, CMD_BLINK_LEFT, final_key);
    attempt_skill_unlock(my_key, CMD_BLINK_RIGHT, final_key);

    if (is_skill_unlocked(my_key, CMD_BLINK_UP))
        printf("TEAM-1 : CMD_BLINK 4종 해금 완료\n");
    else
        printf("TEAM-1 : CMD_BLINK 4종 해금 실패 ㅜㅜ\n");
}

// [문제 4] CMD_HEAL_ALL 해금 (20251413 이종석)
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

// [문제 5] CMD_RANGE_ATTACK 해금 (20251398 홍주아)
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

// [문제 6] CMD_BLESS 해금 (20251398 홍주아)
void solve_problem_6_bless(int my_key) {
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

// [문제 7] CMD_H_ATTACK / CMD_V_ATTACK 해금 (20251389 김유미)
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
        printf("TEAM-1 : H/V 공격 해금 실패 ㅜㅜ\n");
}

// [문제 8] CMD_SECRETE 해금 (20251389 김유미)
void solve_problem_8_secret(int my_key) {
    char final_key[32] = "";
    char targetName[50] = "";

    for (int i = 0; i < count; i++) {
        if (strstr(list[i].name, "Stone") != NULL) {
            strcpy_s(targetName, sizeof(targetName), list[i].name);
            break;
        }
    }

    if (targetName[0] == '\0') {
        printf("TEAM-1 (문제 8) : 'Stone' 아이템을 찾을 수 없습니다.\n");
        return;
    }

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

    if (bestToken[0] == '\0') {
        printf("TEAM-1 (문제 8) : 가장 긴 자음 토큰을 찾을 수 없습니다.\n");
        return;
    }

    strncpy_s(final_key, sizeof(final_key), bestToken, _TRUNCATE);

    attempt_skill_unlock(my_key, CMD_SECRETE, final_key);

    if (is_skill_unlocked(my_key, CMD_SECRETE))
        printf("TEAM-1 : CMD_SECRETE 해금 완료\n");
    else
        printf("TEAM-1 : CMD_SECRETE 해금 실패 ㅜㅜ\n");
}

// =================================================================================================
// [PART 3] 최적화된 AI 로직 구현부
// =================================================================================================

int player_b_strategy(const Player* my_info, const Player* opponent_info) {
    if (!my_info || !opponent_info) return CMD_REST;

    // 현재 상태 분석
    int my_hp = get_player_hp(my_info);
    int my_mp = get_player_mp(my_info);
    int opp_hp = get_player_hp(opponent_info);
    int my_x = get_player_x(my_info);
    int my_y = get_player_y(my_info);
    int opp_x = get_player_x(opponent_info);
    int opp_y = get_player_y(opponent_info);
    int dist = calculate_distance(my_info, opponent_info);
    int my_key = my_secret_key;

    int same_row = (my_y == opp_y);
    int same_col = (my_x == opp_x);

    // 스킬 사용 가능 여부 체크
    int can_strike = (dist <= 1 && my_mp >= 2 && is_skill_unlocked(my_key, CMD_STRIKE));
    int can_attack = (dist <= 1);
    int can_range = (dist == 2 && my_mp >= 1 && is_skill_unlocked(my_key, CMD_RANGE_ATTACK));
    int can_heal_all = (my_mp >= 2 && is_skill_unlocked(my_key, CMD_HEAL_ALL));
    int can_heal = (my_mp >= 1);
    int can_h_attack = (same_row && my_mp >= 3 && is_skill_unlocked(my_key, CMD_H_ATTACK));
    int can_v_attack = (same_col && my_mp >= 3 && is_skill_unlocked(my_key, CMD_V_ATTACK));
    int can_poison = (dist <= 1 && my_mp >= 5 && is_skill_unlocked(my_key, CMD_POISON));

    // =========================
    // Phase 0: 생존 우선 (긴급 상황)
    // =========================

    if (my_hp <= 1) {
        // 절대 위기: 무조건 회복
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "긴급 힐!");
        }

        if (can_heal_all) return CMD_HEAL_ALL;
        if (can_heal) return CMD_HEAL;

        // 힐이 불가능하면 마지막 발악 공격
        if (can_strike) return CMD_STRIKE;
        if (can_attack) return CMD_ATTACK;
        return CMD_REST;
    }

    // HP 2 이하 + 안전 거리: 미리 광역 회복
    if (my_hp <= 2 && can_heal_all && dist >= 3) {
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "미리 광역 힐");
        }
        return CMD_HEAL_ALL;
    }

    // =========================
    // Phase 1: 킬각 포착 (최우선)
    // =========================

    if (opp_hp <= 2) {
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "킬각 포착!");
        }

        // 거리 1: 즉시 공격
        if (dist <= 1) {
            if (can_strike) return CMD_STRIKE;
            return CMD_ATTACK;
        }

        // 거리 2: 원거리 공격
        if (can_range) return CMD_RANGE_ATTACK;

        // 라인 공격 가능
        if (can_h_attack) return CMD_H_ATTACK;
        if (can_v_attack) return CMD_V_ATTACK;

        // 킬각인데 거리가 멀면 추격
        if (my_x != opp_x) {
            return (my_x < opp_x) ? CMD_RIGHT : CMD_LEFT;
        }
        if (my_y != opp_y) {
            return (my_y < opp_y) ? CMD_DOWN : CMD_UP;
        }
    }

    // =========================
    // Phase 2: 근접전 우위 확보
    // =========================

    if (dist <= 1) {
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "근접전 우위!");
        }

        // STRIKE로 2 데미지 딜링
        if (can_strike) return CMD_STRIKE;

        // 독 스킬 (장기전 대비, 상대 HP 4 이상일 때만)
        if (can_poison && my_mp >= 5 && opp_hp >= 4) {
            return CMD_POISON;
        }

        // 기본 공격
        return CMD_ATTACK;
    }

    // =========================
    // Phase 3: 거리 2 견제
    // =========================

    if (can_range) {
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "거리 2 견제");
        }
        return CMD_RANGE_ATTACK;
    }

    // =========================
    // Phase 4: 라인 공격 (MP 충분할 때)
    // =========================

    if (my_mp >= 3) {
        if (can_h_attack) {
            if (is_skill_unlocked(my_key, CMD_SECRETE)) {
                set_custom_secrete_message(my_key, "가로 라인 공격");
            }
            return CMD_H_ATTACK;
        }
        if (can_v_attack) {
            if (is_skill_unlocked(my_key, CMD_SECRETE)) {
                set_custom_secrete_message(my_key, "세로 라인 공격");
            }
            return CMD_V_ATTACK;
        }
    }

    // =========================
    // Phase 5: HP 관리 (안전 거리에서만)
    // =========================

    if (my_hp <= 3 && dist >= 3) {
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "안전 힐");
        }

        if (can_heal_all) return CMD_HEAL_ALL;
        if (can_heal) return CMD_HEAL;
    }

    // =========================
    // Phase 6: MP 관리 (안전 거리에서만)
    // =========================

    if (my_mp <= 1 && dist >= 3) {
        if (is_skill_unlocked(my_key, CMD_SECRETE)) {
            set_custom_secrete_message(my_key, "MP 충전");
        }
        return CMD_REST;
    }

    // =========================
    // Phase 7: 이동 로직 (추격)
    // =========================

    if (is_skill_unlocked(my_key, CMD_SECRETE)) {
        set_custom_secrete_message(my_key, "추격 이동");
    }

    if (my_x != opp_x) {
        return (my_x < opp_x) ? CMD_RIGHT : CMD_LEFT;
    }

    if (my_y != opp_y) {
        return (my_y < opp_y) ? CMD_DOWN : CMD_UP;
    }

    // =========================
    // 최종: 기본값
    // =========================

    return CMD_REST;
}

// =================================================================================================
// [PART 4] 시스템 진입점
// =================================================================================================

void student2_ai_entry(void) {

    // AI 등록 (최적화된 전략 함수 사용)
    my_secret_key = register_player_ai("TEAM-1", player_b_strategy);

    // CSV 파일 읽기
    ReadFile();

    printf("\n>>> TEAM-1 스킬 해금 시도 중 (문제 1~8) <<<\n");

    // 문제 1~8 해금 시도
    solve_problem_1_poison(my_secret_key);
    solve_problem_2_strike(my_secret_key);
    solve_problem_3_blink(my_secret_key);
    solve_problem_4_heal_all(my_secret_key);
    solve_problem_5_range(my_secret_key);
    solve_problem_6_bless(my_secret_key);
    solve_problem_7_hv(my_secret_key);
    solve_problem_8_secret(my_secret_key);

    // CMD_SECRETE가 해금되었다면 메시지 설정
    if (is_skill_unlocked(my_secret_key, CMD_SECRETE)) {
        set_custom_secrete_message(my_secret_key, "2P가 진짜 보스다 ^^");
    }

    printf("\nTEAM-1 : [최적화 AI] 초기화 완료. 아무키나 누르시오.\n");
    getchar();
}