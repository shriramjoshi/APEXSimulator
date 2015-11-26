#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "sim.h"

#define SPACE " "									//Tokenize Instruction based on space
#define STALL_PIPELINE 1
#define NO_STALL_PIPELINE 0

int nTotalInstructions;
const int nInitialInstAddress = 20000;				//When first instruction is fetched it is assumed to be at address 20000
int nOpLeft = 0;									//For execution
int nOpRight = 0;									//For execution
int bPipelineStall = NO_STALL_PIPELINE;
int instruction_index = 1;
struct _stInstruction *head = NULL;					//Contains the list of all instructions
int nForBZ = -1;

int fetch_stage(FILE**, char*, ST_PIPELINE**);
int decode_stage(FILE**, char*, ST_PIPELINE**);
int execute_stage(FILE**, char*, ST_PIPELINE**);
int memory_stage(FILE**, char*, ST_PIPELINE**);
int writeback_stage(FILE**, char*, ST_PIPELINE**);
void init(FILE**, char*, ST_PIPELINE**);
void simulate(FILE**, char*, ST_PIPELINE** , int);

/**
 *
 */
void gettotalinstructions(FILE** fp)
{
	char ch;
	while(!feof(*fp))
	{
		ch = fgetc(*fp);
		if(ch == '\n')
			nTotalInstructions++;
	}
	fseek(*fp, 0, SEEK_SET);
}

/**
 *
 */
static char* readnxtinst(FILE** fp, char* pcInstruction)
{
	int n = 0;
	if(*fp != NULL)
	{
		for(; n < instruction_index; ++n)
		{
			fgets(pcInstruction, 30, *fp);
		}
		instruction_index = 1;
		return pcInstruction;
	}
	return NULL;
}

/**
 *
 */
static void init_pipeline(ST_PIPELINE** pipeline)
{
	if(*pipeline == NULL)
		*pipeline = (ST_PIPELINE*)calloc(5, sizeof(ST_PIPELINE));
	//memset(&((*pipeline)), 0, sizeof(ST_PIPELINE));
}

/**
 * Returns the enum of type instruction
 */
static E_INSTRUCTION_TYPE getinsttype(char* instruction)
{
	if(strstr(instruction, "ADD"))
		return E_I_ADD;
	if(strstr(instruction, "SUB"))
		return E_I_SUB;
	if(strstr(instruction, "MOVC"))
		return E_I_MOVC;
	if(strstr(instruction, "MOV"))
		return E_I_MOV;
	if(strstr(instruction, "MUL"))
		return E_I_MUL;
	if(strstr(instruction, "AND"))
		return E_I_AND;
	if(strstr(instruction, "STORE"))
		return E_I_STORE;
	if(strstr(instruction, "OR"))
		return E_I_OR;
	if(strstr(instruction, "EX-OR"))
		return E_I_XOR;
	if(strstr(instruction, "LOAD"))
		return E_I_LOAD;
	if(strstr(instruction, "BZ"))
		return E_I_BZ;
	if(strstr(instruction, "BNZ"))
		return E_I_BNZ;
	if(strstr(instruction, "JUMP"))
		return E_I_JUMP;
	if(strstr(instruction, "BAL"))
		return E_I_BAL;
	if(strstr(instruction, "HALT"))
		return E_I_HALT;
	return -1;
}


/*
 *
 */
