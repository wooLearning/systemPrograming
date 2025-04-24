/*
 * ���ϸ� : my_assembler.c
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 *
 */

 /*
  *
  * ���α׷��� ����� �����Ѵ�.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

  // ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20203023.h"

/* ------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ����
 * ��ȯ : ���� = 0, ���� = < 0
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
 *		   ���� �߰������� �������� �ʴ´�.
 * ------------------------------------------------------------
 */

token_line = 0;

int main(int args, char* arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	make_symtab_output("output_symtab.txt");         //  ���� ������ ��� ����
	make_literaltab_output("output_littab.txt");     //  ���� ������ ��� ����

	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	//make_opcode_output("output___t.txt");
	make_objectcode_output("output_objectcode.txt"); //  ���� ������ ��� ����
	//return 0;
}

/* ------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���.
  * ------------------------------------------------------------
*/

int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst_table.txt")) < 0)
		return -1;
	if ((result = init_input_file("input-1.txt")) < 0)
		return -1;
	return result;
}

/* ------------------------------------------------------------
 * ���� : �ӽ��� ���� ��� �ڵ��� ����(inst_table.txt)�� �о�
 *       ���� ��� ���̺�(inst_table)�� �����ϴ� �Լ��̴�.
 *
 *
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *
 * =======================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | \n |
 * ============================================================
 *
 * ------------------------------------------------------------
 */	

int init_inst_file(char* inst_file)
{
	FILE* file;
	int errno;
	char line[30] = { NULL };
	inst_index = 0;//inst_index init

	/* add your code here */
	fopen_s(&file, inst_file, "r");
	if (file == NULL) {//exception 
		printf("Error opening inst_table.txt");
		return -1;
	}
	while (fgets(line, sizeof(line), file) != NULL) {
		inst_table[inst_index] = (inst*)malloc(sizeof(inst));
		sscanf_s(line, "%s %d %hhx %d",
			inst_table[inst_index]->str, sizeof(inst_table[inst_index]->str),
			&inst_table[inst_index]->format,
			&inst_table[inst_index]->op,
			&inst_table[inst_index]->ops);//using sscanf for instruction table
		/*printf("%s %d %x %d\n",
			inst_table[inst_index]->str,
			inst_table[inst_index]->format,
			inst_table[inst_index]->op,
			inst_table[inst_index]->ops);*/
		inst_index++;
	}
	fclose(file);
}

/* ------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ� ����(input.txt)�� �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���δ����� �����Ѵ�.
 *
 * ------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
	FILE* file;
	int errno;
	char line[100] = { NULL };
	line_num = 0;//input_data index init

	/* add your code here */
	fopen_s(&file, input_file, "r");
	if (file == NULL) {//exeption 
		printf("Error opening inst_table.txt");
		return -1;
	}
	while (fgets(line, sizeof(line), file) != NULL) {
		if (line_num < MAX_LINES) {
			input_data[line_num] = (char*)malloc(100);
			if (input_data[line_num] == NULL) {
				return -1;
			}
			input_data[line_num] = _strdup(line);
			line_num++;
		}
	}
	return errno;
}


/* ------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
 *        �н� 1�� ���� ȣ��ȴ�.
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
 * ------------------------------------------------------------
 */
int token_parsing(char* str)
{
	/* add your code here */
	int max_split = 8;//max split time
	char* arr_line[8] = { NULL };//init
	char* arr_operand[MAX_OPERAND];
	int len1 = 0;
	int len2 = 0;//operand length

	if (str == NULL) return -1;
	if (str == '.') return -1;//. is comment => skip the comment line

	// initial block
	token_table[token_line] = (token*)malloc(sizeof(token));
	if (token_table[token_line] == NULL) return -1; //NULL exception
	
	token_table[token_line]->label = (char*)malloc(6 * sizeof(char));
	token_table[token_line]->operator = (char*)malloc(6 * sizeof(char));

	if (token_table[token_line]->label == NULL ||
		token_table[token_line]-> operator == NULL) return -1;
	
	for (int i = 0; i < 6; i++) {
		token_table[token_line]->label[i] = NULL;
		token_table[token_line]->operator[i] = NULL;
	}
	for (int i = 0; i < MAX_OPERAND; i++) {
		token_table[token_line]->operand[i] = (char*)malloc(6 * sizeof(char));
		for (int j = 0; j < 6; j++) {
			token_table[token_line]->operand[j] = NULL;//each operand max length is 6
		}
	}

	/*
	////////////////////////////////////////////////////////////////////////////////////////////
	[0] => label [1] => operator [2]=> operand [3] => tap  split
	////////////////////////////////////////////////////////////////////////////////////////////
	*/
	len1 = split_tap(str, arr_line); // split like the comment above

	token_table[token_line]->label = _strdup(arr_line[0]);//label parssing
	token_table[token_line]->operator = _strdup(arr_line[1]);//operator parssing

	if (!(arr_line[2] == NULL)) {
		len2 = split_comma(arr_line[2], arr_operand);// comma parssing if operand is divided by comma
		for (int i = 0; i < len2; i++) {
			token_table[token_line]->operand[i] = _strdup(arr_operand[i]);//operand parssing
		}
	}
	/*printf("%s %s %s %s %s    \n",
		token_table[token_line]->label,
		token_table[token_line]->operator,
		token_table[token_line]->operand[0],
		token_table[token_line]->operand[1],
		token_table[token_line]->operand[2]);*/
	token_line++;
	return 0;
}

