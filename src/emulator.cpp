#include <cstring>
#include <iostream>

#include "emulator.h"
#include "sdl/emulator_random_generator.h"
#include "sdl/sdl_input.h"
#include "input_type.h"
#include "sdl/sdl_emu_renderer.h"

namespace chipotto
{
	Emulator::Emulator()
	{
#pragma region OPCODE_BINDINGS

		Opcodes[0x0] = std::bind(&Emulator::Opcode0, this, std::placeholders::_1);
		Opcodes[0x1] = std::bind(&Emulator::Opcode1, this, std::placeholders::_1);
		Opcodes[0x2] = std::bind(&Emulator::Opcode2, this, std::placeholders::_1);
		Opcodes[0x3] = std::bind(&Emulator::Opcode3, this, std::placeholders::_1);
		Opcodes[0x4] = std::bind(&Emulator::Opcode4, this, std::placeholders::_1);
		Opcodes[0x5] = std::bind(&Emulator::Opcode5, this, std::placeholders::_1);
		Opcodes[0x6] = std::bind(&Emulator::Opcode6, this, std::placeholders::_1);
		Opcodes[0x7] = std::bind(&Emulator::Opcode7, this, std::placeholders::_1);
		Opcodes[0x8] = std::bind(&Emulator::Opcode8, this, std::placeholders::_1);
		Opcodes[0x9] = std::bind(&Emulator::Opcode9, this, std::placeholders::_1);
		Opcodes[0xA] = std::bind(&Emulator::OpcodeA, this, std::placeholders::_1);
		Opcodes[0xB] = std::bind(&Emulator::OpcodeB, this, std::placeholders::_1);
		Opcodes[0xC] = std::bind(&Emulator::OpcodeC, this, std::placeholders::_1);
		Opcodes[0xD] = std::bind(&Emulator::OpcodeD, this, std::placeholders::_1);
		Opcodes[0xE] = std::bind(&Emulator::OpcodeE, this, std::placeholders::_1);
		Opcodes[0xF] = std::bind(&Emulator::OpcodeF, this, std::placeholders::_1);
#pragma endregion //OPCODE_BINDINGS

		SetFonts();

		renderer = new SDLEmuRenderer(64, 32);
		input_class = new SDLInput();
		random_generator = new EmulatorRandomGenerator();
	}

	Emulator::~Emulator()
	{
		if (renderer)
		{
			delete renderer;
		}
		if (input_class)
		{
			delete input_class;
		}
		if (random_generator)
		{
			delete random_generator;
		}
	}

	bool Emulator::Load(const Gamefile* gamefile)
	{
		memcpy(MemoryMapping.data() + PC, gamefile->bytecode, gamefile->size);
		return true;
	}

	bool Emulator::Tick(const float deltatime)
	{
		DelayTimerDeltaTicks -= deltatime;
		SoundTimerDeltaTicks -= deltatime;

		if (DelayTimer > 0 && DelayTimerDeltaTicks <= 0)
		{
			DelayTimer--;
			DelayTimerDeltaTicks = SIXTYHERTZ_S;
		}

		if (SoundTimer > 0 && SoundTimerDeltaTicks <= 0)
		{
			SoundTimer--;
			SoundTimerDeltaTicks = SIXTYHERTZ_S;
		}

		while (input_class->IsInputPending())
		{
			InputType InputType = input_class->GetInputEventType();

			switch (InputType)
			{
			case chipotto::InputType::KEYDOWN:
				Registers[WaitForKeyboardRegister_Index] = input_class->GetKey();
				Suspended = false;
				PC += 2;
				break;
			case chipotto::InputType::QUIT:
				return false;
			case chipotto::InputType::NONE:
				break;
			default:
				break;
			}
		}

		if (Suspended)
			return true;

		uint16_t opcode = MemoryMapping[PC + 1] + (static_cast<uint16_t>(MemoryMapping[PC]) << 8);
#ifdef DEBUG_BUILD
		std::cout << std::hex << "0x" << PC << ": 0x" << opcode << "  -->  ";
#endif

		OpcodeStatus status = Opcodes[opcode >> 12](opcode);
#ifdef DEBUG_BUILD
		std::cout << std::endl;
#endif
		if (status == OpcodeStatus::IncrementPC)
		{
			PC += 2;
		}
		return status != OpcodeStatus::NotImplemented && status != OpcodeStatus::StackOverflow && status != OpcodeStatus::Error;
	}

