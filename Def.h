#ifndef DEF_H
#define	DEF_H

#include <string>

#define P_SHORT_NAME    "QGLC"
#define P_NAME          "Q GL Compiler"
#define P_VERSION       "1.0.3"
#define P_ABOUT         " Raphael Quintao\n Yousef Otacilio\n Ailton Gomes"

/* Palavras Reservadas */
static std::string T_POINT = "dponto";
static std::string T_COLOR = "dcor";
static std::string T_CONST = "dconst";
static std::string T_FACE = "dface";
static std::string T_OBJECT = "dobjeto";
static std::string T_LIGHT = "dluz";
static std::string T_VAR = "dvar";
static std::string T_INTERGER = "inteiro";
static std::string T_REAL = "real";
static std::string T_COMM_SCALE = "escala";
static std::string T_COMM_PAUSE = "pausa";
static std::string T_COMM_ROTTRANS = "rottrans";
static std::string T_COMM_LIGHT = "luz";
static std::string T_WHILE = "enquanto";
static std::string T_DO = "faca";
static std::string T_IF = "se";
static std::string T_THEN = "entao";
static std::string T_ELSE = "senao";
static std::string T_BEGIN = "inicio";
static std::string T_END = "fim";

/* Separadores */
static std::string T_COMMA = ",";
static std::string T_PARENS_L = "(";
static std::string T_PARENS_R = ")";
static std::string T_TERMINATOR = "enter";
static std::string T_NEWLINE = "enter";

/* Operadores Lógicos */
static std::string T_LOGIC_AND = "e";
static std::string T_LOGIC_OR = "ou";
static std::string T_LOGiC_NOT = "nao";
static std::string T_LOGIC_EQUAL = "==";
static std::string T_LOGIC_LESS = "<";
static std::string T_LOGIC_GREATER = ">";
static std::string T_LOGIC_DIFF = "<>";
static std::string T_LOGIC_LESS_EQUAL = "<=";
static std::string T_LOGIC_GREATER_EQUAL = ">=";

/* Operadores Aritméticos */
static std::string T_SUM = "+";
static std::string T_SUB = "-";
static std::string T_MUL = "*";
static std::string T_DIV = "/";
static std::string T_ASSIGN = "=";


#endif
