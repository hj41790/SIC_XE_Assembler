/* 
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_DIRECTIVE 32

#define TYPE_INSTRUCTION	0
#define TYPE_DIRECTIVE		1
#define TYPE_COMMENT		2

/* 
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit {
	/* add your code here */
	char inst_name[10];
	unsigned char opcode;	// 기계어 코드
	int format;				// 명령어 형식

	// 명령어의 피연산자 개수
	// 0이면 확실히 0개, 1이면 1개 이상, 
	// 그외는 반드시 그만큼의 피연산자가 필요하다는 의미로 사용
	int num_operand;		
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/* 
 * 지시어 목록 파일로부터 정보를 받아와 생성하는 구조체 변수
 * 기계어 코드는 존재하지 않으므로 이름과 피연산자 개수만 저장 
 */
typedef struct directive_unit{
	char dir_name[10];
	int num_operand;
}directive;

directive *dir_table[MAX_DIRECTIVE];
int dir_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;

int label_num; 

/* 
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit {
	char *label;
	char *operator; 
	char *operand[MAX_OPERAND];
	char *comment;
	char opindex;	// operator와 일치하는 테이블 인덱스 저장
	char optype;	// 토큰의 타입을 저장하여 보다 편하게 관리
					// 위의 TYPE으로 시작하는 전처리문을 참고하여 값 설정
	//char nixbpe; // 추후 프로젝트에서 사용된다.
	
};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 */
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

static int locctr;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_dir_file(char *dir_file);
int init_input_file(char *input_file);
int search_opcode(char *str);
int search_directive(char *str);
void make_opcode_output(char *file_name);

/* 추후 프로젝트에서 사용하게 되는 함수*/
static int assem_pass1(void);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
