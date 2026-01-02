#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"
#include "CPU.h"


CPU::CPU()
{
    SP = 0xFFFE;
    PC = 0;
    CPU::initOpcodes();
}

bool CPU::halfCarryAdd(uint8_t a, uint8_t b, uint8_t carry)
{
    return ((a & 0x0F) + (b & 0x0F) + carry) > 0x0F;
}

void CPU::setBC(uint16_t value) {
    r[B] = (value & 0xF) >> 8;
    r[C] = (value & 0x0F);
}
uint16_t CPU::getBC() {
    return (r[B] << 8) | (r[C]);
}
void CPU::setDE(uint16_t value) {
    r[D] = (value & 0xF) >> 8;
    r[E] = (value & 0x0F);
}
uint16_t CPU::getDE() {
    return (r[D] << 8) | (r[E]);
}
void CPU::setHL(uint16_t value) {
    r[H] = (value & 0xF) >> 8;
    r[L] = (value & 0x0F);
}
uint16_t CPU::getHL() {
    return (r[H] << 8) | (r[L]);
}

uint8_t CPU::readMemory(uint16_t addr)
{
    return bus->read(addr);
}



void CPU::writeMemory(uint16_t addr, uint8_t value)
{
    bus->write(addr, value);
}

void CPU::attachBus(Bus *newBus)
{
    bus = newBus;
}


