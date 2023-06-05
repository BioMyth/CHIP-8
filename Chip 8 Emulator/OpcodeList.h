#ifndef _OPCODE_LIST_H_
#define _OPCODE_LIST_H_
#include "Chip8.h"
class Chip8;

void case0(Chip8 cpu){
	switch(cpu.current & 0x000F){
	case(0x0E):
			cpu.PC=cpu.STACK[cpu.SP--]+2;
			break;
	case(0x00):
		for(int i=0; i < 64*32; i++)
			cpu.SCREEN[i] = 0;
		cpu.update=true;
		cpu.PC+=2;
		break;
	default:
		std::cout<<"Could not find opcode(0x00): "<<std::hex<<std::uppercase<< cpu.current <<std::nouppercase<<std::dec<<std::endl;
		cpu.PC+=2;
		break;
	}
}
void case1(Chip8 cpu){
		cpu.PC=cpu.current & 0x0FFF;
}

void case2(Chip8 cpu){
		cpu.STACK[++cpu.SP]=cpu.PC;
		cpu.PC=cpu.current & 0x0FFF;
}

void case3(Chip8 cpu){
		cpu.PC+=(cpu.REG[cpu.current>>8 & 0x0F] == (0x00FF & cpu.current) ? 4 : 2);
}

void case4(Chip8 cpu){
		cpu.PC+=(cpu.REG[cpu.current>>8 & 0x0F] != (0x00FF & cpu.current) ? 2 : 4);
}

void case5(Chip8 cpu){
		cpu.PC+=(cpu.REG[cpu.current>>8 & 0x0F] == cpu.REG[cpu.current>>4 & 0x0F] ? 4 : 2);
}

void case6(Chip8 cpu){
		cpu.REG[cpu.current>>8 & 0x0F]=cpu.current & 0x00FF;
		cpu.PC+=2;
}

void case7(Chip8 cpu){
		cpu.REG[cpu.current>>8 & 0x0F]+=cpu.current & 0x00FF;
		cpu.PC+=2;
}

void case8(Chip8 cpu){
		switch(cpu.current & 0x000F){
		case(0x0):
			cpu.REG[cpu.current>>8 & 0x0F]=cpu.REG[cpu.current>>4 & 0x0F];
			break;

		case(0x1):
			cpu.REG[cpu.current>>8 & 0x0F]|=cpu.REG[cpu.current>>4 & 0x0F];
			break;

		case(0x2):
			cpu.REG[cpu.current>>8 & 0x0F]&=cpu.REG[cpu.current>>4 & 0x0F];
			break;

		case(0x3):
			cpu.REG[cpu.current>>8 & 0x0F]^=cpu.REG[cpu.current>>4 & 0x0F];
			break;

		case(0x4):
			cpu.REG[0xF]=(0xFF-cpu.REG[cpu.current>>8 & 0x0F] < cpu.REG[cpu.current>>4 & 0x0F]);
			cpu.REG[cpu.current>>8 & 0x0F]+=cpu.REG[cpu.current>>4 & 0x0F];
			break;

		case(0x5):
			cpu.REG[0xF]=(cpu.REG[cpu.current>>8 & 0x0F] > cpu.REG[cpu.current>>4 & 0x0F]);
			cpu.REG[cpu.current>>8 & 0x0F]=cpu.REG[cpu.current>>8 & 0x0F] - cpu.REG[cpu.current>>4 & 0x0F];
			break;

		case(0x6):
			cpu.REG[0xF]=cpu.REG[cpu.current>>8 & 0x0F] & 0x01;
			cpu.REG[cpu.current>>8 & 0x0F]>>=1;
			break;

		case(0x7):
			cpu.REG[0xF]=(cpu.REG[cpu.current>>8 & 0x0F] < cpu.REG[cpu.current>>4 & 0x0F]);
			cpu.REG[cpu.current>>8 & 0x0F]=cpu.REG[cpu.current>>4 & 0x0F] - cpu.REG[cpu.current>>8 & 0x0F] ;
			break;

		case(0xE):
			cpu.REG[0xF]=cpu.REG[cpu.current>>8 & 0x0F]>>7;
			cpu.REG[cpu.current>>8 & 0x0F]<<=1;
			break;
		}
		cpu.PC+=2;
}

