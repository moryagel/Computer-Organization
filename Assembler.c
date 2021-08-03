#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#define LINE_SIZE 500
#define MAX_ADDRESSES 4096
#define LINE_HEX_SIZE 4
#define strdup _strdup

typedef struct label_s { //linked list of labels
	char* name;
	int value;
	struct label_s* next;
}Label;

void removePrefixAndComment(char* line);
int opcode(char* cmd);
int sub_opcode(char* cmd);
int register_code(char* reg);
int extractImmVal(char* ptr, Label* head, int mem_addr, int code);
void writeToOutputFile(char* name, int mem_addr);
void freeList();
int handleWordCommand(char* name, int mem_addr);
int handleCommands(char* name);
void handleLabels(char* name);

Label* head;
char mem[MAX_ADDRESSES][LINE_HEX_SIZE + 1];

/******************************************************************
 * Description: This is the main function
 *****************************************************************/

int main(int argc, char** argv) {

	if (argc != 3)
		exit(1);

	int mem_addr = 0;
	memset(mem, (int)'0', MAX_ADDRESSES * (LINE_HEX_SIZE + 1));
	if (!(head = (Label*)malloc(sizeof(Label))))
		exit(1);

	// Get Labels
	handleLabels(argv[1]);

	// Get Cmds
	mem_addr = handleCommands(argv[1]);

	// Get .word
	mem_addr = handleWordCommand(argv[1], mem_addr);

	// write to output file
	writeToOutputFile(argv[2], mem_addr);

	// clear labels list
	freeList();

}

/******************************************************************
 * Description: This function handle with labels cmmands
 * Input: the name
 * Output: int - the address
 *****************************************************************/
void handleLabels(char* name) {
	FILE* fp;
	char* ptr, line[LINE_SIZE + 1];
	int addr = 0;
	Label* list = head, * prev;
	memset(line, 0, LINE_SIZE + 1);

	if (!(fp = fopen(name, "r"))) {
		perror("fopen");
		exit(1);
	}

	while (fgets(line, LINE_SIZE, fp)) {

		removePrefixAndComment(line);
		if (*line == '\0')
			continue;

		if (strchr(line, ':')) {
			// its a label
			ptr = strtok(line, ":");
			list->name = strdup(ptr);
			list->value = addr;
			list->next = (Label*)malloc(sizeof(Label));
			list->next->next = NULL;
			list->next->name = NULL;
			prev = list;
			list = list->next;
			prev->next = list;


			// if theres a cmd after label
			if ((ptr = strtok(NULL, ":"))) {
				removePrefixAndComment(ptr);
				if (*ptr != '\0' && strncmp(ptr, ".word", 5) != 0)
					addr++;
			}
		}
		else if (strncmp(line, ".word", 5) != 0)
			addr++;

	}

	fclose(fp);
}
/******************************************************************
 * Description: This function handle with regular cmmands
 * Input: the name
 * Output: int - the address
 ******************************************************************/
int handleCommands(char* name) {
	FILE* fp;
	char* ptr;
	int rd, rs, imm, code, mem_addr = 0;
	char line[LINE_SIZE + 1];
	memset(line, 0, LINE_SIZE + 1);

	if (!(fp = fopen(name, "r"))) {
		perror("fopen");
		exit(1);
	}

	while (fgets(line, LINE_SIZE, fp)) {
		removePrefixAndComment(line);
		if (*line == '\0' || strncmp(line, ".word", 5) == 0)
			continue;

		if ((ptr = strchr(line, ':'))) {
			if (strlen(ptr) == 1)
				continue;
			ptr = strdup(ptr + 1);
			memset(line, 0, LINE_SIZE * sizeof(char));
			strcpy(line, ptr);
			removePrefixAndComment(line);
			if (*line == '\0' || strncmp(line, ".word", 5) == 0)
				continue;
		}

		ptr = strtok(line, " ,\t");
		code = opcode(ptr);
		ptr = strtok(NULL, " ,\t");
		rd = register_code(ptr);
		ptr = strtok(NULL, " ,\t");
		rs = register_code(ptr);

		ptr = strtok(NULL, " ,\t");
		if (code == 0)
			imm = sub_opcode(ptr);
		else
			imm = extractImmVal(ptr, head, mem_addr, code);

		if (code < 5)
			sprintf(mem[mem_addr], "%01X%01X%01X%01X", code & 0x0f, rd & 0x0f, rs & 0x0f, imm & 0x0f);
		else if (code < 9)
			sprintf(mem[mem_addr], "%01X%01X%02X", code & 0x0f, rd & 0x0f, imm & 0x0ff);
		else
			sprintf(mem[mem_addr], "%01X%03X", code & 0x0f, imm & 0x0fff);

		mem_addr++;
	}
	fclose(fp);

	return mem_addr;
}