void CPU::initOpcodes() {
    // Default all opcodes to UNDEFINED
    for (int i = 0; i < 256; ++i) {
        opcodes[i] = { "UNDEFINED", 4, &CPU::UNDEFINED};
    }

    // Specific opcodes
    opcodes[0x00] = { "NOP", 4, &CPU::NOP};
    
    opcodes[0x0A] = { "LD A,(BC)", 8, &CPU::LD_A_BC };
    opcodes[0x1A] = { "LD A,(DE)", 8, &CPU::LD_A_DE };
    opcodes[0x02] = { "LD (BC),A", 8, &CPU::LD_BC_A };
    opcodes[0x12] = { "LD (DE),A", 8, &CPU::LD_DE_A };

    opcodes[0xFA] = { "LD A,(nn)", 16, &CPU::LD_A_nn };
    opcodes[0xEA] = { "LD (nn),A", 16, &CPU::LD_nn_A };

    opcodes[0xF2] = { "LDH A,(C)", 8, &CPU::LDH_A_C };
    opcodes[0xE2] = { "LDH (C),A", 8, &CPU::LDH_C_A };
    opcodes[0xF0] = { "LDH A,(n)", 12, &CPU::LDH_A_n };
    opcodes[0xE0] = { "LDH (n),A", 12, &CPU::LDH_n_A };

    opcodes[0x3A] = { "LD A,(HL-)", 8, &CPU::LD_A_HLdec };
    opcodes[0x32] = { "LD (HL-),A", 8, &CPU::LD_HLdec_A };
    opcodes[0x2A] = { "LD A,(HL+)", 8, &CPU::LD_A_HLinc };
    opcodes[0x22] = { "LD (HL+),A", 8, &CPU::LD_HLinc_A };

    opcodes[0x08] = { "LD (nn),SP", 20, &CPU::LD_nn_SP };
    opcodes[0xF9] = { "LD SP,HL", 8, &CPU::LD_SP_HL };

    opcodes[0xE8] = { "LD HL,SP+e", 12, &CPU::LD_HL_SPe };

    // Patterned instructions
    // LD r,r 0x40-0x7F excluding 0x76 (HALT)
    for (int i = 0x40; i <= 0x7F; ++i) {
        if (i == 0x76) continue;
        opcodes[i] = { "LD r,r", 4, &CPU::LD_r_r};
    }

    // INC r8: 0x04,0x0C,0x14,0x1C,0x24,0x2C,0x34,0x3C
    

    // ADD A,r: 0x80-0x87
    for (int i = 0x80; i <= 0x87; ++i) {
        if (i == 0x86) continue;
        opcodes[i] = { "ADD r", 4, &CPU::ADD_r };
    }

    int ld_rn_opcodes[] = { 0x06,0x0E,0x16,0x1E,0x26,0x2E,0x36,0x3E };
    for (int op : ld_rn_opcodes) {
        opcodes[op] = { "LD r,n", 8, &CPU::LD_r_n };
    }

    for (int i = 0x46; i <= 0x7E; i += 0x08)
    {
        opcodes[i] = { "LD r,(HL)", 8, &CPU::LD_r_HL };
    }

    for (int i = 0x70; i <= 0x75; i++)
    {
        opcodes[i] = { "LD (HL),r", 8, &CPU::LD_HL_r };
    }

    int LD_rr_nn_opcodes[] = { 0x01,0x11,0x21,0x31 };
    for (int op : LD_rr_nn_opcodes) {
        opcodes[op] = { "LD_rr_nn",8,&CPU::LD_rr_nn };
    }
    

    opcodes[0x36] = { "LD (HL),n", 12, &CPU::LD_HL_n };
    opcodes[0xC5] = { "PUSH BC", 16, &CPU::PUSH_rr };
    opcodes[0xD5] = { "PUSH DE", 16, &CPU::PUSH_rr };
    opcodes[0xE5] = { "PUSH HL", 16, &CPU::PUSH_rr };
    opcodes[0xF5] = { "PUSH AF", 16, &CPU::PUSH_rr };

    opcodes[0xC1] = { "POP BC", 12, &CPU::POP_rr };
    opcodes[0xD1] = { "POP DE", 12, &CPU::POP_rr };
    opcodes[0xE1] = { "POP HL", 12, &CPU::POP_rr };
    opcodes[0xF1] = { "POP AF", 12, &CPU::POP_rr };

    opcodes[0x86] = { "ADD (HL)", 8, &CPU::ADD_HL };
    opcodes[0x8E] = { "ADC (HL)",8,&CPU::ADC_HL };
    opcodes[0xCE] = { "ADC n",8,&CPU::ADC_n};

    for (int op = 0x90; op <= 0x97; op++)
    {
        if (op == 0x96) continue;
        opcodes[op] = { "SUB r",4,&CPU::SUB_r };
    }

    opcodes[0x96] = { "SUB (HL)",8,&CPU::SUB_HL};
    opcodes[0xD6] = { "SUB n",8,&CPU::SUB_n };

    for (int op = 0x98; op <= 0x9F; op++)
    {
        if (op == 0x9E) continue;
        opcodes[op] = { "SBC r",4,&CPU::SBC_r };
    }

    opcodes[0x9E] = { "SBC (HL)",8,&CPU::SBC_HL };
    opcodes[0xDE] = { "SBC n",8,&CPU::SBC_n};

    for (int op = 0xB8; op <= 0xBF; op++)
    {
        if (op == 0xBE) continue;
        opcodes[op] = { "CP r",4,&CPU::CP_r };
    }

    opcodes[0xBE] = { "CP (HL)",8,&CPU::CP_HL};
    opcodes[0xFE] = { "CP n",8,&CPU::CP_n};

    int incOpcodes[] = { 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x3C };
    for (int op : incOpcodes) {
        opcodes[op] = { "INC r8", 4, &CPU::INC_r };
    }

    opcodes[0x34] = { "INC (HL)",12,&CPU::INC_HL };

    int decOpcodes[] = {
    0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x3D
    };

    for (int op : decOpcodes)
    {
        opcodes[op] = { "DEC r8", 4, &CPU::DEC_r };
    }

    opcodes[0x35] = { "DEC (HL)", 12, &CPU::DEC_HL };

    for (int op = 0xA0; op <= 0xA7; op++)
    {
        if (op == 0xA6) continue; // (HL)
        opcodes[op] = { "AND r", 4, &CPU::AND_r };
    }

    
    opcodes[0xA6] = { "AND (HL)", 8, &CPU::AND_HL };

    
    opcodes[0xE6] = { "AND n", 8, &CPU::AND_n };

    
    for (int op = 0xB0; op <= 0xB7; op++)
    {
        if (op == 0xB6) continue; // (HL)
        opcodes[op] = { "OR r", 4, &CPU::OR_r };
    }

    
    opcodes[0xB6] = { "OR (HL)", 8, &CPU::OR_HL };

    
    opcodes[0xF6] = { "OR n", 8, &CPU::OR_n };

    for (int op = 0xA8; op <= 0xAF; op++)
    {
        if (op == 0xAE) continue; // (HL)
        opcodes[op] = { "XOR r", 4, &CPU::XOR_r};
    }

    opcodes[0xAE] = { "XOR (HL)",8, &CPU::XOR_HL };
    opcodes[0xEE] = { "XOR n",8, &CPU::XOR_n};
    opcodes[0x3F] = { "CCF",4, &CPU::CCF};
    opcodes[0x37] = { "SCF",4, &CPU::SCF};
    opcodes[0x27] = { "DAA", 4, &CPU:DAA}; //gotta figure out how this one works first
    opcodes[0x2F] = { "CPL", 4, &CPU:CPL};


    int inc_rr_opcodes = { 0x03,0x13,0x23,0x33 };
    for (int op : inc_rr_opcodes)
    {
        opcodes[op] = { "INC rr", 8, &CPU::INC_rr };
    }

    int dec_rr_opcodes = { 0x0B,0x1B,0x2B,0x3B};
    for (int op : dec_rr_opcodes)
    {
        opcodes[op] = { "DEC rr", 8, &CPU::DEC_rr};
    }

    int add_hl_rr_opcodes = { 0x09,0x19,0x29,0x39};
    for (int op : add_hl_rr_opcodes)
    {
        opcodes[op] = { "ADD HL, rr", 8, &CPU::ADD_HL_rr};
    }

    opcodes[0xE8] = { "ADD SP,e", 16, &CPU::ADD_SP_e };
    opcodes[0x07] = { "RLCA",4,&CPU::RLCA};
    opcodes[0x0F] = { "RRCA",4,&CPU::RRCA};
    opcodes[0x17] = { "RLA",4,&CPU::RLA};
    opcodes[0x1F] = { "RRA",4,&CPU::RRA};

}