void case9(Chip8 cpu){
		cpu.PC+=(cpu.REG[cpu.current>>8 & 0x0F]!=cpu.REG[cpu.current>>4 & 0x0F] ? 4 : 2);
}

void caseA(Chip8 cpu){
		cpu.INDEX=0x0FFF & cpu.current;
		cpu.PC+=2;
}
void caseB(Chip8 cpu){
		cpu.PC=(0x0FFF & cpu.current) + cpu.REG[0];
}

void caseC(Chip8 cpu){
		cpu.REG[cpu.current>>8 & 0x0F]=rand() & (cpu.current & 0x00FF);
		cpu.PC+=2;
}
void caseD(Chip8 cpu){
		cpu.REG[0xF]=0;
		for(int y=0; y < (cpu.current & 0xF); y++){
			for(int x=0; x < 8; x++){
				if (cpu.SCREEN[cpu.REG[cpu.current>>8 & 0x0F]+x+(cpu.REG[cpu.current>>4 & 0x0F]+y)*64] & cpu.RAM[cpu.INDEX+y]>>(7-x) & 0x1)
					cpu.REG[0xF]=1;
				cpu.SCREEN[cpu.REG[cpu.current>>8 & 0x0F]+x+(cpu.REG[cpu.current>>4 & 0x0F]+y)*64] ^= cpu.RAM[cpu.INDEX+y]>>(7-x) & 0x1;
			}
		}
		cpu.update=true;
		cpu.PC+=2;
}

void caseE(Chip8 cpu){
		switch(cpu.current & 0xFF){
		case(0x9E):
		//	cpu.PC+=2;
			break;
		case(0xA1):
			cpu.PC+=2;
			break;
		cpu.PC+=2;
}


void caseF(Chip8 cpu){
		switch(cpu.current & 0xFF){
		case(0x07):
			cpu.REG[cpu.current>>8 & 0xF]=cpu.DELAY;
			break;

		case(0x0A):
			std::cout<<"Input:";
			break;

		case(0x15):
			cpu.DELAY=cpu.REG[cpu.current>>8 & 0xF];
			break;

		case(0x18):
			cpu.SOUND=cpu.REG[cpu.current>>8 & 0xF];
			break;

		case(0x1E):
			cpu.INDEX+=cpu.REG[cpu.current>>8 & 0xF];
			break;

		case(0x29):
			cpu.INDEX = cpu.REG[(cpu.current & 0x0F00) >> 8] * 0x5;
			break;

		case(0x33):
			cpu.RAM[cpu.INDEX]=cpu.REG[cpu.current>>8 & 0xF]/100;
			cpu.RAM[cpu.INDEX+1]=(cpu.REG[cpu.current>>8 & 0xF]%100)/10;
			cpu.RAM[cpu.INDEX+2]=cpu.REG[cpu.current>>8 & 0xF]%10;
			break;

		case(0x55):
			for(int i=0x0; i <= (cpu.current>>8 & 0xF); i++)
				cpu.RAM[cpu.INDEX+i]=cpu.REG[i];
			break;

		case(0x65):
			for(int i=0x0; i <= (cpu.current>>8 & 0xF); i++)
				cpu.REG[i]=cpu.RAM[cpu.INDEX+i];
			break;
		}
		cpu.PC+=2;
}


void (*opcodes[15])(Chip8)={case0,case1,case2,case3,case4,case5,case6,case7,case8,case9,caseA,caseB,caseC,caseD,caseE,caseF};

#endif
