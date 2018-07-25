#include "Chip8.h"

unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


Chip8::Chip8():PC(0x200),INDEX(0),current(0),SP(0),DELAY(0),SOUND(0),update(false),cpuEmu(nullptr),render(nullptr),input(nullptr),timer(nullptr){
	this->console=GetConsoleWindow();
	
	GetWindowRect(this->console, &this->r);
	MoveWindow(this->console,this->r.left,this->r.top,550,490,TRUE);

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hOut, &SBInfo);
    NewSBSize.X = SBInfo.dwSize.X - 2;
    NewSBSize.Y = SBInfo.dwSize.Y;
	SetConsoleScreenBufferSize(hOut, NewSBSize);

	CursorInfo.dwSize=100;
	CursorInfo.bVisible=false;
	SetConsoleCursorInfo(hOut,&CursorInfo);

	for(int i=0; i < 4096; i++){
		if(i<16){
			REG[i]=0;
			STACK[i]=0;
			KEY[i]=0;
		}
		if(i<64*32)
			SCREEN[i/64][i%64]=0;
		if(i<80)
			RAM[i]=chip8_fontset[i];
		else
			RAM[i]=0;
	}
	KEYMAP[0x1] = '1';
	KEYMAP[0x2] = '2';
	KEYMAP[0x3] = '3';
	KEYMAP[0xC] = '4';
	KEYMAP[0x4] = 'q';
	KEYMAP[0x5] = 'w';
	KEYMAP[0x6] = 'e';
	KEYMAP[0xD] = 'r';
	KEYMAP[0x7] = 'a';
	KEYMAP[0x8] = 's';
	KEYMAP[0x9] = 'd';
	KEYMAP[0xE] = 'f';
	KEYMAP[0xa] = 'z';
	KEYMAP[0x0] = 'x';
	KEYMAP[0xB] = 'c';
	KEYMAP[0xF] = 'v';
};

Chip8::~Chip8(){
	//delete[] SCREEN;
	delete cpuEmu;
	delete render;
	delete input;
}


void Chip8::loadProgram(char* name){
	std::ifstream in;

	in.open(name,std::ios::binary);
	
	int i = 0x200;
	
	in.seekg (0, std::ios::end);
	
	int length = static_cast<int>(in.tellg());
	
	if(length < 4096 - 0x200){
		in.seekg (0, std::ios::beg);
		in.read (reinterpret_cast<char *>(&RAM[0x200]),length);
        std::cout << "Program Loaded" << std::endl;
	}
	else{
		std::cout<<"File is too large"<<std::endl;
	}
	in.close();
}

bool isKeyPressed(char key){
	return ((GetAsyncKeyState(key)>>15) & 0x1);
}

void Chip8::loopCycle(){
	std::chrono::time_point<std::chrono::high_resolution_clock> time;
	time=std::chrono::high_resolution_clock::now();
	bool pause=false;
	while(!*finished){
		//KEY[0x4] = isKeyPressed('1');
		//KEY[0x5] = isKeyPressed('2');
		//KEY[0x6] = isKeyPressed('3');
		//KEY[0xD] = isKeyPressed('4');
		KEY[0x1] = isKeyPressed('1');
		KEY[0x2] = isKeyPressed('2');
		KEY[0x3] = isKeyPressed('3');
		KEY[0xC] = isKeyPressed('4');

		KEY[0x4] = isKeyPressed('Q');
		KEY[0x5] = isKeyPressed('W');
		KEY[0x6] = isKeyPressed('E');
		KEY[0xD] = isKeyPressed('R');
		
		KEY[0x7] = isKeyPressed('A');
		KEY[0x8] = isKeyPressed('S');
		KEY[0x9] = isKeyPressed('D');
		KEY[0xE] = isKeyPressed('F');

		KEY[0xA] = isKeyPressed('Z');
		KEY[0x0] = isKeyPressed('X');
		KEY[0xB] = isKeyPressed('C');
		KEY[0xF] = isKeyPressed('V');
		cycle();
		if(update){
			update=false;
			if(render!=nullptr && render->joinable()){
				render->join();
				delete render;
			}
			render=new std::thread(&Chip8::display,this);
		}

		if(isKeyPressed('0'))
			*finished=true;
		if(isKeyPressed('9'))
			while(!isKeyPressed('8'));
		std::this_thread::sleep_until(time);
		time+=std::chrono::milliseconds(1000/CPU_CLOCK);
			//std::this_thread::sleep_for((time-std::chrono::high_resolution_clock::now()).count() >0 ? time-std::chrono::high_resolution_clock::now() : std::chrono::milliseconds(0));
			//std::chrono::milliseconds(16));//1000/CPU_CLOCK));
	}
}