bool CPU::usesHL(uint8_t opcode)
{
    int dst = (opcode >> 3) & 7;
    int src = opcode & 7;
    return dst == 6 || src == 6;
}

uint8_t CPU::fetch8()
{
    uint8_t v = bus->read(PC);
    PC++;
    return v;
}

uint16_t CPU::fetch16()
{
    uint16_t lo = fetch8();
    uint16_t hi = fetch8();
    return lo | (hi << 8);
}

void CPU::execute(uint8_t opcode) {
    (this->*opcodes[opcode].execute)(opcode);
}

void CPU::UNDEFINED(uint8_t opcode)
{
    PC++;
}

void CPU::NOP(uint8_t opcode)
{
    PC++;
}



void CPU::LD_r_r(uint8_t opcode)
{
    int dst = (opcode >> 3) & 0x07;
    int src = opcode & 0x07;
    uint16_t hlAddr = (r[H] << 8) | r[L];

    uint8_t value = (src == HL_INDIRECT) ? bus->read(hlAddr) : r[src];

    if (dst == HL_INDIRECT)
        bus->write(hlAddr, value);
    else
        r[dst] = value;
}

void CPU::LD_r_n(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00111000) >> 3;
    uint8_t imm = fetch8(); //immediate value

    //HL is not valid register here
    if (reg == HL_INDIRECT) {
        
        return;
    }
    r[reg] = imm;
}

void CPU::LD_r_HL(uint8_t opcode) {
    int dst = (opcode >> 3) & 0x07;
    uint16_t hlAddr = (r[H] << 8) | r[L];
    r[dst] = bus->read(hlAddr);
}

void CPU::LD_HL_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint16_t hl = (r[H] << 8) | (r[L]);
    uint8_t value = r[reg];

    bus->write(hl, value);
}

void CPU::LD_HL_n(uint8_t opcode)
{
 
    uint16_t hl = (r[H] << 8) | (r[L]);
    uint8_t data = fetch8();
    bus->write(hl, data);
}

void CPU::LD_A_BC(uint8_t opcode)
{
    uint16_t bc = (r[B] << 8) | r[C];
    r[A] = bus->read(bc);
}

void CPU::LD_A_DE(uint8_t opcode)
{
    uint16_t de = (r[D] << 8) | r[E];
    r[A] = bus->read(de);
}

void CPU::LD_BC_A(uint8_t opcode)
{
    uint16_t bc = (r[B] << 8) | r[C];
    bus->write(bc, r[A]);
}

void CPU::LD_DE_A(uint8_t opcode)
{
    uint16_t de = (r[D] << 8) | r[E];
    bus->write(de, r[A]);
}

