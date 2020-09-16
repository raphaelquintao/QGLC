#ifndef LEXER_H
#define	LEXER_H

#include <iostream>
#include <fstream>
#include <sstream>

#include "Def.h"
#include "Util.h"
#include "SymbolsTable.h"

using namespace std;

class Lexer {
private:
    SymbolsTable *STable;
    fstream FileReader;
    char current;
    int n_token;
    int line;
    int state;

    int GetChar() {
        FileReader.get(current);
        if (!isalpha((int) current) && !isdigit((int) current) && current != ' ' && current != '.' && current != ',' && current != ';' && current != ':' && current != '(' && current != ')'
                && current != '[' && current != ']' && current != '{' && current != '}' && current != '+' && current != '-' && current != '=' && current != '"' && current != '\''
                && current != '/' && current != '$' && current != '!' && current != '*' && current != '?' && current != '>' && current != '<' && current != '\n' && current != '\r')
            PrintError(line, "caractere invalido");
        return 0;
    }

public:

    Lexer(std::string source_file, SymbolsTable *SymbTable) {
        FileReader.open(source_file, fstream::in);
        if (!FileReader.is_open()) {
            printf("%s: error: %s: No such file.\n", P_SHORT_NAME, source_file.c_str());
            exit(EXIT_FAILURE);
        }
        STable = SymbTable;
        n_token = 0;
        line = 1;
        state = 0;
        GetChar();
    }

    /**
     *  Retorna linha atual de leitura no arquivo. 
     */
    int GetLine() {
        return line;
    }

