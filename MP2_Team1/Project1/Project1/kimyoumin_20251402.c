//학생의 Action의 return 값은 
//return Wait(hp, name);
//return Heal(hp, name);
//둘중 하나여야함

int KUM(int hp, char* name)
{
	int r = rand() % 10;

	if (hp > 50) {
		return Wait(hp, name);
		/*if (hp < 50) {
			if (r < 7) {
				return Heal(hp, name);
			}
			return Wait(hp, name);
		}
		else {
			return Wait(hp, name);
		}무한으로 버팀.. 왜지*/
	}
	if (hp < 50) {
		if (r < 7) {
			return Heal(hp, name);
		}
		return Wait(hp, name);
	}
	else {
		return Wait(hp, name);
	}
}