	void Emulator::SetFonts()
	{
		// 0
		MemoryMapping[0x0] = 0xF0;
		MemoryMapping[0x1] = 0x90;
		MemoryMapping[0x2] = 0x90;
		MemoryMapping[0x3] = 0x90;
		MemoryMapping[0x4] = 0xF0;
		// 1
		MemoryMapping[0x5] = 0x20;
		MemoryMapping[0x6] = 0x60;
		MemoryMapping[0x7] = 0x20;
		MemoryMapping[0x8] = 0x20;
		MemoryMapping[0x9] = 0x70;
		// 2
		MemoryMapping[0xA] = 0xF0;
		MemoryMapping[0xB] = 0x10;
		MemoryMapping[0xC] = 0xF0;
		MemoryMapping[0xD] = 0x80;
		MemoryMapping[0xE] = 0xF0;
		// 3
		MemoryMapping[0x0F] = 0xF0;
		MemoryMapping[0x10] = 0x10;
		MemoryMapping[0x11] = 0xF0;
		MemoryMapping[0x12] = 0x10;
		MemoryMapping[0x13] = 0xF0;
		// 4
		MemoryMapping[0x14] = 0x90;
		MemoryMapping[0x15] = 0x90;
		MemoryMapping[0x16] = 0xF0;
		MemoryMapping[0x17] = 0x10;
		MemoryMapping[0x18] = 0x10;
		// 5
		MemoryMapping[0x19] = 0xF0;
		MemoryMapping[0x1A] = 0x80;
		MemoryMapping[0x1B] = 0xF0;
		MemoryMapping[0x1C] = 0x10;
		MemoryMapping[0x1D] = 0xF0;
		// 6
		MemoryMapping[0x1E] = 0xF0;
		MemoryMapping[0x1F] = 0x80;
		MemoryMapping[0x20] = 0xF0;
		MemoryMapping[0x21] = 0x90;
		MemoryMapping[0x22] = 0xF0;
		// 7
		MemoryMapping[0x23] = 0xF0;
		MemoryMapping[0x24] = 0x10;
		MemoryMapping[0x25] = 0x20;
		MemoryMapping[0x26] = 0x40;
		MemoryMapping[0x27] = 0x40;
		// 8
		MemoryMapping[0x28] = 0xF0;
		MemoryMapping[0x29] = 0x90;
		MemoryMapping[0x2A] = 0xF0;
		MemoryMapping[0x2B] = 0x90;
		MemoryMapping[0x2C] = 0xF0;
		// 9
		MemoryMapping[0x2D] = 0xF0;
		MemoryMapping[0x2E] = 0x90;
		MemoryMapping[0x2F] = 0xF0;
		MemoryMapping[0x30] = 0x10;
		MemoryMapping[0x31] = 0xF0;
		// A
		MemoryMapping[0x32] = 0xF0;
		MemoryMapping[0x33] = 0x90;
		MemoryMapping[0x34] = 0xF0;
		MemoryMapping[0x35] = 0x90;
		MemoryMapping[0x36] = 0x90;
		// B
		MemoryMapping[0x37] = 0xE0;
		MemoryMapping[0x38] = 0x90;
		MemoryMapping[0x39] = 0xE0;
		MemoryMapping[0x3A] = 0x90;
		MemoryMapping[0x3B] = 0xE0;
		// C
		MemoryMapping[0x3C] = 0xF0;
		MemoryMapping[0x3D] = 0x80;
		MemoryMapping[0x3E] = 0x80;
		MemoryMapping[0x3F] = 0x80;
		MemoryMapping[0x40] = 0xF0;
		// D
		MemoryMapping[0x41] = 0xE0;
		MemoryMapping[0x42] = 0x90;
		MemoryMapping[0x43] = 0x90;
		MemoryMapping[0x44] = 0x90;
		MemoryMapping[0x45] = 0xE0;
		// E
		MemoryMapping[0x46] = 0xF0;
		MemoryMapping[0x47] = 0x80;
		MemoryMapping[0x48] = 0xF0;
		MemoryMapping[0x49] = 0x80;
		MemoryMapping[0x4A] = 0xF0;
		// F
		MemoryMapping[0x46] = 0xF0;
		MemoryMapping[0x47] = 0x80;
		MemoryMapping[0x48] = 0xF0;
		MemoryMapping[0x49] = 0x80;
		MemoryMapping[0x4A] = 0x80;
	}

#pragma region Opcode Categories

