#define _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS
#define _CRT_SECURE_NO_WARNINGS
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#pragma warning(disable:4996)

#define ERROR_CODE	0
#define MAX_READ	5 
#define MEM_SPACE   4096 

signed short SHRT_MAX_MISP = 0x7FFF;
signed short SHRT_MIN_MISP = 0x8000;
unsigned long lower16bits_MASK_MISP = 0x0000FFFF;


short saturated_shorts(short a, short b, int op);
int ascii2int(char ch);
short signExtensionSimm4(int c);
short signExtensionSimm8(int c);
short intCharsToInt(int a, int b, int c, int d);

int main(int argc, char* argv[])
{
	unsigned int PC = 0;
	int MEM[MEM_SPACE][4];
	char buf[MAX_READ]; //each line in memin.txt has 4 charcters
	char* line; //hold every line in its turn
	int num_of_lines_memin = 0;
	int num_of_operated_instructions = 0;
	int indexOfCharOfHex = 0;
	short R[16]; //16 registers, each one of 16 bits
	if (argc != 6)//assuming the arguments are at order and valid
	{
		printf("Number of arguments isn't valid! exiting!");
		return ERROR_CODE;
	}

	FILE* file = fopen(argv[1], "r");
	FILE* memout = fopen(argv[2], "w");
	FILE* regout = fopen(argv[3], "w");
	FILE* trace = fopen(argv[4], "w");
	FILE* count = fopen(argv[5], "w");

	if (file == NULL || memout == NULL || regout == NULL || trace == NULL || count == NULL) {
		printf("something went wrong with opening the file!");
		return ERROR_CODE;
	}


	for (int a = 0; a < 16; a++) { //initialize the registers array to zeros
		R[a] = 0;
	}

	for (int b = 0; b < MEM_SPACE; b++) //initialize the memory array to zeros
	{
		for (int d = 0; d < 4; d++)
			MEM[b][d] = 0;
	}

	line = fgets(buf, sizeof(buf), file);//reads the first line from the test txt
	if (line) //if the file isnt emty, enters the if statment
	{
		for (int c = 0; c < 4; c++)
		{
			MEM[num_of_lines_memin][c] = ascii2int(buf[c]);
		}
		num_of_lines_memin++;
	}
	while (line) //in this loop we will fill the 2d array: for each line in memin.txt, we will have the int represntation of each of the four char
	{
		line = fgets(buf, sizeof(buf), file);
		line = fgets(buf, sizeof(buf), file);
		if (line)
		{
			for (int e = 0; e < 4; e++)
			{
				MEM[num_of_lines_memin][e] = ascii2int(buf[e]);
			}
			num_of_lines_memin++;
		}
		else break;//if we read a line that is NULL means we reached the end of the file
	}

	while (true) //T H E M A I N L O O P 
	{
		if (PC < MEM_SPACE && PC >= 0)
		{
			fprintf(trace, "%04X ", PC);
			fprintf(trace, "%04X ", (unsigned short)intCharsToInt(MEM[PC][0], MEM[PC][1], MEM[PC][2], MEM[PC][3]));
			for (int f = 0; f < 15; f++)
				fprintf(trace, "%04X ", R[f]);
			fprintf(trace, "%04X\n", R[15]);
			switch (MEM[PC][0])
			{
			case 0: //noimm opcode
				switch (MEM[PC][3])
				{
				case 0: //add
					R[MEM[PC][1]] = saturated_shorts(R[MEM[PC][1]], R[MEM[PC][2]], 0);
					break;
				case 1://sub
					R[MEM[PC][1]] = saturated_shorts(R[MEM[PC][1]], R[MEM[PC][2]], 1);
					break;
				case 2:
					R[MEM[PC][1]] = R[MEM[PC][1]] & R[MEM[PC][2]]; //bitwiseAND
					break;
				case 3:
					R[MEM[PC][1]] = R[MEM[PC][1]] | R[MEM[PC][2]]; //bitwiseOR
					break;
				case 4: //sll
					R[MEM[PC][1]] = saturated_shorts(R[MEM[PC][1]], R[MEM[PC][2]], 4); //sll
					break;
				case 5:
					R[MEM[PC][1]] = saturated_shorts(R[MEM[PC][1]], R[MEM[PC][2]], 5); //srl, since short type is signed, it is an arithmetic shift
					break;
				case 6:
					R[MEM[PC][1]] = saturated_shorts(((unsigned short)R[MEM[PC][1]]), R[MEM[PC][2]], 6); //srl, logical shift
					break;
				case 7:
					R[MEM[PC][1]] = saturated_shorts(R[MEM[PC][1]], R[MEM[PC][2]], 7); // multiply
					printf("%d", R[MEM[PC][1]]);
					break;
				case 8: //jr
					PC = (R[MEM[PC][1]] & 0xfff) - 1;
					if (!(PC < MEM_SPACE && PC >= 0)) {
						printf("PC is not in the valid range of the mamory space! goodby!");
						return;
					}
					break;
					// an option to add more optional "reserved values"
				}
				break;
			case 1: // beq
				if (R[MEM[PC][1]] == R[MEM[PC][2]])
				{
					PC = PC + 1 + signExtensionSimm4(MEM[PC][3]) - 1;
					if (!(PC < MEM_SPACE && PC >= 0)) {
						printf("PC is not in the valid range of the mamory space! goodby!");
						return;
					}
				}
				break;
			case 2://bne
				if (R[MEM[PC][1]] != R[MEM[PC][2]])
				{
					PC = PC + 1 + signExtensionSimm4(MEM[PC][3]) - 1;
					if (!(PC < MEM_SPACE && PC >= 0)) {
						printf("PC is not in the valid range of the mamory space! goodby!");
						return;
					}
				}
				break;
			case 3://lw 
				R[MEM[PC][1]] = (unsigned short)intCharsToInt(MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][0], MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][1], MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][2], MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][3]);
				break;
			case 4://sw 
				MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][0] = (R[MEM[PC][1]] & 0xF000) >> 12;
				MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][1] = (R[MEM[PC][1]] & 0x0F00) >> 8;
				MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][2] = (R[MEM[PC][1]] & 0x00F0) >> 4;
				MEM[(R[MEM[PC][2]] + signExtensionSimm4(MEM[PC][3])) & 0xfff][3] = (R[MEM[PC][1]] & 0x000F);
				break;
			case 5://bgtz 
				if (R[MEM[PC][1]] > 0)
				{
					PC = PC + 1 + signExtensionSimm8((MEM[PC][2] << 4) | MEM[PC][3]) - 1;
					if (!(PC < MEM_SPACE && PC >= 0)) {
						printf("PC is not in the valid range of the mamory space! goodby!");
						return;
					}
				}
				break;
			case 6://blez
				if (R[MEM[PC][1]] <= 0)
				{
					PC = PC + 1 + signExtensionSimm8((MEM[PC][2] << 4) | MEM[PC][3]) - 1;
					if (!(PC < MEM_SPACE && PC >= 0)) {
						printf("PC is not in the valid range of the mamory space! goodby!");
						return;
					}
				}
				break;
			case 7://limm
				R[MEM[PC][1]] = signExtensionSimm8((MEM[PC][2] << 4) | MEM[PC][3]);
				break;
			case 8://lhi
				R[MEM[PC][1]] = (((MEM[PC][2] << 4) | MEM[PC][3]) << 8) | (R[MEM[PC][1]] & 0xff);
				break;
			case 9://j
				PC = (MEM[PC][1] << 8) | (MEM[PC][2] << 4) | MEM[PC][3] - 1;
				if (!(PC < MEM_SPACE && PC >= 0)) {
					printf("PC is not in the valid range of the mamory space! goodby!");
					return;
				}
				break;
			case 10://jal
				R[15] = ((PC + 1) & 0xfff);
				PC = (MEM[PC][1] << 8) | (MEM[PC][2] << 4) | MEM[PC][3] - 1;
				if (!(PC < MEM_SPACE && PC >= 0)) {
					printf("PC is not in the valid range of the mamory space! goodby!");
					return;
				}
				break;
			case 11:
				//reserved
				break;
			case 12:
				break;
			case 13:
				break;
			case 14:
				//reserved
				break;
			case 15://halt
				//end of main loop, means we finished all the operaions. lets print the memort state at the end of operation:
				for (int f = 0; f < 16; f++)
					fprintf(regout, "%04X\n", R[f]);
				for (int g = 0; g < MEM_SPACE; g++)
					fprintf(memout, "%04X\n", (unsigned short)intCharsToInt(MEM[g][0], MEM[g][1], MEM[g][2], MEM[g][3]));
				num_of_operated_instructions++;
				fprintf(count, "%d\n", num_of_operated_instructions);
				printf("exiting the simulator because i got a halt instruction");
				return; //exit the simulator
				break;
			}
			//break;
			PC++;
			num_of_operated_instructions++;
		}
		else
		{
			printf("PC is not in the valid range of the mamory space! goodby!");
			return;
		}
	}

}
/******************************************************************
 * Description: This function generates a short value out of four integers.
 * Input:        4 ints- each one of them contain a HEX char of the current operation when the function is called
 * Output:      a,b,c,d cascaded (together they are of type short).
 ******************************************************************/
