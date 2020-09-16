#ifndef UTIL_H
#define	UTIL_H

#include <stdarg.h>
#include <string>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include "SymbolsTable.h"

// ANSI escape codes 
#define FBOLD "\x1B[1m"
#define KRST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// Tipos Semânticos
#define CLASSE_VAR     10
#define CLASSE_CONST   11
#define CLASSE_PONTO   12
#define CLASSE_FACE    13
#define CLASSE_OBJETO  14
#define CLASSE_LUZ     15
#define CLASSE_COR     16

#define TIPO_INTEIRO   20
#define TIPO_REAL      21
#define TIPO_LOGICO    22
#define TIPO_VAZIO     -2
#define VAZIO          -2

#define OP_MUL   24
#define OP_DIV   25
#define OP_AND   26
#define OP_ADD   27
#define OP_SUB   28
#define OP_OR    29

#define OP_DIFF     30
#define OP_LESS     31
#define OP_GREATER  32
#define OP_EQUAL    33
#define OP_LEQUAL   34
#define OP_GEQUAL   35

#define TOKEN_TYPE_ID       "ID"
#define TOKEN_TYPE_CONST    "const"

#define DEBUG_M

namespace patch {

    template < typename T > std::string to_string(const T& n) {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

typedef struct Register {
    int token_number; // Para tokens de palavras reservadas
    int address;
    std::string lexeme;
    int data_type; // int, real ou vazio

    std::string interger_part;
    std::string decimal_part;

} Register;

const char* UTIL_PrintClass(int class_number) {
    switch (class_number) {
        case CLASSE_VAR:
            return "VAR";
        case CLASSE_CONST:
            return "CONST";
        case CLASSE_PONTO:
            return "PONTO";
        case CLASSE_FACE:
            return "FACE";
        case CLASSE_OBJETO:
            return "OBJETO";
        case CLASSE_LUZ:
            return "LUZ";
        case CLASSE_COR:
            return "COR";
        case VAZIO:
            return "VAZIO";
        default:
            return "N_C";
    }
}

const char* UTIL_PrintType(int type_number) {
    switch (type_number) {
        case TIPO_INTEIRO:
            return "INTEIRO";
        case TIPO_REAL:
            return "REAL";
        case TIPO_LOGICO:
            return "LOGICO";
        case VAZIO:
            return "VAZIO";
        default:
            return "NotType";
    }
}

void DEBUG_Printf(const char *fmt, ...) {
#ifdef DEBUG_M
    //    printf(FBOLD KYEL "-DEBUG: " KRST);
    printf("-DEBUG: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
}

std::string RealToStr(std::string lexeme) {
    std::string interger_part;
    interger_part = lexeme.substr(0, lexeme.find(".", 0));
    std::string decimal_part;
    if (lexeme.find(".", 0) != std::string::npos) {
        if (interger_part.size() <= 1)
            interger_part += "0";
        decimal_part = lexeme.substr(lexeme.find(".", 0) + 1, lexeme.size()).append(4, '0').substr(0, 4);
    } else {
        decimal_part = "0000";
    }
    return interger_part + "." + decimal_part;
}

int16_t NumToInt16_IntergerPart(std::string lexeme) {
    std::string interger_part;

    lexeme = lexeme.substr(lexeme.find("#", 0) + 1, lexeme.size()); //remove #
    //    std::string temp2 = temp.substr(temp.find("-", 0) + 1, temp.size()); //remove sinal

    interger_part = lexeme.substr(0, lexeme.find(".", 0));

    int16_t temp = atoi(interger_part.c_str());
    //    DEBUG_Printf("I: %s | %d\n", interger_part.c_str(), temp);
    return temp;
}

int16_t NumToInt16_DecimalPart(std::string lexeme) {

    std::string decimal_part;
    if (lexeme.find("-", 0) != std::string::npos)
        decimal_part += "-";
    if (lexeme.find(".", 0) != std::string::npos)
        decimal_part += lexeme.substr(lexeme.find(".", 0) + 1, lexeme.size()).append(4, '0').substr(0, 4);
    else
        decimal_part += "0000";

    int16_t temp = atoi(decimal_part.c_str());
    //    DEBUG_Printf("D: %s | %d\n", decimal_part.c_str(), temp);
    return temp;
}

int16_t RegCod(std::string reg) {
    if (reg == "A") {
        return 1;
    } else if (reg == "B") {
        return 2;
    } else if (reg == "C") {
        return 3;
    } else if (reg == "D") {
        return 4;
    } else if (reg == "E") {
        return 5;
    } else if (reg == "F") {
        return 6;
    } else {
        return 0;
    }
}

/**
 * Imprime Erro e aborta a execução.
 * @param line Linha onde foi encontrado erro.
 * @param msg Mensagem a ser impressa.
 * @param lexeme Lexema encontrado.
 */
void PrintError(int line, std::string msg, std::string lexeme) {
    //    printf(FBOLD KRED "%02d:%s [%s].\n" KRST, line, msg.c_str(), lexeme.c_str());
    printf("%02d:%s [%s].\n", line, msg.c_str(), lexeme.c_str());
    exit(EXIT_FAILURE);
}

/**
 * Imprime Erro e aborta a execução.
 * @param line Linha onde foi encontrado erro.
 * @param msg Mensagem a ser imprimida.
 */
void PrintError(int line, std::string msg) {
    //    printf(FBOLD KRED "%02d:%s.\n" KRST, line, msg.c_str());
    printf("%02d:%s.\n", line, msg.c_str());
    exit(EXIT_FAILURE);
}

/**
 * Imprime Erro e aborta a execução.
 * @param msg Mensagem a ser imprimida.
 * @param lexeme Lexema encontrado.
 */
void PrintError(std::string msg, std::string lexeme) {
    //    printf(FBOLD KRED "%s [%s].\n" KRST, msg.c_str(), lexeme.c_str());
    printf("%s [%s].\n", msg.c_str(), lexeme.c_str());
    exit(EXIT_FAILURE);
}

/**
 * Imprime Erro e aborta a execução.
 * @param msg Mensagem a ser imprimida.
 */
void PrintError(std::string msg) {
    //    printf(FBOLD KRED "%s.\n" KRST, msg.c_str());
    printf("%s.\n", msg.c_str());
    //    printf(FBOLD "\n%s\n" KRST, "Execução terminada com erro!");
    exit(EXIT_FAILURE);
}

std::string toLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}
#endif

