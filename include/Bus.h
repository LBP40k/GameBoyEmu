#pragma once
#include <array>
#include <cstdint>

class Bus
{
public:
    Bus();

    uint8_t rom[0x8000];      // 32 KB ROM
    uint8_t vram[0x2000];     // 8 KB Video RAM
    uint8_t eram[0x2000];     // 8 KB Cartridge RAM
    uint8_t wram[0x2000];     // 8 KB Work RAM
    uint8_t oam[0xA0];        // 160 bytes Sprite memory
    uint8_t hram[0x7F];       // 127 bytes High RAM
    uint8_t ie;               // Interrupt Enable

    void loadBootRom();
	uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t value);
private:
};