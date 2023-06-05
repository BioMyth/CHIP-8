#include <iostream>
#include "Chip8.h"

using namespace std;

int main(){
	Chip8 interpreter;
	int i=0;
	bool hasEnded=false;
	interpreter.loadProgram((char *)("./Roms/test_opcode.ch8"));
	interpreter.start(&hasEnded);
	interpreter.join();
	std::cout<<"All Done"<<std::endl;
	std::cin.ignore();
	return 0;
}
