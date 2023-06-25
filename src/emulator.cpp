#include "emulator.h"

namespace chipotto
{
    Emulator::Emulator()
	{
#pragma region KEYBINDINGS  // CHANGED BINDINGS TO MATCH KEYPAD LAYOUT
		KeyboardMap[SDLK_1] = 0x1;
		KeyboardMap[SDLK_2] = 0x2;
		KeyboardMap[SDLK_3] = 0x3;
		KeyboardMap[SDLK_4] = 0xC;
		KeyboardMap[SDLK_q] = 0x4;
		KeyboardMap[SDLK_w] = 0x5;
		KeyboardMap[SDLK_e] = 0x6;
		KeyboardMap[SDLK_r] = 0xD;
		KeyboardMap[SDLK_a] = 0x7;
		KeyboardMap[SDLK_s] = 0x8;
		KeyboardMap[SDLK_d] = 0x9;
		KeyboardMap[SDLK_f] = 0xE;
		KeyboardMap[SDLK_z] = 0xA;
		KeyboardMap[SDLK_x] = 0x0;
		KeyboardMap[SDLK_c] = 0xB;
		KeyboardMap[SDLK_v] = 0xF;
#pragma endregion //KEYBINDINGS

		for (const auto& pair : KeyboardMap)
		{
			KeyboardValuesMap[pair.second] = static_cast<SDL_Scancode>(pair.first);
		}
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


		Window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * 10, height * 10, 0);
		if (!Window)
		{
			SDL_Log("Unable to create window: %s", SDL_GetError());
			return;
		}
		Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!Renderer)
		{
			SDL_Log("Unable to create renderer: %s", SDL_GetError());
			SDL_DestroyWindow(Window);
			return;
		}
		Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		if (!Texture)
		{
			SDL_Log("Unable to create texture: %s", SDL_GetError());
			SDL_DestroyRenderer(Renderer);
			SDL_DestroyWindow(Window);
			return;
		}

        input_class = new EmulatorInput();
	}

    Emulator::~Emulator()
    {
        if(Texture)
        {
            SDL_DestroyTexture(Texture);
        }
        if(Renderer)
        {
            SDL_DestroyRenderer(Renderer);
        }
        if(Window)
        {
            SDL_DestroyWindow(Window);
        }
        if(input_class)
        {
            delete input_class;
        }
    }

    bool Emulator::LoadFromFile(std::filesystem::path Path)
    {
		std::ifstream file;
        file.open(Path, std::ios::binary);
        if (!file.is_open()) return false;

        auto file_size = std::filesystem::file_size(Path);

        file.read(reinterpret_cast<char*>(MemoryMapping.data() + PC), file_size);
        file.close();
	}

    bool Emulator::Tick()
    {
        uint64_t tick = SDL_GetTicks64();

        if (DelayTimer > 0 && tick >= DeltaTimerTicks)
        {
            DelayTimer--;
            DeltaTimerTicks = 17 + SDL_GetTicks64();
        }

        if (SoundTimer > 0 && tick >= SoundTimerTicks)
        {
            DelayTimer--;
            SoundTimerTicks = 17 + SDL_GetTicks64();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                SDL_Keycode key = event.key.keysym.sym;
                if (KeyboardMap.contains(key))
                {
                    Registers[WaitForKeyboardRegister_Index] = KeyboardMap[key];
                    Suspended = false;
                    PC += 2;
                }
            }
            if (event.type == SDL_QUIT)
            {
                return false;
            }
        }
        SDL_PumpEvents();

        if (Suspended)
            return true;

        uint16_t opcode = MemoryMapping[PC + 1] + (static_cast<uint16_t>(MemoryMapping[PC]) << 8);
#ifndef EMU_TEST
        std::cout << std::hex << "0x" << PC << ": 0x" << opcode << "  -->  ";
#endif

        OpcodeStatus status = Opcodes[opcode >> 12](opcode);