short intCharsToInt(int a, int b, int c, int d)
{
	return (a << 12 | b << 8 | c << 4 | d);
}
/******************************************************************
 * Description: This function does sign extension from 4 bits to 16 bits.
 * Input:       a 4 bit integer.
 * Output:      the number with sign extension to 16 bits.
 ******************************************************************/
short signExtensionSimm4(int c)
{
	if ((c & 0x8) != 0) //means c represents a negative number
		return (0xFFF8 | c);
	return (0x0007 & c); //else: means c represents a positive number
}
/******************************************************************
 * Description: This function does sign extension from 8 bits to 16 bits.
 * Input:       a 8 bit integer.
 * Output:      the number with sign extension to 16 bits.
 ******************************************************************/
short signExtensionSimm8(int c)
{
	if ((c & 0x80) != 0) //means c represents a negative number
		return (0xFF80 | c);
	return (0x007F & c); //else: means c represents a positive number
}
/******************************************************************
 * Description: This function converts ascii representaion to int representaion
 * Input:      a value of type character
 * Output:     the corresponding numeric value
 ******************************************************************/
int ascii2int(char ch)//input arguments: a character. returns its numeric value.
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	return ERROR_CODE;
}
/******************************************************************
 * Description: This function converts performs desired operation on two short values.
 * Input:      two numeric values of type short, and an operator indicator
 * Output:	   the operation's result value.
 * note: if the result is longer than 16 bits, the function will return only the 16 lower bits.
 ******************************************************************/

short saturated_shorts(short a, short b, int op)
{
	long temp;
	switch (op)
	{
	case 0:
		temp = (long)a + (long)b;
		break;
	case 1:
		temp = (long)a - (long)b;
		break;
	case 4:
		temp = (long)a << (long)b;
		break;
	case 5:
		temp = (long)a >> (long)b;
		break;
	case 6:
		temp = (long)a >> (long)b;
		break;
	case 7:
		temp = (long)a * (long)b;
		break;
	}
	if (temp > SHRT_MAX_MISP || temp < SHRT_MIN_MISP)
		return (short)(temp & lower16bits_MASK_MISP);
	return (short)temp;
}