// split by tap
int split_tap(char* data, char* arr[]) {
	int cnt = 0;
	char* start = data;
	char* end;
	data[strcspn(data, "\n")] = '\0';//\n remove
	while (cnt < 8) {
		end = strchr(start, '\t');//\t pointer
		if (end == NULL) {//when \t all remove
			arr[cnt++] = _strdup(start);
			break;
		}
		else {
			int len = end - start;
			char* token = (char*)malloc(len + 1);
			strncpy_s(token, len + 1, start, len);
			token[len] = '\0';//string slicing
			arr[cnt++] = token;
		}
		start = end + 1;
	}
}

//split by comma
int split_comma(char* data, char* arr[]) {
	char* context = NULL;
	data[strcspn(data, "\n")] = '\0';//\n remove
	char* token = strtok_s(data, ",", &context);//, slicing
	int cnt = 0;
	while (token != NULL) {//token copy
		arr[cnt] = token;
		token = strtok_s(NULL, ",", &context);
		cnt++;
	}
	return cnt;
}

/* ------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*          2. ��ū ���̺��� token_parsing()�� ȣ���Ͽ� �����Ѵ�.
*          3. assem_pass2 �������� ����ϱ� ���� �ɺ����̺� �� ���ͷ� ���̺��� �����Ѵ�.
*
*
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	     ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
*        OPCODE ��� ���α׷������� �ɺ����̺�, ���ͷ����̺��� �������� �ʾƵ� �ȴ�.
*        �׷���, ���� ������Ʈ 1�� �����ϱ� ���ؼ��� �ɺ����̺�, ���ͷ����̺��� �ʿ��ϴ�.
*
* ------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
	 * token_parsing()�� ȣ���Ͽ� _token�� ����
	 */
	int i = 0;
	int j = 0;

	int index;//instruction table index
	sym_index = 0;// symbol table index
	char* temp = NULL;//space for operator => argument for search_opcode
	int isFormat4 = 0;

	/*literal ���� ���� */
	literal_index = 0;
	int literal_len = 0;
	int literal_flag = 0;

	int file_sec = 0;//0, 1, 2

	for (i = 0; i < line_num; i++) {// whole line process => tokenization
		isFormat4 = 0;//format4 init
		temp = NULL;//temp init

		/*token_parsing �� �ȵ��� �� ���� ó��*/
		if (token_parsing(input_data[i]) == -1) return -1;
		if (token_table[token_line - 1] == NULL) return -1;
		
		//START�϶�
		if (token_table[token_line - 1]->operator != NULL &&
			(strcmp("START",token_table[token_line - 1]->operator) == 0)) {
			locctr = 0;//locctor init
			token_table_addr[token_line-1] = locctr;//ta-pc�� ���� �� ����
			strcpy_s(sym_table[sym_index].symbol, sizeof(sym_table[sym_index].symbol), token_table[token_line - 1]->label);
			sym_table[sym_index].addr = locctr;
			sym_index++;
			continue;
		}

		//CSEC�� END�� �� literal ó��
		if (token_table[token_line - 1]->operator != NULL) {
			if ((strcmp("CSECT", token_table[token_line - 1]->operator) == 0)
				|| (strcmp("END", token_table[token_line - 1]->operator) == 0)) { 
				if (literal_flag) {
					literal_flag = 0;
					for (; j < literal_index; j++) {
						literal_table[j].addr = locctr;
						literal_len = strlen(literal_table[j].literal);
						if (literal_table[j].literal[1] == 'C') {//char ó��
							locctr += (literal_len - 4);
						}
						else if (literal_table[j].literal[1] == 'X') {//byte ó��
							locctr += (literal_len - 4) / 2;
						}
					}
				}
				code[file_sec++].p_length = locctr;//program length ó��(header)
			}
		}

		if (token_table[token_line - 1]->operator != NULL &&
			(strcmp(token_table[token_line - 1]->operator,"CSECT") == 0)) { //if CSECT
			locctr = 0;//if CSECT locctr init

			//symbol table ����
			strcpy_s(sym_table[sym_index].symbol, sizeof(sym_table[sym_index].symbol), token_table[token_line - 1]->label);
			sym_table[sym_index].addr = locctr;
			sym_index++;

			//address ����
			token_table_addr[token_line - 1] = locctr;
			
			continue;
		}

		//symbol table register
		// NULL pointer check and non valid value cheking and comment check
		if (token_table[token_line - 1]->label != NULL 
			&& token_table[token_line - 1]->label[0] != '\0'
			&& token_table[token_line - 1]->label[0] != '.'
			&& token_table[token_line - 1]->label[0] != '*') {//if label is valid, store in struct;symtable 
			strcpy_s(sym_table[sym_index].symbol, sizeof(sym_table[sym_index].symbol), token_table[token_line - 1]->label);
			sym_table[sym_index].addr = locctr;//label �ּҰ� �� label ���ڿ� ����
			sym_index++;
		}

		//= literal checking �� locctor update
		if ((token_table[token_line - 1]->operator != NULL) && literal_flag && (strcmp("LTORG", token_table[token_line - 1]->operator) == 0)) {
			literal_flag = 0;
			for (; j < literal_index; j++) {
				literal_table[j].addr = locctr;
				token_table_addr[token_line - 1] = locctr;
				literal_len = strlen(literal_table[j].literal);
				if (literal_table[j].literal[1] == 'C') {
					locctr += (literal_len-4);
				}
				else if (literal_table[j].literal[1] == 'X') {
					locctr += (literal_len - 4) / 2;
				}
			}
			continue;
		}

		//literal �� �� literal table�� �����ϰ� �ּҰ� ���߿� ������Ʈ�Ͽ� literal table�� ���
        if (token_table[token_line - 1]->operand[0] != NULL && (token_table[token_line - 1]->operand[0][0] == '=')) {
			if (literal_check(token_table[token_line - 1]->operand[0])) { // duplicate checking
				literal_flag = 1;
				literal_table[literal_index].literal = _strdup(token_table[token_line - 1]->operand[0]);
				literal_index++;
			}
		}

		//+ fromat4 checking
		if (token_table[token_line - 1]->operator != NULL) {
			temp = token_table[token_line - 1]->operator;
			if (token_table[token_line - 1]->operator[0] == '+') {
				temp++;
				isFormat4 = 1;
			}
		}

		token_table_addr[token_line - 1] = locctr;//�ּҰ� ����

		//search_opcode
		if (temp == NULL) {
			index = -1;
		}
		else {
			index = search_opcode(temp);//if it doesn't exist, return -1;
			if (index != -1) {
				if (inst_table[index]->format == 2) {//format 2
					locctr += 2;
				}
				else {
					if (isFormat4) {//format 4
						locctr += 4;
					}
					else {//format 3
						locctr += 3;
					}
				}
			}
			if (strcmp(temp, "RESW") == 0) {//if RESW ,add word size
				if (token_table[token_line - 1]->operand == NULL) {
					return -1;
				}
				else {
					locctr += atoi(token_table[token_line - 1]->operand[0]) * 3;//word
				}
			}
			else if (strcmp(temp, "RESB") == 0) {//if RESW ,add byte size
				if (token_table[token_line - 1]->operand == NULL) {
					return -1;
				}
				else {
					locctr += atoi(token_table[token_line - 1]->operand[0]);//byte
				}
			}
			if (strcmp(temp, "WORD") == 0) {//if RESW ,add word size
				locctr += 3;
			}
			else if (strcmp(temp, "BYTE") == 0) {//if RESW ,add byte size
				locctr++;
			}
		}
		//printf("%04x\n",token_table_addr[token_line-1]);
	}
	return 0;
}

