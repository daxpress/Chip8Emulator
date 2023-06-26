#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <functional>
#include <unordered_map>
#include "SDL.h"

namespace chipotto
{
	class IInputCommand
	{
		public:
		virtual const uint8_t* GetKeyboardState() = 0;
		virtual ~IInputCommand() = default;
	};

	class IRandomGenerator
	{
		public:
		virtual uint8_t GetRandomByte() = 0;
	};

	enum class OpcodeStatus
	{
		IncrementPC,
		NotIncrementPC,
		NotImplemented,
		StackOverflow,
		WaitForKeyboard,
		Error
	};

	 class Emulator
	{
	public:
		Emulator();
		~Emulator();

		Emulator(const Emulator& other) = delete;
		Emulator& operator=(const Emulator& other) = delete;
		Emulator(Emulator&& other) = delete;

		bool LoadFromFile(std::filesystem::path Path);

		bool Tick();

		inline bool IsValid() const
		{
			if (!Window || !Renderer || !Texture)
				return false;
			return true;
		}


		OpcodeStatus Opcode0(const uint16_t opcode);

		OpcodeStatus Opcode1(const uint16_t opcode);

		OpcodeStatus Opcode2(const uint16_t opcode);

		OpcodeStatus Opcode3(const uint16_t opcode);

		OpcodeStatus Opcode4(const uint16_t opcode);

		OpcodeStatus Opcode5(const uint16_t opcode);

		OpcodeStatus Opcode6(const uint16_t opcode);

		OpcodeStatus Opcode7(const uint16_t opcode);

		OpcodeStatus Opcode8(const uint16_t opcode);

		OpcodeStatus Opcode9(const uint16_t opcode);

		OpcodeStatus OpcodeA(const uint16_t opcode);

		OpcodeStatus OpcodeB(const uint16_t opcode);

		OpcodeStatus OpcodeC(const uint16_t opcode);

		OpcodeStatus OpcodeD(const uint16_t opcode);

		OpcodeStatus OpcodeE(const uint16_t opcode);

		OpcodeStatus OpcodeF(const uint16_t opcode);

		void HardResetEmulator();

#ifdef EMU_TEST
	public:
	inline uint16_t GetPC() const {return PC;}
	inline uint16_t GetI() const {return I;}
	inline void SetI(const uint16_t new_i) {I = new_i;}
	inline void SetSP(const uint8_t new_sp) {SP = new_sp;}
	inline uint8_t GetSP() const {return SP;}
	inline bool GetIsSuspended() const {return Suspended;}
	inline int GetWidth() const {return width;}
	inline int GetHeight() const {return height;}
	inline uint8_t GetDelayTimer() const {return DelayTimer;}
	inline uint8_t GetSoundTimer() const {return SoundTimer;}
	inline SDL_Texture* GetTexture() const {return Texture;}
	inline SDL_Renderer* GetRenderer() const {return Renderer;}
	inline std::array<uint8_t, 0x1000>& GetMemoryMapping() {return MemoryMapping;}
	inline std::array<uint8_t, 0x10>& GetRegisters() {return Registers;}
	inline std::array<uint16_t, 0x10>& GetStack() {return Stack;}
	inline IInputCommand* GetInputClass() {return input_class;}
	inline void SetInputClass(IInputCommand* new_input_class) {input_class = new_input_class;}
	inline IRandomGenerator* GetRandGenerator() {return random_generator;}
	inline void SetRandomGenerator(IRandomGenerator* new_rand_gen) {random_generator = new_rand_gen;}

#endif //EMU_TEST

	private:
		void SetFonts();

	private:
		std::array<uint8_t, 0x1000> MemoryMapping;
		std::array<uint8_t, 0x10> Registers;
		std::array<uint16_t, 0x10> Stack;
		std::array<std::function<OpcodeStatus(const uint16_t)>, 0x10> Opcodes;

		std::unordered_map<SDL_Keycode, uint8_t> KeyboardMap;
		std::array<SDL_Scancode, 0x10> KeyboardValuesMap;

		uint16_t I = 0x0;
		uint8_t DelayTimer = 0x0;
		uint8_t SoundTimer = 0x0;
		uint16_t PC = 0x200;
		uint8_t SP = 0xFF;

		bool Suspended = false;
		uint8_t WaitForKeyboardRegister_Index = 0;
		uint64_t DeltaTimerTicks = 0;
		uint64_t SoundTimerTicks = 0;

		SDL_Window* Window = nullptr;
		SDL_Renderer* Renderer = nullptr;
		SDL_Texture* Texture = nullptr;
		int width = 64;
		int height = 32;

		IInputCommand* input_class = nullptr;
		IRandomGenerator* random_generator = nullptr;
	};

	class KeyboardStateInputCommand : public IInputCommand
	{
		public:
		virtual const uint8_t* GetKeyboardState() override;
	};

	class EmulatorRandomGenerator : public IRandomGenerator
	{
		public:
		virtual uint8_t GetRandomByte() override;
	};
}