#ifndef EMU_TEST
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

    OpcodeStatus Emulator::Opcode0(const uint16_t opcode)
    {
        if ((opcode & 0xFF) == 0xE0)
        {
#ifndef EMU_TEST
            std::cout << "CLS";
#endif
            uint8_t *pixels = nullptr;
            int pitch;
            int result = SDL_LockTexture(Texture, nullptr, reinterpret_cast<void **>(&pixels), &pitch);
            memset(pixels, 0, pitch * height);
            SDL_UnlockTexture(Texture);
            SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);
            SDL_RenderPresent(Renderer);
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0xEE)
        {
            if (SP > 0xF && SP < 0xFF)
                return OpcodeStatus::StackOverflow;
#ifndef EMU_TEST
            std::cout << "RET";
#endif
            PC = Stack[SP & 0xF];
            SP -= 1;
            return OpcodeStatus::IncrementPC;
        }
        return OpcodeStatus::NotImplemented;
    }

    OpcodeStatus Emulator::Opcode1(const uint16_t opcode)
    {
        uint16_t address = opcode & 0x0FFF;
#ifndef EMU_TEST
        std::cout << "JP 0x" << address;
#endif
        PC = address;
        return OpcodeStatus::NotIncrementPC;
    }

    OpcodeStatus Emulator::Opcode2(const uint16_t opcode)
    {
        uint16_t address = opcode & 0xFFF;
#ifndef EMU_TEST
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

    OpcodeStatus Emulator::Opcode3(const uint16_t opcode)
    {
        uint8_t register_index = (opcode >> 8) & 0xF;
        uint8_t value = opcode & 0xFF;
#ifndef EMU_TEST
        std::cout << "SE V" << (int)register_index << ", 0x" << (int)value;
#endif
        if (Registers[register_index] == value)
            PC += 2;
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::Opcode4(const uint16_t opcode)
    {
        uint8_t register_index = (opcode >> 8) & 0xF;
        uint8_t value = opcode & 0xFF;
#ifndef EMU_TEST
        std::cout << "SNE V" << (int)register_index << ", 0x" << (int)value;
#endif
        if (Registers[register_index] != value)
            PC += 2;
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::Opcode5(const uint16_t opcode)
    {
        uint8_t register_vx_index = (opcode >> 8) & 0xF;
        uint8_t register_vy_index = (opcode >> 4) & 0xF;
#ifndef EMU_TEST
        std::cout << "SE V" << (int)register_vx_index << ", V" << (int)register_vy_index;
#endif
        if (Registers[register_vx_index] == Registers[register_vy_index])
            PC += 2;
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::Opcode6(const uint16_t opcode)
    {
        uint8_t register_index = (opcode >> 8) & 0xF;
        uint8_t register_value = opcode & 0xFF;
        Registers[register_index] = register_value;
#ifndef EMU_TEST
        std::cout << "LD V" << (int)register_index << ", 0x" << (int)register_value;
#endif
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::Opcode7(const uint16_t opcode)
    {
        uint8_t register_index = (opcode >> 8) & 0xF;
        uint8_t value = opcode & 0xFF;
#ifndef EMU_TEST
        std::cout << "ADD V" << (int)register_index << ", 0x" << (int)value;
#endif
        Registers[register_index] += value;
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::Opcode8(const uint16_t opcode)
    {
        if ((opcode & 0xF) == 0x0)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            Registers[registerX_index] = Registers[registerY_index];
#ifndef EMU_TEST
            std::cout << "LD V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0x2)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            Registers[registerX_index] &= Registers[registerY_index];
#ifndef EMU_TEST
            std::cout << "AND V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0x3)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            Registers[registerX_index] ^= Registers[registerY_index];
#ifndef EMU_TEST
            std::cout << "XOR V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0x4)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            int result = static_cast<int>(Registers[registerX_index]) + Registers[registerY_index];
            if (result > 255)
                Registers[0xF] = 1;
            else
                Registers[0xF] = 0;
            Registers[registerX_index] += Registers[registerY_index];
#ifndef EMU_TEST
            std::cout << "ADD V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0x5)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            if (Registers[registerX_index] > Registers[registerY_index])
                Registers[0xF] = 1;
            else
                Registers[0xF] = 0;
            Registers[registerX_index] -= Registers[registerY_index];
#ifndef EMU_TEST
            std::cout << "SUB V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0x6)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            Registers[0xF] = Registers[registerX_index] & 0x1;
            Registers[registerX_index] >>= 1;
#ifndef EMU_TEST
            std::cout << "SHR V" << (int)registerX_index << "{, V" << (int)registerY_index << "}";
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0x7)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            if (Registers[registerY_index] > Registers[registerX_index])
                Registers[0xF] = 1;
            else
                Registers[0xF] = 0;
            Registers[registerX_index] = Registers[registerY_index] - Registers[registerX_index];
#ifndef EMU_TEST
            std::cout << "SUB V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xF) == 0xE)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            Registers[0xF] = Registers[registerX_index] >> 7;
            Registers[registerX_index] <<= 1;
#ifndef EMU_TEST
            std::cout << "SHL V" << (int)registerX_index << "{, V" << (int)registerY_index << "}";
#endif
            return OpcodeStatus::IncrementPC;
        }
        else
        {
            return OpcodeStatus::NotImplemented;
        }
    }

    OpcodeStatus Emulator::Opcode9(const uint16_t opcode)
    {
        if ((opcode & 0xF) == 0x0)
        {
            uint8_t registerX_index = (opcode >> 8) & 0xF;
            uint8_t registerY_index = (opcode >> 4) & 0xF;
            if(Registers[registerX_index] != Registers[registerY_index])
            {
                PC += 2;
            }
#ifndef EMU_TEST
            std::cout << "SNE V" << (int)registerX_index << ", V" << (int)registerY_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
    }

    OpcodeStatus Emulator::OpcodeA(const uint16_t opcode)
    {
        uint16_t value = (opcode & 0xFFF);
#ifndef EMU_TEST
        std::cout << "LD I, 0x" << (int)value;
#endif
        I = value;
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::OpcodeB(const uint16_t opcode)
    {
        uint16_t address = (opcode & 0x0FFF);
#ifndef EMU_TEST
        std::cout << "JP V0, 0x" << address;
#endif
        PC = address + Registers[0x0];
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::OpcodeC(const uint16_t opcode)
    {
        uint8_t register_index = (opcode >> 8) & 0xF;
        uint8_t random_mask = opcode & 0xFF;
#ifndef EMU_TEST
        std::cout << "RND V" << (int)register_index << ", 0x" << (int)random_mask;
#endif
        Registers[register_index] = (std::rand() % 256) & random_mask;
        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::OpcodeD(const uint16_t opcode)
    {
        uint8_t registerX_index = (opcode >> 8) & 0xF;
        uint8_t registerY_index = (opcode >> 4) & 0xF;
        uint8_t sprite_height = opcode & 0xF;
#ifndef EMU_TEST
        std::cout << "DRW V" << (int)registerX_index << ", V" << (int)registerY_index << ", " << (int)sprite_height;
#endif

        uint8_t x_coord = Registers[registerX_index] % width;
        uint8_t y_coord = Registers[registerY_index] % height;

        uint8_t *pixels = nullptr;
        int pitch;
        int result = SDL_LockTexture(Texture, nullptr, reinterpret_cast<void **>(&pixels), &pitch);
        if (result != 0)
        {
            SDL_Log("Failed to lock texture");
            return OpcodeStatus::Error;
        }

        for (int y = 0; y < sprite_height; ++y)
        {
            if (y + y_coord >= height)
                break;
            uint8_t row_byte = MemoryMapping[I + y];
            for (int x = 0; x < 8; x++)
            {
                uint8_t pixel_color = (row_byte >> (7 - x)) & 0x1;
                uint8_t color = 0;
                if (pixel_color)
                {
                    color = 0xFF;
                }
                if (x + x_coord >= width)
                    break;
                int pixel_index = (x + x_coord) * 4 + pitch * (y + y_coord);
                uint8_t existing_pixel = pixels[pixel_index];
                color ^= existing_pixel;

                if (existing_pixel != 0 && color != 0)
                {
                    Registers[0xF] = 0x1;
                }

                pixels[pixel_index + 0] = color;
                pixels[pixel_index + 1] = color;
                pixels[pixel_index + 2] = color;
                pixels[pixel_index + 3] = color;
            }
        }

        SDL_UnlockTexture(Texture);

        SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);
        SDL_RenderPresent(Renderer);

        return OpcodeStatus::IncrementPC;
    }

    OpcodeStatus Emulator::OpcodeE(const uint16_t opcode)
    {
        if ((opcode & 0xFF) == 0xA1)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "SKNP V" << (int)register_index;
#endif
            const uint8_t *keys_state = input_class->GetKeyboardState();
            if (keys_state[KeyboardValuesMap[Registers[register_index]]] == 0)
            {
                PC += 2;
            }
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x9E)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "SKP V" << (int)register_index;
#endif
            const uint8_t *keys_state = input_class->GetKeyboardState();
            if (keys_state[KeyboardValuesMap[Registers[register_index]]] == 1)
            {
                PC += 2;
            }
            return OpcodeStatus::IncrementPC;
        }
        return OpcodeStatus::NotImplemented;
    }

    OpcodeStatus Emulator::OpcodeF(const uint16_t opcode)
    {
        if ((opcode & 0xFF) == 0x55)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD [I], V" << (int)register_index;
#endif
            for (uint8_t i = 0; i <= register_index; ++i)   // ONE BUG WAS HERE
            {
                MemoryMapping[I + i] = Registers[i];
            }
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x65)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD V" << (int)register_index << ", [I]";
#endif
            for (uint8_t i = 0; i <= register_index; ++i)   //ONE BUG WAS HERE
            {
                Registers[i] = MemoryMapping[I + i];    //ONE BUG WAS HERE
            }
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x33)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
            uint8_t value = Registers[register_index];
            MemoryMapping[I] = value / 100;
            MemoryMapping[I + 1] = (value - (MemoryMapping[I] * 100)) / 10; // ONE BUG WAS HERE
            MemoryMapping[I + 2] = value % 10;
#ifndef EMU_TEST
            std::cout << "LD B, V" << (int)register_index;
#endif
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x29)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD F, V" << (int)register_index;
#endif
            I = 5 * Registers[register_index];
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x0A)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD V" << (int)register_index << ", K";
#endif
            WaitForKeyboardRegister_Index = register_index;
            Suspended = true;
            return OpcodeStatus::WaitForKeyboard;
        }
        else if ((opcode & 0xFF) == 0x1E)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "ADD I, V" << (int)register_index;
#endif
            I += Registers[register_index];
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x15)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD DT, V" << (int)register_index;
#endif
            DelayTimer = Registers[register_index];
            DeltaTimerTicks = 17 + SDL_GetTicks64();
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x18)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD ST, V" << (int)register_index;
#endif
            SoundTimer = Registers[register_index];
            SoundTimerTicks = 17 + SDL_GetTicks64();
            return OpcodeStatus::IncrementPC;
        }
        else if ((opcode & 0xFF) == 0x07)
        {
            uint8_t register_index = (opcode >> 8) & 0xF;
#ifndef EMU_TEST
            std::cout << "LD V" << (int)register_index << ", DT";
#endif
            Registers[register_index] = DelayTimer;
            return OpcodeStatus::IncrementPC;
        }
        else
        {
            return OpcodeStatus::NotImplemented;
        }
    }

    void Emulator::HardResetEmulator()
    {
        I = 0x0;
		DelayTimer = 0x0;
		SoundTimer = 0x0;
		PC = 0x200;
		SP = 0xFF;

        Suspended = false;
		WaitForKeyboardRegister_Index = 0;
		DeltaTimerTicks = 0;
        SoundTimerTicks = 0;

        memset(MemoryMapping.data(), 0, MemoryMapping.size() * sizeof(uint8_t));
        memset(Registers.data(), 0, Registers.size() * sizeof(uint8_t));
        memset(Stack.data(), 0, Stack.size() * sizeof(uint16_t));

        SetFonts();

        uint8_t *pixels = nullptr;
        int pitch;
        int result = SDL_LockTexture(Texture, nullptr, reinterpret_cast<void **>(&pixels), &pitch);
        memset(pixels, 0, pitch * height);
        SDL_UnlockTexture(Texture);
        SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);
        SDL_RenderPresent(Renderer);
    }


    const uint8_t *EmulatorInput::GetKeyboardState()
    {
        return SDL_GetKeyboardState(nullptr);   //returns the actual keyboard state
    }
}