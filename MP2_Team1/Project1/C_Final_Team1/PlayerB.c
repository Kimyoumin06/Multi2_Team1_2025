#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h> // size_t 정의 포함
#include <ctype.h>  // isvowel을 대체하기 위해 strcspn 사용 (문제 7/8 로직에 필요할 수 있음)

#define MAX_Weapon 100 // PlayerB.c 원본

// 아이템 정보를 저장할 구조체 (PlayerB.c 원본 사용, 버퍼 크기만 넉넉하게 조정)
typedef struct {
    int id;
    char name[50];
    char slot[10];
    int ATK;
    int DEF;
    int HP;
    char CURSE[50]; // 안전을 위해 50으로 확장
    char KEY_FRAG[50]; // 안전을 위해 50으로 확장
}Weapon;

// 전역 변수: 모든 문제 풀이 및 AI에서 접근 가능하도록 변경
Weapon list[MAX_Weapon];
int count = 0;
int my_secret_key; // PlayerB.c 원본에서 my_secret_key로 등록됨

// CSV 파일명 (AI1-2_C_Final.docx 기준)
static const char* TARGET_CSV = "game_puzzle_en.csv";

    // =================================================================================================
    // [PART 1] 공통 함수 (CSV 로드 및 거리 계산)
    // =================================================================================================

    // PlayerB.c 원본의 거리 계산 함수
    static int calculate_distance(const Player* p1, const Player* p2) {
        int dx = abs(get_player_x(p1) - get_player_x(p2));
        int dy = abs(get_player_y(p1) - get_player_y(p2));
        return dx + dy;
    }

    // ID로 아이템을 찾아주는 도우미 함수 (전역 list, count 사용)
    Weapon* find_item_by_id(int id) {
        for (int i = 0; i < count; i++) {
            if (list[i].id == id) return &list[i];
        }
        return NULL;
    }

    // PlayerB.c 원본의 CSV 파일 읽기 함수 (전역 list, count 사용하도록 수정)
    void ReadFile(void) {
        FILE* fp;

        // TARGET_CSV (game_puzzle_en.csv) 사용
        if (fopen_s(&fp, TARGET_CSV, "r") != 0 || fp == NULL) {
            printf("[ReadFile] 오류: CSV 파일을 찾을 수 없습니다. (%s)\n", TARGET_CSV);
            return;
        }

        char line[256];
        // 헤더 건너뛰기
        if (fgets(line, sizeof(line), fp) == NULL) {
            fclose(fp);
            return;
        }

        count = 0;
        while (fgets(line, sizeof(line), fp) != NULL && count < MAX_Weapon) {
            size_t len = strlen(line);
            // 개행 문자 제거
            while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
                line[len - 1] = '\0';
                len--;
            }
            if (len == 0) continue;

            char* context = NULL;
            // strtok_s 사용
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
    // [PART 2] 문제 풀이 함수들 (1~8번)
    // =================================================================================================

    // -----------------------------------------------------------------------------------------
    // [문제 1] CMD_POISON 해금 (PlayerB.c 원본 로직 - Skill 6: 독해금)
    // -----------------------------------------------------------------------------------------
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
            printf("TEAM-1 (문제 1/Skill 6) : CMD_POISON 해금 완료\n");
        else
            printf("TEAM-1 (문제 1/Skill 6) : CMD_POISON 해금 실패 ㅜㅜ\n");
    }

    // -----------------------------------------------------------------------------------------
    // [문제 2] CMD_STRIKE 해금 (PlayerB.c 원본 로직 - Skill 7: 강타)
    // -----------------------------------------------------------------------------------------
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
            printf("TEAM-1 (문제 2/Skill 7) : CMD_STRIKE 해금 완료\n");
        else
            printf("TEAM-1 (문제 2/Skill 7) : CMD_STRIKE 해금 실패 ㅜㅜ\n");
    }

    // -----------------------------------------------------------------------------------------
    // [문제 3] CMD_BLINK_UP 해금 (PlayerB.c 원본 로직 - Skill 8~11: 점멸)
    // -----------------------------------------------------------------------------------------
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
                    if (is_skill_unlocked(my_key, CMD_BLINK_UP))
                        printf("TEAM-1 (문제 3/Skill 8) : CMD_BLINK 해금 성공!\n");
                    else
                        printf("TEAM-1 (문제 3/Skill 8) : CMD_BLINK 해금 실패 ㅜㅜ\n");
    }

    // -----------------------------------------------------------------------------------------
    // [문제 4] CMD_HEAL_ALL 해금 (PlayerB.c 원본 로직 - Skill 13: 회복2)
    // -----------------------------------------------------------------------------------------
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
            printf("TEAM-1 (문제 4/Skill 13) : CMD_HEAL_ALL 해금 성공!\n");
        else
            printf("TEAM-1 (문제 4/Skill 13) : CMD_HEAL_ALL 해금 실패 ㅜㅜ\n");
    }

    // -----------------------------------------------------------------------------------------
    // [문제 5] CMD_RANGE_ATTACK 해금 (9999999.txt 로직 통합 - Skill 14: 원거리공격)
    // -----------------------------------------------------------------------------------------
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
            // fopen_s 사용
            if (fopen_s(&fp, TARGET_CSV, "rb") == 0 && fp != NULL) {

                // 파일의 시작(오프셋 0)에서 N 바이트 앞으로 이동 (N번째 글자는 인덱스 N-1)
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
            printf("TEAM-1 (문제 5/Skill 14) : CMD_RANGE_ATTACK 해금 성공! 정답: [%s]\n", final_key);
        else
            printf("TEAM-1 (문제 5/Skill 14) : CMD_RANGE_ATTACK 해금 실패... 시도한 키: [%s]\n", final_key);
    }

    // -----------------------------------------------------------------------------------------
    // [문제 6] CMD_SUICIDE 해금 (9999999.txt 로직 통합 - Skill 16: 자폭)
    // -----------------------------------------------------------------------------------------
    void solve_problem_6_suicide(int my_key) {

        char final_key[100] = "";
        char combined[2000] = { 0 };

        
        for (int i = 0; i < count; i++) {
            if (strstr(list[i].name, "Sword") != NULL) {
                // strcat_s 사용
                strcat_s(combined, sizeof(combined), list[i].KEY_FRAG);
            }
        }

        char temp[2000];
        strcpy_s(temp, sizeof(temp), combined);

        char* next_token = NULL;
        // strtok_s 사용
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

            // CMD_SUICIDE 해금 시도
            attempt_skill_unlock(my_key, CMD_BLESS, final_key);
            if (is_skill_unlocked(my_key, CMD_BLESS))
                printf("TEAM-1 (문제 6/Skill 16) : CMD_SUICIDE 해금 성공! 정답: [%s]\n", final_key);
            else
                printf("TEAM-1 (문제 6/Skill 16) : CMD_SUICIDE 해금 실패... 시도한 키: [%s]\n", final_key);
    }

    // -----------------------------------------------------------------------------------------
    // [문제 7] CMD_H_ATTACK / CMD_V_ATTACK 해금 (20251209-161632.txt 로직 이식 - Skill 17/18: 가로/세로 마법)
    // -----------------------------------------------------------------------------------------
    void solve_problem_7_hv(int my_key) {
        // PlayerB.c의 Weapon 구조체 정의에 맞게 버퍼 크기를 조정 (50)
        char final_key[16] = "";
        char bestName[50] = "";
        char bestCurse[50] = "";
        int  maxNameLen = -1;
        int  minCurseLen = 9999;

        // 전역 list 사용
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
            printf("TEAM-1 (문제 7/Skill 17/18) : 데이터 부족으로 해금 실패.\n");
            return;
        }

        char head[4] = { 0 };
        strncpy_s(head, sizeof(head), bestName, 3);

        char tail[4] = { 0 };
        int clen = (int)strlen(bestCurse);
        if (clen >= 3) {
            char* start = (char*)bestCurse + (clen - 3); // const char*를 char*로 형변환 필요 (strncpy_s 원형에 따라)
            strncpy_s(tail, sizeof(tail), start, 3);
        }
        else {
            // 3글자 미만이면 전체 복사 
            strncpy_s(tail, sizeof(tail), bestCurse, sizeof(tail) - 1);
        }

        sprintf_s(final_key, sizeof(final_key), "%s%s", head, tail);
        // 정답: GreILU

        attempt_skill_unlock(my_key, CMD_H_ATTACK, final_key);
        attempt_skill_unlock(my_key, CMD_V_ATTACK, final_key);

        if (is_skill_unlocked(my_key, CMD_H_ATTACK))
            printf("TEAM-1 (문제 7/Skill 17/18) : CMD_H_ATTACK/CMD_V_ATTACK 해금 완료! 정답: [%s]\n", final_key);
        else
            printf("TEAM-1 (문제 7/Skill 17/18) : H/V 공격 해금 실패 ㅠㅠ 시도한 키: [%s]\n", final_key);
    }

    // -----------------------------------------------------------------------------------------
    // [문제 8] CMD_SECRETE 해금 (20251209-161632.txt 로직 이식 - Skill 19: 비밀 스킬)
    // -----------------------------------------------------------------------------------------
    void solve_problem_8_secret(int my_key) {
        char final_key[32] = "";
        char targetName[50] = ""; // PlayerB.c 구조체 NAME 크기 50

        for (int i = 0; i < count; i++) {
            if (strstr(list[i].name, "Stone") != NULL) {
                strcpy_s(targetName, sizeof(targetName), list[i].name);
                break;
            }
        }

        if (targetName[0] == '\0') {
            printf("TEAM-1 (문제 8/Skill 19) : 'Stone' 아이템을 찾을 수 없습니다.\n");
            return;
        }

        char buf[50];
        strcpy_s(buf, sizeof(buf), targetName);
        const char* delims = "AEIOUaeiou";

        char* context_vowels = NULL;
        // strtok_s 사용
        char* tok = strtok_s(buf, delims, &context_vowels);

        char   bestToken[50] = "";
        size_t bestLen = 0;
        while (tok != NULL) {
            size_t len = strlen(tok);
            // 더 길면 교체 (같으면 기존 것 유지) [cite: 121]
            if (len > bestLen) {
                bestLen = len;
                strcpy_s(bestToken, sizeof(bestToken), tok);
            }
            // strtok_s 반복 호출
            tok = strtok_s(NULL, delims, &context_vowels);
        }

        if (bestToken[0] == '\0') {
            printf("TEAM-1 (문제 8/Skill 19) : 가장 긴 자음 토큰을 찾을 수 없습니다.\n");
            return;
        }

        strncpy_s(final_key, sizeof(final_key), bestToken, _TRUNCATE);

            // CMD_SECRETE 해금 시도 [cite: 129]
            attempt_skill_unlock(my_key, CMD_SECRETE, final_key);

            if (is_skill_unlocked(my_key, CMD_SECRETE))
                printf("TEAM-1 (문제 8/Skill 19) : CMD_SECRETE 해금 완료! 정답: [%s]\n", final_key);
            else
                printf("TEAM-1 (문제 8/Skill 19) : CMD_SECRETE 해금 실패 ㅠㅠ 시도한 키: [%s]\n", final_key);
    }


    // =================================================================================================
    // [PART 3] AI 로직 구현부 (기존 PlayerB의 개선된 로직 유지)
    // =================================================================================================

    // 9999999.txt의 AI 전략 함수 (PlayerB의 변수명에 맞게 통합)
    int player_b_strategy(const Player* my_info, const Player* opponent_info) {

        // PlayerB.c 원본의 calculate_distance 사용
        int dist = calculate_distance(my_info, opponent_info);
        int mp = get_player_mp(my_info);
        int hp = get_player_hp(my_info);
        int my_x = get_player_x(my_info);
        int opp_x = get_player_x(opponent_info);
        int my_y = get_player_y(my_info);
        int opp_y = get_player_y(opponent_info);
        int my_key = my_secret_key;

        // 1. 위기 관리 (HP 2 이하 시 HEAL 또는 HEAL_ALL)
        if (hp <= 2 && mp >= 1) {
            if (mp >= 2 && is_skill_unlocked(my_key, CMD_HEAL_ALL)) return CMD_HEAL_ALL;
            return CMD_HEAL;
        }

        // 2. 근접 전투 (거리 1 이하 시 STRIKE 또는 ATTACK)
        if (dist <= 1) {
            if (mp >= 2 && is_skill_unlocked(my_key, CMD_STRIKE)) return CMD_STRIKE;
            return CMD_ATTACK;
        }

        // 3. 원거리 견제 (거리 2 시 RANGE_ATTACK)
        if (dist == 2 && mp >= 1) {
            if (is_skill_unlocked(my_key, CMD_RANGE_ATTACK)) return CMD_RANGE_ATTACK;
        }

        // 4. 가로/세로 공격 (추가 해금 스킬 활용)
        // 문제 7(CMD_H_ATTACK/CMD_V_ATTACK)이 해금되었을 때 사용
        if (my_x == opp_x && mp >= 3 && is_skill_unlocked(my_key, CMD_V_ATTACK)) return CMD_V_ATTACK;
        if (my_y == opp_y && mp >= 3 && is_skill_unlocked(my_key, CMD_H_ATTACK)) return CMD_H_ATTACK;

        // 5. 추격
        if (my_x < opp_x) return CMD_RIGHT;
        if (my_x > opp_x) return CMD_LEFT;
        if (my_y < opp_y) return CMD_DOWN;
        if (my_y > opp_y) return CMD_UP;

        return CMD_REST;
    }


    // =================================================================================================
    // [PART 4] 시스템 진입점 (PlayerB.c 원본 로직 + 5~8번 추가)
    // =================================================================================================

    void student2_ai_entry() {

        // 1. AI 등록 (기존 PlayerB.c는 simple_killer_ai2였으나, 개선된 player_b_strategy로 교체)
        my_secret_key = register_player_ai("TEAM-1", player_b_strategy); // TEAM-1 유지

        // 2. CSV 파일 읽기 (전역 변수 list, count에 저장)
        ReadFile();

        printf("\n>>> TEAM-1 스킬 해금 시도 중 (문제 1~8) <<<\n");

        // ------------------------------------------------------------------
        // 문제 1~4 (PlayerB.c 원본 로직)
        // ------------------------------------------------------------------
        solve_problem_1_poison(my_secret_key);
        solve_problem_2_strike(my_secret_key);
        solve_problem_3_blink(my_secret_key);
        solve_problem_4_heal_all(my_secret_key);

        // ------------------------------------------------------------------
        // 문제 5, 6 (9999999.txt 로직 통합)
        // ------------------------------------------------------------------
        solve_problem_5_range(my_secret_key);
        solve_problem_6_suicide(my_secret_key); // DOCX의 문제 번호 16(자폭)에 해당

        // ------------------------------------------------------------------
        // 문제 7, 8 (20251209-161632.txt 로직 통합)
        // ------------------------------------------------------------------
        solve_problem_7_hv(my_secret_key); // Skill 17/18: 가로/세로 마법
        solve_problem_8_secret(my_secret_key); // Skill 19: 비밀 스킬

        printf("\nTEAM-1 : 플레이어 초기화 완료. 아무키나 누르시오.\n");
        getchar();
    }