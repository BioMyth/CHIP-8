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


Chip8::Chip8() :Emu(4096, 64, 32), PC(0x200), INDEX(0), current(0), SP(0), DELAY(0), SOUND(0), update(false) {
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 64; x++)
            SCREEN[y][x] = 0;
    for (int i = 0; i < 16; i++) {
        REG[i] = 0;
        STACK[i] = 0;
        KEY[i] = 0;
    }

    memcpy(&RAM[0], chip8_fontset, 80);
    paused = true;
    accumulator = 0;
    timer = 0;

	KEYMAP[0x1] = '1';
	KEYMAP[0x2] = '2';
	KEYMAP[0x3] = '3';
	KEYMAP[0xC] = '4';
	KEYMAP[0x4] = 'Q';
	KEYMAP[0x5] = 'W';
	KEYMAP[0x6] = 'E';
	KEYMAP[0xD] = 'R';
	KEYMAP[0x7] = 'A';
	KEYMAP[0x8] = 'S';
	KEYMAP[0x9] = 'D';
	KEYMAP[0xE] = 'F';
	KEYMAP[0xa] = 'Z';
	KEYMAP[0x0] = 'X';
	KEYMAP[0xB] = 'C';
	KEYMAP[0xF] = 'V';

    InitializeChip8OpCodes(codes);
#ifdef DEBUG
#ifdef OPCODE_MAP
    output.open("map.rec");
#else
    output.open("old.rec");
#endif
#endif // DEBUG

};

Chip8::~Chip8(){
    finished = true;
#ifdef DEBUG
    output.close();
#endif // DEBUG
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

void Chip8::cycle(float delta){

    accumulator += delta;
    timer += delta;
    renderTimer += delta;

    if (update) {
        gpu.setBuffer(SCREEN);
        updateScreen();
        update = false;
    }
        
    if (timer >= 1000.f / 60) {
        timer -= 1000.f / 60;
        if (this->DELAY)
            this->DELAY--;
        if (this->SOUND)
            this->SOUND--;
    }

    if (accumulator >= 1000.f / clock) {
        accumulator -= 1000.f / clock;
        updateInput();
        step();
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


void Chip8::step(){

    if (this->SOUND) /*Play Sound*/;
#ifdef OPCODE_MAP
    current = RAM[PC++]<<8 | RAM[PC++];

    opcode_size key;
    if ( codes.keyExists(key = current & 0xFFFF) );
    else if (codes.keyExists(key = current & 0xF0FF) );
    else if (codes.keyExists(key = current & 0xF00F) );
    else if (codes.keyExists(key = current & 0xF000) );
    else {
        //PC += 2;
        return;
    }
    if (key & 0xFF == 0xEE)
        OutputDebugString(L"Bad Call");
    codes[key](current, *this);
#else
    
    current = RAM[PC] << 8 | RAM[PC + 1];
    if (((current >> 12) & 0xF) > 0xF)
        throw std::runtime_error("Invalid instruction");
	
    else
		(OPCODES[current>>12 & 0xF])(current);
#endif  
#ifdef DEBUG
    output << "0x" << std::hex << static_cast<unsigned int>(current) << "\t" << std::hex << PC << "\t" << std::hex << STACK[SP] << std::endl;
#endif
}