/******************************************************************
 * Description: This function handle with words cmmands
 * Input:
 * Output: int - the address
 ******************************************************************/
int handleWordCommand(char* name, int mem_addr) {
	FILE* fp;
	int value, address;
	char* ptr;
	char line[LINE_SIZE + 1];
	memset(line, 0, LINE_SIZE + 1);

	if (!(fp = fopen(name, "r"))) {
		perror("fopen");
		exit(1);
	}

	while (fgets(line, LINE_SIZE, fp)) {
		removePrefixAndComment(line);
		if (*line == '\0')
			continue;

		ptr = strtok(line, " ,\t");

		if (strcmp(ptr, ".word") != 0)
			continue;

		ptr = strtok(NULL, " ,\t");

		if (strncmp(ptr, "0x", 2) == 0)
			address = (int)(strtol(ptr, NULL, 16));
		else
			address = (int)(strtol(ptr, NULL, 10));

		ptr = strtok(NULL, " ,\t");

		if (strncmp(ptr, "0x", 2) == 0)
			value = (int)(strtol(ptr, NULL, 16));
		else
			value = (int)(strtol(ptr, NULL, 10));

		sprintf(mem[address], "%04X", value & 0x0ffff);

		if (address > mem_addr)
			mem_addr = address;

	}

	fclose(fp);

	return mem_addr;
}
/******************************************************************
 * Description: This function free the linked_list of the labels. 
 * Input:       
 * Output:      void
 ******************************************************************/
void freeList() {
	Label* prev;
	while (head) {
		prev = head->next;
		free(head);
		head = prev;
	}
}
/******************************************************************
 * Description: This function write to output file.
 * Input:       The name of the output file, the mem_address
 * Output:      void
 ******************************************************************/

void writeToOutputFile(char* name, int mem_addr) {
	FILE* fp;
	if (!(fp = fopen(name, "w"))) {
		perror("fopen");
		exit(1);
	}

	for (int j = 0; j < mem_addr + 1; j++) {
		fwrite(mem[j], sizeof(char), LINE_HEX_SIZE, fp);
		fwrite("\n", sizeof(char), 1, fp);
	}

	fclose(fp);
}

/******************************************************************
 * Description: This function make a new line (using strdup and strcpy) that identical to line but without spaces (or any prefixes) or comments 
 * Input:       The name of the opcode
 * Output:      void
 ******************************************************************/
void removePrefixAndComment(char* line) {
	char* ptr = line;

	while ((*ptr == ' ' || *ptr == '\t'))
		ptr++;

	if (*ptr == '#' || *ptr == '\n') {
		memset(line, 0, LINE_SIZE + 1);
		return;
	}

	ptr = strtok(ptr, "#\r\n");
	ptr = strdup(ptr);
	memset(line, 0, LINE_SIZE + 1);
	strcpy(line, ptr);
}

/******************************************************************
 * Description: This function returns the number of the opcode by comparing the
 * name of the opcode
 * Input:       The name of the opcode
 * Output:      int - The number of the opcode
 ******************************************************************/
