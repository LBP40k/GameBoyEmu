#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Bus.h"

class CPU;

struct Instruction {
	std::string name;
	uint8_t cycles;
	void (CPU::* execute)(uint8_t opcode);
};

class CPU
{
private:
	bool usesHL(uint8_t opcode);
public:
	CPU();
	int cyclesRemaining = 0;


	std::array<Instruction, 256> opcodes;
	std::array<Instruction, 256> cbOpcodes;


	enum Reg8 { B, C, D, E, H, L, HL_INDIRECT, A};
	uint8_t r[8];

	uint8_t getF()
	{
		uint8_t f = 0;
		if (FlagZ) f |= 0x80; // bit 7
		if (FlagN) f |= 0x40; // bit 6
		if (FlagH) f |= 0x20; // bit 5
		if (FlagC) f |= 0x10; // bit 4
		// lower 4 bits are always 0
		return f;
	}
	void setF(uint8_t f)
	{
		FlagZ = f & 0x80;
		FlagN = f & 0x40;
		FlagH = f & 0x20;
		FlagC = f & 0x10;
	}

	bool FlagZ = false;
	bool FlagN = false;
	bool FlagH = false;
	bool FlagC = false;

	uint16_t PC;
	uint16_t SP;

	Bus *bus;

	uint8_t readMemory(uint16_t addr);
	void writeMemory(uint16_t addr, uint8_t value);
	void attachBus(Bus *newBus);
	
	void setBC(uint16_t value);
	uint16_t getBC();
	void setDE(uint16_t value);
	uint16_t getDE();
	void setHL(uint16_t value);
	uint16_t getHL();

	

	uint8_t fetch8();
	uint16_t fetch16();

	void execute(uint8_t opcode);


	//Instructions
	void UNDEFINED(uint8_t opcode);
	void NOP(uint8_t opcode);

	void INC_r(uint8_t opcode);

	void INC_HL(uint8_t opcode);

	void DEC_r(uint8_t opcode);

	void DEC_HL(uint8_t opcode);

	void AND_r(uint8_t opcode);

	void AND_HL(uint8_t opcode);

	void AND_n(uint8_t opcode);

	void OR_r(uint8_t opcode);

	void OR_HL(uint8_t opcode);

	void OR_n(uint8_t opcode);

	void XOR_r(uint8_t opcode);

	void XOR_HL(uint8_t opcode);

	void XOR_n(uint8_t opcode);

	void CCF(uint8_t opcode);

	void SCF(uint8_t opcode);

	void DAA(uint8_t opcode);

	void CPL(uint8_t opcode);

	void INC_rr(uint8_t opcode);

	void DEC_rr(uint8_t opcode);

	void ADD_HL_rr(uint8_t opcode);

	void ADD_SP_e(uint8_t opcode);

	void RLCA(uint8_t opcode);

	void RRCA(uint8_t opcode);

	void RLA(uint8_t opcode);

	void LD_r_r(uint8_t opcode);

	void LD_r_n(uint8_t opcode);

	void LD_r_HL(uint8_t opcode);

	void LD_HL_r(uint8_t opcode);

	void LD_HL_n(uint8_t opcode);

	void LD_A_BC(uint8_t opcode);

	void LD_A_DE(uint8_t opcode);

	void LD_BC_A(uint8_t opcode);

	void LD_DE_A(uint8_t opcode);

	void LD_A_nn(uint8_t opcode);

	void LD_nn_A(uint8_t opcode);

	void LDH_A_C(uint8_t opcode);

	void LDH_C_A(uint8_t opcode);

	void LDH_A_n(uint8_t opcode);

	void LDH_n_A(uint8_t opcode);

	void LD_A_HLdec(uint8_t opcode);

	void LD_HLdec_A(uint8_t opcode);

	void LD_A_HLinc(uint8_t opcode);

	void LD_HLinc_A(uint8_t opcode);

	void LD_rr_nn(uint8_t opcode);

	void LD_nn_SP(uint8_t opcode);

	void LD_SP_HL(uint8_t opcode);

	void PUSH_rr(uint8_t opcode);

	void POP_rr(uint8_t opcode);

	void LD_HL_SPe(uint8_t opcode);

	void ADD_r(uint8_t opcode);

	void ADD_HL(uint8_t opcode);

	void ADD_n(uint8_t opcode);

	void ADC_r(uint8_t opcode);

	void ADC_HL(uint8_t opcode);

	void ADC_n(uint8_t opcode);

	void SUB_r(uint8_t opcode);

	void SUB_HL(uint8_t opcode);

	void SUB_n(uint8_t opcode);

	void SBC_r(uint8_t opcode);

	void SBC_HL(uint8_t opcode);

	void SBC_n(uint8_t opcode);

	void CP_r(uint8_t opcode);

	void CP_HL(uint8_t opcode);

	void CP_n(uint8_t opcode);

private:
	void initOpcodes();
	bool halfCarryAdd(uint8_t a, uint8_t b,uint8_t carry);



	


	
	
	

};

