#include <iostream>
#include "Chip8.h"

using namespace std;

int main(){
	Chip8 tmp;
	int i=0;
	bool hasEnded=false;
	tmp.loadProgram("invaders.c8");
	//tmp.loadProgram("pong2.c8");
	//tmp.loadProgram("tetris.c8");
	//std::cout<<static_cast<int>('s')<<std::endl;
	tmp.start(&hasEnded);
	//for(int i=40; i > 0; i--){
	//	std::this_thread::sleep_for(std::chrono::seconds(1));
		//std::cout<<i<<" seconds left"<<std::endl;
	//}asdf
	//hasEnded=true;
	tmp.join();
	std::cout<<"All Done"<<std::endl;
	std::cin.ignore();
	return 0;
}