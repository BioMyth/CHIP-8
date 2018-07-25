#pragma once
#include "Chip8.h"

void (*OPCODES[16])(Chip8*) = {
[](Chip8* parent){
	switch(parent->current & 0x000F){
	case(0x0E):			//Clears the screen.
			parent->PC=parent->STACK[parent->SP--];
			break;
	case(0x00):			//Returns from a subroutine.
		for(int y=0; y < 32; y++){
			for(int x=0; x < 64; x++)
				parent->SCREEN[y][x]=0;
			parent->SCREEN[y][64]=1;
		}
		//parent->update=true;
		break;
	default:
		std::cout<<"Could not find opcode(0x00): "<<std::hex<<std::uppercase<< parent->current <<std::nouppercase<<std::dec<<std::endl;
		break;	
	}
	parent->PC+=2;
},

// Jumps to address NNN.
[](Chip8* parent){
		parent->PC=parent->current & 0x0FFF;
},

// Calls subroutine at NNN.
[](Chip8* parent){
		parent->STACK[++parent->SP]=parent->PC;
		parent->PC=parent->current & 0x0FFF;
},

// Skips the next instruction if REG[X] equals NN.
[](Chip8* parent){
		parent->PC+=(parent->REG[parent->current>>8 & 0x0F] == (0x00FF & parent->current) ? 4 : 2);
},

// Skips the next instruction if REG[X] doesn't equal NN.
[](Chip8* parent){
		parent->PC+=(parent->REG[parent->current>>8 & 0x0F] != (0x00FF & parent->current) ? 4 : 2);
},

// Skips the next instruction if REG[X] equals REG[Y].
[](Chip8* parent){
		parent->PC+=(parent->REG[parent->current>>8 & 0x0F] == parent->REG[parent->current>>4 & 0x0F] ? 4 : 2);
},

// Sets REG[X] to NN.
[](Chip8* parent){
		parent->REG[parent->current>>8 & 0x0F]=parent->current & 0x00FF;
		parent->PC+=2;
},

// Adds NN to REG[X].
[](Chip8* parent){
		parent->REG[parent->current>>8 & 0x0F]+=parent->current & 0x00FF;
		parent->PC+=2;
},

[](Chip8* parent){
		switch(parent->current & 0x000F){
		case(0x0):				// Sets REG[X] to the value of REG[Y].
			parent->REG[parent->current>>8 & 0x0F]=parent->REG[parent->current>>4 & 0x0F];
			break;

		case(0x1):				// Sets VX to REG[X] or REG[Y].
			parent->REG[parent->current>>8 & 0x0F] |= parent->REG[parent->current>>4 & 0x0F];
			break;

		case(0x2):				// Sets REG[X] to REG[X] and REG[Y].
			parent->REG[parent->current>>8 & 0x0F] &= parent->REG[parent->current>>4 & 0x0F];
			break;

		case(0x3):				// 	Sets REG[X] to REG[X] xor REG[Y].
			parent->REG[parent->current>>8 & 0x0F]^=parent->REG[parent->current>>4 & 0x0F];
			break;

		case(0x4):				// 	Adds REG[Y] to REG[X]. REG[F] is set to 1 when there's a carry, and to 0 when there isn't.
			parent->REG[0xF]=(0xFF-parent->REG[parent->current>>8 & 0x0F] < parent->REG[parent->current>>4 & 0x0F]);
			parent->REG[parent->current>>8 & 0x0F]+=parent->REG[parent->current>>4 & 0x0F];
			break;

		case(0x5):				// REG[Y] is subtracted from REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
			parent->REG[0xF]=(parent->REG[parent->current>>8 & 0x0F] > parent->REG[parent->current>>4 & 0x0F]);
			parent->REG[parent->current>>8 & 0x0F]=parent->REG[parent->current>>8 & 0x0F] - parent->REG[parent->current>>4 & 0x0F];
			break;

		case(0x6):				// Shifts REG[X] right by one. REG[F] is set to the value of the least significant bit of REG[X] before the shift.
			parent->REG[0xF]=parent->REG[parent->current>>8 & 0x0F] & 0x01;
			parent->REG[parent->current>>8 & 0x0F]>>=1;
			break;

		case(0x7):				// Sets REG[X] to REG[Y] minus REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
			parent->REG[0xF]=(parent->REG[parent->current>>8 & 0x0F] < parent->REG[parent->current>>4 & 0x0F]);
			parent->REG[parent->current>>8 & 0x0F]=parent->REG[parent->current>>4 & 0x0F] - parent->REG[parent->current>>8 & 0x0F] ;
			break;

		case(0xE):				//	Shifts REG[X] left by one. REG[F] is set to the value of the most significant bit of REG[X] before the shift
			parent->REG[0xF]=parent->REG[parent->current>>8 & 0x0F]>>7;
			parent->REG[parent->current>>8 & 0x0F]<<=1;
			break;
		}
		parent->PC+=2;
},

// Skips the next instruction if REG[X] doesn't equal REG[Y].
[](Chip8* parent){
		parent->PC+=(parent->REG[parent->current>>8 & 0x0F]!=parent->REG[parent->current>>4 & 0x0F] ? 4 : 2);
},

// Sets INDEX to the address NNN.
[](Chip8* parent){
	parent->INDEX=0x0FFF & parent->current;
	parent->PC+=2;
},

// Jumps to the address NNN plus REG[0].
[](Chip8* parent){
	parent->PC=(0x0FFF & parent->current) + parent->REG[0];
},

// 	Sets REG[X] to the result of a bitwise and operation on a random number and NN.
[](Chip8* parent){
		parent->REG[parent->current>>8 & 0x0F]=rand() & (parent->current & 0x00FF);
		parent->PC+=2;
},

// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
// Each row of 8 pixels is read as bit-coded starting from memory location INDEX; INDEX value doesn’t change after parent Instruction. 
// REG[F] is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
[](Chip8* parent){
	byte baseX=parent->REG[(parent->current>>8) & 0x0F];
	byte baseY=parent->REG[(parent->current>>4) & 0x0F];
	bool tmp;
	parent->REG[0xF]=0;
	for(int y=0; y < (parent->current & 0xF); y++){
		for(int x=0; x < 8; x++){
			
			if (parent->SCREEN[baseY+y][baseX+x] && (parent->RAM[parent->INDEX+y]>>(7-x) & 0x1)){
				parent->REG[0xF]=1;
			}
			parent->SCREEN[baseY + y][baseX + x] ^= (parent->RAM[parent->INDEX + y] >> (7 - x) & 0x1); 
			parent->SCREEN[baseY+y][64]=1;
		}			
	}
	parent->update=true;
	parent->PC+=2;
},

[](Chip8* parent){
		switch(parent->current & 0xFF){
		case(0x9E):				// Skips the next instruction if the key stored in REG[X] is pressed.
			if(parent->KEY[parent->REG[parent->current>>8 & 0xF]])
				parent->PC+=2;
			break;
		case(0xA1):				//	Skips the next instruction if the key stored in REG[X] isn't pressed.
			if(!parent->KEY[parent->REG[parent->current>>8 & 0xF]])
				parent->PC+=2;
			break;
		}
		parent->PC+=2;
},
		
// 
[](Chip8* parent){
		switch(parent->current & 0xFF){
		case(0x07):
			parent->REG[parent->current>>8 & 0xF]=parent->DELAY;
			break;

		case(0x0A):
			{
			bool currentKeys[16];
			bool found=false;
			for(int i=0; i < 0xF; i++)
				currentKeys[i]=parent->KEY[i];
			for(int i=0; i < 0xF; i++){
				if(currentKeys[i]!=parent->KEY[i] && parent->KEY[i]){
					parent->REG[parent->current>>8 & 0xF]=i;
					found=true;
					break;
				}
			}
			if(found)
				parent->PC-=2;
			break;
			}
		case(0x15):
			parent->DELAY=parent->REG[parent->current>>8 & 0xF];
			break;

		case(0x18):
			parent->SOUND=parent->REG[parent->current>>8 & 0xF];
			break;

		case(0x1E):
			parent->INDEX+=parent->REG[parent->current>>8 & 0xF];
			break;

		case(0x29):
			parent->INDEX = parent->REG[(parent->current & 0x0F00) >> 8] * 0x5;
			break;

		case(0x33):
			parent->RAM[parent->INDEX]=parent->REG[parent->current>>8 & 0xF]/100;
			parent->RAM[parent->INDEX+1]=(parent->REG[parent->current>>8 & 0xF]%100)/10;
			parent->RAM[parent->INDEX+2]=parent->REG[parent->current>>8 & 0xF]%10;
			break;

		case(0x55):
			for(int i=0x0; i <= (parent->current>>8 & 0xF); i++)
				parent->RAM[parent->INDEX+i]=parent->REG[i];
			break;

		case(0x65):
			for(int i=0x0; i <= (parent->current>>8 & 0xF); i++)
				parent->REG[i]=parent->RAM[parent->INDEX+i];
			break;	
		}
		parent->PC+=2;
}

};