int literal_check(char* temp) {//duplicate checking �ߺ��� skip (literal ������ �� �� �־)
	int i = 0;
	for (i = 0; i < literal_index; i++) {
		if (strcmp(literal_table[i].literal,temp) == 0) {
			return 0;
		}
	}
	return 1;
}


/* ------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
 * �Ű� : ��ū ������ ���е� ���ڿ�
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
 * ���� : ���� ��� ���̺��� Ư�� ��� �˻��Ͽ�, �ش� ��� ��ġ�� �ε����� ��ȯ�Ѵ�.
 *        '+JSUB'�� ���� ���ڿ��� ���� ó���� �����Ӱ� ó���Ѵ�.
 *
 * ------------------------------------------------------------
 */
int search_opcode(char* str)
{
	/* add your code here */
	// search in inst_table index 
	int i = 0;
	for (i = 0; i < inst_index; i++) {
		if (strcmp(inst_table[i]->str, str) == 0) {
			//printf("Opcode found: %s\n", str);
			return i;//return index
		}
	}
	return -1;
}

/* ------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : �ҽ��ڵ� ��ɾ� �տ� OPCODE�� ��ϵ� �ڵ带 ���Ͽ� ����Ѵ�.
*        ������ NULL���� ���´ٸ� ���α׷��� ����� stdout���� ������
*        ȭ�鿡 ������ش�.
*
*        OPCODE ��� ���α׷��� ���� output ������ �����ϴ� �Լ��̴�.
*        (���� ������Ʈ 1������ ���ʿ�)
*
* ------------------------------------------------------------
*/
void make_opcode_output(char* file_name)
{
	/* add your code here */
	FILE* fp;
	char* temp;
	int index = 0;
	
	fopen_s(&fp, file_name, "w");//open file
	if (fp == NULL) {//NULL exception
		printf("Failure");
		return;
	}
	// output file making
	for (int i = 0; i < token_line; i++) {
		temp = NULL;
		if (token_table[i]->operator != NULL) {
			temp = token_table[i]->operator;
			if (token_table[i]->operator[0] == '+') {
				temp++;
			}
			index = search_opcode(temp);//opcode search
		}

		if (token_table[i]->label[0] != '\0') {//if doesn't exist, don't write
			fprintf(fp, "%s\t", token_table[i]->label);
		}
		else {
			fprintf(fp, "\t");
		}
		if (token_table[i]->operator != '\0') {//if doesn't exist, don't write
			fprintf(fp, "%s\t", token_table[i]->operator);
		}
		else {
			fprintf(fp, "\t");
		}
		if (token_table[i]->operand[0] != '\0') {//if doesn't exist, don't write
			fprintf(fp, "%s", token_table[i]->operand[0]);
			if (token_table[i]->operand[1] != '\0') {//if doesn't exist, don't write
				fprintf(fp, ",%s", token_table[i]->operand[1]);
			}
			if (token_table[i]->operand[2] != '\0') {//if doesn't exist, don't write
				fprintf(fp, ",%s", token_table[i]->operand[2]);
			}
		}
		fprintf(fp, "\t");
		if (index != -1) {//if It's not operator, don't write
			fprintf(fp, "\t%02x", inst_table[index]->op);
		}
		fprintf(fp, "\n");
	}
	fclose(fp); //file close
	printf("file generating is done: %s\n", file_name);
}

