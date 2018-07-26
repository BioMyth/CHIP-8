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


Chip8::Chip8():PC(0x200),INDEX(0),current(0),SP(0),DELAY(0),SOUND(0),update(false),cpuEmu(nullptr),render(nullptr){
	this->console=GetConsoleWindow();
	
	GetWindowRect(this->console, &this->r);
	MoveWindow(this->console,this->r.left,this->r.top,550,600,TRUE);

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
		//if(i<80)
		//	RAM[i]=chip8_fontset[i];
	}
    memcpy(&RAM[0], chip8_fontset, 80);
    paused = true;
    accumulator = 0;
    timer = 0;

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
    *finished = true;
    join();
	delete[] SCREEN;
	delete cpuEmu;
	delete render;
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
        in.close();
	}
	else{
        in.close();
        throw std::length_error("File is too large to load into Chip8 memory");
	}
}

bool isKeyPressed(char key){
	return ((GetAsyncKeyState(key)>>15) & 0x1);
}

void Chip8::loopCycle(){
	//std::chrono::duration<float, std::milli> currentTime;
    std::chrono::duration<float, std::milli> lastTime;

    lastTime = std::chrono::steady_clock::now().time_since_epoch();
	
    while(!*finished){
        emuControls();
#ifdef DEBUG
        if (paused) std::cout << "Paused" << std::endl;
#endif
        if (paused) {
            lastTime = std::chrono::steady_clock::now().time_since_epoch();
            continue;
        }
        
        std::chrono::duration<float, std::milli> now = std::chrono::steady_clock::now().time_since_epoch();
        
        float delta = (now - lastTime).count();
        lastTime = now;
        accumulator += delta;
        timer += delta;
        
        if (timer >= 1000.0 / 60) {
            timer -= 1000.0 / 60;
            if (this->DELAY)
                this->DELAY--;
            if (this->SOUND)
                this->SOUND--;
        }

        if (accumulator >= 1000.0 / CPU_CLOCK) {
            accumulator -= 1000.0 / CPU_CLOCK;
            //if (accumulator > 1000.0 / CPU_CLOCK) abort();
            updateInput();
            cycle();
            if (update) {
                update = false;
                //if (render != nullptr && render->joinable()) {
                //    render->join();
                //    delete render;
                //}
                //render = new std::thread(&Chip8::display, this);
                std::thread(&Chip8::display, this).detach();
            }
        }
	}
}

void Chip8::updateInput(){
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
}

void Chip8::emuControls() {
    if (isKeyPressed('0')) *finished = true;
    if (isKeyPressed('9')) paused = true;
    if (paused) {
        if (isKeyPressed('8'))
            paused = false;
    }
    //save state
    //load state
}

void Chip8::start(bool *finished=false){
	this->finished=finished;
    this->paused = false;
	//if(cpuEmu)
	//	delete cpuEmu;
	//cpuEmu=new std::thread(&Chip8::loopCycle,this);
    loopCycle();
}

void Chip8::join(){
	if(cpuEmu && cpuEmu->joinable())
		cpuEmu->join();
	if(render && render->joinable())
		render->join();
	/*if(input && input->joinable())
		input->join();
	if(timer && timer->joinable())
		timer->join();*/
}


void Chip8::display(){
    char strBuffer[65 * 32];
		//char tmp[65];
		COORD pos={0,0};
		//SetConsoleCursorPosition(hOut,pos);
		for(int y=0; y < 32; y++){
			//if(SCREEN[y][64]){
			//	SCREEN[y][64]=0;
				pos.Y=y;
                //memcpy(tmp, SCREEN[y], 64);
				for(int x=0; x < 64; x++){
					//tmp[x]=(
                    strBuffer[y * 64 + x]   = (SCREEN[y][x] ? (219) : ' ');
				}
				//tmp[64]='\0';
                strBuffer[y * 65 + 64] = '\n';
				//SetConsoleCursorPosition(hOut,pos);
				//WriteFile(hOut,tmp,strlen(tmp),NULL,NULL);
			//}
			//pos.Y=33;
			//SetConsoleCursorPosition(hOut,pos);
		}
        strBuffer[65 * 32 - 1] = '\0';
        SetConsoleCursorPosition(hOut, pos);
        WriteFile(hOut, strBuffer, 65 * 32 - 1, NULL, NULL);
}

void Chip8::cycle(){

    if (this->SOUND) std::cout << "\a";
	current=RAM[PC]<<8 | RAM[PC+1];
	
    if (((current >> 12) & 0xF) > 0xF)
        std::cout << "Invalid instruction: " << std::ios::hex << current << std::ios::dec << std::endl;
	
    else
		(OPCODES[current>>12 & 0xF])();
#ifdef DEBUG
    std::cout << "Current instruction is " << std::ios::hex << current << " at location " << PC << std::ios::dec << std::endl;
#endif
}