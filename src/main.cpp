#include <iostream>
#include <sstream>
#include <iomanip>
#include "Bus.h"
#include "CPU.h"


std::string toHex(uint8_t value, bool uppercase = true) {
	std::stringstream ss;
	if (uppercase) ss << std::uppercase;
	ss << "0x"
		<< std::hex
		<< std::setw(2)
		<< std::setfill('0')
		<< (int)value;
	return ss.str();
}

int main()
{
	Bus bus;
	CPU cpu;

	std::cout << "Reading from memory: \n";
	cpu.attachBus(&bus);
	
	while (true)
	{
		uint8_t opcode = cpu.fetch8();
		cpu.execute(opcode);
		std::cout << "Opcode: " << toHex(opcode) << '\n';
		std::cout << "A register: " << toHex(cpu.r[7]) << '\n';
		if (opcode == 0x00)
		{

			std::cout << "Done!\n";
			break;
		}
		
	}

	
	return 0;
}