void CPU::LD_A_nn(uint8_t opcode)
{
    uint8_t low = fetch8();
    uint8_t high = fetch8();
    uint16_t nn = (high << 8) | low; //address
    r[A] = bus->read(nn);
}

void CPU::LD_nn_A(uint8_t opcode)
{
    uint8_t low = fetch8();
    uint8_t high = fetch8();
    uint16_t nn = (high << 8) | low; //address
    bus->write(nn, r[A]);
}

void CPU::LDH_A_C(uint8_t opcode)
{
    uint16_t addr = (0xFF00) | r[C];
    r[A] = bus->read(addr);
}

void CPU::LDH_C_A(uint8_t opcode)
{
    uint16_t addr = (0xFF00) | r[C];
    bus->write(addr, r[A]);
}

void CPU::LDH_A_n(uint8_t opcode)
{
    uint8_t n = fetch8();
    uint16_t addr = (0xFF00) | n;
    r[A] = bus->read(addr);
}

void CPU::LDH_n_A(uint8_t opcode)
{
    uint8_t n = fetch8();
    uint16_t addr = (0xFF00) | n;
    bus->write(addr, r[A]);
}

void CPU::LD_A_HLdec(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | (r[L]);
    r[A] = bus->read(hl);

    hl--;
    r[H] = (hl >> 8) & 0xFF;
    r[L] = (hl & 0xFF);
}

void CPU::LD_HLdec_A(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | (r[L]);
    bus->write(hl, r[A]);

    hl--;
    r[H] = (hl >> 8) & 0xFF;
    r[L] = (hl & 0xFF);
}

void CPU::LD_A_HLinc(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | (r[L]);
    r[A] = bus->read(hl);

    hl++;
    r[H] = (hl >> 8) & 0xFF;
    r[L] = (hl & 0xFF);
}

void CPU::LD_HLinc_A(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | (r[L]);
    bus->write(hl, r[A]);

    hl++;
    r[H] = (hl >> 8) & 0xFF;
    r[L] = (hl & 0xFF);
}

void CPU::LD_rr_nn(uint8_t opcode)
{
    uint8_t low = fetch8();
    uint8_t high = fetch8();
    uint16_t nn = (high << 8) | low;

    switch (opcode)
    {
    case 0x01: // LD BC, nn
        r[B] = (nn >> 8) & 0xFF;
        r[C] = nn & 0xFF;
        break;
    case 0x11: // LD DE, nn
        r[D] = (nn >> 8) & 0xFF;
        r[E] = nn & 0xFF;
        break;
    case 0x21: // LD HL, nn
        r[H] = (nn >> 8) & 0xFF;
        r[L] = nn & 0xFF;
        break;
    case 0x31: // LD SP, nn
        SP = nn;
        break;
    }
}

void CPU::LD_nn_SP(uint8_t opcode)
{
    uint8_t low = fetch8();
    uint8_t high = fetch8();
    uint16_t addr = (high << 8) | low;

    bus->write(addr, SP & 0xFF);         // low byte
    bus->write(addr + 1, (SP >> 8) & 0xFF); // high byte
}

void CPU::LD_SP_HL(uint8_t opcode)
{
    SP = (r[H] << 8) | r[L];
}

void CPU::PUSH_rr(uint8_t opcode)
{
    uint16_t value;

    switch (opcode)
    {
    case 0xC5: // PUSH BC
        value = (r[B] << 8) | r[C];
        break;
    case 0xD5: // PUSH DE
        value = (r[D] << 8) | r[E];
        break;
    case 0xE5: // PUSH HL
        value = (r[H] << 8) | r[L];
        break;
    case 0xF5: // PUSH AF
        value = (r[A] << 8) | getF(); // use packed flags
        break;
    default:
        return;
    }

    SP--;
    bus->write(SP, (value >> 8) & 0xFF); // high byte
    SP--;
    bus->write(SP, value & 0xFF);        // low byte
}

void CPU::POP_rr(uint8_t opcode)
{
    uint8_t low = bus->read(SP++);
    uint8_t high = bus->read(SP++);

    switch (opcode)
    {
    case 0xC1: // POP BC
        r[B] = high;
        r[C] = low;
        break;
    case 0xD1: // POP DE
        r[D] = high;
        r[E] = low;
        break;
    case 0xE1: // POP HL
        r[H] = high;
        r[L] = low;
        break;
    case 0xF1: // POP AF
        r[A] = high;
        setF(low); // unpack flags into booleans
        break;
    }
}

