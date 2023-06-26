#include "clove-unit.h"

#include "emulator.h"

#define CLOVE_SUITE_NAME TestEmulator

#pragma region MOCKS
// class used to mock the keyboard state from SDL and be able to pass custom keyboard states
class MockKeyboardStateInputCommand : public chipotto::IInputCommand
{
public:
	virtual const uint8_t *GetKeyboardState() override{return fake_keyboard_state;}
    ~MockKeyboardStateInputCommand(){if(fake_keyboard_state) delete fake_keyboard_state;}

	uint8_t *fake_keyboard_state = nullptr;
};

class MockRandomGenerator : public chipotto::IRandomGenerator
{
public:
    virtual uint8_t GetRandomByte() override { return 0xFF;}
};

#pragma endregion //MOCKS


chipotto::Emulator* emulator = nullptr;

CLOVE_SUITE_SETUP_ONCE()
{
    emulator = new chipotto::Emulator();
}

CLOVE_SUITE_TEARDOWN()
{
    emulator->HardResetEmulator();
}

CLOVE_SUITE_TEARDOWN_ONCE()
{
    delete emulator;
}

#pragma region TESTS

CLOVE_TEST(CLS)
{
    SDL_Texture* texture = emulator->GetTexture();
    int height = emulator->GetHeight();
    int pitch;
    uint8_t* pixels = nullptr;
    if(SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch) != 0)
    {
        CLOVE_FAIL();
    }
    memset(pixels, 0xFF, pitch * height);    // texture is not clean
    SDL_UnlockTexture(texture);

    uint8_t* pixels_zeros = reinterpret_cast<uint8_t*>(malloc(pitch * height));
    memset(pixels_zeros, 0, pitch * height);

    // run CLS opcode
    emulator->Opcode0(0x00E0);

    if(SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch) != 0)
    {
        CLOVE_FAIL();
    }

    if(memcmp(pixels, pixels_zeros, pitch * height) == 0)
    {
        CLOVE_PASS();
    }
    else{
        CLOVE_FAIL();
    }
    SDL_UnlockTexture(texture);
}

CLOVE_TEST(RET)
{
    auto& stack = emulator->GetStack();
    uint8_t stack_ptr = 0x05;
    emulator->SetSP(stack_ptr);

    stack[stack_ptr] = 0xFF;

    emulator->Opcode0(0x0EE);

    uint16_t PC = emulator->GetPC();
    uint8_t new_SP = emulator->GetSP();

    CLOVE_UINT_EQ(0xFF, PC);
    CLOVE_UINT_EQ(stack_ptr - 1, new_SP);

}

CLOVE_TEST(JMP)
{
    emulator->Opcode1(0x1220);
    uint16_t PC = emulator->GetPC();
    CLOVE_UINT_EQ(0x220, PC);
}

CLOVE_TEST(CALL)
{
    uint8_t old_SP = emulator->GetSP();
    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode2(0x2344);

    uint8_t new_SP = emulator->GetSP();
    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ((old_SP+1)%0x100, new_SP);

    auto stack = emulator->GetStack();
    CLOVE_UINT_EQ(old_PC, stack[new_SP]);

    CLOVE_UINT_EQ(0x344, new_PC);
}

CLOVE_TEST(SE_V_BYTE_EQUAL)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x44;

    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode3(0x3044);

    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ(old_PC + 2, new_PC);
}

CLOVE_TEST(SE_V_BYTE_DIFF)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x43;

    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode3(0x3044);

    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ(old_PC, new_PC);
}

CLOVE_TEST(SNE_V_BYTE_DIFF)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x43;

    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode4(0x4044);

    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ(old_PC + 2, new_PC);
}

CLOVE_TEST(SNE_V_BYTE_EQUAL)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x44;

    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode4(0x4044);

    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ(old_PC, new_PC);
}

CLOVE_TEST(SE_VX_VY_EQUAL)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x44;
    registers[0x9] = 0x44;

    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode5(0x5090);

    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ(old_PC + 2, new_PC);
}

CLOVE_TEST(SE_VX_VY_DIFF)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x44;
    registers[0xD] = 0x12;

    uint16_t old_PC = emulator->GetPC();

    emulator->Opcode5(0x50D0);

    uint16_t new_PC = emulator->GetPC();

    CLOVE_UINT_EQ(old_PC, new_PC);
}

CLOVE_TEST(LD_V_BYTE)
{
    emulator->Opcode6(0x6D33);

    auto& registers = emulator->GetRegisters();
    CLOVE_UINT_EQ(0x33, registers[0xD]);
}