/* ------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ� Ȥ�� ���
* ��ȯ : ����
* ���� : ������ NULL���� ���´ٸ� ���α׷��� ����� stdout���� ������
*        ȭ�鿡 ������ش�.
*
* ------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
	/* add your code here */
	FILE* fp;
	int i = 0;

	fopen_s(&fp, file_name, "w");//open file
	if (fp == NULL) {//NULL exception
		printf("Failure");
		return;
	}
	//function 3�� �̹Ƿ� 3�� ������
	for (i = 0; i < 2; i++) {
		fprintf(fp, "%s\t%04x\n", sym_table[i].symbol, sym_table[i].addr);
		printf("%s\t%04x\n", sym_table[i].symbol, sym_table[i].addr);
	}
	for (i = 2; i < sym_index; i++) {
		if (sym_table[i].addr == 0)printf("\n");
		fprintf(fp, "%s\t%04x\n", sym_table[i].symbol, sym_table[i].addr);
		printf("%s\t%04x\n", sym_table[i].symbol, sym_table[i].addr);
	}
	fclose(fp); //file close
	printf("file generating is done: %s\n\n", file_name);
}


/* ------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ������ NULL���� ���´ٸ� ���α׷��� ����� stdout���� ������
*        ȭ�鿡 ������ش�.
*
* ------------------------------------------------------------
*/
void make_literaltab_output(char* filename)
{
	/* add your code here */
	FILE* fp;
	int i = 0;

	fopen_s(&fp, filename, "w");//open file
	if (fp == NULL) {//NULL exception
		printf("Failure");
		return;
	}
	for (i = 0; i < literal_index; i++) {
		fprintf(fp, "%s\t%04x\n", literal_table[i].literal, literal_table[i].addr);
		printf("%s\t%04x\n", literal_table[i].literal, literal_table[i].addr);
	}
	fclose(fp); //file close
	printf("file generating is done: %s\n\n", filename);
}

/*file�� �˻� ���� ���ϱ�*/
void split_table() {
	int i = 0, n = 0;

	sym_len[0] = 2;
	for (i = 1; i < MAX_SEC; i++) {
		sym_len[i] = 0;
	}
	for (i = 2; i < sym_index; i++) {
		if (sym_table[i].addr != 0) {//CSECT�϶� symbol table�� �ּҰ��� 0���� �ٲ��.
			sym_len[n]++;
		}
		else {
			sym_len[++n]++;
		}
	}
}