void CPU::LD_HL_SPe(uint8_t opcode)
{
    FlagZ = 0;
    FlagN = 0;

    std::int8_t e = fetch8();
    
    uint16_t sum = SP + e;

    FlagH = ((SP & 0x0F) + (e & 0x0F)) > 0x0F;
    FlagC = ((SP & 0xFF) + (uint8_t)e) > 0xFF;

    r[H] = (sum >> 8) & 0xFF;
    r[L] = sum & 0xFF;
}

void CPU::ADD_r(uint8_t opcode)
{
    uint8_t reg = opcode & 0b00000111;
    uint16_t result = r[A] + r[reg];
    
    FlagZ = ((result & 0xFF) == 0);
    FlagN = false;
    FlagH = ((r[A] & 0x0F) + (r[reg] & 0x0F)) > 0x0F; // half-carry
    FlagC = result > 0xFF; // full carry

    r[A] = result & 0xFF;
}

void CPU::ADD_HL(uint8_t opcode)
{

    uint16_t hl = (r[H] << 8) | r[L];
    
    uint8_t value = bus->read(hl);
    uint16_t sum = r[A] + value;

    FlagZ = ((sum & 0xFF) == 0);
    FlagN = false;
    FlagH = ((r[A] & 0x0F) + (value & 0x0F)) > 0x0F;
    FlagC = sum > 0xFF;

    r[A] = sum & 0xFF;
}

void CPU::ADD_n(uint8_t opcode)
{
    uint8_t n = fetch8();
    uint16_t sum = r[A] + n;

    FlagZ = (sum == 0);
    FlagN = false;
    FlagH = halfCarryAdd(r[A], n,0);
    FlagC = (sum > 0xFF);
}

void CPU::ADC_r(uint8_t opcode)
{
    uint8_t reg = opcode & 0b00000111;
    uint8_t value = r[reg];
    uint8_t carry = FlagC ? 1 : 0;

    uint16_t sum = r[A] + value + carry;

    FlagZ = ((sum & 0xFF) == 0);
    FlagN = 0;
    FlagH = halfCarryAdd(r[A],value,carry);
    FlagC = sum > 0xFF;

    r[A] = sum & 0xFF;
}

void CPU::ADC_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];

    uint8_t value = bus->read(hl);
    uint8_t carry = FlagC ? 1 : 0;

    uint16_t sum = r[A] + value + carry;

    FlagZ = ((sum & 0xFF) == 0);
    FlagN = 0;
    FlagH = halfCarryAdd(r[A], value, carry);
    FlagC = sum > 0xFF;

    r[A] = sum & 0xFF;
}

void CPU::ADC_n(uint8_t opcode)
{
    uint8_t value = fetch8();
    uint8_t carry = FlagC ? 1 : 0;

    uint16_t sum = r[A] + value + carry;

    FlagZ = ((sum & 0xFF) == 0);
    FlagN = 0;
    FlagH = halfCarryAdd(r[A], value, carry);
    FlagC = sum > 0xFF;

    r[A] = sum & 0xFF;
}

void CPU::SUB_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint8_t value = r[reg];

    uint8_t a = r[A];
    uint16_t diff = r[A] - value;

    FlagZ = ((diff & 0xFF) == 0);
    FlagN = 1;
    FlagH = (a & 0x0F) < (value & 0x0F);
    FlagC = a < value;

    r[A] = diff & 0xFF;

}

void CPU::SUB_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);

    uint8_t a = r[A];
    uint16_t diff = a - value;

    FlagZ = ((diff & 0xFF) == 0);
    FlagN = 1;
    FlagH = (a & 0x0F) < (value & 0x0F);
    FlagC = a < value;

    r[A] = diff;
}

void CPU::SUB_n(uint8_t opcode)
{
    uint8_t n = fetch8();
    uint16_t diff = r[A] - n;

    FlagZ = (diff & 0xFF) == 0;
    FlagN = 1;
    FlagH = (r[A] & 0x0F) < (n & 0x0F);
    FlagC = (r[A] < n);

    r[A] = diff;
}