	OpcodeStatus Emulator::Opcode0(const uint16_t opcode)
	{
		switch (opcode & 0xFF)
		{
		case 0xE0:
			return CLS();
		case 0xEE:
			return RET();
		default:
			return OpcodeStatus::NotImplemented;    // SYS addr is ignored
		}
	}

	OpcodeStatus Emulator::Opcode1(const uint16_t opcode)
	{
		uint16_t address = opcode & 0x0FFF;
		return JP(address);
	}

	OpcodeStatus Emulator::Opcode2(const uint16_t opcode)
	{
		uint16_t address = opcode & 0xFFF;
		return CALL(address);
	}

	OpcodeStatus Emulator::Opcode3(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;
		uint8_t value = opcode & 0xFF;
		return SE_VX_BYTE(register_index, value);
	}

	OpcodeStatus Emulator::Opcode4(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;
		uint8_t value = opcode & 0xFF;
		return SNE_VX_BYTE(register_index, value);
	}

	OpcodeStatus Emulator::Opcode5(const uint16_t opcode)
	{
		uint8_t register_vx_index = (opcode >> 8) & 0xF;
		uint8_t register_vy_index = (opcode >> 4) & 0xF;
		return SE_VX_VY(register_vx_index, register_vy_index);
	}

	OpcodeStatus Emulator::Opcode6(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;
		uint8_t register_value = opcode & 0xFF;
		return LD_VX_BYTE(register_index, register_value);
	}

	OpcodeStatus Emulator::Opcode7(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;
		uint8_t value = opcode & 0xFF;
		return ADD_VX_BYTE(register_index, value);
	}

	OpcodeStatus Emulator::Opcode8(const uint16_t opcode)
	{
		uint8_t registerX_index = (opcode >> 8) & 0xF;
		uint8_t registerY_index = (opcode >> 4) & 0xF;

		switch (opcode & 0xF)
		{
		case 0x0:
			return LD_VX_VY(registerX_index, registerY_index);
		case 0x1:
			return OR_VX_VY(registerX_index, registerY_index);
		case 0x2:
			return AND_VX_VY(registerX_index, registerY_index);
		case 0x3:
			return XOR_VX_VY(registerX_index, registerY_index);
		case 0x4:
			return ADD_VX_VY(registerX_index, registerY_index);
		case 0x5:
			return SUB_VX_VY(registerX_index, registerY_index);
		case 0x6:
			return SHR_VX_VY(registerX_index, registerY_index);
		case 0x7:
			return SUBN_VX_VY(registerX_index, registerY_index);
		case 0xE:
			return SHL_VX_VY(registerX_index, registerY_index);
		default:
			return OpcodeStatus::NotImplemented;
		}
	}

