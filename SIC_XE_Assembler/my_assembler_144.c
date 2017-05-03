/*
 * 화일명 : my_assembler.c 
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

#include "my_assembler_144.h"

#pragma warning(disable:4996)
#pragma warning(disable:4018)

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[]) 
{
	if(init_my_assembler()< 0)
	{
		printf("init_my_assembler: initialization fail!\n"); 
		return -1 ; 
	}

//	make_opcode_output("output_144.txt");


	if(assem_pass1() < 0 ){
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n") ;
		system("PAUSE");
		return -1 ; 
	}
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ;
		system("PAUSE");
		return -1 ; 
	}

	make_objectcode_output("output_144.txt") ; 

	system("PAUSE");

	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result ; 

	if((result = init_inst_file("inst.data")) < 0 )
		return -1 ;

	// 지시어 탐색 함수 추가
	if((result = init_dir_file("dir.data")) < 0 )
		return -1;
	
	if((result = init_input_file("input.txt")) < 0 )
		return -1 ; 
	return result ; 
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 기타 : 기계어 목록파일 형식은 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 |
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE * file;
	int errnum = 0;

	/* add your code here */

	inst *record;
	inst_index = 0;

	if (inst_file == NULL) return -1;

	if((file = fopen(inst_file, "r"))==NULL){
		errnum = -1;
	}
	else{
		while(1){
			// inst 구조체 포인터에 동적 메모리 할당 및 초기화
			record = (inst*)malloc(sizeof(inst));
			memset(record, 0, sizeof(inst));

			int opcode;
			int num = fscanf(file, "%s %d %x %d", record->inst_name, &record->format,
												  &opcode, &record->num_operand);

			// scanf에서의 %x 기본자료형이 int 이므로 
			// int로 받은 뒤 캐스팅하여 값 복사
			record->opcode = (char)opcode;

			if(num<=0) break;		// normal termination
			else if(num<4){
				errnum = -2;		// not formal record
				break;
			}
			
			// 테이블에 레코드 추가
			inst_table[inst_index++] = record;
		
		}

		free(record);
		
	}

	fclose(file);

	return errnum;
}

