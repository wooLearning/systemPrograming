/*
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000

#define MAX_COLUMNS 4
#define MAX_OPERAND 3

/*���� �߰��� define*/
#define MAX_SEC 3 // file section 3�� MAX �߰� ����


 /*
 * instruction ����� �����ϴ� ����ü�̴�.
 * instruction ��� ���Ϸκ��� ������ �޾ƿͼ� �����Ѵ�.
 * instruction ��� ���Ͽ��� ���κ��� �ϳ��� instruction�� �����Ѵ�.
 *
 */
typedef struct _inst
{
	char str[10];//name
	unsigned char op;//opcode
	int format;//formant
	int ops;//number of oprend
} inst;


// ��� �����ϴ� ���̺�
inst* inst_table[MAX_INST];
int inst_index;


// ����� �� �ҽ��ڵ带 ���Ϸκ��� �ҷ��� ���κ��� �����ϴ� ���̺�
char* input_data[MAX_LINES];
static int line_num;

int label_num;


/*
 * ����� �� �ҽ��ڵ带 ��ū���� ��ȯ�Ͽ� �����ϴ� ����ü �����̴�.
 * operator �������� renaming�� ����Ѵ�.
 */
typedef struct _token {
	char* label;
	char* operator;
	char* operand[MAX_OPERAND];
	char comment[100];
	char nixbpe;    // ���� ������Ʈ���� ���
} token;

// ����� �� �ҽ��ڵ带 5000���α��� �����ϴ� ���̺�
token* token_table[MAX_LINES];
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������Ʈ���� ��� ����
 */
typedef struct _symbol
{
	char symbol[10];
	int addr;
} symbol;

/*
* ���ͷ��� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ��� �̸�, ���ͷ��� ��ġ�� �����ȴ�.
* ���� ������Ʈ���� ��� ����
*/
typedef struct _literal {
	char* literal;
	int addr;
} literal;

symbol sym_table[MAX_LINES];
static int sym_index;

literal literal_table[MAX_LINES];
static int literal_index;


/**
 * ������Ʈ �ڵ� ��ü�� ���� ������ ��� ����ü�̴�.
 * Header Record, Define Recode,
 * Modification Record � ���� ������ ��� �����ϰ� �־�� �Ѵ�. ��
 * ����ü ���� �ϳ������� object code�� ����� �ۼ��� �� �ֵ��� ����ü�� ����
 * �����ؾ� �Ѵ�.
 *
 * ���� ������Ʈ���� ��� ����
 */
typedef struct _object_code {
	/* add fields */

	 // H' line  
	char p_name[7];//program name
	int h_start_addr;//start adress
	int p_length;//program length

	//D' line
	char d_name[10][7];//10��??
	int d_addr[10];

	//R' line
	char r_name[10][7];

	//T' line
	int t_addr[MAX_LINES];//each line  start address
	int t_length[MAX_LINES]; //each line length
	int t_code[MAX_LINES];//code
	int t_format[MAX_LINES];

	// M' line
	int m_addr[MAX_LINES]; 
	int m_length[MAX_LINES];
	char* m_name[MAX_LINES];
	char m_sign[MAX_LINES];

	// E' line
	int end;
} object_code;
object_code code[MAX_SEC];

static int locctr;
//--------------

static char* input_file;
static char* output_file;

/*���� �߰��� ����*/
static int token_table_addr[MAX_LINES];//������ ������� TA - PC�� ���ؼ�
static int sym_len[MAX_SEC];//symbol �˻� ����
static int lit_len[10] = {0};//literal searching arange (in pass1 split literal table;



int init_my_assembler(void);
int init_inst_file(char* inst_file);
int init_input_file(char* input_file);
int token_parsing(char* str);
int search_opcode(char* str);
static int assem_pass1(void);
void make_opcode_output(char* file_name);
void make_symtab_output(char* file_name);
void make_literaltab_output(char* filename);
static int assem_pass2(void);
void make_objectcode_output(char* file_name);

/*���� �߰��� �Լ�*/
int split_tap(char* data, char* arr[]);// tap ����
int split_comma(char* data, char* arr[]);// comma ����
int literal_check(char* temp);// literal �ߺ� üũ
int search_opcode(char* str);//operator -> opcode�� ��ȯ

void split_table();
int reg_num(char c);
int search_sym(char* s, int mode);
int search_lit(char* s);
//void search_table(char* s, int mode);//�� ���� ��������