	OpcodeStatus Emulator::Opcode9(const uint16_t opcode)
	{
		uint8_t registerX_index = (opcode >> 8) & 0xF;
		uint8_t registerY_index = (opcode >> 4) & 0xF;
		return SNE_VX_VY(registerX_index, registerY_index);
	}

	OpcodeStatus Emulator::OpcodeA(const uint16_t opcode)
	{
		uint16_t Addr = (opcode & 0xFFF);
		return LD_I_ADDR(Addr);
	}

	OpcodeStatus Emulator::OpcodeB(const uint16_t opcode)
	{
		uint16_t address = (opcode & 0x0FFF);
		return JP_V0_ADDR(address);
	}

	OpcodeStatus Emulator::OpcodeC(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;
		uint8_t random_mask = opcode & 0xFF;
		return RND_VX_BYTE(register_index, random_mask);
	}

	OpcodeStatus Emulator::OpcodeD(const uint16_t opcode)
	{
		uint8_t registerX_index = (opcode >> 8) & 0xF;
		uint8_t registerY_index = (opcode >> 4) & 0xF;
		uint8_t sprite_height = opcode & 0xF;
		return DRW_VX_VY_NIBBLE(registerX_index, registerY_index, sprite_height);
	}

	OpcodeStatus Emulator::OpcodeE(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;
		switch (opcode & 0xFF)
		{
		case 0x9E:
			return SKP_VX(register_index);
		case 0xA1:
			return SKNP_VX(register_index);
		default:
			return OpcodeStatus::NotImplemented;
		}
	}

	OpcodeStatus Emulator::OpcodeF(const uint16_t opcode)
	{
		uint8_t register_index = (opcode >> 8) & 0xF;

		switch (opcode & 0xFF)
		{
		case 0x07:
			return LD_VX_DT(register_index);
		case 0x0A:
			return LD_VX_K(register_index);
		case 0x15:
			return LD_DT_VX(register_index);
		case 0x18:
			return LD_ST_VX(register_index);
		case 0x1E:
			return ADD_I_VX(register_index);
		case 0x29:
			return LD_F_VX(register_index);
		case 0x33:
			return LD_B_VX(register_index);
		case 0x55:
			return LD_I_VX(register_index);
		case 0x65:
			return LD_VX_I(register_index);
		default:
			return OpcodeStatus::NotImplemented;
		}
	}

#pragma endregion

#pragma region Opcode Instructions

