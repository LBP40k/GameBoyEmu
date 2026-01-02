#include "Bus.h"

#include <fstream>
#include <iostream>
#include <vector>

Bus::Bus()
{
    std::fill(std::begin(rom), std::end(rom), 0);
    std::fill(std::begin(vram), std::end(vram), 0);
    std::fill(std::begin(eram), std::end(eram), 0);
    std::fill(std::begin(wram), std::end(wram), 0);
    std::fill(std::begin(oam), std::end(oam), 0);
    std::fill(std::begin(hram), std::end(hram), 0);
    ie = 0;
    std::vector<uint8_t> testRom = {
    0x3E, 0x0F,
    0x06, 0x01,
    0x80,

    0x3E, 0xFF,
    0xC6, 0x01,

    0x21, 0x00, 0xC0,
    0x36, 0x01,
    0x3E, 0x0F,
    0x86,

    0x00, 0x00, 0x00, 0x00
    };

    for (int i = 0; i < testRom.size(); i++)
    {
        rom[i] = testRom[i];
        printf("%02X ", rom[i]);
    }
    /*Bus::loadBootRom();*/
}



void Bus::loadBootRom()
{
    std::cout << "Loading boot rom...\n";
    std::ifstream file(RESOURCES_PATH "dmg_boot.bin",std::ios::binary);
    if (!file) {
        std::cerr << "Failed to load boot from!\n";
    }

    std::vector<uint8_t> romData((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    for (size_t i = 0; i < romData.size(); i++)
    {
        rom[i] = romData[i];
        printf("%02X ", rom[i]);
    }

}

uint8_t Bus::read(uint16_t addr) {
    if (addr < 0x8000) return rom[addr];
    if (addr < 0xA000) return vram[addr - 0x8000];
    if (addr < 0xC000) return eram[addr - 0xA000];
    if (addr < 0xE000) return wram[addr - 0xC000];
    if (addr < 0xFE00) return wram[addr - 0xE000]; // mirror
    if (addr < 0xFEA0) return oam[addr - 0xFE00];
    if (addr < 0xFF80) {
        // I/O registers
        return 0; // placeholder
    }
    if (addr < 0xFFFF) return hram[addr - 0xFF80];
    return ie;
}

void Bus::write(uint16_t addr, uint8_t value) {
    if (addr < 0x8000) return; // ROM is read-only
    if (addr < 0xA000) vram[addr - 0x8000] = value;
    else if (addr < 0xC000) eram[addr - 0xA000] = value;
    else if (addr < 0xE000) wram[addr - 0xC000] = value;
    else if (addr < 0xFE00) wram[addr - 0xE000] = value; // mirror
    else if (addr < 0xFEA0) oam[addr - 0xFE00] = value;
    else if (addr < 0xFF80) {
        // I/O registers: can create io[0x80] array
    }
    else if (addr < 0xFFFF) hram[addr - 0xFF80] = value;
    else ie = value;
}