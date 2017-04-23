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

#include "my_assembler.h"

#pragma warning(disable:4996)

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

	make_opcode_output("output_144.txt");

	
	/* 
	 * 추후 프로젝트 1에서 사용되는 부분
	 *
	if(assem_pass1() < 0 ){
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output") ; 
	*/
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


	// 1. 라인 전체가 주석인가? ( 첫 글자가 '.' 으로 시작)
	int i;
	for(i=0; i<strlen(input_data[index]); i++)
		if(input_data[index][i]!=' ' || input_data[index][i]!='\t') break;
	
	if(input_data[index][i]=='.'){			// 주석인 경우
		unit->comment = input_data[index];	// 라인 전체를 comment에 등록
		unit->optype = TYPE_COMMENT;		// 토큰 타입을 주석으로 설정
		token_table[token_line++] = unit;	// 토큰 테이블에 등록 후 반환
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
		token_operand = strtok(token_next, ",");
		if(token_operand==NULL){
			// 피연산자가 있어야 하는데 없는 경우
			result = -1;
			return result;
		}
		else{

			// 둘 이상의 피연산자의 경우 공백 없이 ',' 으로 연결된 
			// 하나의 토큰으로 가정하고 파싱한다.
			unit->operand[0] = token_operand;

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

	// 파싱 완료한 토큰을 테이블에 등록
	token_table[token_line++] = unit;

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






/* --------------------------------------------------------------------------------*
 * ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
 * --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

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

	/* add your code here */

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
	/* add your code here */

}