/* ----------------------------------------------------------------------------------
* 설명 : 머신을 위한 지시어목록 파일을 읽어 지시어 목록 테이블(dir_table)을
*        생성하는 함수이다.
* 매계 : 지시어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 기타 : 지시어 목록파일 형식은 다음과 같다.
*
*	===============================================================================
*		   | 이름 | 오퍼랜드의 갯수 |
*	===============================================================================
*
* ----------------------------------------------------------------------------------
*/
int init_dir_file(char *dir_file)
{
	FILE *file;
	int errnum = 0;

	directive *record;
	dir_index = 0;

	if (dir_file == NULL) return -1;

	if((file = fopen(dir_file, "r"))==NULL){
		errnum = -1;
	}
	else{
		while(1){
			// directive 구조체 포인터에 동적 메모리 할당 및 초기화
			record = (directive*)malloc(sizeof(directive));
			memset(record, 0, sizeof(directive));

			int num = fscanf(file, "%s %d", record->dir_name, &record->num_operand);

			if(num<=0) break;	// EOF이면 break;
			else if(num<2){		// 온전한 데이터가 아닌경우 error
				errnum = -2;
				break;
			}

			// 테이블에 레코드 추가
			dir_table[dir_index++] = record;
		}

		free(record);
	}

	fclose(file);

	return errnum;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어오는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE * file;
	int errnum = 0;

	/* add your code here */

	// 한 번에 한 줄 읽어오는 버퍼 생성 및 초기화
	int bufsize = 100;
	char *buf = (char*)malloc(sizeof(char)*bufsize);
	memset(buf, 0, bufsize);

	token_line = 0;
	line_num = 0;
	section_line = 0;
	cur_section = 0;
	literal_line = 0;
	symbol_line = 0;
	modify_line = 0;

	if((file = fopen(input_file, "r"))==NULL) {
		errnum = -1;
	}
	else{
		while(fgets(buf, bufsize, file)!=NULL){

			// 끝 문자가 개행문자인 경우 삭제
			if(buf[strlen(buf) - 1]=='\n') buf[strlen(buf)-1] = '\0';

			// 테이블에 등록 후 새 메모리 할당
			input_data[line_num++] = buf;
			buf = (char*)malloc(sizeof(char)*bufsize);

			// MAX_LINES 보다 긴 코드이면 에러
			if(line_num>=MAX_LINES){
				errnum = -1;
				break;
			}
		}
	}

	free(buf);
	fclose(file);
	
	return errnum;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다. 
 * 매계 : 소스코드의 라인번호  
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(int index) 
{
	/* add your code here */

	int result = 0;
	char *token_next;
	char *token_operand;

	token *unit = (token*)malloc(sizeof(token));
	memset(unit, 0, sizeof(token));

	unit->addr = locctr;
	unit->section = section_line;

	// 토큰을 테이블에 등록
	token_table[token_line++] = unit;

	// 1. 라인 전체가 주석인가? ( 첫 글자가 '.' 으로 시작)
	int i;
	for(i=0; i<strlen(input_data[index]); i++)
		if(input_data[index][i]!=' ' || input_data[index][i]!='\t') break;
	
	if(input_data[index][i]=='.'){			// 주석인 경우
		unit->comment = input_data[index];	// 라인 전체를 comment에 등록
		unit->optype = TYPE_COMMENT;		// 토큰 타입을 주석으로 설정
		return result;
	}

	// 2. 첫 토큰이 명령어 혹은 지시어인가?
	if((token_next = strtok(input_data[index], " \t"))==NULL) return -1;
	
	if((unit->opindex = search_opcode(token_next))>=0){
		// 첫 토큰이 명령어인 경우 명령어를 등록하고 토큰 타입을 명령어로 설정
		unit->operator = token_next;
		unit->optype = TYPE_INSTRUCTION;		
	}
	else if((unit->opindex = search_directive(token_next))>=0){
		// 첫 토큰이 지시어인 경우 지시어를 등록하고 토큰 타입을 지시어로 설정
		unit->operator = token_next;
		unit->optype = TYPE_DIRECTIVE;
	}
	else{
		
		// 둘 다 아닌 경우 해당 토큰을 label로 분류하고
		// 다음 토큰은 무조건 명령어 혹은 지시어여야 한다.
	
		unit->label = token_next;
		token_next = strtok(NULL, " \t");

		if((unit->opindex = search_opcode(token_next))>=0){
			unit->operator = token_next;
			unit->optype = TYPE_INSTRUCTION;
		}
		else if((unit->opindex = search_directive(token_next))>=0){
			unit->operator = token_next;
			unit->optype = TYPE_DIRECTIVE;
		}
		else{
			// 명령어 or 지시어가 없으므로 에러
			return -1;
		}
	
	}

	// 3. 오퍼랜드와 주석을 분류하여 추출
	if((unit->optype==TYPE_INSTRUCTION && inst_table[unit->opindex]->num_operand>0) ||
		unit->optype==TYPE_DIRECTIVE && dir_table[unit->opindex]->num_operand>0){

		// 토큰 타입이 명령어 혹은 지시어이면서 피연산자 개수가 1개 이상인 경우
		// 다음 토큰을 피연산자로 등록하고 그 뒷부분 전체를 주석으로 등록

		token_next = strtok(NULL, " \t");	// operand
		unit->comment = strtok(NULL, "\n");	// comment

		// Operand Parsing
		if(token_next==NULL){
			// 피연산자가 있어야 하는데 없는 경우
			result = -1;
			return result;
		}
		else {

			// 명령어일때 둘 이상의 피연산자의 경우 공백 없이 ',' 으로 연결된 
			// 하나의 토큰으로 가정하고 파싱한다.
			token_operand = strtok(token_next, ",");
			unit->operand[0] = token_operand;

			// literal check
			if (token_operand[0] == '=' && search_literal(unit->operand[0], unit->section)<0) 
				add_literal_table(token_operand);

			int i;
			for(i=1; i<MAX_OPERAND; i++){
				token_operand = strtok(NULL, ",");
				if(token_operand==NULL) break;
				else unit->operand[i] = token_operand;
			}
		}

	}
	else{
		// 피연산자가 없는 경우 나머지 전체를 주석으로 등록
		unit->comment = strtok(NULL, "\n");
	}


	/* directive 수행 및 symbol table 등록 */

	if (unit->optype == TYPE_DIRECTIVE && execute_directive(unit)<0) {
		return -1;
	}

	if (unit->optype == TYPE_INSTRUCTION && unit->label != NULL) {
		// sybol table search and register

		int index;
		if ((index = search_globalSymbol(unit->label)) >= 0) {
		
			if (symbol_table[index]->section<0 && !(symbol_table[index]->flag&F_EXIST)) {
				// EXTREF로 이미 선언된 심볼인데 정의되지 않은 경우
				symbol_table[index]->addr = unit->addr;
				symbol_table[index]->flag |= F_EXIST;
				symbol_table[index]->section = unit->section;
			
			}
			else return -1;
		}
		else if (search_localSymbol(unit->label, unit->section) < 0) {
			add_symbol_table(unit->label, unit, F_EXIST);
		}
		else return -1;

	}


	/* location counter 계산 */
	if (unit->optype == TYPE_INSTRUCTION) {
		if (unit->operator[0] == '+') locctr += 4;
		else locctr += inst_table[unit->opindex]->format;
	}


	return result;
}


/* ----------------------------------------------------------------------------------
* 설명 : 토큰으로 기계어 코드를 작성하는 함수이다.
* 매계 : 기계어 코드를 작성할 토큰
* 반환 : 출력이 필요한 경우 = 1, 출력이 필요 없는 경우 = 0 , 에러 < 0
* 주의 : 기계어 코드로 변환된 내용은 토큰의 code멤버에 저장된다.
* ----------------------------------------------------------------------------------
*/
int make_machine_code(int token_index) {

	int result = 0;
	int tmp = 0;
	char *code = NULL;

	token *unit = token_table[token_index];

	if (unit->optype == TYPE_INSTRUCTION) {

		int index = unit->opindex;
		
		int format = inst_table[index]->format;
		if (format == 1) {

			// 오퍼랜드가 있으면 안됨!
			if (unit->operand[0] != NULL) return -1;

			code = (char*)malloc(sizeof(char) * 3);
			memset(code, 0, 3);
			sprintf(code, "%02X", inst_table[index]->opcode);
		}
		else if (format == 2) {
			
			int param = inst_table[index]->num_operand;
			int p1=0, p2=0;
			
			if (param == 1) {
				// 오퍼랜드는 1개만 있어야 함
				if (unit->operand[0] == NULL || unit->operand[1] != NULL) return -1;

				// 레지스터 번호 찾기
				if ((p1 = search_register(unit->operand[0])) < 0) return -1;
			}
			else if (param == 2) {
				// 오퍼랜드가 2개 있어야 함
				if (unit->operand[0] == NULL || unit->operand[1] == NULL) return -1;

				// 레지스터 번호 찾기
				if ((p1 = search_register(unit->operand[0])) < 0) return -1;
				if ((p2 = search_register(unit->operand[1])) < 0) return -1;

			}
			else return -1;
			
			
			code = (char*)malloc(sizeof(char) * 5);
			memset(code, 0, 5);
			sprintf(code, "%02X%01X%01X", inst_table[index]->opcode, p1, p2);
			

		}
		else {

			int param = inst_table[index]->num_operand;
		
			if (unit->operator[0] == '+') {
				// format 4

				unit->nixbpe |= BIT_E;

				if (param > 0) {

					// operand number check
					if (unit->operand[0] == NULL) return -1;

					// indirect, immediate addressing check
					if (unit->operand[0][0] == '@') unit->nixbpe |= BIT_N;
					else if (unit->operand[0][0] == '#') unit->nixbpe |= BIT_I;
					else unit->nixbpe |= BIT_N | BIT_I;

					// symbol check & make modification unit
					if ((tmp = search_globalSymbol(unit->operand[0])) < 0) return -1;
					else if (symbol_table[tmp]->refSection & (1 << unit->section)) {
						
						modification *modi = (modification*)malloc(sizeof(modification));
						memset(modi, 0, sizeof(modification));

						modi->section = unit->section;
						modi->length = 5;
						modi->operation = 1;
						modi->symbol_index = tmp;
						modi->token_index = token_index;

						modify_table[modify_line++] = modi;

					}
					else return -1;


					// X bit check
					if (unit->operand[1] != NULL && strcmp(unit->operand[1], "X") == 0)
						unit->nixbpe |= BIT_X;

				}
				else {
					// no operand
					unit->nixbpe |= BIT_N | BIT_I;
				}

				code = (char*)malloc(sizeof(char) * 9);
				memset(code, 0, 9);

				tmp = (inst_table[index]->opcode << 24) | (unit->nixbpe << 20);
				sprintf(code, "%08X", (unsigned)tmp);
			}
			else {
				// format 3

				int addr = 0;

				if (param > 0) {

					// operand number check
					if (unit->operand[0] == NULL) return -1;

					// indirect, immediate addressing check
					if (unit->operand[0][0] == '@') unit->nixbpe |= BIT_N;
					else if (unit->operand[0][0] == '#') unit->nixbpe |= BIT_I;
					else unit->nixbpe |= BIT_N | BIT_I;

					// X bit check
					if (unit->operand[1] != NULL && strcmp(unit->operand[1], "X") == 0)
						unit->nixbpe |= BIT_X;

					if (unit->operand[0][0] == '#') {
						addr = atoi(&unit->operand[0][1]);
					}
					else if (unit->operand[0][0] == '=') {
					
						if ((tmp = search_literal(unit->operand[0], unit->section)) < 0) return -1;
						if (literal_table[tmp]->addr <= 0) return -1;

						addr = literal_table[tmp]->addr - unit->addr - 3;
						
						// 12bit 로 표현 가능해야한다.
						if (addr < 4096 && addr > -4096) unit->nixbpe |= BIT_P;
						else return -1;
					}
					else {
						// 로컬 심볼이고 값이 정의되어 있어야 한다.
						if ((tmp = search_localSymbol(unit->operand[0], unit->section)) < 0) return -1;
						if (!(symbol_table[tmp]->flag & F_EXIST)) return -1;

						addr = symbol_table[tmp]->addr - unit->addr - 3;

						// 12bit 로 표현 가능해야한다.
						if (addr < 4096 && addr > -4096) unit->nixbpe |= BIT_P;
						else return -1;
					}

				}
				else {
					// no operand
					unit->nixbpe |= BIT_N | BIT_I;
				}

				code = (char*)malloc(sizeof(char) * 7);
				memset(code, 0, 7);

				addr &= 0x000FFF;
				tmp = (inst_table[index]->opcode << 16) | (unit->nixbpe << 12) | addr;
				sprintf(code, "%06X", (unsigned)tmp);
			
			}
		
		
		}


		result = 1;

	}
	else if (unit->optype == TYPE_DIRECTIVE && unit->opindex == 5) {
		//BYTE

		if (unit->operand[0][0] == 'C' || unit->operand[0][0] == 'c') {

			int length = (strlen(unit->operand[0]) - 3)*2;
			code = (char*)malloc(sizeof(char)*(length + 1));
			memset(code, 0, length + 1);

			int i;
			for (i = 0; i < length; i++) {
				sprintf(code, "%s%02X", code, unit->operand[0][i + 2]);
			}

		}
		else if (unit->operand[0][0] == 'X' || unit->operand[0][0] == 'x') {

			int length = strlen(unit->operand[0]) - 3;
			code = (char*)malloc(sizeof(char)*(length + 1));
			memset(code, 0, length + 1);

			int i;
			for (i = 0; i < length; i++) {
				sprintf(code, "%s%c", code, unit->operand[0][i + 2]);
			}

		}
		else
			return -1;

		result = 1;

	}
	else if (unit->optype == TYPE_DIRECTIVE && unit->opindex == 6) {
		// WORD

		if (isNumeric(unit->operand[0]) > 0) {

			code = (char*)malloc(sizeof(char) * 7);
			memset(code, 0, 7);
			tmp = atoi(unit->operand[0]);
			sprintf(code, "%06X", tmp);
		}
		else if (isExpression(unit->operand[0]) > 0) {
		
			// 수식인 경우
			char *operand = unit->operand[0];
			char name[10] = { 0 };
			int len = strlen(operand);

			char op = '+';
			int i, idx;
			for (i = 0, idx = 0; i <= len; i++, idx++) {

				if (operand[i] == '+' || operand[i] == '-' || i == len) {

					// 연산자가 나타난 경우 계산
					name[idx] = '\0';
					int index = search_localSymbol(name, unit->section);

					if (index >= 0 && symbol_table[index]->flag & F_EXIST) {
						// 값이 존재하는 로컬심볼의 경우 계산
						if (op == '+')
							tmp += symbol_table[tmp]->addr;
						else
							tmp -= symbol_table[tmp]->addr;

					}
					else if ((index=search_globalSymbol(name))>=0) {
						// 전역 심볼이면서 EXTREF 선언한 경우
						if (symbol_table[index]->refSection & (1 << unit->section)) {

							modification *modi = (modification*)malloc(sizeof(modification));
							memset(modi, 0, sizeof(modification));

							modi->section = unit->section;
							modi->length = 6;
							modi->operation = (op == '+') ? 1 : 0;
							modi->symbol_index = index;
							modi->token_index = token_index;

							modify_table[modify_line++] = modi;

						}
						else return -1;

					}
					else return -1;

					op = operand[i];
					idx = -1;
				}
				else {
					name[idx] = operand[i];
				}

			}

			if (i > len) {

				code = (char*)malloc(sizeof(char) * 7);
				memset(code, 0, 7);
				sprintf(code, "%06X", tmp);

			}


			result = 1;

		}
		else return -1;


	}
	else if (unit->optype == TYPE_LITERAL) {

		int index = search_literal(unit->operator, unit->section);
		if (index < 0) return -1;

		

		if (unit->operator[1] == 'C' || unit->operator[1] == 'c') {
			
			int length = literal_table[index]->length;
			code = (char*)malloc(sizeof(char)*(length + 1));
			memset(code, 0, length + 1);

			int i;
			for (i = 0; i < length; i++) {
				sprintf(code, "%s%X", code, unit->operator[i + 3]);
			}

		}
		else if (unit->operator[1] == 'X' || unit->operator[1] == 'x') {

			int length = (literal_table[index]->length)*2;
			code = (char*)malloc(sizeof(char)*(length + 1));
			memset(code, 0, length + 1);

			int i;
			for (i = 0; i < length; i++) {
				sprintf(code, "%s%c", code, unit->operator[i + 3]);
			}

		}
		else
			return -1;

		result = 1;
	}



	unit->code = code;
	return result;
}












/* -----------------------------------------------------------------------------
* 설명 : 지시어에 맞춰 기능을 수행하는 함수이다.
* 매계 : 토큰 구조체 변수
* 반환 : 정상처리 > 0 , 아닌 경우 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int execute_directive(token *unit) 
{
	if (unit->optype != TYPE_DIRECTIVE) return -1;

	if (unit->opindex == 0) {
		// START

		// 오퍼랜드 수, 오퍼랜드 형식 체크
		if (unit->operand[1] != NULL) return -1;
		if (isNumeric(unit->operand[0]) < 0) return -1;

		section *sect = (section *)malloc(sizeof(section));
		memset(sect, 0, sizeof(section));
		sect->name = unit->label;
		section_table[section_line] = sect;

		locctr = start_addr = atoi(unit->operand[0]);
	}
	else if (unit->opindex == 1) {	
		//EXTDEF
		int i, index;
		for (i = 0; unit->operand[i] != NULL; i++) {

			if ((index = search_globalSymbol(unit->operand[i])) >= 0 && symbol_table[index]->section<0) {
				// EXTDEF로 선언되어 있지만 
				// 외부에서 EXTREF로 심볼테이블에 추가만 해놓은 경우
				symbol_table[index]->section = unit->section;
			
			}
			else if (search_localSymbol(unit->operand[i], unit->section)<0 && search_globalSymbol(unit->label)<0) {
				// 선언되어 있지 않은 심볼이면 심볼테이블에 추가
				int tmp = add_symbol_table(unit->operand[i], unit, F_EXTDEF);
				strcpy(symbol_table[tmp]->symbol, unit->operand[i]);
			}
			else return -1;
		}
	}
	else if (unit->opindex == 2) {
		//EXTREF
		int i, index;
		for (i = 0; unit->operand[i] != NULL; i++) {

			// 로컬 심볼이면 안됨
			if (search_localSymbol(unit->operand[i], unit->section) >= 0) return -1;

			if ((index = search_globalSymbol(unit->operand[i]))<0) {
				// 전역 심볼로 등록되어 있지 않다면 새로 추가
				// EXTDEF로 추가하되 section 정보는 없애고 참조 정보 추가
				int tmp = add_symbol_table(unit->operand[i], unit, F_EXTDEF);
				strcpy(symbol_table[tmp]->symbol, unit->operand[i]);
				symbol_table[tmp]->section = -1;
				symbol_table[tmp]->refSection |= (1 << unit->section);
			}
			else {
				// 전역 심볼로 등록되어 있다면 참조 정보 추가
				symbol_table[index]->refSection |= (1 << unit->section);
			}
			


		}
	}
	else if (unit->opindex == 3 || unit->opindex == 4) {
		// RESW / RESB

		int index;
		if (search_localSymbol(unit->label, unit->section) < 0 && search_globalSymbol(unit->label) < 0) {
			// 로컬 심볼로도 전역 심볼로도 정의되지 않은 새로운 심볼인 경우
			add_symbol_table(unit->label, unit, F_EXIST);
		}
		else if ((index = search_localSymbol(unit->label, unit->section)) >= 0) {
			if ((symbol_table[index]->flag & F_EXTDEF) && !(symbol_table[index]->flag & F_EXIST)) {
				// 현재 섹션에서 EXTDEF로 선언하였고, 아직 정의되지 않은 경우
				symbol_table[index]->addr = unit->addr;
				symbol_table[index]->flag |= F_EXIST;
			}
			else return -1;
		}
		else return -1;
		
		// location counter calculation
		if (unit->opindex == 3) locctr += 3 * atoi(unit->operand[0]);
		else locctr += atoi(unit->operand[0]);
	}
	else if (unit->opindex == 5) {
		// BYTE

		// 오퍼랜드는 하나만 사용해야함
		if (unit->operand[1] != NULL) return -1;

		char *value = unit->operand[0];

		// C'__' / X'__' format check
		int len = strlen(value);
		if (value[1] != '\'' && value[1] != '\"') return -1;
		if (value[1] != value[len - 1]) return -1;


		int index;
		if ((index = search_globalSymbol(unit->label)) >= 0) {
			if (symbol_table[index]->section == unit->section && !(symbol_table[index]->flag & F_EXIST)){
				// EXTDEF symbol & no definition
				symbol_table[index]->addr = unit->addr;
				symbol_table[index]->flag |= F_EXIST;
			}
			else return -1;

		}
		else if (search_localSymbol(unit->label, unit->section) < 0) {
			// no global symbol & no local symbol
			add_symbol_table(unit->label, unit, F_EXIST);
		}
		else
			return -1;

		if (value[0] == 'c' || value[0] == 'C') {
			locctr += strlen(value) - 3;
		}
		else if (value[0] == 'x' || value[0] == 'X') {
			locctr += (strlen(value) - 3) / 2;
		}
		else return -1;

	}
	else if (unit->opindex == 6) {
		// WORD
		int index;
		if ((index = search_globalSymbol(unit->label)) >= 0) {
			if (symbol_table[index]->section == unit->section && !(symbol_table[index]->flag & F_EXIST)) {
				// same section EXTDEF symbol & value not exist
				symbol_table[index]->addr = unit->addr;
				symbol_table[index]->flag |= F_EXIST;
			}
			else return -1;
		
		}
		else if (search_localSymbol(unit->label, unit->section) < 0) {
			// no definition
			add_symbol_table(unit->label, unit, F_EXIST);
		}
		else return -1;

		locctr += 3;
	}
	else if (unit->opindex == 7) {
		// LTORG
		execute_LTORG();
	}
	else if (unit->opindex == 8) {
		// EQU

		// 한 개의 오퍼랜드만 가능
		if (unit->operand[1] != NULL) return -1;


		// 심볼테이블 추가
		int index, tmp;

		if ((index = search_globalSymbol(unit->label)) >= 0) {
			if (symbol_table[index]->section == unit->section && !(symbol_table[index]->flag&F_EXIST)) {
				// 현재 섹션에서 EXTDEF로 선언하였고 값이 정의되지 않은 경우 값 추가
				symbol_table[index]->flag |= F_EQU | F_EXIST;
			}
			else return -1;
		}
		else if (search_localSymbol(unit->label, unit->section) < 0) {
			// 지역심볼, 전역심볼 둘 다 아닐 때
			index = add_symbol_table(unit->label, unit, F_EQU | F_EXIST);
		}
		else - 1;

		// 값 계산
		if (strcmp(unit->operand[0], "*") == 0) {
			symbol_table[index]->addr = locctr;
		}
		else if (isExpression(unit->operand[0])) {
		
			// 수식인 경우

			char *operand = unit->operand[0];
			char name[10] = { 0 };
			int len = strlen(operand);

			char op = '+';
			int i, idx, result = 0;
			for (i = 0, idx = 0; i <= len; i++, idx++) {
			
				if (operand[i] == '+' || operand[i] == '-' || i==len) {

					// 연산자가 나타난 경우 계산
					name[idx] = '\0';
					int tmp = search_localSymbol(name, unit->section);

					if (tmp>=0 && symbol_table[tmp]->flag & F_EXIST) {
						// 값이 존재하는 로컬심볼의 경우에만 계산 가능
						if(op == '+')
							result += symbol_table[tmp]->addr;
						else
							result -= symbol_table[tmp]->addr;
					
					}
					else return -1;

					op = operand[i];
					idx = -1;
				}
				else {
					name[idx] = operand[i];
				}
			
			}

			symbol_table[index]->addr = result;

		}
		else if((tmp = search_localSymbol(unit->operand[0], unit->section))>=0 && (symbol_table[tmp]->flag&F_EXIST)){
			// 로컬 심볼이고 값이 정의된 경우
			symbol_table[index]->addr = symbol_table[tmp]->addr;
		}
		else if(isNumeric(unit->operand[0])){
			// 숫자로만 이루어진 경우 
			symbol_table[index]->addr = atoi(unit->operand[0]);
		}
		else return -1;

		unit->addr = symbol_table[index]->addr;		// 명령어주소를 값으로 변경

	}
	else if (unit->opindex == 9) {}
	else if (unit->opindex == 10) {
		//CSECT

		// LTORG
		execute_LTORG();

		// calculate section length & initialze location counter
		section_table[section_line]->length = locctr - start_addr;
		locctr = start_addr = 0;

		// generate new section
		section *sect = (section*)malloc(sizeof(section));
		memset(sect, 0, sizeof(section));
		sect->name = unit->label;
		section_table[++section_line] = sect;

		unit->addr = locctr;
		unit->section = section_line;

	}
	else if (unit->opindex == 11) {}
	else if (unit->opindex == 12) {
		//END

		// LTORG & last section length calculation
		execute_LTORG();
		section_table[section_line]->length = locctr - start_addr;

		// save return address
		int index = search_localSymbol(unit->operand[0], 0);
		if (index < 0) return -1;

		return_addr = symbol_table[index]->addr;

	}

	return 1;
}

/* -----------------------------------------------------------------------------
* 설명 : 지시어에 맞춰 기능을 수행하는 함수이다.
* 매계 : 문자
* 반환 : 정상처리 = 16진수 값 , 아닌 경우 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int charToHex(char ch)
{
	if (ch >= '0' && ch <= '9') return ch - '0';
	else if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
	else return -1;
}

/* -----------------------------------------------------------------------------
* 설명 : 문자열이 수식인지 확인하는 함수이다.
* 매계 : 문자열
* 반환 : 수식인 경우 > 0 , 아닌 경우 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int isExpression(char *str) 
{
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == '+' || str[i] == '-') break;
	}

	if (i == strlen(str)) return -1;
	else return 1;
}

/* -----------------------------------------------------------------------------
* 설명 : 문자열이 숫자로 이루어진 문자열인지 확인하는 함수이다.
* 매계 : 문자열
* 반환 : 숫자로만 이루어진 경우 > 0 , 오류 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int isNumeric(char *str) 
{
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i]<'0' || str[i]>'9') break;
	}

	if (i == strlen(str)) return 1;
	else return -1;
}


/* -----------------------------------------------------------------------------
* 설명 : LTORG 지시어를 수행하는 함수이다.
* 매계 : 
* 반환 : 
* 주의 :
*
* ------------------------------------------------------------------------------
*/
void execute_LTORG() 
{
	int i;
	for (i = 0; i < literal_line; i++) {
		if (literal_table[i]->addr == 0) break;
	}

	token *unit;
	char *label = "*";

	for (; i < literal_line; i++) {
		literal_table[i]->addr = locctr;

		unit = (token*)malloc(sizeof(token));
		memset(unit, 0, sizeof(token));

		unit->addr = locctr;
		unit->label = label;
		unit->operator = literal_table[i]->name;
		unit->optype = TYPE_LITERAL;
		unit->section = section_line;

		locctr += literal_table[i]->length;
		token_table[token_line++] = unit;
	}

}

/* -----------------------------------------------------------------------------
* 설명 : 리터럴을 리터럴 테이블에 추가하는 함수이다.
* 매계 : 리터럴 문자열
* 반환 : 정상종료 = 리터럴 값 길이, 에러 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int add_literal_table(char *operand)
{
	// NULL pointer check
	if (operand == NULL) return -1;

	// =C'__' format check
	int len = strlen(operand);
	if (operand[0] != '=') return -1;
	if (operand[2] != '\'' && operand[2] != '\"') return -1;
	if (operand[2] != operand[len - 1]) return -1;


	lit *unit = (lit*)malloc(sizeof(lit));
	memset(unit, 0, sizeof(lit));

	if (operand[1] == 'C' || operand[1] == 'c'){
		unit->length = len - 4;
	}
	else if (operand[1] == 'X' || operand[1] == 'x') {
		unit->length = (len - 4) / 2;
	}
	else return -1;

	unit->name = operand;
	unit->section = section_line;
	unit->addr = 0;

	literal_table[literal_line++] = unit;

	return unit->length;
}

/* -----------------------------------------------------------------------------
* 설명 : 새로운 심볼을 심볼 테이블에 추가하는 함수이다.
* 매계 : 토큰 구조체 변수
* 반환 : 정상종료 = 추가된 심볼테이블 인덱스, 에러 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int add_symbol_table(char *name,  token *unit, int flag)
{
	// error check
	if (unit == NULL) return -1;
	if (name == NULL) return -1;

	symbol *sym = (symbol*)malloc(sizeof(symbol));
	memset(sym, 0, sizeof(symbol));

	strcpy(sym->symbol, name);
	sym->flag = flag;
	sym->section = unit->section;
	sym->addr = unit->addr;

	symbol_table[symbol_line++] = sym;

	return symbol_line - 1;
}

/* -----------------------------------------------------------------------------
* 설명 : 레지스터의 이름을 기계어 코드로 반환해주는 함수이다.
* 매계 : 찾을 레지스터 이름
* 반환 : 정상종료 = 레지스터 번호, 에러 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int search_register(char *str)
{
	int i;
	for (i = 0; i < MAX_REGISTER; i++) {
		if (strcmp(reg_table[i], str) == 0) return i;
	}

	return -1;
}

/* -----------------------------------------------------------------------------
* 설명 : 리터럴 테이블에서 이름으로 검색하는 함수이다.
* 매계 : 찾을 리터럴 이름, 검색할 섹션
* 반환 : 정상종료 = 찾은 리터럴테이블 인덱스, 에러 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int search_literal(char *str, int section) {

	int i;
	int result = -1;

	if (str == NULL) return -1;

	for (i = 0; i < literal_line; i++) {
		if (strcmp(str, literal_table[i]->name) == 0 && literal_table[i]->section == section) {
			// same name, same section
			result = i;
			break;
		}
	}

	return result;

}

/* -----------------------------------------------------------------------------
* 설명 : 심볼테이블에서 해당 섹션내 선언된 심볼을 찾는 함수이다.
* 매계 : 찾을 심볼 이름, 검색할 섹션
* 반환 : 정상종료 = 찾은 심볼테이블 인덱스, 에러 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int search_localSymbol(char *str, int section)
{
	int i;
	int result = -1;

	if (str == NULL) return -1;
	if (str[0] == '@') str++;
	
	for (i = 0; i < symbol_line; i++) {
		if (strcmp(str, symbol_table[i]->symbol) == 0 && section == symbol_table[i]->section) {
			// same label, same section
			result = i;
			break;
		}
	}

	return result;
}

/* -----------------------------------------------------------------------------
* 설명 : EXTDEF로 선언된 심볼 중에서 이름으로 심볼을 검색하는 함수이다.
* 매계 : 찾을 심볼 이름
* 반환 : 정상종료 = 찾은 심볼테이블 인덱스, 에러 < 0
* 주의 :
*
* ------------------------------------------------------------------------------
*/
int search_globalSymbol(char *str)
{
	int i;
	int result = -1;

	if (str == NULL) return -1;
	if (str[0] == '@') str++;

	for (i = 0; i < symbol_line; i++) {
		if (strcmp(str, symbol_table[i]->symbol) == 0 && (symbol_table[i]->flag & F_EXTDEF)) {
			// same name, EXTDEF symbol
			result = i;
			break;
		}
	}

	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str) 
{
	/* add your code here */

	int result = -1;
	int i;

	if(str==NULL) return -1;
	if(str[0]=='+') str++;

	for(i=0; i<inst_index; i++){
		if(strcmp(inst_table[i]->inst_name, str)==0){
			result = i;
			break;
		}
	}

	return result;

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 지시어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 지시어 테이블 인덱스, 에러 < 0
* 주의 :
*
* ----------------------------------------------------------------------------------
*/
int search_directive(char *str)
{
	int result = -1;
	int i;

	if(str==NULL) return -1;

	for(i=0; i<dir_index; i++){
		if(strcmp(dir_table[i]->dir_name, str)==0){
			result = i;
			break;
		}
	}
	return result;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	/* add your code here */

	FILE *file;
	int i;

	if(file_name==NULL){

		// file_name이 NULL인 경우 표준출력을 파일스트림으로 연다.
		// 정상적으로 열지 못한다면 에러 메시지 출력 후 반환한다.

		if ((file = fdopen(1, "w")) == NULL) {
			printf("stdout discriptor open error\n");
			return;
		}
	}
	else if((file=fopen(file_name, "w"))==NULL){

		// file_name이 NULL이 아닌 경우 해당 이름으로 파일을 연다.
		// 정상적으로 열지 못한다면 에러 메세지 출력 후 반환한다.

		printf("output file open error\n");
		return;
	}

	for(i=0; i<line_num; i++){
	
		// 해당 라인을 토큰으로 파싱
		if(token_parsing(i)<0){
			printf("parsing error\n");
			break;
		}

		if(token_table[i]->optype==TYPE_COMMENT){
			// 파싱 된 토큰 타입이 주석인 경우 전체를 출력
			fprintf(file, "%s", token_table[i]->comment);
		}
		else{
		
			// 주석이 아닌 경우 label - operator - operand
			// 순서로 명령어와 지시어 타입 출력이 같다.

			// label 출력
			if(token_table[i]->label!=NULL)
				fprintf(file, "%s\t", token_table[i]->label);
			else
				fprintf(file, "\t");

			// operator 출력
			fprintf(file, "%s\t", token_table[i]->operator);

			// operand 출력
			if(token_table[i]->operand[0]!=NULL){
				int j;
				fprintf(file, "%s", token_table[i]->operand[0]);
				for(j=1; j<MAX_OPERAND; j++){
					if(token_table[i]->operand[j]==NULL) break;
					fprintf(file, ",%s", token_table[i]->operand[j]);
				}
			}
		
		}
		
		
		// 토큰 타입이 명령어인 경우에만 기계어 코드 출력
		if(token_table[i]->optype==TYPE_INSTRUCTION)
			fprintf(file, "\t\t%02X", inst_table[token_table[i]->opindex]->opcode);
		

		fprintf(file, "\n");
	
	}

	fflush(file);

	// 표준출력을 파일스트림을 연 경우 닫으면 더이상의 출력이 
	//불가능하므로 파일 출력의 경우만 파일을 닫도록 한다.
	if(file_name!=NULL) fclose(file);

}



/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

	int i;

	for (i = 0; i < line_num; i++) {

		// 해당 라인을 토큰으로 파싱
		if (token_parsing(i)<0) {
			printf("parsing error\n");
			return -1;
		}
	
	}

	return 0;

}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	int i;
	token *unit;

	for (i = 0; i < token_line; i++) {
		unit = token_table[i];

		if (unit->optype == TYPE_COMMENT) {
			printf("%-s\n", unit->comment);
			continue;
		}

		// print address
		if (unit->optype == TYPE_INSTRUCTION || unit->optype == TYPE_LITERAL) {
			printf("%04X\t", unit->addr);
		}
		else {
			switch (unit->opindex) {
			case 1:
			case 2:
			case 7:
			case 12:
				printf("%4c\t", ' ');
				break;
			default:
				printf("%04X\t", unit->addr);
			}
		}


		// print label
		if (unit->label != NULL) printf("%-10s", unit->label);
		else printf("%10c", ' ');

		// print operator
		printf("%-10s", unit->operator);

		// print operand
		if (unit->operand[0] != NULL) {

			char operand[100] = { 0 };
			sprintf(operand, "%s", token_table[i]->operand[0]);

			int j;
			for (j = 1; j<MAX_OPERAND; j++) {
				if (token_table[i]->operand[j] == NULL) break;
				sprintf(operand, "%s,%s", operand, unit->operand[j]);
			}

			printf("%-15s", operand);
		}
		else printf("%15c", ' ');

		// print machine code
		int code;
		if ((code = make_machine_code(i)) < 0) return -1;
		else if (code > 0) printf("\t%s", unit->code);

		printf("\n");

	}

	return 0;

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	FILE *file;
	token *unit;
	int i;

	if (file_name == NULL) {

		// file_name이 NULL인 경우 표준출력을 파일스트림으로 연다.
		// 정상적으로 열지 못한다면 에러 메시지 출력 후 반환한다.

		if ((file = fdopen(1, "w")) == NULL) {
			printf("stdout discriptor open error\n");
			return;
		}
	}
	else if ((file = fopen(file_name, "w")) == NULL) {

		// file_name이 NULL이 아닌 경우 해당 이름으로 파일을 연다.
		// 정상적으로 열지 못한다면 에러 메세지 출력 후 반환한다.

		printf("output file open error\n");
		return;
	}

	int sect = 0;				// section number
	int start = 0;				// start address
	int count = 0;				// byte counter
	char value[70] = { 0 };		// object code

	for (i = 0; i < token_line; i++) {
	
		unit = token_table[i];

		if (unit->optype == TYPE_DIRECTIVE) {

			if (unit->opindex == 0) {
				// START
				start = unit->addr;
				fprintf(file, "H%-6s%06X%06X\n", unit->label, unit->addr, section_table[unit->section]->length);
			}
			else if (unit->opindex == 1) {
				// EXTDEF
				fprintf(file, "D");

				int j;
				for (j = 0; j < MAX_OPERAND; j++) {
					if (unit->operand[j] == NULL) break;
					int index = search_globalSymbol(unit->operand[j]);
					fprintf(file, "%-6s%06X", symbol_table[index]->symbol, symbol_table[index]->addr);
				}

				fprintf(file, "\n");

			}
			else if (unit->opindex == 2) {
				//EXTREF
				fprintf(file, "R");

				int j;
				for (j = 0; j < MAX_OPERAND; j++) {
					if (unit->operand[j] == NULL) break;
					int index = search_globalSymbol(unit->operand[j]);
					fprintf(file, "%-6s", symbol_table[index]->symbol);
				}

				fprintf(file, "\n");

			}
			else if (unit->opindex == 5 || unit->opindex == 6) {
				// BYTE & WORD

				// check Modification record
				int j;
				for (j = 0; j < modify_line; j++) {
					if (modify_table[j]->token_index == i) {
						modify_table[j]->location = start + count;
					}
				}

				// check Text record
				if (start + count != unit->addr || count + strlen(unit->code)/2 > 30) {

					fprintf(file, "T%06X%02X%s\n", start, count, value);

					start = unit->addr;
					count = strlen(unit->code) / 2;
					memset(value, 0, sizeof(value));
					strcpy(value, unit->code);

				}
				else {
					count += strlen(unit->code) / 2;
					strcat(value, unit->code);
				}

			}
			else if (unit->opindex == 10 || unit->opindex == 12) {
				// CSECT & END

				// check Text record
				if (count > 0) {
					fprintf(file, "T%06X%02X%s\n", start, count, value);

					start = unit->addr;
					count = 0;
					memset(value, 0, sizeof(value));
				}

				// check Modification record
				int j;
				for (j = 0; j < modify_line; j++) {
					if (modify_table[j]->section == sect) {
						fprintf(file, "M%06X%02X%c%s\n", modify_table[j]->location,
							modify_table[j]->length,
							(modify_table[j]->operation == 1) ? '+' : '-',
							symbol_table[modify_table[j]->symbol_index]->symbol);
					
					}
				}

				// print End record
				if (unit->section - 1 == 0) fprintf(file, "E%06X\n\n", return_addr);
				else fprintf(file, "E\n\n");

				// print Head record (CSECT)
				if (unit->opindex == 10) {
					start = unit->addr;
					fprintf(file, "H%-6s%06X%06X\n", unit->label, unit->addr, section_table[unit->section]->length);
				}

				sect++;

			}
			
		}
		else if (unit->optype == TYPE_INSTRUCTION || unit->optype == TYPE_LITERAL) {

			// check Modification record
			int j;
			for (j = 0; j < modify_line; j++) {
				if (modify_table[j]->token_index == i) {
					modify_table[j]->location = start + count + 1;
				}
			}

			// check Text record
			if (start + count != unit->addr || count + strlen(unit->code)/2 > 30) {
			
				fprintf(file, "T%06X%02X%s\n", start, count, value);

				start = unit->addr;
				count = strlen(unit->code) / 2;
				memset(value, 0, sizeof(value));
				strcpy(value, unit->code);

			}
			else {
				count += strlen(unit->code) / 2;
				strcat(value, unit->code);
			}
		
		}
	
	
	}

	fclose(file);

}
