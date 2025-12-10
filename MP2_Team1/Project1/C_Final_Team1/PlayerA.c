/**
 * =================================================================================================
 * [공격적 러시 전략 AI - PlayerA]
 * =================================================================================================
 * 핵심 전략:
 * 1. 초반 버스트 딜링 (Burst Damage)
 * 2. 공격 타이밍 최적화 (Timing Optimization)
 * 3. MP 효율 극대화 (Resource Management)
 * 4. 압박 전술 (Pressure Tactics)
 * =================================================================================================
 */

#include "api.h"
#include <stdlib.h> 
#include <stdio.h> 

 // 전투 데이터 추적
typedef struct {
    int total_turns;
    int damage_dealt;
    int damage_taken;
    int mp_used;
    int last_action;
    int consecutive_attacks;  // 연속 공격 카운트
    int opponent_hp_history[20];  // 상대 HP 히스토리
    int history_count;
} BattleData;

// 전역 변수
int my_secret_key;
BattleData battle_data = { 0, 0, 0, 0, 0, 0, {0}, 0 };

// =================================================================================================
// [유틸리티 함수]
// =================================================================================================

static int calculate_distance(const Player* p1, const Player* p2) {
    int dx = abs(get_player_x(p1) - get_player_x(p2));
    int dy = abs(get_player_y(p1) - get_player_y(p2));
    return dx + dy;
}

// 상대 HP 변화량 추적
void track_opponent_hp(int current_hp) {
    if (battle_data.history_count < 20) {
        battle_data.opponent_hp_history[battle_data.history_count++] = current_hp;
    }
}

// 상대가 회복했는지 확인
int opponent_just_healed(int current_hp) {
    if (battle_data.history_count < 2) return 0;
    int prev_hp = battle_data.opponent_hp_history[battle_data.history_count - 2];
    return (current_hp > prev_hp);
}

// =================================================================================================
// [공격성 평가 함수]
// =================================================================================================

/**
 * 공격성 지수 계산 (0~100)
 * 높을수록 더 공격적으로 플레이
 */
int calculate_aggression_level(const Player* my_info, const Player* opponent_info, int dist) {
    int aggression = 50;  // 기본값: 중립

    int my_hp = get_player_hp(my_info);
    int my_mp = get_player_mp(my_info);
    int opp_hp = get_player_hp(opponent_info);

    // HP 우위 = 공격성 증가
    if (my_hp > opp_hp + 2) aggression += 30;
    else if (my_hp > opp_hp) aggression += 15;
    else if (my_hp < opp_hp - 2) aggression -= 25;
    else if (my_hp < opp_hp) aggression -= 10;

    // MP 충분 = 공격성 증가
    if (my_mp >= 5) aggression += 20;
    else if (my_mp >= 3) aggression += 10;
    else if (my_mp <= 1) aggression -= 15;

    // 거리 가까움 = 공격성 증가
    if (dist <= 1) aggression += 25;
    else if (dist == 2) aggression += 10;
    else if (dist >= 5) aggression -= 10;

    // 상대가 약함 = 공격성 극대화
    if (opp_hp <= 2) aggression += 35;
    else if (opp_hp <= 4) aggression += 20;

    // 연속 공격 중이면 공격성 유지
    if (battle_data.consecutive_attacks >= 2) aggression += 15;

    return (aggression < 0) ? 0 : ((aggression > 100) ? 100 : aggression);
}

// =================================================================================================
// [공격적 러시 전략 AI]
// =================================================================================================

/**
 * 메인 AI 로직: 공격 우선, 빠른 게임 종료 지향
 */
