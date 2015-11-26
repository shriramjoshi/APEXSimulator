/*
 * sim.h
 *
 *  Created on: 08-Nov-2015
 *      Author: shriram
 */

#ifndef SIM_H_
#define SIM_H_

#define INITMEMADDRESS 20000	//First instruction is at memory location 20000
#define PIPELINENODES 5			//5 for 5 pipeline stages Fetch, Decode, Execute,Memory,Write-back

int PC = 0;						//Program counter assumed to be pointing at 20000 location
int literalvalue = 0;			//literal value holder ex MOVC R1 100 so this variable holds value 100
int R[8];						//8 Architectural registers	R0 to R7
int register_status[8];			//Tells whether register contains valid data or no. 0 valid 1 invalid
int memory[10000];				//Memory viewed as 4 bytes wide & from 0 to 9999
int X = 0;						//Used as register to hold value for BAL instruction
typedef enum
{
	E_PL_STAGE_FETCH = 0,		// Fetch stage
	E_PL_STAGE_DECODE,			// D/RF stage
	E_PL_STAGE_EXECUTION,		// Execution stage
	E_PL_STAGE_MEM,				// Memory stage
	E_PL_STAGE_WB				// Write-back stage
}E_PIPELINE_STAGE_TYPE;

typedef enum
{
	E_I_ADD = 0,
	E_I_SUB,
	E_I_MOVC,
	E_I_MOV,
	E_I_MUL,
	E_I_AND,
	E_I_OR,
	E_I_XOR,
	E_I_LOAD,
	E_I_STORE,
	E_I_BZ,
	E_I_BNZ,
	E_I_JUMP,
	E_I_BAL,
	E_I_HALT
}E_INSTRUCTION_TYPE;

typedef enum
{
	R_STATUS_VALID = 0,
	R_STATUS_INVALID = -9999
}E_REGISTER_STATUS;

/**
 *	Structure contains the information about instruction
 */
typedef struct
{
	E_INSTRUCTION_TYPE enInstType;
	int left_ip;								//Architectural register left input operand index in register array
	int right_ip;								//Architectural register right input operand index in register array
	int op;										//Architectural register output	index in register array
	int instr_address;							//address of instruction in memory starting from 20000
	char pcCommand[15];
}ST_INSTRUCT;

/**
 * Structure contains the information about pipeline stage
 */
typedef struct
{
	E_PIPELINE_STAGE_TYPE enStageType;
	ST_INSTRUCT _stInstruction;
} ST_PLSTAGE;


typedef struct
{
	ST_PLSTAGE _stPlStage;
	int resultaddress;							//holds index into register array
	int literval;
}ST_PIPELINE;

struct _stregfile
{
	int ip1;					//input operand 1
	int ip2;					//input operand 2
	int op;						//output operand
}ST_REG_FILE;

#endif /* SIM_H_ */