int decode_instruction(char* pcNextInst, ST_PIPELINE** pipeline)
{
	int nInstType = getinsttype(pcNextInst);
	char left_ip[3];
	char right_ip[3];
	char dest_address[7];
	char buf[15];
	strcpy(buf, pcNextInst);
	char* pcTok = strtok(buf, " ");
	pcTok = strtok(NULL, " ");			//To ignore instruction type
	int nIndex = 0;
	while(pcTok != NULL)
	{
		if(nIndex == 0)
		{
			strcpy(dest_address, pcTok);
		}
		else if(nIndex == 1)
		{
			strcpy(left_ip, pcTok);
		}
		else if(nIndex == 2)
		{
			strcpy(right_ip, pcTok);
		}
		pcTok = strtok(NULL, " ");
		nIndex++;
	}
	switch (nInstType) {
		case E_I_ADD:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_ADD;
			}
			break;
		case E_I_SUB:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_SUB;
			}
			break;
		case E_I_MOVC:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = atoi(left_ip);
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = -1;
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_MOVC;
			}
			break;
		case E_I_MOV:
			{

				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 2] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = -1;
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_MOV;
			}
			break;
		case E_I_MUL:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_MUL;
			}
			break;
		case E_I_AND:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_AND;
			}
			break;
		case E_I_OR:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_OR;
			}
			break;
		case E_I_XOR:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_XOR;
			}
			break;
		case E_I_LOAD:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = atoi(right_ip);
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_LOAD;
			}
			break;
		case E_I_STORE:
			{
				(*pipeline)[E_PL_STAGE_DECODE].resultaddress = (dest_address[strlen(dest_address) - 1] - '0');
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (left_ip[strlen(left_ip) - 1] - '0');
				if(strstr(right_ip, "R") == NULL)
				{
					literalvalue = atoi(right_ip);
					(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = -1;
				}
				else
				{
					(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = (right_ip[strlen(right_ip) - 2] - '0');
				}
				(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_STORE;
			}
			break;
		case E_I_BZ:
			literalvalue = atoi(dest_address);
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_BZ;
			break;
		case E_I_BNZ:
			literalvalue = atoi(dest_address);
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_BNZ;
			break;
		case E_I_JUMP:
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip = (dest_address[strlen(dest_address) - 1] - '0');
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip = -1;
			literalvalue = atoi(left_ip);
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_JUMP;
			break;
		case E_I_BAL:
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_BAL;
			break;
		case E_I_HALT:
			(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType = E_I_HALT;
			break;
		default:break;
		}
	return nInstType;
}

void printr()
{
	printf("\n%d %d %d %d %d %d %d %d", R[0],R[1],R[2],R[3],R[4],R[5],R[6],R[7]);
}

void printmem()
{
	int i = 0;
	printf("\n Memory ->");
	for(; i < 100; ++i)
	{
		printf("\t %d", memory[i]);
	}
}

void display(int nCycle, ST_PIPELINE** pipeline)
{
	printf("\n----------------------------------------------------------");
	printf("\n For cycle %d", nCycle);
	printf("\n Stage : Fetch");
	printf("\n PC - %d", (*pipeline)[E_PL_STAGE_FETCH]._stPlStage._stInstruction.instr_address);
	printf("Instruction : %s", (*pipeline)[E_PL_STAGE_FETCH]._stPlStage._stInstruction.pcCommand);

	printf("\n Stage : Decode");
	printf("\n PC - %d", (*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.instr_address);
	printf("\n Instruction : %s", (*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.pcCommand);

	printf("\n Stage : Execution");
	printf("\n PC - %d", (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.instr_address);
	printf("\n Instruction : %s", (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.pcCommand);

	printf("\n Stage : Memory");
	printf("\n Instruction : %s", (*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.pcCommand);

	printf("\n Stage : Writeback");
	printf("\n Instruction : %s", (*pipeline)[E_PL_STAGE_WB]._stPlStage._stInstruction.pcCommand);
	printf("\n----------------------------------------------------------");
}

/**
 *
 */
void executeinstruction(ST_PIPELINE** pipeline)
{
	int nInstType = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType;
	switch (nInstType) {
	case E_I_ADD:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
					R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] +
					R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
		}
	break;
	case E_I_SUB:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] -
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
		}
		break;
	case E_I_MOVC:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip;
		}
		break;
	case E_I_MOV:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
					R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip];
		}
		break;
	case E_I_MUL:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] *
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
		}
		break;
	case E_I_AND:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] &
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
		}
		break;
	case E_I_OR:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] |
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
		}
		break;
	case E_I_XOR:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] ^
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
		}
		break;
	case E_I_LOAD:
		{
			(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] +
							(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip;
		}
		break;
	case E_I_STORE:
		{
			if((*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip != -1)
			{
				(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] +
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.right_ip];
			}
			else
			{

				(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] +
							literalvalue;
			}
		}
		break;
	case E_I_BZ:
		break;
	case E_I_BNZ:
		break;
	case E_I_JUMP:
		(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op =
							R[(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.left_ip] +
							literalvalue;
		break;
	case E_I_BAL:
		X = R[(*pipeline)[E_PL_STAGE_EXECUTION].resultaddress] + (*pipeline)[E_PL_STAGE_EXECUTION].literval;
		break;
	case E_I_HALT:
		break;
	default:break;
	}

}

int takebranch(ST_PIPELINE** pipeline, int nInstType)
{
	if(nInstType == E_I_BZ)
	{
		if((*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.op == 0)
			return 1;														//Condition matched
	}
	else if(nInstType == E_I_BNZ)
	{
		if((*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.op != 0)
			return 1;														//Condition matched
	}
	return 0;
}

void commands()
{
	printf("\n 1) Initialise	init");
	printf("\n 2) Simulate		simulate <n>		n represents number of clock cycles");
	printf("\n 3) Display		display");
}

int main(int argc, char* argv[])
{
	FILE* fp = NULL;
	char pcNextInst[30];
	ST_PIPELINE *pipeline = NULL;
	char pcCommand[15];
	if(argv == NULL)
	{
		printf("\nWrong command line parameters");
		exit(0);
	}
	else
	{
		memset(&register_status, R_STATUS_VALID, sizeof(8));	//Initially all registers are valid
		while(1)
		{
			commands();
			printf("\nEnter command : ");
			fgets(pcCommand, 15, stdin);
			if(strstr(pcCommand, "init") != NULL)
			{
				fp = fopen(argv[1], "r");
				gettotalinstructions(&fp);
				init(&fp, pcNextInst, &pipeline);
			}
			else if(strstr(pcCommand, "simulate") != NULL)
			{
				char* pcTemp = pcCommand;
				pcTemp += 9;
				int nCycles = atoi(pcTemp);
				if(nCycles <= 0)
				{
					printf("\n No. of cycles can not be less than or equal to 0");
					continue;
				}
				else
				{
					fp = fopen(argv[1], "r");
					init(&fp, pcNextInst, &pipeline);
					simulate(&fp, pcNextInst, &pipeline, nCycles);
				}
			}
			else if(strstr(pcCommand, "display") != NULL)
			{
				printr();
			}
		}
	}
	fclose(fp);
	free(pipeline);
	return 0;
}

int fetch_stage(FILE** fp,  char* pcNextInst,ST_PIPELINE** pipeline)
{
	if(bPipelineStall != STALL_PIPELINE)
	{
		if(readnxtinst(fp, pcNextInst) != NULL)
		{
			(*pipeline)[E_PL_STAGE_FETCH]._stPlStage.enStageType = E_PL_STAGE_FETCH;
			(*pipeline)[E_PL_STAGE_FETCH]._stPlStage._stInstruction.instr_address = PC;
			strcpy((*pipeline)[E_PL_STAGE_FETCH]._stPlStage._stInstruction.pcCommand, pcNextInst);
			++PC;
		}
		else
		{
			//if file end initialise structure with -1
			memset(&((*pipeline)[E_PL_STAGE_FETCH]), -1, sizeof(ST_PLSTAGE));
		}
	}
	return NO_STALL_PIPELINE;
}

int decode_stage(FILE** fp,  char* pcNextInst,ST_PIPELINE** pipeline)
{
	if(bPipelineStall != STALL_PIPELINE)
	{
		memcpy(&((*pipeline)[E_PL_STAGE_DECODE]), &((*pipeline)[E_PL_STAGE_FETCH]), sizeof(ST_PIPELINE));
		(*pipeline)[E_PL_STAGE_DECODE]._stPlStage.enStageType = E_PL_STAGE_DECODE;


	}
	if((*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.enInstType != -1)
	{
		int nInstType = decode_instruction(pcNextInst, pipeline);

		if(nInstType == E_I_BZ)
		{
			nForBZ = (*pipeline)[E_PL_STAGE_EXECUTION].resultaddress;
			if(register_status[nForBZ] == R_STATUS_INVALID)
			{
				printf("\n STALL");
				bPipelineStall = STALL_PIPELINE;
				//return STALL_PIPELINE;
			}
			else
				bPipelineStall = NO_STALL_PIPELINE;
		}
		else if(nInstType != E_I_MOVC || nInstType != E_I_BNZ || nInstType != E_I_HALT)
		{
			if(nInstType == E_I_STORE)
			{
				if(register_status[(*pipeline)[E_PL_STAGE_DECODE].resultaddress] == R_STATUS_INVALID)
					STALL_PIPELINE;
			}
			else if(register_status[(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.left_ip] == R_STATUS_INVALID)
			{
				bPipelineStall = STALL_PIPELINE;

			}
			else if((*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip != -1)
			{
				if(register_status[(*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.right_ip] == R_STATUS_INVALID)
				{
					bPipelineStall = STALL_PIPELINE;
				}
			}
			else
				bPipelineStall = NO_STALL_PIPELINE;
		}
	}

	return fetch_stage(fp, pcNextInst, pipeline);
}

int execute_stage(FILE** fp,  char* pcNextInst,ST_PIPELINE** pipeline)
{
	if(bPipelineStall != STALL_PIPELINE)
	{
		memcpy(&((*pipeline)[E_PL_STAGE_EXECUTION]), &((*pipeline)[E_PL_STAGE_DECODE]), sizeof(ST_PIPELINE));
		(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage.enStageType = E_PL_STAGE_EXECUTION;
		if((*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType != -1)
		{
			if((*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType == E_I_BZ ||
							(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType == E_I_BNZ)
			{
				if(takebranch(pipeline, (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType))
				{
					memset(&((*pipeline)[E_PL_STAGE_FETCH]), -1, sizeof(ST_PIPELINE));
					memset(&((*pipeline)[E_PL_STAGE_DECODE]), -1, sizeof(ST_PIPELINE));
					strcpy((*pipeline)[E_PL_STAGE_FETCH]._stPlStage._stInstruction.pcCommand, "NOP");
					strcpy((*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.pcCommand, "NOP");
					if(literalvalue > 0)
					{
						instruction_index = literalvalue;
						PC = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.instr_address + instruction_index;
					}
					else
					{
						instruction_index = ((*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.instr_address - abs(literalvalue) - INITMEMADDRESS) - 1;
						PC = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.instr_address - instruction_index;
						rewind(*fp);
					}
					//No call to decode & fetch
					///return (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType;
				}
			}
			else if((*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType == E_I_JUMP)
			{
				executeinstruction(pipeline);
				memset(&((*pipeline)[E_PL_STAGE_FETCH]), -1, sizeof(ST_PIPELINE));
				memset(&((*pipeline)[E_PL_STAGE_DECODE]), -1, sizeof(ST_PIPELINE));
				strcpy((*pipeline)[E_PL_STAGE_FETCH]._stPlStage._stInstruction.pcCommand, "NOP");
				strcpy((*pipeline)[E_PL_STAGE_DECODE]._stPlStage._stInstruction.pcCommand, "NOP");
				if(PC < (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op)
				{
					instruction_index = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op - PC;
					PC = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op;
				}
				else
				{
					instruction_index = (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.op - INITMEMADDRESS + 1;
					PC = INITMEMADDRESS + instruction_index;
					rewind(*fp);
				}
				//return (*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType;
			}
			if((*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType != -1 ||
					(*pipeline)[E_PL_STAGE_EXECUTION]._stPlStage._stInstruction.enInstType != E_I_HALT)
			{
				executeinstruction(pipeline);
				register_status[(*pipeline)[E_PL_STAGE_EXECUTION].resultaddress] = R_STATUS_INVALID;
				return decode_stage(fp, pcNextInst, pipeline);
			}
		}
	}
	return decode_stage(fp, pcNextInst, pipeline);
}

int memory_stage(FILE** fp,  char* pcNextInst,ST_PIPELINE** pipeline)
{
	memcpy(&((*pipeline)[E_PL_STAGE_MEM]), &((*pipeline)[E_PL_STAGE_EXECUTION]), sizeof(ST_PIPELINE));
	(*pipeline)[E_PL_STAGE_MEM]._stPlStage.enStageType = E_PL_STAGE_MEM;
	if((*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.enInstType != -1)
	{
		int memIndex = (*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.op;
		if((*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.enInstType == E_I_LOAD)
		{
			(*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.op = memory[memIndex];
		}
		else if((*pipeline)[E_PL_STAGE_MEM]._stPlStage._stInstruction.enInstType == E_I_STORE)
		{
			memory[memIndex] = R[(*pipeline)[E_PL_STAGE_MEM].resultaddress];
		}
	}
	execute_stage(fp, pcNextInst, pipeline);
	return 1;
}

int writeback_stage(FILE** fp,  char* pcNextInst,ST_PIPELINE** pipeline)
{
	memcpy(&((*pipeline)[E_PL_STAGE_WB]), &((*pipeline)[E_PL_STAGE_MEM]), sizeof(ST_PIPELINE));
	if((*pipeline)[E_PL_STAGE_WB]._stPlStage._stInstruction.enInstType == E_I_HALT)
	{
		return E_I_HALT;
	}
	int nInstType = (*pipeline)[E_PL_STAGE_WB]._stPlStage._stInstruction.enInstType;
	if(nInstType != -1)
	{
		if(nInstType != E_I_STORE ||  nInstType != E_I_BZ || nInstType != E_I_BNZ)
		{
			if(nInstType != E_I_JUMP )
			{
				if(nInstType == E_I_MOV)	//workaround
				{
					R[(*pipeline)[E_PL_STAGE_WB].resultaddress] =
							R[(*pipeline)[E_PL_STAGE_WB]._stPlStage._stInstruction.left_ip];
				}
				else// if(nInstType != E_I_JUMP) //workaround
				{
					R[(*pipeline)[E_PL_STAGE_WB].resultaddress] = (*pipeline)[E_PL_STAGE_WB]._stPlStage._stInstruction.op;
				}
				register_status[(*pipeline)[E_PL_STAGE_WB].resultaddress] = R_STATUS_VALID;
			}
		}

	}
	memory_stage(fp, pcNextInst, pipeline);

	return 1;
}


/**
 *	Initialise all the registers & state of the pipeline
 */
void init(FILE** fp, char* pcNextInst, ST_PIPELINE **pipeline)
{
	if(fp != NULL)
	{
		init_pipeline(pipeline);						//Create and initialise the pipeline
		PC = INITMEMADDRESS;							//initialise program counter
	}
}

/**
 *
 */
void simulate(FILE** fp, char* pcNextInst, ST_PIPELINE** pipeline, int nCycles)
{
	int nCycle = 0;
	static int flag = 0;
	while(nCycle < nCycles + 4)
	{
		if(flag == 0)
		{
			fetch_stage(fp, pcNextInst, pipeline);			//Fetch stage execution
			nCycle++;
			decode_stage(fp, pcNextInst, pipeline);
			nCycle++;
			execute_stage(fp, pcNextInst, pipeline);
			nCycle++;
			memory_stage(fp, pcNextInst, pipeline);
			nCycle++;
			writeback_stage(fp, pcNextInst, pipeline);
			//display(nCycle, pipeline);
			nCycle++;
			flag = 1;


		}
		else if(flag == 1)	//After one loop we just need to call write_back function it will implicitly call other stage functions.
		{
			int nRet = writeback_stage(fp, pcNextInst, pipeline);
			if(nRet != E_I_HALT)
				nCycle++;
			if(nRet == E_I_HALT)
			{
				printf("\nHalt instruction occurred at %d\n", (*pipeline)[E_PL_STAGE_WB]._stPlStage._stInstruction.instr_address + 1);
				break;
			}

		}
		display(nCycles, pipeline);
	}

	printr();
	printmem();
}