int aggressive_rush_ai(const Player* my_info, const Player* opponent_info) {

    battle_data.total_turns++;

    // 현재 상태 분석
    int dist = calculate_distance(my_info, opponent_info);
    int my_hp = get_player_hp(my_info);
    int my_mp = get_player_mp(my_info);
    int opp_hp = get_player_hp(opponent_info);
    int my_x = get_player_x(my_info);
    int opp_x = get_player_x(opponent_info);
    int my_y = get_player_y(my_info);
    int opp_y = get_player_y(opponent_info);

    // 상대 HP 추적
    track_opponent_hp(opp_hp);

    // 공격성 지수 계산
    int aggression = calculate_aggression_level(my_info, opponent_info, dist);

    // =================================================================================
    // [Phase 1] 긴급 생존 체크 (최소한의 방어)
    // =================================================================================

    if (my_hp <= 1) {
        // 절대 위기: 무조건 회복
        battle_data.consecutive_attacks = 0;
        set_custom_secrete_message(my_secret_key, "긴급 회복!");

        if (my_mp >= 2 && is_skill_unlocked(my_secret_key, CMD_HEAL_ALL)) {
            return CMD_HEAL_ALL;
        }
        if (my_mp >= 1) {
            return CMD_HEAL;
        }
        return CMD_REST;  // MP도 없으면 휴식
    }

    // HP 2이고 적이 바로 옆에 있으면 회복 고려 (하지만 킬 찬스 우선)
    if (my_hp == 2 && dist == 1) {
        if (opp_hp <= 2 && my_mp >= 2) {
            // 동귀어진 가능: 선공으로 승리
            set_custom_secrete_message(my_secret_key, "선공 승리!");
            battle_data.consecutive_attacks++;

            if (is_skill_unlocked(my_secret_key, CMD_STRIKE)) {
                return CMD_STRIKE;
            }
            return CMD_ATTACK;
        }

        // 적이 강하면 회복
        if (my_mp >= 1) {
            battle_data.consecutive_attacks = 0;
            set_custom_secrete_message(my_secret_key, "전술적 회복");
            return CMD_HEAL;
        }
    }

    // =================================================================================
    // [Phase 2] 킬 찬스 감지 (최우선 공격)
    // =================================================================================

    // 적 HP 2 이하 & 공격 가능 = 무조건 처치
    if (opp_hp <= 2) {

        // 거리 1: 즉시 공격
        if (dist == 1) {
            set_custom_secrete_message(my_secret_key, "마무리!");
            battle_data.consecutive_attacks++;

            if (opp_hp == 2 && my_mp >= 2 && is_skill_unlocked(my_secret_key, CMD_STRIKE)) {
                return CMD_STRIKE;  // 2 데미지로 확실하게
            }
            return CMD_ATTACK;
        }

        // 거리 2: 원거리 공격
        if (dist == 2 && my_mp >= 1 && is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK)) {
            set_custom_secrete_message(my_secret_key, "원거리 마무리!");
            battle_data.consecutive_attacks++;
            return CMD_RANGE_ATTACK;
        }

        // 같은 줄: 직선 마법
        if (my_mp >= 3) {
            if (my_y == opp_y && is_skill_unlocked(my_secret_key, CMD_H_ATTACK)) {
                set_custom_secrete_message(my_secret_key, "가로 마법 마무리!");
                battle_data.consecutive_attacks++;
                return CMD_H_ATTACK;
            }
            if (my_x == opp_x && is_skill_unlocked(my_secret_key, CMD_V_ATTACK)) {
                set_custom_secrete_message(my_secret_key, "세로 마법 마무리!");
                battle_data.consecutive_attacks++;
                return CMD_V_ATTACK;
            }
        }

        // 빠른 접근
        set_custom_secrete_message(my_secret_key, "마무리 돌진!");
        if (my_x < opp_x) return CMD_RIGHT;
        if (my_x > opp_x) return CMD_LEFT;
        if (my_y < opp_y) return CMD_DOWN;
        if (my_y > opp_y) return CMD_UP;
    }

    // =================================================================================
    // [Phase 3] 공격 콤보 시스템 (aggression >= 60)
    // =================================================================================

    if (aggression >= 60) {

        // 거리 1: 근접 폭딜
        if (dist == 1) {

            // POISON + STRIKE 콤보 (장기 데미지)
            if (my_mp >= 4 && opp_hp >= 5 &&
                is_skill_unlocked(my_secret_key, CMD_POISON) &&
                battle_data.consecutive_attacks == 0) {
                set_custom_secrete_message(my_secret_key, "독 + 강타 콤보!");
                battle_data.consecutive_attacks++;
                return CMD_POISON;
            }

            // STRIKE: 고효율 공격
            if (my_mp >= 2 && is_skill_unlocked(my_secret_key, CMD_STRIKE)) {
                set_custom_secrete_message(my_secret_key, "강타!");
                battle_data.consecutive_attacks++;
                return CMD_STRIKE;
            }

            // ATTACK: 기본 공격
            set_custom_secrete_message(my_secret_key, "연속 공격!");
            battle_data.consecutive_attacks++;
            return CMD_ATTACK;
        }

        // 거리 2: 원거리 압박
        if (dist == 2 && my_mp >= 1 && is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK)) {
            set_custom_secrete_message(my_secret_key, "원거리 압박!");
            battle_data.consecutive_attacks++;
            return CMD_RANGE_ATTACK;
        }

        // 거리 3+: 직선 마법 찬스
        if (my_mp >= 3) {
            if (my_y == opp_y && is_skill_unlocked(my_secret_key, CMD_H_ATTACK)) {
                set_custom_secrete_message(my_secret_key, "가로 마법 폭격!");
                battle_data.consecutive_attacks++;
                return CMD_H_ATTACK;
            }
            if (my_x == opp_x && is_skill_unlocked(my_secret_key, CMD_V_ATTACK)) {
                set_custom_secrete_message(my_secret_key, "세로 마법 폭격!");
                battle_data.consecutive_attacks++;
                return CMD_V_ATTACK;
            }
        }
    }

    // =================================================================================
    // [Phase 4] 중립 전략 (30 <= aggression < 60)
    // =================================================================================

    if (aggression >= 30) {

        // 거리 1: 안정적 공격
        if (dist == 1) {

            // MP 있으면 STRIKE
            if (my_mp >= 2 && is_skill_unlocked(my_secret_key, CMD_STRIKE)) {
                set_custom_secrete_message(my_secret_key, "안정적 강타");
                return CMD_STRIKE;
            }

            // 기본 공격
            set_custom_secrete_message(my_secret_key, "견제 공격");
            return CMD_ATTACK;
        }

        // 거리 2: 선택적 원거리 공격
        if (dist == 2 && my_mp >= 2 && is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK)) {
            set_custom_secrete_message(my_secret_key, "견제");
            return CMD_RANGE_ATTACK;
        }
    }

    // =================================================================================
    // [Phase 5] 방어 모드 (aggression < 30)
    // =================================================================================

    if (aggression < 30) {

        // 회복 우선
        if (my_hp <= 4 && my_mp >= 1) {
            battle_data.consecutive_attacks = 0;
            set_custom_secrete_message(my_secret_key, "회복 모드");

            if (my_mp >= 2 && my_hp <= 3 && is_skill_unlocked(my_secret_key, CMD_HEAL_ALL)) {
                return CMD_HEAL_ALL;
            }
            return CMD_HEAL;
        }

        // 거리 1이면 일단 공격 (공격이 최선의 방어)
        if (dist == 1) {
            set_custom_secrete_message(my_secret_key, "방어적 공격");
            return CMD_ATTACK;
        }

        // 거리 2 유지하며 견제
        if (dist == 2 && my_mp >= 1 && is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK)) {
            set_custom_secrete_message(my_secret_key, "안전 거리 견제");
            return CMD_RANGE_ATTACK;
        }

        // MP 회복 필요
        if (my_mp <= 2 && dist >= 3) {
            battle_data.consecutive_attacks = 0;
            set_custom_secrete_message(my_secret_key, "MP 충전");
            return CMD_REST;
        }
    }

    // =================================================================================
    // [Phase 6] 기동 전략
    // =================================================================================

    // BLINK: 빠른 접근 (공격성 높을 때)
    if (aggression >= 70 && my_mp >= 2 && dist >= 4) {

        if (my_y > opp_y && is_skill_unlocked(my_secret_key, CMD_BLINK_DOWN)) {
            set_custom_secrete_message(my_secret_key, "점멸 돌진!");
            return CMD_BLINK_DOWN;
        }
        if (my_y < opp_y && is_skill_unlocked(my_secret_key, CMD_BLINK_UP)) {
            set_custom_secrete_message(my_secret_key, "점멸 돌진!");
            return CMD_BLINK_UP;
        }
        if (my_x < opp_x && is_skill_unlocked(my_secret_key, CMD_BLINK_RIGHT)) {
            set_custom_secrete_message(my_secret_key, "점멸 돌진!");
            return CMD_BLINK_RIGHT;
        }
        if (my_x > opp_x && is_skill_unlocked(my_secret_key, CMD_BLINK_LEFT)) {
            set_custom_secrete_message(my_secret_key, "점멸 돌진!");
            return CMD_BLINK_LEFT;
        }
    }

    // 일반 이동: 공격성에 따라 추격 or 후퇴
    if (aggression >= 40) {
        // 추격
        set_custom_secrete_message(my_secret_key, "추격");
        if (my_x < opp_x) return CMD_RIGHT;
        if (my_x > opp_x) return CMD_LEFT;
        if (my_y < opp_y) return CMD_DOWN;
        if (my_y > opp_y) return CMD_UP;
    }
    else {
        // 거리 유지/후퇴
        if (dist <= 2) {
            set_custom_secrete_message(my_secret_key, "거리 확보");
            if (my_x < opp_x) return CMD_LEFT;
            if (my_x > opp_x) return CMD_RIGHT;
            if (my_y < opp_y) return CMD_UP;
            if (my_y > opp_y) return CMD_DOWN;
        }
        else {
            set_custom_secrete_message(my_secret_key, "조심스러운 접근");
            if (my_x < opp_x) return CMD_RIGHT;
            if (my_x > opp_x) return CMD_LEFT;
            if (my_y < opp_y) return CMD_DOWN;
            if (my_y > opp_y) return CMD_UP;
        }
    }

    // =================================================================================
    // [Phase 7] 버프 및 기타
    // =================================================================================

    // BLESS: MP 충분하고 안전할 때
    if (my_mp >= 6 && dist >= 3 && is_skill_unlocked(my_secret_key, CMD_BLESS)) {
        set_custom_secrete_message(my_secret_key, "강화!");
        return CMD_BLESS;
    }

    // 기본값
    battle_data.consecutive_attacks = 0;
    return CMD_REST;
}

