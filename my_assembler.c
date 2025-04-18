/*
 * 파일명 : my_assembler.c
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 *
 */

 /*
  *
  * 프로그램의 헤더를 정의한다.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

  // 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20203023.h"

/* ------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일
 * 반환 : 성공 = 0, 실패 = < 0
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
 *		   또한 중간파일을 생성하지 않는다.
 * ------------------------------------------------------------
 */

token_line = 0;

int main(int args, char* arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_symtab_output("output_symtab.txt");         //  추후 과제에 사용 예정
	make_literaltab_output("output_littab.txt");     //  추후 과제에 사용 예정

	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	// make_objectcode_output("output_objectcode.txt"); //  추후 과제에 사용 예정
	//return 0;
}

/* ------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다.
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
 * 설명 : 머신을 위한 기계 코드목록 파일(inst_table.txt)을 읽어
 *       기계어 목록 테이블(inst_table)을 생성하는 함수이다.
 *
 *
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *
 * =======================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | \n |
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
 * 설명 : 어셈블리 할 소스코드 파일(input.txt)을 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 라인단위로 저장한다.
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
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
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
	
	printf("%s %s %s %s %s    ",
		token_table[token_line]->label,
		token_table[token_line]->operator,
		token_table[token_line]->operand[0],
		token_table[token_line]->operand[1],
		token_table[token_line]->operand[2]);
		
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
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*          2. 토큰 테이블은 token_parsing()을 호출하여 설정한다.
*          3. assem_pass2 과정에서 사용하기 위한 심볼테이블 및 리터럴 테이블을 생성한다.
*
*
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	     따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
*        OPCODE 출력 프로그램에서는 심볼테이블, 리터럴테이블을 생성하지 않아도 된다.
*        그러나, 추후 프로젝트 1을 수행하기 위해서는 심볼테이블, 리터럴테이블이 필요하다.
*
* ------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	/* input_data의 문자열을 한줄씩 입력 받아서
	 * token_parsing()을 호출하여 _token에 저장
	 */
	int i = 0;
	int index;
	sym_index = 0;// symbol table index
	char* temp = NULL;//space for operator => argument for search_opcode
	int isFormat4 = 0;
	literal_index = 0;
	int literal_len = 0;
	int literal_flag = 0;
	int j = 0;

	for (i = 0; i < line_num; i++) {// whole line process => tokenization
		isFormat4 = 0;//format4 init
		temp = NULL;
		//printf("%s",input_data[i]);
		if (token_parsing(input_data[i]) != -1) {//call token_parsing()
			if (token_table[token_line - 1] != NULL) {
				if (token_table[token_line - 1]->operator != NULL &&
					(strcmp(token_table[token_line - 1]->operator,"START") == 0)) {
					locctr = 0;//if label is start, location counter doesn't raise.
					token_table_addr[token_line-1] = locctr;//ta-pc를 위한 값 저장
					strcpy_s(sym_table[sym_index].symbol, sizeof(sym_table[sym_index].symbol), token_table[token_line - 1]->label);
					sym_table[sym_index].addr = locctr;
					sym_index++;
					continue;
				}

				if (token_table[token_line - 1]->operator != NULL) {
					if ((strcmp(token_table[token_line - 1]->operator,"CSECT") == 0)
						|| (strcmp(token_table[token_line - 1]->operator,"END") == 0)) { //CSEC과 END일 때 literal 처리해줘야
						if (literal_flag) {
							literal_flag = 0;
							for (; j < literal_index; j++) {
								literal_table[j].addr = locctr;
								literal_len = strlen(literal_table[j].literal);
								if (literal_table[j].literal[1] == 'C') {
									locctr += (literal_len - 4);
								}
								else if (literal_table[j].literal[1] == 'X') {//if byte?????
									locctr += (literal_len - 4) / 2;
								}
							}
						}
					}
				}

				if (token_table[token_line - 1]->operator != NULL &&
					(strcmp(token_table[token_line - 1]->operator,"CSECT") == 0)) { //if CSECT
					locctr = 0;//if CSECT locctr init
					strcpy_s(sym_table[sym_index].symbol, sizeof(sym_table[sym_index].symbol), token_table[token_line - 1]->label);
					sym_table[sym_index].addr = locctr;
					token_table_addr[token_line - 1] = locctr;
					sym_index++;
					continue;
				}

				//symbol table register
				if (token_table[token_line - 1]->label != NULL // NULL pointer check and non valid value cheking and comment check
					&& token_table[token_line - 1]->label[0] != '\0'
					&& token_table[token_line - 1]->label[0] != '.'
					&& token_table[token_line - 1]->label[0] != '*') {//if label is valid, store in struct;symtable 
					strcpy_s(sym_table[sym_index].symbol, sizeof(sym_table[sym_index].symbol), token_table[token_line - 1]->label);
					sym_table[sym_index].addr = locctr;
					sym_index++;
				}

				//= literal checking
				if ((token_table[token_line - 1]->operator != NULL) && literal_flag && (strcmp(token_table[token_line - 1]->operator, "LTORG") == 0)) {
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
				}

				//literal 일 때
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

				//search_opcode
				token_table_addr[token_line - 1] = locctr;
				if (temp == NULL) {
					index = -1;
				}
				else {
					index = search_opcode(temp);
					if (index != -1) {//if it doesn't exist, return -1;
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
			}
		}
		printf("%04x\n",token_table_addr[token_line-1]);
	}
	return 0;
}

int literal_check(char* temp) {//duplicate checking 중복은 skip
	int i = 0;
	for (i = 0; i < literal_index; i++) {
		if (strcmp(literal_table[i].literal,temp) == 0) {
			return 0;
		}
	}
	return 1;
}


/* ------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
 * 매계 : 토큰 단위로 구분된 문자열
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
 * 주의 : 기계어 목록 테이블에서 특정 기계어를 검색하여, 해당 기계어가 위치한 인덱스를 반환한다.
 *        '+JSUB'과 같은 문자열에 대한 처리는 자유롭게 처리한다.
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 소스코드 명령어 앞에 OPCODE가 기록된 코드를 파일에 출력한다.
*        파일이 NULL값이 들어온다면 프로그램의 결과를 stdout으로 보내어
*        화면에 출력해준다.
*
*        OPCODE 출력 프로그램의 최종 output 파일을 생성하는 함수이다.
*        (추후 프로젝트 1에서는 불필요)
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
			//printf("%s  %d\n", temp, index);
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명 혹은 경로
* 반환 : 없음
* 주의 : 파일이 NULL값이 들어온다면 프로그램의 결과를 stdout으로 보내어
*        화면에 출력해준다.
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
	for (i = 0; i < 2; i++) {
		fprintf(fp, "%s\t", sym_table[i].symbol);
		fprintf(fp, "%04x\n", sym_table[i].addr);
		printf("%s\t%04x\n", sym_table[i].symbol, sym_table[i].addr);
	}
	for (i = 2; i < sym_index; i++) {
		if (sym_table[i].addr == 0)printf("\n");
		fprintf(fp, "%s\t", sym_table[i].symbol);
		fprintf(fp, "%04x\n", sym_table[i].addr);
		printf("%s\t%04x\n", sym_table[i].symbol, sym_table[i].addr);
	}

	fclose(fp); //file close
	printf("file generating is done: %s\n\n", file_name);
}


/* ------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 파일이 NULL값이 들어온다면 프로그램의 결과를 stdout으로 보내어
*        화면에 출력해준다.
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
		fprintf(fp, "%s\t", literal_table[i].literal);
		fprintf(fp, "%04x\n", literal_table[i].addr);
		printf("%s\t%04x\n", literal_table[i].literal, literal_table[i].addr);
	}
	fclose(fp); //file close
	printf("file generating is done: %s\n\n", filename);
}


void make_object() {//byte word 처리해야함 ㅈ같네 literal도

	int ta = 0, pc = 0;// target address and program counter
	int index=0;
	char* temp;
	char* literal_temp=NULL;
	int format = 0;
	int last = 0;//last 3byte or 5byte
	int file_num = 0;//file 3개 0 : main 1 : REDREC 2: WEREC
	int reg1=0, reg2=0;
	int op = 0;
	int literal_flag = 0;
	
	split_table();

	for (int i =0; i < token_line; i++) {
		op = -1;
		format = -1; literal_flag = 0;
		ta = 0; pc = 0;
		if (token_table[i]->operator != NULL) {
			//HEADER저장
        	if (strcmp("START", token_table[i]->operator) == 0) {
				code[file_num].p_name[0] = 'H';
				strcpy((code[file_num].p_name + 1), token_table[i]->label);
				continue;
			}
			if (strcmp("EXTDEF", token_table[i]->operator) == 0) {
				for (int j = 0; j < 3; j++) {
					if (token_table[i]->operand[j] != NULL) {
						strcpy(code[file_num].d_name[j], token_table[i]->operand[j]);
						code[file_num].d_addr[j] = search_sym(token_table[i]->operand[j], file_num);
					}
				}
				continue;
			}
			if (strcmp("EXTREF", token_table[i]->operator) == 0) {
				for (int j = 0; j < 3; j++) {
					if (token_table[i]->operand[j] != NULL) {
						strcpy(code[file_num].r_name[j], token_table[i]->operand[j]);
					}
				}
				continue;
			}
			if (strcmp("CSECT", token_table[i]->operator) == 0) {
				file_num++;
				literal_flag = 0;
				continue;
			}
		}
		
		//START t line setting
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

		

		///////////////
		//bp setting and last 
		///////////////

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
		
		//foramt 2 3 4 others condition
		if (format == -1) {
			if (token_table[i]->operator != NULL) {

				if ((strcmp(token_table[i]->operator,"WORD") == 0) || strcmp(token_table[i]->operator,"BYTE") == 0) {
					if (token_table[i]->operand[0] != NULL) {
						char* temp2 = strchr(token_table[i]->operand[0], '\'');
						int t = 0;
						code[file_num].t_code[i] = 0;
						if (temp2 == NULL) {
							code[file_num].t_code[i] = 0;
						}
						else {
							if (token_table[i]->operand[0] != 'X') {
								while (*temp2 != '\'') {
									code[file_num].t_code[i] += atoi(*temp2);
									code[file_num].t_code[i] = code[file_num].t_code[i] << 4;
									temp2++;
								}
							}
							else if (token_table[i]->operand[0] != 'C') {
								while (*temp2 != '\'') {
									code[file_num].t_code[i] += *temp2;
									code[file_num].t_code[i] = code[file_num].t_code[i] << 4;
									temp2++;
								}
							}
						}
					}
				
				}
				
				if (strcmp("LTORG", token_table[i]->operator) == 0 && (literal_flag)) {//literal flag free
					literal_flag = 0;
					char* temp2 = strchr(token_table[i]->operand[0], '\'');
					int t = 0;
					code[file_num].t_code[i] = 0;
					literal_temp = NULL;
					while (*temp2 != '\'') {
						code[file_num].t_code[i] += *temp2;
						code[file_num].t_code[i] = code[file_num].t_code[i] << 4;
						temp2++;
					}
				}
			}
			printf("%x \n", code[file_num].t_code[i]);
			continue;
		}
		else {
			if (format == 2) {
				reg1 = 0; reg2 = 0;
				if (token_table[i]->operand[0] != NULL) {
					reg1 = reg_num(token_table[i]->operand[0][0]);
				}
				if (token_table[i]->operand[1] != NULL) {
					reg2 = reg_num(token_table[i]->operand[1][0]);
				}
				code[file_num].t_code[i] = (op << 8);
				last = (reg1 << 4) + (reg2);
				code[file_num].t_code[i] += last;
				printf("%x \n", code[file_num].t_code[i]);
				continue;
			}
			else if (format == 3) {

				if (token_table[i]->operand[0] != NULL) {
					ta = search_sym(token_table[i]->operand[0], file_num);//target address 
					if (token_table[i]->operand[0][0] == '@') {//@
						ta = search_sym(token_table[i]->operand[0] + 1, file_num);
					}
					if (token_table[i]->operand[0][0] == '=') {//=
						ta = search_lit(token_table[i]->operand[0]);
						literal_temp = (char*)malloc(strlen(token_table[i]->operand[0]));
						strcpy(literal_temp, token_table[i]->operand[0]);
						literal_flag = 1;
					}
					if (ta == -1) {//modified 처리 해주기
						last = 0;
						if (token_table[i]->operand[0][0] == '#') {
							last = atoi(token_table[i]->operand[0] + 1);
						}
					}
					else {
						pc = token_table_addr[i + 1];
						last = ta - pc;
						if (last < 2048 && last >= -2048) {//12bit arrange check
							token_table[i]->nixbpe += 0B00000010;//0000_0010
							last &= (0xFFF);//signed bit 오염 방지
						}
						else {
							token_table[i]->nixbpe += 0B00000100;//In this project doesn't know base addr 
						}
					}
				}
				else {//format3이면서 operand 없을 때에도 ni bit 업데이트
					token_table[i]->nixbpe += 0B00110000;
					last = 0;

				}
			}
			else if (format == 4) {
				ta = search_sym(token_table[i]->operand[0], file_num);
				if (ta == -1) {//modified 처리 해주기
					last = 0;
					if (token_table[i]->operand[0][0] == '#') {
						last = atoi(token_table[i]->operand[0] + 1);
					}
				}
				else {
					last = ta;
				}
			}

			/*code 업데이트*/
			code[file_num].t_code[i] = (op << 4) + token_table[i]->nixbpe;
			if (format == 3) {
				code[file_num].t_code[i] = code[file_num].t_code[i] << 12;//3*4
			}
			else if (format == 4) {
				code[file_num].t_code[i] = code[file_num].t_code[i] << 20;//5*4
			}
			code[file_num].t_code[i] += last;

			printf("%x \n", code[file_num].t_code[i]);
		}
	}
}

