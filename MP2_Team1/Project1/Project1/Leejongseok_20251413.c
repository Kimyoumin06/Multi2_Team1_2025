int LJS(int hp, char* name)
{
	if (hp <= 25) {
		return Heal(hp, name);
	}
	if (hp <= 50) {
		int r = rand() % 10;
		if (r < 7) {
			return Wait(hp, name);
		}
		else {
			return Heal(hp, name);
		}
	}
	return Wait(hp, name);
}