CLOVE_TEST(ADD_V_BYTE)
{
    emulator->GetRegisters()[0x3] = 0x0F;

    emulator->Opcode7(0x7310);

    auto& registers = emulator->GetRegisters();
    
    CLOVE_UINT_EQ(0x1F, registers[0x3]);
}

CLOVE_TEST(LD_VX_VY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x1] = 0x0F;
    registers[0xD] = 0x05;

    emulator->Opcode8(0x81D0);

    CLOVE_UINT_EQ(0x05, registers[0x1]);
}

CLOVE_TEST(OR_VX_VY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    emulator->Opcode8(0x82A1);

    CLOVE_UINT_EQ(0x0F | 0x05, registers[0x2]);
}

CLOVE_TEST(AND_VX_VY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    emulator->Opcode8(0x82A2);

    CLOVE_UINT_EQ(0x0F & 0x05, registers[0x2]);
}

CLOVE_TEST(XOR_VX_VY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    emulator->Opcode8(0x82A3);

    CLOVE_UINT_EQ(0x0F ^ 0x05, registers[0x2]);
}

CLOVE_TEST(ADD_VX_VY_NO_CARRY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    registers[0xF] = 0; //carry at zero

    emulator->Opcode8(0x82A4);

    CLOVE_UINT_EQ(0x0F + 0x05, registers[0x2]);
    CLOVE_UINT_EQ(0, registers[0xF]);
}

CLOVE_TEST(ADD_VX_VY_CARRY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0xFF;
    registers[0xA] = 0x01;

    registers[0xF] = 0; //carry at zero

    emulator->Opcode8(0x82A4);

    CLOVE_UINT_EQ(0x00, registers[0x2]);
    CLOVE_UINT_EQ(1, registers[0xF]);
}

CLOVE_TEST(SUB_VX_VY_NO_BORROW)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    registers[0xF] = 0;

    emulator->Opcode8(0x82A5);

    CLOVE_UINT_EQ(0x0F - 0x05, registers[0x2]);
    CLOVE_UINT_EQ(1, registers[0xF]);
}

CLOVE_TEST(SUB_VX_VY_BORROW)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    registers[0xF] = 0;

    emulator->Opcode8(0x8A25);

    CLOVE_UINT_EQ(0xF6, registers[0xA]);
    CLOVE_UINT_EQ(0, registers[0xF]);
}

CLOVE_TEST(SHR_VX_ODD)
{
    auto& registers = emulator->GetRegisters();
    registers[0x5] = 0x01;

    registers[0xF] = 0;

    emulator->Opcode8(0x8506);

    CLOVE_UINT_EQ(0, registers[0x5]);
    CLOVE_UINT_EQ(1, registers[0xF]);
}

CLOVE_TEST(SHR_VX_EVEN)
{
    auto& registers = emulator->GetRegisters();
    registers[0x5] = 0x02;

    registers[0xF] = 0;

    emulator->Opcode8(0x8506);

    CLOVE_UINT_EQ(1, registers[0x5]);
    CLOVE_UINT_EQ(0, registers[0xF]);
}

CLOVE_TEST(SUBN_VX_VY_BORROW)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    registers[0xF] = 0;

    emulator->Opcode8(0x82A7);

    CLOVE_UINT_EQ(0xF6, registers[0x2]);
    CLOVE_UINT_EQ(0, registers[0xF]);
}

CLOVE_TEST(SUBN_VX_VY_NO_BORROW)
{
    auto& registers = emulator->GetRegisters();
    registers[0x2] = 0x0F;
    registers[0xA] = 0x05;

    registers[0xF] = 0;

    emulator->Opcode8(0x8A27);

    CLOVE_UINT_EQ(0x0F - 0x05, registers[0xA]);
    CLOVE_UINT_EQ(1, registers[0xF]);
}

CLOVE_TEST(SHL_VX_CARRY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x5] = 0xFF;

    registers[0xF] = 0;

    emulator->Opcode8(0x850E);

    CLOVE_UINT_EQ((0xFF << 1)%0x100, registers[0x5]);
    CLOVE_UINT_EQ(1, registers[0xF]);
}

CLOVE_TEST(SHL_VX_NO_CARRY)
{
    auto& registers = emulator->GetRegisters();
    registers[0x5] = 0x02;

    registers[0xF] = 0;

    emulator->Opcode8(0x850E);

    CLOVE_UINT_EQ(0x04, registers[0x5]);
    CLOVE_UINT_EQ(0, registers[0xF]);
}