//literal도 많아지면 이런식으로 관리해야할 것 같음.
//이번 과제에서는 literal이 파일 당 하나이고 주소값이 다르기 때문에 굳이 구분하지 않았다.
int search_sym(char* s , int mode) {//mode : file 범위
	
	int i = 0, start=0, end=0;

	if (mode == 0) {
		start = 0;
		end = sym_len[0];
	}
	else if(mode == 1){
		start = sym_len[0];
		end = start + sym_len[1];
	}
	else if (mode == 2) {
		start = sym_len[0] + sym_len[1];
		end = start + sym_len[2];
	}
	
	for (i = start; i < end; i++) {
		if (strcmp(&sym_table[i].symbol,s) == 0) {
			return sym_table[i].addr;
		}
	}
	return -1;
}

int search_lit(char* s) {
	for (int i = 0; i < literal_index; i++) {
		if (strcmp(literal_table[i].literal, s) == 0) {
			return literal_table[i].addr;
		}
	}
	return -1;
}

void split_table() {
	int i = 0;
	int n = 0;

	/*file별 검색 범위 정하기*/
	sym_len[0] = 2;
	for (i = 1; i < MAX_SEC; i++) {
		sym_len[i] = 0;
	}
	for (i = 2; i < sym_index; i++) {
		if (sym_table[i].addr != 0) {
			sym_len[n]++;
		}
		else {
			sym_len[++n]++;
		}
	}
	/*for (i = 0; i < MAX_SEC; i++) {
		printf("asdf %d\n",sym_len[i]);
	}*/
}

///reg table
//return regoster number
int reg_num(char c) {
	switch (c)
	{
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
 * 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
 *		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
 *		   다음과 같은 작업이 수행되어 진다.
 *		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0, 에러발생 = < 0
 * 주의 :
 * ------------------------------------------------------------
 */

static int assem_pass2(void)
{
	/* add your code here */
	make_object();

}

/* ------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 파일이 NULL값이 들어온다면 프로그램의 결과를 stdout으로 보내어
*        화면에 출력해준다.
*        명세서의 주어진 출력 결과와 완전히 동일해야 한다.
*        예외적으로 각 라인 뒤쪽의 공백 문자 혹은 개행 문자의 차이는 허용한다.
*
* ------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
}
