#ifndef SYMBOSLTABLE_H
#define	SYMBOSLTABLE_H

#include <iostream>
#include <unordered_map>
#include <algorithm>

#include "Def.h"
#include "Util.h"

using namespace std;

typedef struct Symbols_Table_Item {
    string Lexeme; // Lexema do Identificador
    int Address; // Endereço de memória para Geração de Código
    int TokenNumber; // Para Palavras Reservadas
    int Type; // Inteiro ou Real
    int Class; // DCor, DPonto, DLuz, DConst, DFace, DObjeto, DVar, VAZIO
} Symbols_Table_Item;

typedef unordered_map<string, Symbols_Table_Item> CustomHashTable;

class SymbolsTable {
private:
    CustomHashTable hashtable;
    CustomHashTable::hasher HashFunction = hashtable.hash_function();

    void Add(string lexeme, int t_number) {
        string temp_lexeme = toLowerCase(lexeme);

        Symbols_Table_Item temp_item;
        temp_item.Lexeme = temp_lexeme;
        temp_item.TokenNumber = t_number;
        temp_item.Class = VAZIO;
        temp_item.Type = VAZIO;

        hashtable.insert(make_pair(temp_lexeme, temp_item));
    }

    /* Inicia Tabela de Símbolos com Palavras Reservadas */
    void Init() {
        Add(T_POINT, 0);
        Add(T_COLOR, 1);
        Add(T_CONST, 2);
        Add(T_FACE, 3);
        Add(T_OBJECT, 4);
        Add(T_LIGHT, 5);
        Add(T_VAR, 6);
        Add(T_INTERGER, 7);
        Add(T_REAL, 8);
        Add(T_COMM_SCALE, 9);
        Add(T_COMM_PAUSE, 10);
        Add(T_COMM_ROTTRANS, 11);
        Add(T_COMM_LIGHT, 12);
        Add(T_WHILE, 13);
        Add(T_DO, 14);
        Add(T_IF, 15);
        Add(T_THEN, 16);
        Add(T_ELSE, 17);
        Add(T_BEGIN, 18);
        Add(T_END, 19);
        Add(T_COMMA, 20);
        Add(T_PARENS_L, 21);
        Add(T_PARENS_R, 22);
        Add(T_NEWLINE, 23);
        Add(T_LOGIC_AND, 24);
        Add(T_LOGIC_OR, 25);
        Add(T_LOGiC_NOT, 26);
        Add(T_LOGIC_EQUAL, 27);
        Add(T_LOGIC_LESS, 28);
        Add(T_LOGIC_GREATER, 29);
        Add(T_LOGIC_DIFF, 30);
        Add(T_LOGIC_LESS_EQUAL, 31);
        Add(T_LOGIC_GREATER_EQUAL, 32);
        Add(T_SUM, 33);
        Add(T_SUB, 34);
        Add(T_MUL, 35);
        Add(T_DIV, 36);
        Add(T_ASSIGN, 37);
    }
public:

    SymbolsTable() {
        Init();
    }

    /**
     * Insere Identificador
     * @param Lexeme Lexema do Identificador
     * @param Class Classe do Identificador
     * @param Type Tipo do Identificador
     * @return código gerado pela função Hash
     */
    int Insert(string Lexeme, int Class, int Type) {
        string temp_lexeme = toLowerCase(Lexeme);

        Symbols_Table_Item temp_item;
        temp_item.Lexeme = temp_lexeme;
        temp_item.TokenNumber = VAZIO;
        temp_item.Class = Class;
        temp_item.Type = Type;

        hashtable.insert(make_pair(temp_lexeme, temp_item));
        return HashFunction(Lexeme);
    }

    /**
     * Busca por Palavra Reservada
     * @param Lexeme Lexema do Identificador
     * @return Número do Token se encontrar, -1 caso contrário.
     */
    int Search(string Lexeme) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            return i->second.TokenNumber;
        }
        return -1;
    }

    int WasDeclared(string Lexeme) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            if ((i->second.TokenNumber != VAZIO) || (i->second.Class != VAZIO)) {
                return 1;
            }
        }
        return 0;
    }

    /**
     * Atualiza Classe e Tipo do Identificador
     * @param Lexeme Lexema do Identificador
     * @param Class  Nova Classe do Identificador
     * @param Type Novo Tipo do Identificador
     * @return 1 se existe item a ser atualizado, 0 caso contrário.
     */
    int Update(string Lexeme, int Class, int Type) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            i->second.Class = Class;
            i->second.Type = Type;
            return 1;
        }
        return 0;
    }

    /**
     * Atualiza Classe do Identificador
     * @param Lexeme Lexema do Identificador
     * @param Class  Nova Classe do Identificador
     * @return 1 se existe item a ser atualizado, 0 caso contrário.
     */
    int Update_Class(string Lexeme, int Class) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            if (i->second.Class == VAZIO) {
                i->second.Class = Class;
                return 1;
            }
        }
        return 0;
    }

    /**
     * Atualiza Tipo do Identificador
     * @param Lexeme Lexema do Identificador
     * @param Type Novo Tipo do Identificador
     * @return 1 se existe item a ser atualizado, 0 caso contrário.
     */
    int Update_Type(string Lexeme, int Type) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            i->second.Type = Type;
            return 1;
        }
        return 0;
    }

    /**
     * Atualiza Endereço do Identificador
     * @param Lexeme Lexema do Identificador
     * @param Address Novo Endereço do Identificador
     * @return 1 se existe item a ser atualizado, 0 caso contrário.
     */
    int Update_Address(string Lexeme, int Address) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            i->second.Address = Address;
            return 1;
        }
        return 0;
    }

    /**
     * Pega Classe do Identificador
     * @param Lexeme Lexema do Identificador
     * @return Código da Classe caso encontre, VAZIO (-2) caso contrário.
     */
    int GetClass(string Lexeme) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            return i->second.Class;
        }
        return VAZIO;
    }

    /**
     * Pega Tipo do Identificador
     * @param Lexeme Lexema do Identificador
     * @return Código do Tipo caso encontre, VAZIO (-2) caso contrário.
     */
    int GetType(string Lexeme) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            return i->second.Type;
        }
        return VAZIO;
    }

    /**
     * Pega Endereço do Identificador
     * @param Lexeme Lexema do Identificador
     * @return Endereço caso encontre, VAZIO (-2) caso contrário.
     */
    int GetAddress(string Lexeme) {
        CustomHashTable::iterator i = hashtable.find(toLowerCase(Lexeme));
        if (i != hashtable.end()) {
            return i->second.Address;
        }
        return VAZIO;
    }

    /**
     * [DEBUG] Imprime tabela da tela.
     */
    void PrintAll() {
        //        printf("-- Table Size: %lu \n", hashtable.size());
        for (CustomHashTable::iterator c = hashtable.begin(); c != hashtable.end(); c++) {
            if (c->second.TokenNumber == VAZIO)
                printf("  Key: %10.15s - Token Number: %.3d - Class: %6.6s - Type: %7.7s - Address: %.3d \n",
                    c->first.c_str(), c->second.TokenNumber, UTIL_PrintClass(c->second.Class), UTIL_PrintType(c->second.Type), c->second.Address);
        }
    }

};


#endif

