void case0(CPU RAM){
	switch(this->current & 0x000F){
	case(0x0E):			//Clears the screen.
			this->PC=this->STACK[this->SP--];
			break;
	case(0x00):			//Returns from a subroutine.
		for(int y=0; y < 32; y++){
			for(int x=0; x < 64; x++)
				this->SCREEN[y][x]=0;
			this->SCREEN[y][64]=1;
		}
		//this->update=true;
		break;
	default:
		std::cout<<"Could not find opcode(0x00): "<<std::hex<<std::uppercase<< this->current <<std::nouppercase<<std::dec<<std::endl;
		break;	
	}
	this->PC+=2;
}

// Jumps to address NNN.
void Chip8::case1(){
		this->PC=this->current & 0x0FFF;
}

// Calls subroutine at NNN.
void Chip8::case2(){
		this->STACK[++this->SP]=this->PC;
		this->PC=this->current & 0x0FFF;
}

// Skips the next instruction if REG[X] equals NN.
void Chip8::case3(){
		this->PC+=(this->REG[this->current>>8 & 0x0F] == (0x00FF & this->current) ? 4 : 2);
}

// Skips the next instruction if REG[X] doesn't equal NN.
void Chip8::case4(){
		this->PC+=(this->REG[this->current>>8 & 0x0F] != (0x00FF & this->current) ? 4 : 2);
}

// Skips the next instruction if REG[X] equals REG[Y].
void Chip8::case5(){
		this->PC+=(this->REG[this->current>>8 & 0x0F] == this->REG[this->current>>4 & 0x0F] ? 4 : 2);
}

// Sets REG[X] to NN.
void Chip8::case6(){
		this->REG[this->current>>8 & 0x0F]=this->current & 0x00FF;
		this->PC+=2;
}

// Adds NN to REG[X].
void Chip8::case7(){
		this->REG[this->current>>8 & 0x0F]+=this->current & 0x00FF;
		this->PC+=2;
}

void Chip8::case8(){
		switch(this->current & 0x000F){
		case(0x0):				// Sets REG[X] to the value of REG[Y].
			this->REG[this->current>>8 & 0x0F]=this->REG[this->current>>4 & 0x0F];
			break;

		case(0x1):				// Sets VX to REG[X] or REG[Y].
			this->REG[this->current>>8 & 0x0F] |= this->REG[this->current>>4 & 0x0F];
			break;

		case(0x2):				// Sets REG[X] to REG[X] and REG[Y].
			this->REG[this->current>>8 & 0x0F] &= this->REG[this->current>>4 & 0x0F];
			break;

		case(0x3):				// 	Sets REG[X] to REG[X] xor REG[Y].
			this->REG[this->current>>8 & 0x0F]^=this->REG[this->current>>4 & 0x0F];
			break;

		case(0x4):				// 	Adds REG[Y] to REG[X]. REG[F] is set to 1 when there's a carry, and to 0 when there isn't.
			this->REG[0xF]=(0xFF-this->REG[this->current>>8 & 0x0F] < this->REG[this->current>>4 & 0x0F]);
			this->REG[this->current>>8 & 0x0F]+=this->REG[this->current>>4 & 0x0F];
			break;

		case(0x5):				// REG[Y] is subtracted from REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
			this->REG[0xF]=(this->REG[this->current>>8 & 0x0F] > this->REG[this->current>>4 & 0x0F]);
			this->REG[this->current>>8 & 0x0F]=this->REG[this->current>>8 & 0x0F] - this->REG[this->current>>4 & 0x0F];
			break;

		case(0x6):				// Shifts REG[X] right by one. REG[F] is set to the value of the least significant bit of REG[X] before the shift.
			this->REG[0xF]=this->REG[this->current>>8 & 0x0F] & 0x01;
			this->REG[this->current>>8 & 0x0F]>>=1;
			break;

		case(0x7):				// Sets REG[X] to REG[Y] minus REG[X]. REG[F] is set to 0 when there's a borrow, and 1 when there isn't.
			this->REG[0xF]=(this->REG[this->current>>8 & 0x0F] < this->REG[this->current>>4 & 0x0F]);
			this->REG[this->current>>8 & 0x0F]=this->REG[this->current>>4 & 0x0F] - this->REG[this->current>>8 & 0x0F] ;
			break;

		case(0xE):				//	Shifts REG[X] left by one. REG[F] is set to the value of the most significant bit of REG[X] before the shift
			this->REG[0xF]=this->REG[this->current>>8 & 0x0F]>>7;
			this->REG[this->current>>8 & 0x0F]<<=1;
			break;
		}
		this->PC+=2;
}

