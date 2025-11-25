int KYM(int hp, char* name)
{
    if (hp <= 25)
    {
        printf("[%s]생명이 위태롭다!(Heal 선택)/n", name);
        return Heal(hp, name);
    }

    if (hp <= 50)
    {
        printf("[%s]위험한 체력.. 방어 우선(Wait 선택)/n", name);
        return Wait(hp, name);
    }

    if (hp <= 75)
    {
        printf("[%s]아직 괜찮지만.. 대비 필요!(Heal 선택)/n", name);
        return Heal(hp, name);
    }

    printf("[%s]체력 넉넉! 공격적 플레이(Wait 선택)/n", name);
    return Wait(hp, name);
}