// HJA의 행동 패턴 설명
// - 현재 HP가 50 이하일 때는 안정적인 "힐(Heal)" 사용 확률이 높음
// - HP가 50 이상이면 공격적인 "대기(Wait)" 선택 확률이 조금 더 높음
// - 결국 전체적인 행동은 상황 기반 확률 행동 AI처럼 보임
// - 마지막 return 값은 반드시 Wait() 또는 Heal()을 호출해야 한다 (교수님 규칙)

// hp : 현재 체력
// name : 플레이어 이름

int HJA(int hp, char* name)
{
	int r = rand() % 100; // 0~99 난수 생성

	// ---------------------------
	// 1) HP가 낮을수록 무조건적인 생존 우선 전략
	// ---------------------------

	if (hp <= 50)

		// HP가 낮으면 Heal 확률 70%, Wait 확률 30%
	{
		if (r <= 70)
		{
			printf("[%s] HP가 낮아 생존 우선!(Heal 선택)\n", name);
			return Heal(hp, name);
		}
		else
		{
			printf("[%s] 위험하지만 카운터를 노림(Wait 선택)\n", name);
			return Wait(hp, name);
		}
	}

	// ---------------------------
	// 2) HP가 여유 있을 때는 공격적 행동
	// ---------------------------

	else
	{
		// HP가 높을 때 Heal 30%, Wait 70%
		if (r < 70)
		{
			printf("[%s] 체력이 충분하여 공격적!(Wait 선택)\n", name);
			return Wait(hp, name);
		}
		else
		{
			printf("[%s] 안정적인 플레이 선택(Heal 선택)\n", name);
			return Heal(hp, name);
		}
	}
}