//literal�� �������� �̷������� �����ؾ��� �� ����.
//�̹� ���������� literal�� ���� �� �ϳ��̰� �ּҰ��� �ٸ��� ������ ���� �������� �ʾҴ�.
int search_sym(char* s, int mode) {//mode : file ����

	int i = 0, start = 0, end = 0;

	if (mode == 0) {//file 0�� �˻� ����
		start = 0;
		end = sym_len[0];
	}
	else if (mode == 1) {//file 1�� �˻� ����
		start = sym_len[0];
		end = start + sym_len[1];
	}
	else if (mode == 2) {//file 2�� �˻� ����
		start = sym_len[0] + sym_len[1];
		end = start + sym_len[2];
	}

	for (i = start; i < end; i++) {//������ ���� ���� ��ŭ�� �˻�
		if (strcmp(&sym_table[i].symbol, s) == 0) {
			return sym_table[i].addr;
		}
	}
	return -1;
}

/*search literal table*/
int search_lit(char* s) {
	for (int i = 0; i < literal_index; i++) {
		if (strcmp(literal_table[i].literal, s) == 0) {
			return literal_table[i].addr;
		}
	}
	return -1;
}

///reg table
//return regoster number
int reg_num(char c) {
	switch (c){
	case 'A': return 0; break;
	case 'X': return 1; break;
	case 'B': return 3; break;
	case 'S': return 4; break;
	case 'T': return 5; break;
	case 'F': return 6; break;
	default: return 0;  break;
	}
}



/* ------------------------------------------------------------
 * ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
 *		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
 *		   ������ ���� �۾��� ����Ǿ� ����.
 *		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
 * �Ű� : ����
 * ��ȯ : �������� = 0, �����߻� = < 0
 * ���� :
 * ------------------------------------------------------------
 */