void CPU::SBC_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint8_t value = r[reg];
    uint8_t carry = FlagC ? 1 : 0;

    uint16_t diff = r[A] - value - carry;

    FlagZ = (diff & 0xFF) == 0;
    FlagN = 1;
    FlagH = ((r[A] & 0x0F) - (value & 0x0F) - carry) > 0x0F;
    FlagC = r[A] < value + carry;

    r[A] = diff & 0xFF;
}


void CPU::SBC_HL(uint8_t opcode)
{
    
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);
    uint8_t carry = FlagC ? 1 : 0;

    uint16_t diff = r[A] - value - carry;

    FlagZ = (diff & 0xFF) == 0;
    FlagN = 1;
    FlagH = ((r[A] & 0x0F) - (value & 0x0F) - carry) > 0x0F;
    FlagC = r[A] < value + carry;

    r[A] = diff & 0xFF;
}

void CPU::SBC_n(uint8_t opcode)
{
    uint8_t n = fetch8();
    uint8_t value = n;
    uint8_t carry = FlagC ? 1 : 0;

    uint16_t diff = r[A] - value - carry;

    FlagZ = (diff & 0xFF) == 0;
    FlagN = 1;
    FlagH = ((r[A] & 0x0F) - (value & 0x0F) - carry) > 0x0F;
    FlagC = r[A] < value + carry;

    r[A] = diff & 0xFF;
}

void CPU::CP_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint8_t value = r[reg];

    uint8_t a = r[A];
    uint16_t diff = r[A] - value;

    FlagZ = ((diff & 0xFF) == 0);
    FlagN = 1;
    FlagH = (a & 0x0F) < (value & 0x0F);
    FlagC = a < value;
}

void CPU::CP_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);

    uint8_t a = r[A];
    uint16_t diff = a - value;

    FlagZ = ((diff & 0xFF) == 0);
    FlagN = 1;
    FlagH = (a & 0x0F) < (value & 0x0F);
    FlagC = a < value;
}

void CPU::CP_n(uint8_t opcode)
{
    uint8_t n = fetch8();
    uint16_t diff = r[A] - n;

    FlagZ = (diff & 0xFF) == 0;
    FlagN = 1;
    FlagH = (r[A] & 0x0F) < (n & 0x0F);
    FlagC = (r[A] < n);
}

void CPU::INC_r(uint8_t opcode)
{
    uint8_t regIndex = (opcode & 0b00111000) >> 3;


    uint8_t before = r[regIndex];
    r[regIndex]++;

    // Set flags
    FlagZ = (r[regIndex] == 0);
    FlagN = 0;
    FlagH = (before & 0x0F) == 0x0F;
}

void CPU::INC_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);
    uint8_t result = value+1;

    FlagZ = result == 0;
    FlagN = 0;
    FlagH = (value & 0x0F) == 0x0F;

    bus->write(hl, result);
}

void CPU::DEC_r(uint8_t opcode)
{
    uint8_t regIndex = (opcode & 0b00111000) >> 3;


    uint8_t before = r[regIndex];
    r[regIndex]--;

    // Set flags
    FlagZ = (r[regIndex] == 0);
    FlagN = 1;
    FlagH = (before & 0x0F) == 0x00;
}

void CPU::DEC_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);
    uint8_t result = value - 1;

    FlagZ = result == 0;
    FlagN = 1;
    FlagH = (value & 0x0F) == 0x00;

    bus->write(hl, result);
}

void CPU::AND_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint8_t value = r[reg];
    uint8_t result = r[A] & value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 1;
    FlagC = 0;

    r[A] = result;
}

void CPU::AND_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);
    uint8_t result = r[A] & value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 1;
    FlagC = 0;

    r[A] = result;
}

void CPU::AND_n(uint8_t opcode)
{
    uint8_t value = fetch8();
    uint8_t result = r[A] & value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 1;
    FlagC = 0;

    r[A] = result;
}

void CPU::OR_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint8_t value = r[reg];
    uint8_t result = r[A] | value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 0;
    FlagC = 0;

    r[A] = result;
}

void CPU::OR_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);
    uint8_t result = r[A] | value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 0;
    FlagC = 0;

    r[A] = result;
}

void CPU::OR_n(uint8_t opcode)
{
    uint8_t value = fetch8();
    uint8_t result = r[A] | value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 0;
    FlagC = 0;

    r[A] = result;
}

