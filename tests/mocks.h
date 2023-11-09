#pragma once

#include "irandom_generator.h"
#include "iinput_command.h"
#include "input_type.h"
#include "keys.h"

// class used to mock the keyboard state from SDL and be able to pass custom keyboard states
class MockKeyboardStateInputCommand : public chipotto::IInputCommand
{
public:
    virtual const uint8_t* GetKeyboardState() override { return nullptr; };
    virtual bool IsInputPending() override { return false; };
    virtual chipotto::EmuKey GetKey() override { return chipotto::EmuKey::K_NONE; };
    virtual bool IsKeyPressed(const chipotto::EmuKey key) override { return FakeIsKeyPressed; };
    virtual chipotto::InputType GetInputEventType() override { return chipotto::InputType::NONE; };
    
    bool FakeIsKeyPressed = false;

};

class MockRandomGenerator : public chipotto::IRandomGenerator
{
public:
    virtual uint8_t GetRandomByte() override { return 0xFF; }
};
