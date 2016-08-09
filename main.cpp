#include "video.h"
#include "cpu.h"

//got some help from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

int main() {
	Cpu cpu = Cpu();
	cpu.init();
	return 0;
}