void CPU::XOR_r(uint8_t opcode)
{
    uint8_t reg = (opcode & 0b00000111);
    uint8_t value = r[reg];
    uint8_t result = r[A] ^ value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 0;
    FlagC = 0;

    r[A] = result;
}

void CPU::XOR_HL(uint8_t opcode)
{
    uint16_t hl = (r[H] << 8) | r[L];
    uint8_t value = bus->read(hl);
    uint8_t result = r[A] ^ value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 0;
    FlagC = 0;

    r[A] = result;
}

void CPU::XOR_n(uint8_t opcode)
{
    uint8_t value = fetch8();
    uint8_t result = r[A] ^ value;

    FlagZ = (result == 0);
    FlagN = 0;
    FlagH = 0;
    FlagC = 0;

    r[A] = result;
}

void CPU::CCF(uint8_t opcode)
{
    FlagN = 0;
    FlagH = 0;
    FlagC = !FlagC;
}

void CPU::SCF(uint8_t opcode)
{
    FlagN = 0;
    FlagH = 0;
    FlagC = 1;
}

void CPU::DAA(uint8_t opcode)
{
    uint8_t a = r[A];
    uint8_t correction = 0;
    bool carry = FlagC;

    if (!FlagN) {
        if (FlagH || (a & 0x0F) > 9)
            correction |= 0x06;
        if (FlagC || a > 0x99) {
            correction |= 0x60;
            carry = true;
        }
        a += correction;
    }
    else {
        if (FlagH)
            correction |= 0x06;
        if (FlagC)
            correction |= 0x60;
        a -= correction;
    }

    r[A] = a;

    FlagZ = (a == 0);
    FlagH = 0;
    FlagC = carry;
}

void CPU::CPL(uint8_t opcode)
{
    r[A] = ~r[A];
    FlagN = 1;
    FlagH = 1;
}

void CPU::INC_rr(uint8_t opcode)
{
    switch ((opcode >> 4) & 0b11) {
    case 0: setBC(getBC() + 1); break;
    case 1: setDE(getDE() + 1); break;
    case 2: setHL(getHL() + 1); break;
    case 3: SP++; break;
    }
}

void CPU::DEC_rr(uint8_t opcode)
{
    switch ((opcode >> 4) & 0b11) {
    case 0: setBC(getBC() - 1); break;
    case 1: setDE(getDE() - 1); break;
    case 2: setHL(getHL() - 1); break;
    case 3: SP--; break;
    }
}

void CPU::ADD_HL_rr(uint8_t opcode)
{
    uint16_t hl = getHL();
    uint16_t value;

    switch ((opcode >> 4) & 0b11) {
    case 0: value = getBC(); break;
    case 1: value = getDE(); break;
    case 2: value = getHL(); break;
    case 3: value = SP; break;
    }

    uint32_t result = hl + value;

    FlagN = 0;
    FlagH = ((hl & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF;
    FlagC = result > 0xFFFF;

    setHL(result & 0xFFFF);
}

void CPU::ADD_SP_e(uint8_t opcode)
{
    int8_t e = (int8_t)fetch8();
    uint16_t sp = SP;

    uint16_t result = sp + e;

    FlagZ = 0;
    FlagN = 0;
    FlagH = ((sp & 0x0F) + (e & 0x0F)) > 0x0F;
    FlagC = ((sp & 0xFF) + (e & 0xFF)) > 0xFF;

    SP = result;
}

void CPU::RLCA(uint8_t opcode)
{
    uint8_t bit7 = (r[A] >> 7) & 1;

    r[A] = (r[A] << 1) | bit7;

    FlagZ = 0;
    FlagN = 0;
    FlagH = 0;
    FlagC = bit7;
}

void CPU::RRCA(uint8_t opcode)
{
    uint8_t bit0 = (r[A] & 0b00000001);
    uint8_t result = (r[A] >> 1) | (bit0 << 7);
    FlagZ = 0;
    FlagN = 0;
    FlagH = 0;
    FlagC = bit0;
    r[A] = result;
}

void CPU::RLA(uint8_t opcode)
{

    uint8_t bit7 = (r[A] >> 7) & 1;

    r[A] = (r[A] << 1) | FlagC;
    FlagZ = 0;
    FlagN = 0;
    FlagH = 0;
    FlagC = bit7;
    
}