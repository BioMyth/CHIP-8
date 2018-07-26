#include <iostream>
#include "Chip8.h"

int main(){
	Chip8 tmp;
	bool hasEnded=false;
    bool programLoaded = false;
    while (true){
        bool stop = true;
        std::cout << "Press the number next to the program name to load it:" << std::endl
            << "1. invaders" << std::endl
            << "2. pong" << std::endl
            << "3. tetris" << std::endl;
        char in = '\0';
        std::cin >> in;
        switch (in) {
        case '1':
            tmp.loadProgram("invaders.c8");
            break;
        case '2':
            tmp.loadProgram("pong2.c8");
            break;
        case '3':
            tmp.loadProgram("tetris.c8");
            break;
        default:
            stop = false;
            break;
        }
        if (stop) break;
    }
    std::cout << "Starting" << std::endl;
	tmp.start(&hasEnded);
    std::cout << "Joining" << std::endl;
	tmp.join();
	std::cout<<"All Done"<<std::endl;
	std::cin.ignore();
	return 0;
}