CLOVE_TEST(SNE_VX_VY_EQUAL)
{
    auto& registers = emulator->GetRegisters();
    registers[0x5] = 0x02;
    registers[0xA] = 0x02;

    uint16_t old_pc = emulator->GetPC();

    emulator->Opcode9(0x95A0);

    uint16_t new_pc = emulator->GetPC();

    CLOVE_UINT_EQ(old_pc, new_pc);
}

CLOVE_TEST(SNE_VX_VY_NOT_EQUAL)
{
    auto& registers = emulator->GetRegisters();
    registers[0x5] = 0x02;
    registers[0xA] = 0x01;

    uint16_t old_pc = emulator->GetPC();

    emulator->Opcode9(0x95A0);

    uint16_t new_pc = emulator->GetPC();

    CLOVE_UINT_EQ(old_pc + 2, new_pc);
}

CLOVE_TEST(LD_I_ADDR)
{
    emulator->OpcodeA(0xA111);

    uint16_t i_reg = emulator->GetI();

    CLOVE_UINT_EQ(0x111, i_reg);
}

CLOVE_TEST(JP_V0_ADDR)
{
    emulator->GetRegisters()[0x0] = 0x10;
    emulator->OpcodeB(0xB220);

    uint16_t pc = emulator->GetPC();

    CLOVE_UINT_EQ(0x10+0x220, pc);
}

CLOVE_TEST(RND_VX_BYTE_NO_MOCK)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x10;

    emulator->OpcodeC(0xC000);

    CLOVE_UINT_EQ(0x0, registers[0x0]);
}

CLOVE_TEST(RND_VX_BYTE_WITH_MOCK)
{
    auto& registers = emulator->GetRegisters();
    registers[0x0] = 0x00;
    registers[0x2] = 0x00;

    auto* actual_generator = emulator->GetRandGenerator();
    auto* random_mock = new MockRandomGenerator();
    emulator->SetRandomGenerator(random_mock);

    emulator->OpcodeC(0xC0FA);

    CLOVE_UINT_EQ(0xFA, registers[0x0]);

    emulator->OpcodeC(0xC203);

    CLOVE_UINT_EQ(0x03, registers[0x2]);

    emulator->SetRandomGenerator(actual_generator);
    delete random_mock;
}

CLOVE_TEST(DRW_VX_VY_NIBBLE_NO_COLLISION) // needs fixing
{
    emulator->SetI(0x00);   // "0" location
    auto& registers = emulator->GetRegisters();

    registers[0x1] = 0x0;

    uint8_t expected_sprite[] = 
    {
        0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,
    };

    emulator->OpcodeD(0xD115);

    auto texture = emulator->GetTexture();

    int pitch;
    uint8_t* pixels;

    if(SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch) != 0)
    {
        CLOVE_FAIL();
    }

    int comp_res_row_0 = memcmp(&pixels[0], &expected_sprite[0], 16);
    int comp_res_row_1 = memcmp(&pixels[pitch], &expected_sprite[16], 16);
    int comp_res_row_2 = memcmp(&pixels[pitch*2], &expected_sprite[32], 16);
    int comp_res_row_3 = memcmp(&pixels[pitch*3], &expected_sprite[48], 16);
    int comp_res_row_4 = memcmp(&pixels[pitch*4], &expected_sprite[64], 16);

    SDL_UnlockTexture(texture);

    CLOVE_UINT_EQ(0, registers[0xF]);
    CLOVE_INT_EQ(0, comp_res_row_0);
    CLOVE_INT_EQ(0, comp_res_row_1);
    CLOVE_INT_EQ(0, comp_res_row_2);
    CLOVE_INT_EQ(0, comp_res_row_3);
    CLOVE_INT_EQ(0, comp_res_row_4);
}

CLOVE_TEST(DRW_VX_VY_NIBBLE_COLLISION) // needs fixing
{
    emulator->SetI(0x05);   // "1" location
    auto& registers = emulator->GetRegisters();

    registers[0x1] = 0x0;

    emulator->OpcodeD(0xD115);
    emulator->SetI(0x00);   // "0" location
    emulator->OpcodeD(0xD115);

    uint8_t expected_sprite[] = 
    {
        0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0xFF,0xFF,0xFF,0xFF,    0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,    0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00,
    };
    
    auto texture = emulator->GetTexture();
    int pitch;
    uint8_t* pixels;

    if(SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch) != 0)
    {
        CLOVE_FAIL();
    }

    int comp_res_row_0 = memcmp(&pixels[0], &expected_sprite[0], 16);
    int comp_res_row_1 = memcmp(&pixels[pitch], &expected_sprite[16], 16);
    int comp_res_row_2 = memcmp(&pixels[pitch*2], &expected_sprite[32], 16);
    int comp_res_row_3 = memcmp(&pixels[pitch*3], &expected_sprite[48], 16);
    int comp_res_row_4 = memcmp(&pixels[pitch*4], &expected_sprite[64], 16);

    SDL_UnlockTexture(texture);

    CLOVE_UINT_EQ(1, registers[0xF]);
    CLOVE_INT_EQ(0, comp_res_row_0);
    CLOVE_INT_EQ(0, comp_res_row_1);
    CLOVE_INT_EQ(0, comp_res_row_2);
    CLOVE_INT_EQ(0, comp_res_row_3);
    CLOVE_INT_EQ(0, comp_res_row_4);
}