static int assem_pass2(void)
{
	/* add your code here */
	int ta = 0, pc = 0;// target address and program counter
	int index = 0;
	char* temp = NULL;
	int format = 0;//format ����
	int last = 0;//last 3byte or 5byte
	int file_num = 0;//file 3�� 0 : main 1 : REDREC 2: WEREC
	int reg1 = 0, reg2 = 0;//format 2
	int op = 0;//opcode
	int sec = 0;//control section literal processing variable
	int sec_first = 0;// control section literal processing variable (arragne's first value)
	int m_index[MAX_SEC] = { 0, };//modified��

	/*file�� �˻� ���� ���ϱ�*/
	split_table();

	for (int i = 0; i < token_line; i++) {
		code[file_num].t_code[sec_len[file_num]] = 0;
		code[file_num].t_addr[sec_len[file_num]] = token_table_addr[i];
		op = -1;
		format = -1;// literal_flag = 0;
		ta = 0; pc = 0;
		//printf("%s %s %s\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0]);
		if (token_table[i]->operator != NULL) {
			//HEADER����
			if (strcmp("START", token_table[i]->operator) == 0) {
				strcpy((code[file_num].p_name), token_table[i]->label);
				sec_len[file_num]++;
				continue;
			}
			if (strcmp("EXTDEF", token_table[i]->operator) == 0) {
				for (int j = 0; j < 3; j++) {
					if (token_table[i]->operand[j] != NULL) {
						strcpy(code[file_num].d_name[j], token_table[i]->operand[j]);
						code[file_num].d_addr[j] = search_sym(token_table[i]->operand[j], file_num);
					}
				}
				sec_len[file_num]++;
				continue;
			}
			if (strcmp("EXTREF", token_table[i]->operator) == 0) {
				for (int j = 0; j < 3; j++) {
					if (token_table[i]->operand[j] != NULL) {
						strcpy(code[file_num].r_name[j], token_table[i]->operand[j]);
					}
				}
				sec_len[file_num]++;
				continue;
			}
			if (strcmp("CSECT", token_table[i]->operator) == 0) {
				file_num++;
				strcpy((code[file_num].p_name), token_table[i]->label);
			}
		}


		//n, i value setting
		token_table[i]->nixbpe = 0B00000000;
		if (token_table[i]->operand[0] != NULL) {
			if (token_table[i]->operand[0][0] == '@') {
				token_table[i]->nixbpe += 0B00100000;//0010_0000
			}
			else if (token_table[i]->operand[0][0] == '#') {
				token_table[i]->nixbpe += 0B00010000;//0001_0000;
			}
			else {
				token_table[i]->nixbpe += 0B00110000;//0011_0000
			}
		}
		//x setting
		if (token_table[i]->operand[1] != NULL && (token_table[i]->operand[1][0] == 'X')) {
			token_table[i]->nixbpe += 0B00001000;//00_1000
		}

		//e setting
		if (token_table[i]->operator != NULL && (token_table[i]->operator[0] == '+')) {
			token_table[i]->nixbpe += 0B00000001;//0000_0001
		}

		//bp setting and last 
		if (token_table[i]->operator != NULL) {
			temp = token_table[i]->operator;
			if (token_table[i]->operator[0] == '+') {
				temp++;
				format = 4;
				index = search_opcode(temp);
				if (index != -1) {//if it doesn't exist, return -1;
					op = inst_table[index]->op;
				}
			}
			else {
				index = search_opcode(temp);
				if (index != -1) {//if it doesn't exist, return -1;
					format = inst_table[index]->format;
					op = inst_table[index]->op;
				}
			}
		}
		code[file_num].t_format[sec_len[file_num]] = format;

		//foramt 2 3 4 and others condition
		if (format == -1) {

			/*continue ����*/
			if (token_table[i]->operator == NULL) continue;
			if ((strcmp(token_table[i]->operator,"EQU") == 0)) continue;

			// END LTORG SETTING
			if ( (strcmp(token_table[i]->operator,"LTORG") == 0) || (strcmp(token_table[i]->operator,"END") == 0)) {
				char* temp2;
				temp2 = literal_table[sec_first].literal + 1;
				if (*temp2 == 'X') {
					temp2 += 2;
					code[file_num].t_code[sec_len[file_num]] += strtol(temp2, NULL, 16);
					sec_first++;
				}
				else if (*temp2 == 'C') {
					temp2 += 2;
					sec_first++;
					while (*temp2 != '\'') {
						code[file_num].t_code[sec_len[file_num]] += *temp2;
						code[file_num].t_code[sec_len[file_num]] = code[file_num].t_code[sec_len[file_num]] << 8;
						temp2++;
					}
					code[file_num].t_code[sec_len[file_num]] = code[file_num].t_code[sec_len[file_num]] >> 8;//while loop ���� �� �ڷ� �ѹ� shift�������
				}
			}

			if ((strcmp(token_table[i]->operator,"WORD") == 0) || strcmp(token_table[i]->operator,"BYTE") == 0) {

				if (token_table[i]->operand[0] == NULL) continue;

				/*' ó��*/
				char* temp2 = token_table[i]->operand[0];
				while (*temp2 != '\'') temp2++;// ' ã��
				if (temp2 == NULL) {//' ���� ��
					code[file_num].t_code[sec_len[file_num]] = 0;
				}
				else {
					if (token_table[i]->operand[0][0] == 'X') {
						temp2++;
						code[file_num].t_code[sec_len[file_num]] += strtol(temp2, NULL, 16);//16���� ��ȯ
					}
					else if (token_table[i]->operand[0][0] == 'C') {
						temp2++;
						while (*temp2 != '\'') {
							code[file_num].t_code[sec_len[file_num]] += *temp2;
							code[file_num].t_code[sec_len[file_num]] = code[file_num].t_code[sec_len[file_num]] << 8;
							temp2++;
						}
						code[file_num].t_code[sec_len[file_num]] = code[file_num].t_code[sec_len[file_num]] >> 8;
					}
					else {//BUFFEDN-BUFFER ó��
						code[file_num].t_code[sec_len[file_num]] = 0x1000000;
						ta = search_sym(token_table[i]->operand[0], file_num);
						if (ta == -1) {
							char symbol1[20] = { 0 }, symbol2[20] = { 0 };
							char operator = 0;
							char* operand = token_table[i]->operand[0];
							char* op_pos = NULL;

							// ������ ��ġ Ž��
							op_pos = strchr(operand, '+');
							if (op_pos) operator = '+';
							else {
								op_pos = strchr(operand, '-');
								if (op_pos) operator = '-';
							}

							if (op_pos == NULL) continue;

							// ������ �� �κ� -> symbol1
							strncpy(symbol1, operand, op_pos - operand);
							symbol1[op_pos - operand] = '\0';

							// ������ �� �κ� -> symbol2
							strcpy(symbol2, op_pos + 1);
							if (search_sym(symbol1, file_num) == -1) {
								code[file_num].m_addr[m_index[file_num]] = token_table_addr[i];
								code[file_num].m_length[m_index[file_num]] = 6;
								char sign[10];
								sprintf(sign, "+%s", symbol1);
								code[file_num].m_name[m_index[file_num]++] = _strdup(sign);
							}
							if ((search_sym(symbol2, file_num) == -1)) {
								code[file_num].m_addr[m_index[file_num]] = token_table_addr[i];
								code[file_num].m_length[m_index[file_num]] = 6;
								char sign[10];
								sprintf(sign, "%c%s", operator, symbol2);
								code[file_num].m_name[m_index[file_num]++] = _strdup(sign);
							}
						}
					}
				}
			}
			sec_len[file_num]++;
			continue;
		}
		else {//format != -1 �� ��
			if (format == 2) {//format2 ó��
				reg1 = 0; reg2 = 0;
				if (token_table[i]->operand[0] != NULL) {
					reg1 = reg_num(token_table[i]->operand[0][0]);
				}
				if (token_table[i]->operand[1] != NULL) {
					reg2 = reg_num(token_table[i]->operand[1][0]);
				}
				code[file_num].t_code[sec_len[file_num]] = (op << 8);
				last = (reg1 << 4) + (reg2);
				code[file_num].t_code[sec_len[file_num]] += last;
				sec_len[file_num]++;
				continue;//format3�� format4�� �ٸ�
			}
			else if (format == 3) {
				if (token_table[i]->operand[0] != NULL) {
					ta = search_sym(token_table[i]->operand[0], file_num);//target address 
					if (token_table[i]->operand[0][0] == '@') {//@ ���� ã��
						ta = search_sym(token_table[i]->operand[0] + 1, file_num);
					}
					if (token_table[i]->operand[0][0] == '=') {//= literal table���� ã��
						ta = search_lit(token_table[i]->operand[0]);
					}
					if (ta == -1) {//target address �� �� modified ó�� ���ֱ�
						last = 0;
						if (token_table[i]->operand[0][0] == '#') {
							last = atoi(token_table[i]->operand[0] + 1);
						}
						else {
							code[file_num].m_addr[m_index[file_num]] = token_table_addr[i] + 1;
							code[file_num].m_length[m_index[file_num]] = 5;
							char sign[10];
							sprintf(sign, "+%s", token_table[i]->operand[0]);
							code[file_num].m_name[m_index[file_num]++] = _strdup(sign);
						}
					}
					else {
						pc = token_table_addr[i + 1];
						last = ta - pc;
						if (last < 2048 && last >= -2048) {//12bit arrange check
							token_table[i]->nixbpe += 0B00000010;//0000_0010
							last &= (0xFFF);//signed bit ���� ����
						}
						else {
							token_table[i]->nixbpe += 0B00000100;//In this project doesn't know base addr 
						}
					}
				}
				else {//format3�̸鼭 operand ���� ������ ni bit ������Ʈ
					token_table[i]->nixbpe += 0B00110000;
					last = 0;
				}
			}
			else if (format == 4) {
				ta = search_sym(token_table[i]->operand[0], file_num);
				if (ta == -1) {//ta���� �� modified ó�� ���ֱ� 
					last = 0;
					if (token_table[i]->operand[0][0] == '#') {
						last = atoi(token_table[i]->operand[0] + 1);
					}
					else {
						code[file_num].m_addr[m_index[file_num]] = token_table_addr[i] + 1;
						code[file_num].m_length[m_index[file_num]] = 5;
						char sign[10];
						sprintf(sign, "+%s", token_table[i]->operand[0]);
						code[file_num].m_name[m_index[file_num]++] = _strdup(sign);
					}
				}
				else {//format 4�� target address �״�� �Ѿ
					last = ta;
				}
			}

			//format update
			code[file_num].t_format[sec_len[file_num]] = format;
			
			/*code ������Ʈ*/
			code[file_num].t_code[sec_len[file_num]] = (op << 4) + token_table[i]->nixbpe;
			if (format == 3) {
				code[file_num].t_code[sec_len[file_num]] = code[file_num].t_code[sec_len[file_num]] << 12;//3*4
			}
			else if (format == 4) {
				code[file_num].t_code[sec_len[file_num]] = code[file_num].t_code[sec_len[file_num]] << 20;//5*4
			}
			code[file_num].t_code[sec_len[file_num]] += last;

			sec_len[file_num]++;//length update
		}
	}
}

