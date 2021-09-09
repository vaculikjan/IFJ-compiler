/*
 *
 *  Implementace prekladace imperativniho jazyka IFJcode18
 *
 *
 *
 *  scanner.h
 *
 *
 *  xmensi13 - Jan Mensik
 *
 *  xvacul30 - Jan Vaculik
 *  xtrejt00 - Martin Trejtnar
 *  xlesko06 - Patrik Lesko
 *
 */

#ifndef scanner_h
#define scanner_h

//-----------------------------------------------------------

//hlavicka pro lexikalni analyzator

//identifikator

#define ID          0

//klicova slova
#define DEF         10
#define DO          11
#define ELSE        12
#define END         13
#define IF          14
#define NOT         15
#define NIL         16
#define THEN        17
#define WHILE       18

//bila mista
#define BLANK       20
#define TAB         21
#define NEW_LINE    22

//operatory
#define PLUS        30
#define MINUS       31
#define MULT        32
#define DIV         33
#define LOWER       34
#define L_EQ        35
#define GREATER     36
#define G_EQ        37
#define EQUAL       38
#define N_EQUAL     39

//literaly
#define INT_LIT     40
#define FLOAT_LIT   41
#define STRING_LIT  42

//komentare
#define LN_COMMENT  50
#define BL_COMMENT  51

//prirazeni
#define ASSIGN      60

//delims
#define COMMA       70
#define L_BRACKET   71
#define R_BRACKET   72
#define DOLLAR      73  //for PSA
#define SHARP       74  //for PSA

//specialni znaky
#define END_OF_FILE 100

#define READ        110
#define WRITE       111
#define STRLEN      112
#define SUBSTR      113
#define STRI2INT    114
#define INT2CHAR    115
#define INPUTS      116
#define INPUTI      117
#define INPUTF      118

//chybove hlasky
#define LEX_ERROR    -1

//hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
int getNextToken(string *attr);

//-------------------------------------------------------------

//#define OP_CHAR_CNT 8
//#define OP_CNT 10
//#define DELIMETERS_CNT 7 // dunno if 9 :Kappa:
//#define KEYWORDS_CNT 9
//#define WHITESPACE_CNT 3
//
//char whitespaces[WHITESPACE_CNT] = {
//    ' ',
//    '\t',
//    '\n'
//};
//
//char op_char[OP_CHAR_CNT] = {
//    '+',
//    '-',
//    '*',
//    '/',
//    '<',
//    '>',
//    '=',
//    '!'
//};
//
//char *operators[OP_CNT] = {
//    "+",
//    "-",
//    "*",
//    "/",
//    "<",
//    "<=",
//    ">",
//    ">=",
//    "==",
//    "!="
//};
//
//char delimiters[DELIMETERS_CNT] = {
//    '(',
//    ')',
//    ';',
//    ',',
//    '"'
//};
//
//char *keywords[KEYWORDS_CNT] = {
//    "def",
//    "do",
//    "else",
//    "end",
//    "if",
//    "not",
//    "nil",
//    "then",
//    "while"
//
//};
//
//typedef enum{
//    T_ID,
//    T_STRING,
//    T_LINE_COMMENT,
//    T_BLOCK_COMMENT,
//    //operators
//    T_PLUS,     // +
//    T_MINUS,    // -
//    T_TIMES,    // *
//    T_DIV,      // '/'
//    T_LOWER,    // <
//    T_LOWER_EQ, // <=
//    T_GREATER,  // >
//    T_GREATER_EQ,// >=
//    T_EQUAL,    // ==
//    T_NOT_EQUAL,// !=
//    //keywords
//    T_DEF,
//    T_DO,
//    T_ELSE,
//    T_END,
//    T_IF,
//    T_NOT,
//    T_NIL,
//    T_THEN,
//    T_WHILE,
//    //numbers
//    T_INT,
//    T_FLOAT,
//    //boolean
//    T_BOOL_TRUE,
//    T_BOOL_FALSE,
//    //asignment
//    T_ASSIGNMENT
//
//} t_tokenID;
//
//
//typedef struct t_token {
//    t_tokenID token_type;
//    char buffer[100];
//} t_token;
//
//t_token *nas_fajn_token;
//
//
//void get_token(FILE * source,char *lexem);
//
//int append(char *lexem, char input);
//
//bool isoperator(char input);
//
//int chooseoperator(char *input);
//
//int iskeyword(char *input);
//
//bool iswhitespace(char input);
//
//void realloc_check(char *lexem, int *array_length);
//
//struct t_token create_token(struct t_token *Token);
//
//void append_token(int pointer, char *lexem, t_token *Token);


#endif /* scanner_h */
