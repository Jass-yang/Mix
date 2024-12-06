#include<Windows.h>


int main() {
	char str[] = "holleword";
	_asm {
		push 0;
		push 0;
		lea eax,[str];
		push eax;
		push 0;
		call MessageBoxA;
	}

	return 0;
}