	OpcodeStatus Emulator::CLS()
	{
#ifdef DEBUG_BUILD
		std::cout << "CLS";
#endif
		renderer->ClearScreen();
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::RET()
	{
		if (SP > 0xF && SP < 0xFF)
			return OpcodeStatus::StackOverflow;
#ifdef DEBUG_BUILD
		std::cout << "RET";
#endif
		PC = Stack[SP & 0xF];
		SP -= 1;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::JP(uint16_t address)
	{
#ifdef DEBUG_BUILD
		std::cout << "JP 0x" << address;
#endif
		PC = address;
		return OpcodeStatus::NotIncrementPC;
	}

	OpcodeStatus Emulator::CALL(uint16_t address)
	{
#ifdef DEBUG_BUILD
		std::cout << "CALL 0x" << (int)address;
#endif
		if (SP > 0xF)
		{
			SP = 0;
		}
		else
		{
			if (SP < 0xF)
			{
				SP += 1;
			}
			else
			{
				return OpcodeStatus::StackOverflow;
			}
		}
		Stack[SP] = PC;
		PC = address;
		return OpcodeStatus::NotIncrementPC;
	}

	OpcodeStatus Emulator::SE_VX_BYTE(uint8_t Vx, uint8_t byte)
	{
#ifdef DEBUG_BUILD
		std::cout << "SE V" << (int)Vx << ", 0x" << (int)byte;
#endif
		if (Registers[Vx] == byte)
			PC += 2;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SNE_VX_BYTE(uint8_t Vx, uint8_t byte)
	{
#ifdef DEBUG_BUILD
		std::cout << "SNE V" << (int)Vx << ", 0x" << (int)byte;
#endif
		if (Registers[Vx] != byte)
			PC += 2;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SE_VX_VY(uint8_t Vx, uint8_t Vy)
	{
#ifdef DEBUG_BUILD
		std::cout << "SE V" << (int)Vx << ", V" << (int)Vy;
#endif
		if (Registers[Vx] == Registers[Vy])
			PC += 2;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_VX_BYTE(uint8_t Vx, uint8_t byte)
	{
		Registers[Vx] = byte;
#ifdef DEBUG_BUILD
		std::cout << "LD V" << (int)Vx << ", 0x" << (int)byte;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::ADD_VX_BYTE(uint8_t Vx, uint8_t byte)
	{
#ifdef DEBUG_BUILD
		std::cout << "ADD V" << (int)Vx << ", 0x" << (int)byte;
#endif
		Registers[Vx] += byte;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		Registers[Vx] = Registers[Vy];
#ifdef DEBUG_BUILD
		std::cout << "LD V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::OR_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		Registers[Vx] |= Registers[Vy];
#ifdef DEBUG_BUILD
		std::cout << "OR V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::AND_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		Registers[Vx] &= Registers[Vy];
#ifdef DEBUG_BUILD
		std::cout << "AND V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::XOR_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		Registers[Vx] ^= Registers[Vy];
#ifdef DEBUG_BUILD
		std::cout << "XOR V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::ADD_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		int result = static_cast<int>(Registers[Vx]) + Registers[Vy];
		if (result > 255)
			Registers[0xF] = 1;
		else
			Registers[0xF] = 0;
		Registers[Vx] += Registers[Vy];
#ifdef DEBUG_BUILD
		std::cout << "ADD V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SUB_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		if (Registers[Vx] > Registers[Vy])
			Registers[0xF] = 1;
		else
			Registers[0xF] = 0;
		Registers[Vx] -= Registers[Vy];
#ifdef DEBUG_BUILD
		std::cout << "SUB V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SHR_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		Registers[0xF] = Registers[Vx] & 0x1;
		Registers[Vx] >>= 1;
#ifdef DEBUG_BUILD
		std::cout << "SHR V" << (int)Vx << "{, V" << (int)Vy << "}";
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SUBN_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		if (Registers[Vy] > Registers[Vx])
			Registers[0xF] = 1;
		else
			Registers[0xF] = 0;
		Registers[Vx] = Registers[Vy] - Registers[Vx];
#ifdef DEBUG_BUILD
		std::cout << "SUBN V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SHL_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		Registers[0xF] = Registers[Vx] >> 7;
		Registers[Vx] <<= 1;
#ifdef DEBUG_BUILD
		std::cout << "SHL V" << (int)Vx << "{, V" << (int)Vy << "}";
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SNE_VX_VY(uint8_t Vx, uint8_t Vy)
	{
		if (Registers[Vx] != Registers[Vy])
		{
			PC += 2;
		}
#ifdef DEBUG_BUILD
		std::cout << "SNE V" << (int)Vx << ", V" << (int)Vy;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_I_ADDR(uint16_t address)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD I, 0x" << (int)address;
#endif
		I = address;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::JP_V0_ADDR(uint16_t address)
	{
#ifdef DEBUG_BUILD
		std::cout << "JP V0, 0x" << address;
#endif
		PC = address + Registers[0x0];
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::RND_VX_BYTE(uint8_t Vx, uint8_t bytemask)
	{
#ifdef DEBUG_BUILD
		std::cout << "RND V" << (int)Vx << ", 0x" << (int)bytemask;
#endif
		Registers[Vx] = random_generator->GetRandomByte() & bytemask;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::DRW_VX_VY_NIBBLE(uint8_t Vx, uint8_t Vy, uint8_t n_byte)
	{
#ifdef DEBUG_BUILD
		std::cout << "DRW V" << (int)Vx << ", V" << (int)Vy << ", " << (int)n_byte;
#endif

		uint8_t x_coord = Registers[Vx] % width;
		uint8_t y_coord = Registers[Vy] % height;
		bool out_collision = false;

		// Prepare sprite
		const uint8_t* sprite = &MemoryMapping[I];

		int result = renderer->Draw(x_coord, y_coord,
			sprite, n_byte,
			DoWrap, out_collision);

		if (result != 0)
		{
			return OpcodeStatus::Error;
		}
		if (out_collision)
		{
			Registers[0xF] = 0x1;
		}

		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SKP_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "SKP V" << (int)Vx;
#endif
		if (input_class->IsKeyPressed(INT_AS_KEY(Registers[Vx])))
		{
			PC += 2;
		}
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::SKNP_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "SKNP V" << (int)Vx;
#endif
		if (!input_class->IsKeyPressed(INT_AS_KEY(Registers[Vx])))
		{
			PC += 2;
		}
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_VX_DT(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD V" << (int)Vx << ", DT";
#endif
		Registers[Vx] = DelayTimer;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_VX_K(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD V" << (int)Vx << ", K";
#endif
		WaitForKeyboardRegister_Index = Vx;
		Suspended = true;
		return OpcodeStatus::WaitForKeyboard;
	}

	OpcodeStatus Emulator::LD_DT_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD DT, V" << (int)Vx;
#endif
		DelayTimer = Registers[Vx];
		DelayTimerDeltaTicks = SIXTYHERTZ_S;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_ST_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD ST, V" << (int)Vx;
#endif
		SoundTimer = Registers[Vx];
		SoundTimerDeltaTicks = SIXTYHERTZ_S;
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::ADD_I_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "ADD I, V" << (int)Vx;
#endif
		I += Registers[Vx];
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_F_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD F, V" << (int)Vx;
#endif
		I = 5 * Registers[Vx];
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_B_VX(uint8_t Vx)
	{
		uint8_t value = Registers[Vx];
		MemoryMapping[I] = value / 100;
		MemoryMapping[I + 1] = (value - (MemoryMapping[I] * 100)) / 10; // ONE BUG WAS HERE
		MemoryMapping[I + 2] = value % 10;
#ifdef DEBUG_BUILD
		std::cout << "LD B, V" << (int)Vx;
#endif
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_I_VX(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD [I], V" << (int)Vx;
#endif
		for (uint8_t i = 0; i <= Vx; ++i)   // ONE BUG WAS HERE
		{
			MemoryMapping[I + i] = Registers[i];
		}
		return OpcodeStatus::IncrementPC;
	}

	OpcodeStatus Emulator::LD_VX_I(uint8_t Vx)
	{
#ifdef DEBUG_BUILD
		std::cout << "LD V" << (int)Vx << ", [I]";
#endif
		for (uint8_t i = 0; i <= Vx; ++i)   //ONE BUG WAS HERE
		{
			Registers[i] = MemoryMapping[I + i];    //ONE BUG WAS HERE
		}
		return OpcodeStatus::IncrementPC;
	}

#pragma endregion

	void Emulator::HardResetEmulator()
	{
		I = 0x0;
		DelayTimer = 0x0;
		SoundTimer = 0x0;
		PC = 0x200;
		SP = 0xFF;

		Suspended = false;
		WaitForKeyboardRegister_Index = 0;
		DelayTimerDeltaTicks = 0;
		SoundTimerDeltaTicks = 0;

		memset(MemoryMapping.data(), 0, MemoryMapping.size() * sizeof(uint8_t));
		memset(Registers.data(), 0, Registers.size() * sizeof(uint8_t));
		memset(Stack.data(), 0, Stack.size() * sizeof(uint16_t));

		SetFonts();

		renderer->ClearScreen();
	}
}