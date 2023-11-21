#pragma once

#include "export.h"
#include <array>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "gamefile.h"


#define SIXTYHERTZ_S 0.017

namespace chipotto
{
	class IInputCommand;
	class EmuRenderer;
	class IRandomGenerator;

	enum class CHIP8_API OpcodeStatus
	{
		IncrementPC,
		NotIncrementPC,
		NotImplemented,
		StackOverflow,
		WaitForKeyboard,
		Error
	};

	 class CHIP8_API Emulator
	{
	public:
		Emulator() = delete;
		Emulator(EmuRenderer* renderer, IInputCommand* input, IRandomGenerator* random_generator);
		~Emulator();

		Emulator(const Emulator& other) = delete;
		Emulator& operator=(const Emulator& other) = delete;
		Emulator(Emulator&& other) = delete;

		bool Load(const Gamefile* gamefile);

		bool Tick(const float deltatime);

#pragma region Opcode Categories

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

#pragma endregion

#pragma region Opcode Instructions

		OpcodeStatus CLS();
		OpcodeStatus RET();
		OpcodeStatus JP(uint16_t address);
		OpcodeStatus CALL(uint16_t address);
		OpcodeStatus SE_VX_BYTE(uint8_t Vx, uint8_t byte);
		OpcodeStatus SNE_VX_BYTE(uint8_t Vx, uint8_t byte);
		OpcodeStatus SE_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus LD_VX_BYTE(uint8_t Vx, uint8_t byte);
		OpcodeStatus ADD_VX_BYTE(uint8_t Vx, uint8_t byte);
		OpcodeStatus LD_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus OR_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus AND_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus XOR_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus ADD_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus SUB_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus SHR_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus SUBN_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus SHL_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus SNE_VX_VY(uint8_t Vx, uint8_t Vy);
		OpcodeStatus LD_I_ADDR(uint16_t address);
		OpcodeStatus JP_V0_ADDR(uint16_t address);
		OpcodeStatus RND_VX_BYTE(uint8_t Vx, uint8_t bytemask);
		OpcodeStatus DRW_VX_VY_NIBBLE(uint8_t Vx, uint8_t Vy, uint8_t n_byte);
		OpcodeStatus SKP_VX(uint8_t Vx);
		OpcodeStatus SKNP_VX(uint8_t Vx);
		OpcodeStatus LD_VX_DT(uint8_t Vx);
		OpcodeStatus LD_VX_K(uint8_t Vx);
		OpcodeStatus LD_DT_VX(uint8_t Vx);
		OpcodeStatus LD_ST_VX(uint8_t Vx);
		OpcodeStatus ADD_I_VX(uint8_t Vx);
		OpcodeStatus LD_F_VX(uint8_t Vx);
		OpcodeStatus LD_B_VX(uint8_t Vx);
		OpcodeStatus LD_I_VX(uint8_t Vx);
		OpcodeStatus LD_VX_I(uint8_t Vx);


#pragma endregion

		void HardResetEmulator();

		inline void SetDoWrap(const bool do_wrap) { DoWrap = do_wrap;}

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
		inline std::array<uint8_t, 0x1000>& GetMemoryMapping() {return MemoryMapping;}
		inline std::array<uint8_t, 0x10>& GetRegisters() {return Registers;}
		inline std::array<uint16_t, 0x10>& GetStack() {return Stack;}
		inline EmuRenderer* GetRenderer() const { return renderer; }
		inline void SetRenderer(EmuRenderer* new_renderer) { renderer = new_renderer; }
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

		uint16_t I = 0x0;
		uint8_t DelayTimer = 0x0;
		uint8_t SoundTimer = 0x0;
		uint16_t PC = 0x200;
		uint8_t SP = 0xFF;

		bool Suspended = false;
		uint8_t WaitForKeyboardRegister_Index = 0;
		double DelayTimerDeltaTicks = 0;
		double SoundTimerDeltaTicks = 0;

		int width = 64;
		int height = 32;
		bool DoWrap = false;

		EmuRenderer* renderer = nullptr;
		IInputCommand* input_class = nullptr;
		IRandomGenerator* random_generator = nullptr;
	};
}