int opcode(char* cmd) {
	int num;
	if (strcmp(cmd, "noimm") == 0)
		num = 0;
	else if (strcmp(cmd, "beq") == 0)
		num = 1;
	else if (strcmp(cmd, "bne") == 0)
		num = 2;
	else if (strcmp(cmd, "lw") == 0)
		num = 3;
	else if (strcmp(cmd, "sw") == 0)
		num = 4;
	else if (strcmp(cmd, "bgtz") == 0)
		num = 5;
	else if (strcmp(cmd, "blez") == 0)
		num = 6;
	else if (strcmp(cmd, "limm") == 0)
		num = 7;
	else if (strcmp(cmd, "lhi") == 0)
		num = 8;
	else if (strcmp(cmd, "j") == 0)
		num = 9;
	else if (strcmp(cmd, "jal") == 0)
		num = 10;
	else if (strcmp(cmd, "halt") == 0)
		num = 15;
	else
		exit(1);
	return num;
}

/******************************************************************
 * Description: This function returns the number of the imm4 by comparing the
 * name of the subopocode
 * Input:       The name of the subopcode
 * Output:      int - The immediate of the subopcode
 ******************************************************************/
int sub_opcode(char* cmd) {
	int num;
	if (strcmp(cmd, "add") == 0)
		num = 0;
	else if (strcmp(cmd, "sub") == 0)
		num = 1;
	else if (strcmp(cmd, "and") == 0)
		num = 2;
	else if (strcmp(cmd, "or") == 0)
		num = 3;
	else if (strcmp(cmd, "sll") == 0)
		num = 4;
	else if (strcmp(cmd, "srl") == 0)
		num = 5;
	else if (strcmp(cmd, "sra") == 0)
		num = 6;
	else if (strcmp(cmd, "mul") == 0)
		num = 7;
	else if (strcmp(cmd, "jr") == 0)
		num = 8;
	else
		exit(1);
	return num;
}

/******************************************************************
 * Description: This function returns the number of a register by comparing the
 * name of the register
 * Input:       The name of the register
 * Output:      The register number (int)
 ******************************************************************/

int register_code(char* reg) {
	int num;
	if (strcmp(reg, "$zero") == 0)
		num = 0;
	else if (strcmp(reg, "$at") == 0)
		num = 1;
	else if (strcmp(reg, "$v0") == 0)
		num = 2;
	else if (strcmp(reg, "$a0") == 0)
		num = 3;
	else if (strcmp(reg, "$a1") == 0)
		num = 4;
	else if (strcmp(reg, "$t0") == 0)
		num = 5;
	else if (strcmp(reg, "$t1") == 0)
		num = 6;
	else if (strcmp(reg, "$t2") == 0)
		num = 7;
	else if (strcmp(reg, "$t3") == 0)
		num = 8;
	else if (strcmp(reg, "$s0") == 0)
		num = 9;
	else if (strcmp(reg, "$s1") == 0)
		num = 10;
	else if (strcmp(reg, "$s2") == 0)
		num = 11;
	else if (strcmp(reg, "$gp") == 0)
		num = 12;
	else if (strcmp(reg, "$sp") == 0)
		num = 13;
	else if (strcmp(reg, "$fp") == 0)
		num = 14;
	else if (strcmp(reg, "$ra") == 0)
		num = 15;
	else
		exit(1);
	return num;
}

/******************************************************************
 * Description: This function returns the immediate value of a command - the function checks if the
 * Input:       A pointer to command, linked list of labels, mem address and
 * Output:      The register number (int)
 ******************************************************************/
int extractImmVal(char* ptr, Label * head, int mem_addr, int code) {
	int imm = 0;

	if (strncmp(ptr, "0x", 2) == 0) //hexa immediate representation
		imm = (int)(strtol(ptr, NULL, 16));
	else if (*ptr >= '0' && *ptr <= '9') //decimal immediate representation
		imm = (int)(strtol(ptr, NULL, 10));
	else {								// it's a label so take it from the linked list
		Label* list = head;
		while (list != NULL && list->name != NULL) {
			if (strcmp(list->name, ptr) == 0) {
				if (code == 9 || code == 10)
					imm = list->value;
				else
					imm = list->value - mem_addr - 1;
				break;
			}
			list = list->next;
		}
		if (list == NULL)
			exit(1);
	}

	return imm;
}