/* ------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ������ NULL���� ���´ٸ� ���α׷��� ����� stdout���� ������
*        ȭ�鿡 ������ش�.
*        ������ �־��� ��� ����� ������ �����ؾ� �Ѵ�.
*        ���������� �� ���� ������ ���� ���� Ȥ�� ���� ������ ���̴� ����Ѵ�.
*
* ------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
	FILE* fp;
	fopen_s(&fp, file_name, "w");//open file
	if (fp == NULL) {//NULL exception
		printf("Failure");
		return;
	}
	int i = 0, j = 0;

	int file_sec = 0;
	int zflag = 0;//zero flag 0 0 0 ���� ���� �� RESW�� �׳� 0 ����
	int start = 0;//start address
	int t_line = 0;//start, length indexing

	int line_length = 0;
	int line_num = 0;

	/*HEADER�� DEF Line ó��*/
	for (i = 0; i < 3; i++) {
		printf("H%-6s%06x%06x\n", code[i].p_name, code[i].h_start_addr, code[i].p_length);
		fprintf(fp, "H%-6s%06x%06x\n", code[i].p_name, code[i].h_start_addr, code[i].p_length);
		if (code[i].d_name[0][0] != '\0') {
			printf("D");
			fprintf(fp,"D");
			for (int j = 0; j < 10; j++) {
				if (code[i].d_addr[j] != 0) {
					printf("%6s%06x", code[i].d_name[j], code[i].d_addr[j]);
					fprintf(fp, "%6s%06x", code[i].d_name[j], code[i].d_addr[j]);
				}		
			}
			printf("\n");
			fprintf(fp,"\n");
		}

		/*REF Line process*/
		printf("R");
		fprintf(fp, "R");
		for (int j = 0; j < 10; j++) {
			if (code[i].r_name[j][0] != '\0') {
				printf("%-6s", code[i].r_name[j]);
				fprintf(fp,"%-6s", code[i].r_name[j]);
			}
				
		}
		printf("\n");
		fprintf(fp, "\n");


		/*Setting start address and length for T line */
		if (code[i].p_length <= 30) {
			code[i].t_staddr[0] = 0;
			code[i].t_length[0] = code[i].p_length;
		}
		else {
			zflag = 0;
			start = 0;
			t_line = 0;
			for (j = 0; j < sec_len[i]; j++) {
				
				if (code[i].t_code[j] == 0 && (code[i].t_addr[j] != 0)) {
					zflag++;
				}
				else {
					if (zflag > 1) {//0�̾��ٰ� �������� �� ��ĭ �� �� �� �ٲ�
						code[i].t_staddr[t_line] = code[i].t_addr[j];
						code[i].t_length[t_line] = code[i].t_addr[j + 1] - code[i].t_addr[j];
						start = code[i].t_addr[j];
						//printf("----T%06x %02x\n", code[i].t_staddr[t_line], code[i].t_length[t_line]);
					}
				}
				if ((code[i].t_addr[j] - start > 28) || zflag == 1) {//�� �� ���� ����
					
					code[i].t_staddr[t_line] = start;
					code[i].t_length[t_line] = code[i].t_addr[j] - start;
					start = code[i].t_addr[j];
					//printf("line line T%06x %02x\n", code[i].t_staddr[t_line], code[i].t_length[t_line]);
					t_line++;
				}
				if (code[i].t_code[j] == 0x1000000) {//�������� word ���� �� 000000 ó��
					code[i].t_staddr[t_line] = start;
					code[i].t_length[t_line] = code[i].t_addr[j] - start +3 ;
					start = code[i].t_addr[j];
				}
			}
		}

		/*Writing T Line*/
		line_length = 0;
		line_num = 0;
		printf("T%06x%02x", code[i].t_staddr[line_num], code[i].t_length[line_num]);
		fprintf(fp,"T%06x%02x", code[i].t_staddr[line_num], code[i].t_length[line_num]);

		for (j = 0; j < sec_len[i]; j++) {
			if (code[i].t_code[j] != 0) {
				if (line_length >= code[i].t_length[line_num]) {
					if (code[i].t_length[line_num] != 0) {
						printf("\n");
						fprintf(fp,"\n");
						line_num++;
						line_length = 0;
						printf("T%06x%02x", code[i].t_staddr[line_num], code[i].t_length[line_num]);
						fprintf(fp, "T%06x%02x", code[i].t_staddr[line_num], code[i].t_length[line_num]);
					}

				}
				if (code[i].t_format[j] == 2) {
					printf("%04x", code[i].t_code[j]);
					fprintf(fp,"%04x", code[i].t_code[j]);
					line_length += 2;
				}
				else if (code[i].t_format[j] == 3) {
					printf("%06x", code[i].t_code[j]);
					fprintf(fp,"%06x", code[i].t_code[j]);
					line_length += 3;
				}
				else if (code[i].t_format[j] == 4) {
					printf("%08x", code[i].t_code[j]);
					fprintf(fp,"%08x", code[i].t_code[j]);
					line_length += 4;
				}
				else {
					if (code[i].t_code[j] == 0x1000000) {
						printf("%06x", 0);
						fprintf(fp,"%06x", 0);
					}
					else {
						printf("%02x", code[i].t_code[j]);
						fprintf(fp,"%02x", code[i].t_code[j]);
					}
				}
			}
		}
		printf("\n");
		fprintf(fp,"\n");

		/*modulation line ó��*/
		for (int j = 0; j < 5; j++) {
			if (code[i].m_name[j] == NULL) continue;
			if (code[i].m_name[j][0] != '\0') {
				printf("M%06x%02x%-6s \n", code[i].m_addr[j], code[i].m_length[j], code[i].m_name[j]);
				fprintf(fp,"M%06x%02x%-6s \n", code[i].m_addr[j], code[i].m_length[j], code[i].m_name[j]);
			}
				
		}

		/*end line ó��*/
		if (i == 0) {
			printf("E000000");
			fprintf(fp,"E000000");
		}
		else {
			printf("E");
			fprintf(fp,"E");
		}
		printf("\n\n");
		fprintf(fp,"\n\n");
	}

	printf("file generating is done: %s\n\n", file_name);
}