    /** 
     * Lê próximo token do arquivo.
     * @LexReg Registrador léxico para armazenamento dos dados do token.
     * @return 1 se houver token, 0 se não houver(fim do arquivo) e -1 se erro.
     */
    int NextToken(Register *LexReg) {
        stringstream lexeme;

        int EndOfFile = 0;
        do {
            if (FileReader.eof()) {
                EndOfFile++;
                current = 32; //Espaço
            }
            switch (state) {
                case 0:
                    if (current == '{') {
                        state = 14;
                    } else if (current == '.') {
                        lexeme << current;
                        state = 13;
                    } else if (current == '$') {
                        state = 15;
                    } else if (current == '<') {
                        lexeme << current;
                        state = 18;
                    } else if (current == '>') {
                        lexeme << current;
                        state = 21;
                    } else if (current == '=') {
                        lexeme << current;
                        state = 16;
                    } else if (current == '\n') {
                        lexeme << "enter";
                        line++;
                        state = 2;

                    } else if (current == '}') { // nao pode comecar com }
                        PrintError(line, "Final de comentario sem inicio");
                        return -1;

                    } else if ((current == ',') || (current == '(') ||
                            (current == ')') || (current == '+') || (current == '-') ||
                            (current == '*') || (current == '/')) {
                        lexeme << current;
                        state = 2;
                    } else if (isalpha((int) current)) {
                        lexeme << current;
                        state = 1;
                    } else if (isdigit((int) current)) {
                        lexeme << current;
                        state = 3;
                    } else if (current == ' ' || current == '\r') {
                        state = 0;
                    } else {
                        lexeme << current;
                        PrintError(line, "Lexema nao identificado", lexeme.str());
                    }
                    GetChar();
                    break;
                case 1:// Letra lida
                    if (isalpha((int) current)) {
                        lexeme << current;
                        state = 1;
                    } else if (isdigit((int) current)) {
                        lexeme << current;
                        state = 1;
                    } else {
                        state = 2;
                        break;
                    }
                    GetChar();
                    break;
                case 2:
                    int temp;
                    if ((temp = STable->Search(lexeme.str())) >= 0) {// Palavra Reservada
                        LexReg->token_number = temp;
                        LexReg->address = VAZIO;
                        LexReg->lexeme = toLowerCase(lexeme.str());
                        LexReg->data_type = VAZIO;
                        LexReg->interger_part = VAZIO;
                        LexReg->decimal_part = VAZIO;
                    } else { // Identificador
                        LexReg->token_number = VAZIO;
                        LexReg->address = STable->Insert(lexeme.str(), VAZIO, VAZIO);
                        LexReg->lexeme = toLowerCase(lexeme.str());
                        LexReg->data_type = VAZIO;
                        LexReg->interger_part = VAZIO;
                        LexReg->decimal_part = VAZIO;
                    }

                    lexeme.clear();
                    state = 0;
                    return 1;
                case 23: //Inteiro
                    LexReg->token_number = VAZIO;
                    LexReg->address = VAZIO;
                    LexReg->lexeme = lexeme.str();
                    LexReg->data_type = TIPO_INTEIRO;
                    LexReg->interger_part = lexeme.str().c_str();
                    LexReg->decimal_part = "";

                    lexeme.clear();
                    state = 0;
                    return 1;
                case 24: //Real
                    LexReg->token_number = VAZIO;
                    LexReg->address = VAZIO;
                    LexReg->lexeme = lexeme.str();
                    LexReg->data_type = TIPO_REAL;
                    LexReg->interger_part = lexeme.str().c_str();
                    LexReg->decimal_part = lexeme.str().substr(lexeme.str().find(".", 0) + 1, lexeme.str().size()).c_str();

                    lexeme.clear();
                    state = 0;
                    return 1;
                case 3: // 1 D Lido
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 4;
                    } else if (current == '.') {
                        lexeme << current;
                        state = 8;
                    } else if (isalpha((int) current)) {// estado de erro
                        state = 25;
                        break;
                    } else {
                        state = 23; //aceita
                        break;
                    }
                    GetChar();
                    break;
                case 4:// 2 D Lido DD
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 5;
                    } else if (current == '.') {
                        lexeme << current;
                        state = 8;
                    } else if (isalpha((int) current)) {// estado de erro se letra 
                        state = 25;
                        break;
                    } else {
                        state = 23;
                        break;
                    }
                    GetChar();
                    break;
                case 5: // 3 D lido DDD
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 6;
                    } else if (current == '.') {
                        lexeme << current;
                        state = 8;
                    } else if (isalpha((int) current)) {// estado de erro se letra
                        state = 25;
                        break;
                    } else {
                        state = 23;
                        break;
                    }
                    GetChar();
                    break;
                case 6: // 4 D lido DDDD
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 12;
                    } else if (current == '.') {
                        lexeme << current;
                        state = 8;
                    } else if (isalpha((int) current)) {// estado de erro se letra
                        state = 25;
                        break;
                    } else {
                        state = 23;
                        break;
                    }
                    GetChar();
                    break;
                case 7:// Numero após ponto .D
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 9;
                    } else if ((isalpha((int) current)) || current == '.') {// estado de erro se Ponto ou Letra
                        state = 25;
                        break;
                    } else {
                        state = 24; // Retorno de Real
                        break;
                    }
                    GetChar();
                    break;
                case 8: // Numero após ponto .
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 7;
                    } else if ((isalpha((int) current)) || current == '.') {// estado de erro se Ponto ou Letra
                        state = 25;
                        break;
                    } else {
                        state = 24; // retorna Real
                        break;
                    }
                    GetChar();
                    break;
                case 9:// Numero após Ponto .DD
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 10;
                    } else if ((isalpha((int) current)) || current == '.') {// estado de erro se Ponto ou Letra
                        state = 25;
                        break;
                    } else {
                        state = 24;
                        break;
                    }
                    GetChar();
                    break;
                case 10: // Numero após Ponto .DDD
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 11;
                    } else if ((isalpha((int) current)) || current == '.') {// estado de erro se Ponto ou Letra
                        state = 25;
                        break;
                    } else {
                        state = 24;
                        break;
                    }
                    GetChar();
                    break;
                case 11: // .DDDD
                    if ((isalpha((int) current)) || current == '.' || (isdigit((int) current))) {// estado de erro se Ponto ou Letra ou Dig
                        state = 25;
                    } else {
                        state = 24;
                    }
                    break;
                case 12: // Lendo Inteiros 
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 12;
                    } else if ((isalpha((int) current)) || current == '.') {// estado de erro se Ponto ou Letra
                        state = 25;
                    } else {
                        state = 23;
                        break;
                    }
                    GetChar();
                    break;
                case 13: // Começa com PONTO
                    if (isdigit((int) current)) {
                        lexeme << current;
                        state = 7;
                    } else { // Se diferente de Digito ERRO
                        state = 25;
                    }
                    GetChar();
                    break;
                case 14: // Inicio de Comentario
                    if (current == '}') {
                        state = 0;
                    } else if (current == '\n') {// Contar linha no comentario
                        line++;
                        state = 14;
                    } else {
                        state = 14;
                    }
                    GetChar();
                    break;
                case 15: // Depois de $

                    if (current == '\n') {
                        line++;
                        state = 0;
                    } else if (current == ' ' || current == 13) {// Posso ler Espacos apos o $
                        state = 15;
                    } else {// Encontrado qualquer caracter na mesma linha apos o $
                        PrintError(line, "Caracter invalido apos delimitador de quebra linha");
                        return -1;
                    }
                    GetChar();
                    break;
                case 16:// Apos o = nao ha erro
                    if (current == '=') {
                        lexeme << current;
                        state = 19; // ==
                    } else {
                        state = 2; // Retorna =
                        break;
                    }
                    GetChar();
                    break;
                case 17: // >=
                    state = 2; // Aceita
                    break;
                case 18: // <
                    if (current == '>') {// Ret <
                        lexeme << current;
                        state = 20;
                    } else if (current == '=') { // <=
                        lexeme << current;
                        state = 22;
                    } else {
                        state = 2;
                        break;
                    }
                    GetChar();
                    break;
                case 19: // ==
                    state = 2;
                    break;
                case 20: // <>
                    state = 2;
                    break;
                case 21:// >
                    if (current == '=') { // >=
                        lexeme << current;
                        state = 17;
                    } else {
                        state = 2; // Ret >
                        break;
                    }
                    GetChar();
                    break;
                case 22: //<=
                    state = 2;
                    break;
                case 25:// Estado de Erro
                    if ((current != ',') && (current != '(') &&
                            (current != ')') && (current != '+') && (current != '-') &&
                            (current != '*') && (current != '/') && (current != ' ') && (current != '\n') && (current != '\r')) {
                        lexeme << current;
                        state = 25;

                    } else {
                        PrintError(line, "Lexema não identificado", lexeme.str()); // Lexema nao identificado
                        return -1;
                    }
                    GetChar();
                    break;
            }
        } while (EndOfFile < 2);

        if (lexeme.str().size() > 0)
            PrintError(line, "Erro aleatório");
        return 0;
    }

};

#endif