void Chip8::loopInput(){
	while(!*finished){
		KEY[0x4] = isKeyPressed('1');
		KEY[0x5] = isKeyPressed('2');
		KEY[0x6] = isKeyPressed('3');
		KEY[0xD] = isKeyPressed('4');

		//KEY[0x1] = isKeyPressed('q');
		//KEY[0x2] = isKeyPressed('w');
		//KEY[0x3] = isKeyPressed('e');
		//KEY[0xD] = isKeyPressed('r');
		
		//KEY[0x7] = isKeyPressed('a');
		//KEY[0x8] = isKeyPressed('s');
		//KEY[0x9] = isKeyPressed('d');
		//KEY[0xE] = isKeyPressed('f');
		//KEY[0xa] = isKeyPressed('z');
		//KEY[0x0] = isKeyPressed('x');
		//KEY[0xB] = isKeyPressed('c');
		//KEY[0xF] = isKeyPressed('v');
		if(isKeyPressed('0'))
			*finished=true;
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		/*for(int i=0;i <= 0xF; i++){
			if(GetAsyncKeyState(/*static_cast<int>(KEYMAP[i])) & 0x8000)
				KEY[i]=1;
			else
				KEY[i]=0;
		}
		if(GetAsyncKeyState(static_cast<int>('0')) & 0x8000)
			*finished=true;*/
/*		DWORD        mode;          /* Preserved console mode 
		INPUT_RECORD event;         /* Input event 
		/* Get the console input handle 
		HANDLE hstdin = GetStdHandle( STD_INPUT_HANDLE );

		/* Preserve the original console mode 
		GetConsoleMode( hstdin, &mode );

		/* Set to no line-buffering, no echo, no special-key-processing 
		SetConsoleMode( hstdin, 0 );

		if (WaitForSingleObject( hstdin, 0 ) == WAIT_OBJECT_0){
            DWORD count;  /* ignored 

            /* Get the input event 
            ReadConsoleInput( hstdin, &event, 1, &count );

            /* Only respond to key release events 
            if ((event.EventType == KEY_EVENT)){
            //&&  (!event.Event.KeyEvent.bKeyDown ))
				for(int i=0; i < 0xF; i++){
					if(event.Event.KeyEvent.wVirtualKeyCode==KEYMAP[i]){
						if(event.Event.KeyEvent.bKeyDown){
							KEY[i]=true;
						}
						else{
							KEY[i]=false;
						}
					}
				}
				if(event.Event.KeyEvent.wVirtualKeyCode=='0'){
					*finished=true;
				}
			}
              
        }
	}*/
	}
}

void Chip8::start(bool *finished=false){
	this->finished=finished;
	
	if(cpuEmu)
		delete cpuEmu;
	cpuEmu=new std::thread(&Chip8::loopCycle,this);
	
	//if(input)
	//	delete input;
	//input=new std::thread(&Chip8::loopInput,this);

	if(timer)
		delete timer;
	timer=new std::thread([this]{
		while(!*this->finished){
			if(this->DELAY)
				--this->DELAY;
			//if(this->SOUND==1);
				//std::cout<<"BEEP!"<<std::endl;
			if(this->SOUND)
				--this->SOUND;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
		}
	});
}

void Chip8::join(){
	if(cpuEmu && cpuEmu->joinable())
		cpuEmu->join();
	if(render && render->joinable())
		render->join();
	if(input && input->joinable())
		input->join();
	if(timer && timer->joinable())
		timer->join();
}


void Chip8::display(){
		char tmp[65];
		COORD pos={0,0};
		SetConsoleCursorPosition(hOut,pos);
		for(int y=0; y < 32; y++){
			if(SCREEN[y][64]){
				SCREEN[y][64]=0;
				pos.Y=y;
				for(int x=0; x < 64; x++){
					tmp[x]=(SCREEN[y][x] ? (219) : ' ');
				}
				tmp[64]='\0';
				SetConsoleCursorPosition(hOut,pos);
				WriteFile(hOut,tmp,strlen(tmp),NULL,NULL);
			}
			pos.Y=33;
			SetConsoleCursorPosition(hOut,pos);
		}
}

void Chip8::cycle(){
	if(PC < 0 || PC >= 4096)
		std::cerr<<"Accessed Out of Range Memory"<<std::endl;
	current=RAM[PC]<<8 | RAM[PC+1];
	if(((current >> 12) & 0xF) > 0xF)
		std::cout<<std::ios::hex<<current<<std::ios::dec<<std::endl;
	else
		(OPCODES[current>>12 & 0xF])();
}
/*
void Chip8::case0(){
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
}*/