CLOVE_TEST(SKP_VX_PRESSED)
{
    auto& registers = emulator->GetRegisters();
    registers[0x1] = 0xC;

    uint16_t old_pc = emulator->GetPC();

    auto input_class = emulator->GetInputClass();
    int num_keys;
    SDL_GetKeyboardState(&num_keys);
    MockKeyboardStateInputCommand* mock_input = new MockKeyboardStateInputCommand();
    mock_input->fake_keyboard_state = new uint8_t[num_keys];
    emulator->SetInputClass(mock_input);

    mock_input->fake_keyboard_state[SDLK_4] = 1;

    emulator->OpcodeE(0xE19E);

    uint16_t new_pc = emulator->GetPC();

    CLOVE_UINT_EQ(old_pc + 2, new_pc);

    emulator->SetInputClass(input_class);
    delete mock_input;
}

CLOVE_TEST(SKP_VX_NOT_PRESSED)
{
    auto& registers = emulator->GetRegisters();
    registers[0x1] = 0xC;

    uint16_t old_pc = emulator->GetPC();

    emulator->OpcodeE(0xE19E);

    uint16_t new_pc = emulator->GetPC();

    CLOVE_UINT_EQ(old_pc, new_pc);
}

CLOVE_TEST(SKNP_VX_PRESSED)
{
    auto& registers = emulator->GetRegisters();
    registers[0x1] = 0xC;

    uint16_t old_pc = emulator->GetPC();

    emulator->OpcodeE(0xE19E);

    uint16_t new_pc = emulator->GetPC();

    CLOVE_UINT_EQ(old_pc, new_pc);
}

CLOVE_TEST(SKNP_VX_NOT_PRESSED)
{
    auto& registers = emulator->GetRegisters();
    uint8_t expected_key = 0xC;
    registers[0x1] = expected_key;

    uint16_t old_pc = emulator->GetPC();

    auto input_class = emulator->GetInputClass();
    int num_keys;
    SDL_GetKeyboardState(&num_keys);
    MockKeyboardStateInputCommand* mock_input = new MockKeyboardStateInputCommand();
    mock_input->fake_keyboard_state = new uint8_t[num_keys];
    emulator->SetInputClass(mock_input);

    mock_input->fake_keyboard_state[SDLK_4] = 1;

    emulator->OpcodeE(0xE19E);

    uint16_t new_pc = emulator->GetPC();

    CLOVE_UINT_EQ(old_pc + 2, new_pc);

    emulator->SetInputClass(input_class);
    delete mock_input;
}

CLOVE_TEST(LD_VX_DT)
{
    uint8_t delaytimer = emulator->GetDelayTimer();
    auto& registers = emulator->GetRegisters();
    registers[0x4] = 0xF0;

    emulator->OpcodeF(0xF407);

    CLOVE_INT_EQ(delaytimer, registers[0x4]);
}

CLOVE_TEST(LD_VX_K)
{
    auto& registers = emulator->GetRegisters();

    emulator->OpcodeF(0xF40A);

    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_4;
    SDL_PushEvent(&event);
    emulator->Tick();

    CLOVE_INT_EQ(0xC, registers[0x4]);
}

CLOVE_TEST(LD_DT_VX)
{
    auto& registers = emulator->GetRegisters();
    registers[0x4] = 0xF1;

    emulator->OpcodeF(0xF415);

    uint8_t delaytimer = emulator->GetDelayTimer();

    CLOVE_INT_EQ(0xF1, delaytimer);
}

CLOVE_TEST(LD_ST_VX)
{
    auto& registers = emulator->GetRegisters();
    registers[0x4] = 0xF1;

    emulator->OpcodeF(0xF418);

    uint8_t soundtimer = emulator->GetSoundTimer();

    CLOVE_INT_EQ(0xF1, soundtimer);
}