// Skips the next instruction if REG[X] doesn't equal REG[Y].
void Chip8::case9(){
		this->PC+=(this->REG[this->current>>8 & 0x0F]!=this->REG[this->current>>4 & 0x0F] ? 4 : 2);}

// Sets INDEX to the address NNN.
void Chip8::caseA(){
	this->INDEX=0x0FFF & this->current;
	this->PC+=2;}

// Jumps to the address NNN plus REG[0].
void Chip8::caseB(){
	this->PC=(0x0FFF & this->current) + this->REG[0];}

// 	Sets REG[X] to the result of a bitwise and operation on a random number and NN.
void Chip8::caseC(){
		this->REG[this->current>>8 & 0x0F]=rand() & (this->current & 0x00FF);
		this->PC+=2;
}

// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
// Each row of 8 pixels is read as bit-coded starting from memory location INDEX; INDEX value doesn’t change after this Instruction. 
// REG[F] is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
void Chip8::caseD(){
	byte baseX=this->REG[(this->current>>8) & 0x0F];
	byte baseY=this->REG[(this->current>>4) & 0x0F];
	bool tmp;
	this->REG[0xF]=0;
	for(int y=0; y < (this->current & 0xF); y++){
		for(int x=0; x < 8; x++){
			
			if (this->SCREEN[baseY+y][baseX+x] && (this->RAM[INDEX+y]>>(7-x) & 0x1)){
				this->REG[0xF]=1;
			}
			this->SCREEN[baseY + y][baseX + x] ^= (this->RAM[INDEX + y] >> (7 - x) & 0x1); 
			this->SCREEN[baseY+y][64]=1;
		}			
	}
	this->update=true;
	this->PC+=2;
}

void Chip8::caseE(){
		switch(this->current & 0xFF){
		case(0x9E):				// Skips the next instruction if the key stored in REG[X] is pressed.
			if(KEY[REG[current>>8 & 0xF]])
				PC+=2;
			break;
		case(0xA1):				//	Skips the next instruction if the key stored in REG[X] isn't pressed.
			if(!KEY[REG[current>>8 & 0xF]])
				PC+=2;
			break;
		}
		this->PC+=2;
}
		
// 
void Chip8::caseF(){
		switch(this->current & 0xFF){
		case(0x07):
			this->REG[this->current>>8 & 0xF]=this->DELAY;
			break;

		case(0x0A):
			{
			bool currentKeys[16];
			bool found=false;
			for(int i=0; i < 0xF; i++)
				currentKeys[i]=KEY[i];
			for(int i=0; i < 0xF; i++){
				if(currentKeys[i]!=KEY[i] && KEY[i]){
					REG[current>>8 & 0xF]=i;
					found=true;
					break;
				}
			}
			if(found)
				PC-=2;
			break;
			}
		case(0x15):
			this->DELAY=this->REG[this->current>>8 & 0xF];
			break;

		case(0x18):
			this->SOUND=this->REG[this->current>>8 & 0xF];
			break;

		case(0x1E):
			this->INDEX+=this->REG[this->current>>8 & 0xF];
			break;

		case(0x29):
			this->INDEX = this->REG[(this->current & 0x0F00) >> 8] * 0x5;
			break;

		case(0x33):
			this->RAM[this->INDEX]=this->REG[this->current>>8 & 0xF]/100;
			this->RAM[this->INDEX+1]=(this->REG[this->current>>8 & 0xF]%100)/10;
			this->RAM[this->INDEX+2]=this->REG[this->current>>8 & 0xF]%10;
			break;

		case(0x55):
			for(int i=0x0; i <= (this->current>>8 & 0xF); i++)
				this->RAM[this->INDEX+i]=this->REG[i];
			break;

		case(0x65):
			for(int i=0x0; i <= (this->current>>8 & 0xF); i++)
				this->REG[i]=this->RAM[this->INDEX+i];
			break;	
		}
		this->PC+=2;
}