// =================================================================================================
// [시스템 진입점]
// =================================================================================================

void student1_ai_entry() {

    // AI 등록
    my_secret_key = register_player_ai("TEAM-ALPHA-RUSH", aggressive_rush_ai);

    printf("\n>>> TEAM-ALPHA 스킬 해금 시작 <<<\n");

    // 스킬 해금 (기존 하드코딩된 답 사용)
    attempt_skill_unlock(my_secret_key, CMD_POISON, "Ancient_Relic|Doom_Greatsword|Immortal_Sword");
    if (is_skill_unlocked(my_secret_key, CMD_POISON))
        printf("TEAM-ALPHA : CMD_POISON 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_POISON 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_STRIKE, "2key");
    if (is_skill_unlocked(my_secret_key, CMD_STRIKE))
        printf("TEAM-ALPHA : CMD_STRIKE 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_STRIKE 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_BLINK_DOWN, "*A**C**F**T*");
    if (is_skill_unlocked(my_secret_key, CMD_BLINK_DOWN))
        printf("TEAM-ALPHA : CMD_BLINK 4종 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_BLINK 4종 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_HEAL_ALL, "*H*");
    if (is_skill_unlocked(my_secret_key, CMD_HEAL_ALL))
        printf("TEAM-ALPHA : CMD_HEAL_ALL 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_HEAL_ALL 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_RANGE_ATTACK, "\"LOT,A\"");
    if (is_skill_unlocked(my_secret_key, CMD_RANGE_ATTACK))
        printf("TEAM-ALPHA : CMD_RANGE_ATTACK 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_RANGE_ATTACK 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_BLESS, "T");
    if (is_skill_unlocked(my_secret_key, CMD_BLESS))
        printf("TEAM-ALPHA : CMD_BLESS 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_BLESS 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_H_ATTACK, "Inf_03");
    if (is_skill_unlocked(my_secret_key, CMD_H_ATTACK))
        printf("TEAM-ALPHA : CMD_H_ATTACK,CMD_V_ATTACK 해금 완료\n");
    else
        printf("TEAM-ALPHA : CMD_H_ATTACK,CMD_V_ATTACK 해금 실패 ㅜㅜ\n");

    attempt_skill_unlock(my_secret_key, CMD_SECRETE, "wn_St");
    if (is_skill_unlocked(my_secret_key, CMD_SECRETE))
    {
        printf("TEAM-ALPHA : CMD_SECRETE 해금 완료\n");
        set_custom_secrete_message(my_secret_key, "공격적 러시 준비 완료!");
    }
    else
        printf("TEAM-ALPHA : CMD_SECRETE 해금 실패 ㅜㅜ\n");

    printf("\n[공격적 러시 AI] 초기화 완료!\n");
    getchar();
}