CLOVE_TEST(ADD_I_VX)
{
    auto& registers = emulator->GetRegisters();
    registers[0x7] = 0x45;
    emulator->SetI(0x0F);

    emulator->OpcodeF(0xF71E);

    uint16_t i = emulator->GetI();

    CLOVE_UINT_EQ(0x45 + 0x0F, i);
}

CLOVE_TEST(LD_F_VX)
{
    auto& registers = emulator->GetRegisters();
    registers[0x1] = 0x01;  // font "1"
    emulator->SetI(0x0F);

    emulator->OpcodeF(0xF129);

    uint16_t i = emulator->GetI();

    CLOVE_UINT_EQ(0x05, i);
}

CLOVE_TEST(LD_B_VX)
{
    auto& registers = emulator->GetRegisters();

    registers[0x3] = 216;
    uint16_t i = 0x10;

    emulator->SetI(i);

    emulator->OpcodeF(0xF333);

    auto& memory_map = emulator->GetMemoryMapping();

    CLOVE_UINT_EQ(2, memory_map[i+0]);
    CLOVE_UINT_EQ(1, memory_map[i+1]);
    CLOVE_UINT_EQ(6, memory_map[i+2]);
}

CLOVE_TEST(LD_I_VX)
{
    auto& registers = emulator->GetRegisters();

    for (int i = 0; i < 0x10; i++)
    {
        registers[i] = i;
    }

    uint16_t I = 0x10;
    emulator->SetI(I);

    emulator->OpcodeF(0xFF55);

    auto& memory_map = emulator->GetMemoryMapping();

    CLOVE_UINT_EQ(0x0, memory_map[I+0x0]);
    CLOVE_UINT_EQ(0x1, memory_map[I+0x1]);
    CLOVE_UINT_EQ(0x2, memory_map[I+0x2]);
    CLOVE_UINT_EQ(0x3, memory_map[I+0x3]);
    CLOVE_UINT_EQ(0x4, memory_map[I+0x4]);
    CLOVE_UINT_EQ(0x5, memory_map[I+0x5]);
    CLOVE_UINT_EQ(0x6, memory_map[I+0x6]);
    CLOVE_UINT_EQ(0x7, memory_map[I+0x7]);
    CLOVE_UINT_EQ(0x8, memory_map[I+0x8]);
    CLOVE_UINT_EQ(0x9, memory_map[I+0x9]);
    CLOVE_UINT_EQ(0xA, memory_map[I+0xA]);
    CLOVE_UINT_EQ(0xB, memory_map[I+0xB]);
    CLOVE_UINT_EQ(0xC, memory_map[I+0xC]);
    CLOVE_UINT_EQ(0xD, memory_map[I+0xD]);
    CLOVE_UINT_EQ(0xE, memory_map[I+0xE]);
    CLOVE_UINT_EQ(0xF, memory_map[I+0xF]);

    CLOVE_UINT_EQ(I, emulator->GetI());
}

CLOVE_TEST(LD_VX_I)
{
    auto& memory_map = emulator->GetMemoryMapping();

    uint16_t I = 0x10;
    emulator->SetI(I);

    for (int i = 0; i < 0x10; i++)
    {
        memory_map[I+i] = i;
    }


    emulator->OpcodeF(0xFF65);

    auto& registers = emulator->GetRegisters();

    CLOVE_UINT_EQ(0x0, registers[0x0]);
    CLOVE_UINT_EQ(0x1, registers[0x1]);
    CLOVE_UINT_EQ(0x2, registers[0x2]);
    CLOVE_UINT_EQ(0x3, registers[0x3]);
    CLOVE_UINT_EQ(0x4, registers[0x4]);
    CLOVE_UINT_EQ(0x5, registers[0x5]);
    CLOVE_UINT_EQ(0x6, registers[0x6]);
    CLOVE_UINT_EQ(0x7, registers[0x7]);
    CLOVE_UINT_EQ(0x8, registers[0x8]);
    CLOVE_UINT_EQ(0x9, registers[0x9]);
    CLOVE_UINT_EQ(0xA, registers[0xA]);
    CLOVE_UINT_EQ(0xB, registers[0xB]);
    CLOVE_UINT_EQ(0xC, registers[0xC]);
    CLOVE_UINT_EQ(0xD, registers[0xD]);
    CLOVE_UINT_EQ(0xE, registers[0xE]);
    CLOVE_UINT_EQ(0xF, registers[0xF]);

    CLOVE_UINT_EQ(I, emulator->GetI());
}


#pragma endregion //TESTS