#ifndef PARSER_H
#define	PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "Def.h"
#include "SymbolsTable.h"
#include "Lexer.h"
#include "CodeGen.h"

using namespace std;

/* Lets compile that! */
class Parser {
private:
    Register LastReg;
    Register LexReg;
    SymbolsTable STable;
    Lexer *Tokenizer;
    CodeGen Generator;
    string Ref;

    void Error_Syntatic() {
        if (Ref == "eof") PrintError(Tokenizer->GetLine(), "Fim de arquivo nao esperado");
        else PrintError(Tokenizer->GetLine(), "Token nao esperado", LexReg.lexeme);
    }

    /**
     * Pega próximo Token, armazena no Registro Léxico e retorna tipo.
     * @return ID, const, eof
     */
    string GetNext() {
        int HasToken = 0;
        HasToken = Tokenizer->NextToken(&LexReg);
        if (HasToken != -1 && HasToken != 0) {
            if (LexReg.token_number != VAZIO) { // Palavra Reservada
                return LexReg.lexeme;
            } else if (LexReg.address != VAZIO) { // Identificador
                return TOKEN_TYPE_ID;
            } else if (LexReg.interger_part.size() > 0) { // Constante 
                return TOKEN_TYPE_CONST;
            }
        }
        return "eof";
    }

    void MatchToken(string token) {
        if (token != Ref) {
            Error_Syntatic();
        }
        LastReg = LexReg;
        Ref = GetNext();
    }

    void MultEnters() {
        while (Ref == T_NEWLINE)
            MatchToken(T_NEWLINE);
    }

    int EndOfFile() {
        if (Ref == "eof") return 0;
        else if (Ref == T_NEWLINE) {
            Ref = GetNext();
            EndOfFile();
        } else PrintError(Tokenizer->GetLine(), "Fim de arquivo esperado");
        return 0;
    }

    /* Funções para análise semântica */

    /**
     * Atribui classe a identificador na tabela de símbolos.
     * Erro caso já tenha classe.
     * @param ID_Class Código da Classe
     */
    void Semantic_AddClass(int ID_Class) {
        if (!STable.Update_Class(LastReg.lexeme, ID_Class))
            PrintError(Tokenizer->GetLine(), "identificador ja declarado", LastReg.lexeme);
    }

    /**
     * Verifica se identificador já foi declarado e é da classe esperada.
     * @param Class Código da Classe
     */
    void Semantic_VerifyClass(int Class) {
        Semantic_VerifyDeclaration();
        if (STable.GetClass(LastReg.lexeme) != Class)
            PrintError(Tokenizer->GetLine(), "classe de identificador incompativel", LastReg.lexeme);
    }

    /**
     * Verifica se identificador já foi declarado.
     */
    void Semantic_VerifyDeclaration() {
        if (STable.WasDeclared(LastReg.lexeme) == 0)
            PrintError(Tokenizer->GetLine(), "identificador nao declarado", LastReg.lexeme);
    }

    /**
     * Verifica se inteiro está entre 0 e 63.
     */
    void Semantic_IntergerRange() {
        int interger_part = atoi(LastReg.interger_part.c_str());
        int decimal_part = atoi(LastReg.decimal_part.c_str());
        if ((decimal_part > 0) || (interger_part < 0 || interger_part > 63))
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
    }

    int Semantic_AddTypeConst(string Lexeme) {
        int type_const = 0;
        if (LastReg.decimal_part.size() > 0)
            type_const = TIPO_REAL;
        else if (LastReg.interger_part.size() > 0)
            type_const = TIPO_INTEIRO;
        if (!STable.Update_Type(Lexeme, type_const))
            DEBUG_Printf("Ops!!\n");
        return type_const;
    }

    void Semantic_AddTypeVar(string type) {
        int temp_TypeVar = 0;
        if (type == "real")
            temp_TypeVar = TIPO_REAL;
        else if (type == "inteiro")
            temp_TypeVar = TIPO_INTEIRO;
        if (!STable.Update_Type(LastReg.lexeme, temp_TypeVar))
            DEBUG_Printf("Ops!!\n");
    }

public:

    Parser(std::string source_file, int f_optimize = 0, int f_debug = 0) {
        Tokenizer = new Lexer(source_file, &STable);
        Generator.SetArgs(source_file.substr(0, source_file.size() - 3), f_debug, f_optimize);
    }

    void Start() {
        Ref = GetNext();
        Generator.Comment("-------- DECLARACOES --------");
        DECLARACAO();
        Generator.Comment("");
        Generator.Comment("----- CORPO DO PROGRAMA -----");
        B_COMANDOS();
        EndOfFile();
        Generator.HLT();
        Generator.FlushCode();
    }

    void DECLARACAO() {
        while (Ref != T_BEGIN) {
            if (Ref == T_POINT) {
                Generator.Comment(Ref);
                X_DPONTO();
            } else if (Ref == T_LIGHT) {
                Generator.Comment(Ref);
                X_DLUZ();
            } else if (Ref == T_COLOR) {
                Generator.Comment(Ref);
                X_DCOR();
            } else if (Ref == T_FACE) {
                Generator.Comment(Ref);
                X_DFACE();
            } else if (Ref == T_OBJECT) {
                Generator.Comment(Ref);
                X_DOBJETO();
            } else if (Ref == T_CONST) {
                Generator.Comment(Ref);
                X_DCONST();
            } else if (Ref == T_VAR) {
                Generator.Comment(Ref);
                X_DVAR();
            } else if (Ref == T_NEWLINE) {
                MultEnters();
            } else {
                Error_Syntatic();
            }
        }
        Generator.SetTempBase();
    }

    void X_DPONTO() {
        MatchToken(T_POINT);
        MatchToken(T_NEWLINE);
        MultEnters();
        Generator.CommentI(LexReg.lexeme);
        D_3CONST(CLASSE_PONTO);
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == TOKEN_TYPE_ID) {
            Generator.CommentI(LexReg.lexeme);
            D_3CONST(CLASSE_PONTO);
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void X_DLUZ() {
        MatchToken(T_LIGHT);
        MatchToken(T_NEWLINE);
        MultEnters();
        Generator.CommentI(LexReg.lexeme);
        D_3CONST(CLASSE_LUZ);
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == TOKEN_TYPE_ID) {
            Generator.CommentI(LexReg.lexeme);
            D_3CONST(CLASSE_LUZ);
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void D_3CONST(int ID_Class) {
        int ID_Addr;
        int Neg = 0;
        MatchToken(TOKEN_TYPE_ID);
        Semantic_AddClass(ID_Class);
        ID_Addr = Generator.NewData();
        STable.Update_Address(LastReg.lexeme, ID_Addr);
        MatchToken(T_ASSIGN);
        MatchToken(T_PARENS_L);
        for (int i = 0; i < 3; i++) {
            if (Ref == T_SUB) {
                MatchToken(Ref);
                Neg = 1;
            }
            MatchToken(TOKEN_TYPE_CONST);
            if (Neg) {
                Generator.STIF("#-" + LastReg.lexeme, Generator.NewTempF());
                Generator.CommentINLINE("-" + LastReg.lexeme);
                Neg = 0;
            } else {
                Generator.STIF("#" + LastReg.lexeme, Generator.NewTempF());
                Generator.CommentINLINE(LastReg.lexeme);
            }
            if (i < 2)
                MatchToken(T_COMMA);
        }
        MatchToken(T_PARENS_R);
    }

    void X_DCOR() {
        MatchToken(T_COLOR);
        MatchToken(T_NEWLINE);
        MultEnters();
        Generator.CommentI(LexReg.lexeme);
        D_DCOR();
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == TOKEN_TYPE_ID) {
            Generator.CommentI(LexReg.lexeme);
            D_DCOR();
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void D_DCOR() {
        int Addr;
        MatchToken(TOKEN_TYPE_ID);
        Semantic_AddClass(CLASSE_COR);
        Addr = Generator.NewData();
        STable.Update_Address(LastReg.lexeme, Addr);
        MatchToken(T_ASSIGN);
        MatchToken(TOKEN_TYPE_CONST);
        if (LastReg.data_type != TIPO_INTEIRO)
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        Generator.STI("#" + LastReg.lexeme, Generator.NewTemp());
        MatchToken(T_COMMA);
        for (int i = 0; i < 6; i++) {
            MatchToken(TOKEN_TYPE_CONST);
            Semantic_IntergerRange();
            Generator.STI("#" + LastReg.lexeme, Generator.NewTemp());
            if (i < 5)
                MatchToken(T_COMMA);
        }
    }

    void X_DFACE() {
        MatchToken(T_FACE);
        MatchToken(T_NEWLINE);
        MultEnters();
        Generator.CommentI(LexReg.lexeme);
        D_DFACE();
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == TOKEN_TYPE_ID) {
            Generator.CommentI(LexReg.lexeme);
            D_DFACE();
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void D_DFACE() {
        int Addr;
        int num_points = 3;

        MatchToken(TOKEN_TYPE_ID);
        Semantic_AddClass(CLASSE_FACE);
        Addr = Generator.NewTemp();
        STable.Update_Address(LastReg.lexeme, Addr);

        MatchToken("=");
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_COR);
        Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
        Generator.CommentINLINE("End de " + LastReg.lexeme);
        MatchToken(T_COMMA);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_PONTO);
        Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
        Generator.CommentINLINE("End de " + LastReg.lexeme);
        MatchToken(T_COMMA);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_PONTO);
        Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
        Generator.CommentINLINE("End de " + LastReg.lexeme);
        MatchToken(T_COMMA);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_PONTO);
        Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
        Generator.CommentINLINE("End de " + LastReg.lexeme);
        while (Ref == T_COMMA) {
            MatchToken(T_COMMA);
            MatchToken(TOKEN_TYPE_ID);
            Semantic_VerifyClass(CLASSE_PONTO);
            num_points++;
            Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
            Generator.CommentINLINE("End de " + LastReg.lexeme);
        }
        Generator.STI(num_points, Addr);
        Generator.CommentINLINE("Num de Pontos");

    }

    void X_DOBJETO() {
        MatchToken(T_OBJECT);
        MatchToken(T_NEWLINE);
        MultEnters();
        D_DOBJETO();
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == TOKEN_TYPE_ID) {
            D_DOBJETO();
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void D_DOBJETO() {
        int Addr;
        int num_faces = 1;

        MatchToken(TOKEN_TYPE_ID);
        Generator.CommentI(LastReg.lexeme);
        Semantic_AddClass(CLASSE_OBJETO);
        Addr = Generator.NewTemp();
        STable.Update_Address(LastReg.lexeme, Addr);
        Generator.STIF("#1.0000", Generator.NewTempF());
        MatchToken(T_ASSIGN);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_FACE);
        Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
        while (Ref == T_COMMA) {
            MatchToken(T_COMMA);
            MatchToken(TOKEN_TYPE_ID);
            Semantic_VerifyClass(CLASSE_FACE);
            Generator.STI(STable.GetAddress(LastReg.lexeme), Generator.NewTemp());
            num_faces++;
        }
        Generator.STI(num_faces, Addr);
    }

    void X_DVAR() {
        MatchToken(T_VAR);
        MatchToken(T_NEWLINE);
        D_DVAR();
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == T_INTERGER || Ref == T_REAL) {
            D_DVAR();
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void D_DVAR() {
        string var_type;
        if (Ref == T_INTERGER || Ref == T_REAL) {
            var_type = Ref;
            MatchToken(Ref);
            Generator.Comment(var_type + " " + LexReg.lexeme + " - End: " + patch::to_string(Generator.NewData()));
            MatchToken(TOKEN_TYPE_ID);
            Semantic_AddTypeVar(var_type);
            Semantic_AddClass(CLASSE_VAR);
            if (var_type == T_INTERGER)
                STable.Update_Address(LastReg.lexeme, Generator.NewTemp());
            else
                STable.Update_Address(LastReg.lexeme, Generator.NewTempF());
            while (Ref == T_COMMA) {
                MatchToken(T_COMMA);
                Generator.Comment(var_type + " " + LexReg.lexeme + " - End: " + patch::to_string(Generator.NewData()));
                MatchToken(TOKEN_TYPE_ID);
                Semantic_AddTypeVar(var_type);
                Semantic_AddClass(CLASSE_VAR);
                if (var_type == T_INTERGER)
                    STable.Update_Address(LastReg.lexeme, Generator.NewTemp());
                else
                    STable.Update_Address(LastReg.lexeme, Generator.NewTempF());
            }
        } else {
            Error_Syntatic();
        }
    }

    void X_DCONST() {
        MatchToken(T_CONST);
        MatchToken(T_NEWLINE);
        MultEnters();
        D_DCONST();
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == TOKEN_TYPE_ID) {
            D_DCONST();
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void D_DCONST() {
        string ID_Lex;
        int neg = 0;

        MatchToken(TOKEN_TYPE_ID);
        Semantic_AddClass(CLASSE_CONST);
        STable.Update_Address(LastReg.lexeme, Generator.NewData());
        ID_Lex = LastReg.lexeme;
        MatchToken(T_ASSIGN);
        if (Ref == T_SUB) {
            MatchToken(Ref);
            neg = 1;
        }
        MatchToken(TOKEN_TYPE_CONST);
        if (neg == 1) {
            if (Semantic_AddTypeConst(ID_Lex) == TIPO_INTEIRO)
                Generator.STI("#-" + LastReg.lexeme, Generator.NewTemp());
            else
                Generator.STIF("#-" + LastReg.lexeme, Generator.NewTempF());
            neg = 0;
        } else {
            if (Semantic_AddTypeConst(ID_Lex) == TIPO_INTEIRO)
                Generator.STI("#" + LastReg.lexeme, Generator.NewTemp());
            else
                Generator.STIF("#" + LastReg.lexeme, Generator.NewTempF());
        }
        Generator.CommentINLINE(ID_Lex);
    }

    void B_COMANDOS(string caller = "") {
        MatchToken(T_BEGIN);
        MatchToken(T_NEWLINE);
        MultEnters();
        while (Ref == T_WHILE || Ref == T_IF || Ref == T_COMM_SCALE || Ref == T_COMM_PAUSE || Ref == T_COMM_LIGHT || Ref == T_COMM_ROTTRANS || Ref == TOKEN_TYPE_ID || Ref == T_BEGIN) {
            if (Ref == T_BEGIN) {
                B_COMANDOS(T_BEGIN);
            } else {
                COMANDO();
                if (caller == T_THEN) {
                    MultEnters();
                } else if (caller != T_BEGIN) {
                    MatchToken(T_NEWLINE);
                    MultEnters();
                }
            }
        }
        //        MultEnters();
        MatchToken(T_END);
        if (caller == T_THEN) {
            MultEnters();
        } else if (caller != T_IF && caller != T_WHILE) {
            MatchToken(T_NEWLINE);
            MultEnters();
        }
    }

    void COMANDO() {
        Generator.Comment("");
        if (Ref == T_WHILE) {
            Generator.Comment("Comando: " + string(T_WHILE) + " - inicio");
            REPETICAO();
            Generator.Comment("Comando: " + string(T_WHILE) + " - fim");
        } else if (Ref == T_IF) {
            Generator.Comment("Comando: " + string(T_IF) + " - inicio");
            SELECAO();
            Generator.Comment("Comando: " + string(T_IF) + " - fim");
        } else if (Ref == T_COMM_SCALE) {
            Generator.Comment("Comando: " + string(T_COMM_SCALE) + " - inicio");
            ESCALA();
            Generator.Comment("Comando: " + string(T_COMM_SCALE) + " - fim");
        } else if (Ref == T_COMM_PAUSE) {
            Generator.Comment("Comando: " + string(T_COMM_PAUSE) + " - inicio");
            PAUSE();
            Generator.Comment("Comando: " + string(T_COMM_PAUSE) + " - fim");
        } else if (Ref == T_COMM_LIGHT) {
            Generator.Comment("Comando: " + string(T_COMM_LIGHT) + " - inicio");
            C_LUZ();
            Generator.Comment("Comando: " + string(T_COMM_LIGHT) + " - fim");
        } else if (Ref == T_COMM_ROTTRANS) {
            Generator.Comment("Comando: " + string(T_COMM_ROTTRANS) + " - inicio");
            ROTACAO();
            Generator.Comment("Comando: " + string(T_COMM_ROTTRANS) + " - fim");
        } else if (Ref == TOKEN_TYPE_ID) {
            Generator.Comment("Comando: Atribuicao - inicio");
            ATRIBUICAO();
            Generator.Comment("Comando: Atribuicao - fim");
        }
    }

    void REPETICAO() {
        MatchToken(T_WHILE);
        int EXP_Type, EXP_Addr;
        int Rot_Inicio_BIN;
        int Rot_Fim_BIN;
        int Rot_Inicio = Generator.NewLabel();
        int Rot_Fim = Generator.NewLabel();
        Generator.Label(Rot_Inicio);
        Generator.CommentINLINE("Rot. inicio: " + string(T_WHILE));
        Rot_Inicio_BIN = Generator.GetBinCounter();
        Generator.CommentI("EXP do " + string(T_WHILE) + " - inicio");
        EXP(EXP_Type, EXP_Addr);
        Generator.CommentI("EXP do " + string(T_WHILE) + " - fim");
        if (EXP_Type != TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        }

        Generator.LOD("A", EXP_Addr);
        Generator.CommentINLINE("REG <- Valor de EXP");
        Generator.BZR("A", Rot_Fim);
        Generator.CommentINLINE("se REG falso (0) salta");
        Rot_Fim_BIN = Generator.GetBinCounter();
        Generator.ResetTemp();

        MatchToken(T_DO);
        if (Ref != T_NEWLINE) {
            if (Ref == T_BEGIN) B_COMANDOS(T_WHILE);
            else COMANDO();
            Generator.JMP(Rot_Inicio);
            Generator.CommentINLINE("Rot. inicio: " + string(T_WHILE));
            Generator.UpdateBIN_Branch(Generator.GetBinCounter(), Rot_Inicio_BIN);
            Generator.Label(Rot_Fim);
            Generator.CommentINLINE("Rot. fim: " + string(T_WHILE));
            Generator.UpdateBIN_Branch(Rot_Fim_BIN, Generator.GetBinCounter());
        } else Error_Syntatic();

    }

    void SELECAO() {
        int EXP_Type, EXP_Addr;
        int RotFalso_Bin, RotFim_Bin;
        int RotFalso, RotFim;
        MatchToken(T_IF);
        if (Ref == T_NEWLINE) {
            Error_Syntatic();
        }
        Generator.CommentI("Exp SE - inicio");
        EXP(EXP_Type, EXP_Addr);
        if (EXP_Type != TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        }
        Generator.CommentI("Exp SE - fim");
        RotFalso = Generator.NewLabel();

        Generator.LOD("A", EXP_Addr);
        Generator.BZR("A", RotFalso);
        RotFalso_Bin = Generator.GetBinCounter();

        MatchToken(T_THEN);
        if (Ref == T_BEGIN) {
            B_COMANDOS();
            //            B_COMANDOS(TOKEN_ENTAO); //aceitar FIM SE e FIM SENAO
        } else COMANDO();

        if (Ref == T_ELSE) {
            RotFim = Generator.NewLabel();
            Generator.JMP(RotFim);
            Generator.CommentINLINE("Salta para RotFim do SE");
            RotFim_Bin = Generator.GetBinCounter();

            Generator.Label(RotFalso);
            Generator.CommentINLINE("RotFalso de SE");
            Generator.UpdateBIN_Branch(RotFalso_Bin, Generator.GetBinCounter());

            MatchToken(T_ELSE);
            if (Ref == T_BEGIN) {
                B_COMANDOS(T_IF);
                //                B_COMANDOS(TOKEN_ENTAO); //aceitar FIM SE e FIM SENAO
            } else COMANDO();

            Generator.Label(RotFim);
            Generator.CommentINLINE("RotFim de SE");
            Generator.UpdateBIN_Branch(RotFim_Bin, Generator.GetBinCounter());
        } else {
            Generator.Label(RotFalso);
            Generator.CommentINLINE("RotFalso de SE");
            Generator.UpdateBIN_Branch(RotFalso_Bin, Generator.GetBinCounter());
        }
        Generator.ResetTemp();
    }

    void ATRIBUICAO() {
        int EXP_Type, EXP_Addr, ID_Type, ID_Addr;
        string ID_Lex;
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyDeclaration();
        ID_Type = STable.GetType(LastReg.lexeme);
        ID_Addr = STable.GetAddress(LastReg.lexeme);
        ID_Lex = LastReg.lexeme;
        MatchToken(T_ASSIGN);
        EXP(EXP_Type, EXP_Addr);
        Generator.ResetTemp();
        if ((EXP_Type == TIPO_LOGICO) || (EXP_Type != ID_Type)) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type == TIPO_INTEIRO) {
            Generator.LOD("A", EXP_Addr);
            Generator.STO("A", ID_Addr);
            Generator.CommentINLINE("var " + ID_Lex + " -> REG");
        } else if (EXP_Type == TIPO_REAL) {
            Generator.LODF("A", EXP_Addr);
            Generator.STOF("A", ID_Addr);
            Generator.CommentINLINE("var " + ID_Lex + " -> REG");
        }
    }

    void PAUSE() {
        int EXP_Type, EXP_Addr;
        MatchToken(T_COMM_PAUSE);
        EXP(EXP_Type, EXP_Addr);
        if (EXP_Type == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type == TIPO_REAL) {
            Generator.LODF("A", EXP_Addr);
        } else if (EXP_Type == TIPO_INTEIRO) {
            Generator.LOD("A", EXP_Addr);
            Generator.CNV("A", "A");
        }
        Generator.TME("A");
    }

    void C_LUZ() {
        MatchToken(T_COMM_LIGHT);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_LUZ);
        Generator.LDI("A", STable.GetAddress(LastReg.lexeme));
        Generator.CommentINLINE("REG <- End de " + LastReg.lexeme);
        Generator.LGT("A");
        Generator.CommentINLINE("LuzCor <- REG ");
    }

    void ROTACAO() {
        string ID_Lex;
        int ID_Addr;
        int EXP_Type1, EXP_Addr1;
        int EXP_Type2, EXP_Addr2;
        int EXP_Type3, EXP_Addr3;
        int EXP_Type4, EXP_Addr4;
        int EXP_Type5, EXP_Addr5;
        int EXP_Type6, EXP_Addr6;
        MatchToken(T_COMM_ROTTRANS);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_OBJETO);
        ID_Addr = STable.GetAddress(LastReg.lexeme);
        ID_Lex = LastReg.lexeme;
        MatchToken(T_COMMA);
        Generator.CommentI("EXP1 - inicio");
        EXP(EXP_Type1, EXP_Addr1);
        Generator.CommentI("--- EXP - End. do Resultado: " + patch::to_string(EXP_Addr1));
        Generator.CommentI("EXP1 - fim");
        MatchToken(T_COMMA);
        Generator.CommentI("EXP2 - inicio");
        EXP(EXP_Type2, EXP_Addr2);
        Generator.CommentI("--- EXP - End. do Resultado: " + patch::to_string(EXP_Addr2));
        Generator.CommentI("EXP2 - fim");
        MatchToken(T_COMMA);
        Generator.CommentI("EXP3 - inicio");
        EXP(EXP_Type3, EXP_Addr3);
        Generator.CommentI("--- EXP - End. do Resultado: " + patch::to_string(EXP_Addr3));
        Generator.CommentI("EXP3 - fim");
        MatchToken(T_COMMA);
        Generator.CommentI("EXP4 - inicio");
        EXP(EXP_Type4, EXP_Addr4);
        Generator.CommentI("--- EXP - End. do Resultado: " + patch::to_string(EXP_Addr4));
        Generator.CommentI("EXP4 - fim");
        MatchToken(T_COMMA);
        Generator.CommentI("EXP5 - inicio");
        EXP(EXP_Type5, EXP_Addr5);
        Generator.CommentI("--- EXP - End. do Resultado: " + patch::to_string(EXP_Addr5));
        Generator.CommentI("EXP5 - fim");
        MatchToken(T_COMMA);
        Generator.CommentI("EXP6 - inicio");
        EXP(EXP_Type6, EXP_Addr6);
        Generator.CommentI("--- EXP - End. do Resultado: " + patch::to_string(EXP_Addr6));
        Generator.CommentI("EXP6 - fim");

        if (EXP_Type1 == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type1 == TIPO_INTEIRO) {
            Generator.LOD("A", EXP_Addr1);
            Generator.CommentINLINE("REG <- Valor de Exp1");
            Generator.CNV("A", "A");
            Generator.CommentINLINE("Converte para Real");
        } else {
            Generator.LODF("A", EXP_Addr1);
            Generator.CommentINLINE("REG <- Valor de Exp1");
        }
        if (EXP_Type2 == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type2 == TIPO_INTEIRO) {
            Generator.LOD("B", EXP_Addr2);
            Generator.CommentINLINE("REG <- Valor de Exp2");
            Generator.CNV("B", "B");
            Generator.CommentINLINE("Converte para Real");
        } else {
            Generator.LODF("B", EXP_Addr2);
            Generator.CommentINLINE("REG <- Valor de Exp2");
        }
        if (EXP_Type3 == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type3 == TIPO_INTEIRO) {
            Generator.LOD("C", EXP_Addr3);
            Generator.CommentINLINE("REG <- Valor de Exp3");
            Generator.CNV("C", "C");
            Generator.CommentINLINE("Converte para Real");
        } else {
            Generator.LODF("C", EXP_Addr3);
            Generator.CommentINLINE("REG <- Valor de Exp3");
        }
        if (EXP_Type4 == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type4 == TIPO_INTEIRO) {
            Generator.LOD("D", EXP_Addr4);
            Generator.CommentINLINE("REG <- Valor de Exp4");
            Generator.CNV("D", "D");
            Generator.CommentINLINE("Converte para Real");
        } else {
            Generator.LODF("D", EXP_Addr4);
            Generator.CommentINLINE("REG <- Valor de Exp4");
        }
        if (EXP_Type5 == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type5 == TIPO_INTEIRO) {
            Generator.LOD("E", EXP_Addr5);
            Generator.CommentINLINE("REG <- Valor de Exp5");
            Generator.CNV("E", "E");
            Generator.CommentINLINE("Converte para Real");
        } else {
            Generator.LODF("E", EXP_Addr5);
            Generator.CommentINLINE("REG <- Valor de Exp5");
        }
        if (EXP_Type6 == TIPO_LOGICO) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        } else if (EXP_Type6 == TIPO_INTEIRO) {
            Generator.LOD("F", EXP_Addr6);
            Generator.CommentINLINE("REG <- Valor de Exp6");
            Generator.CNV("F", "F");
            Generator.CommentINLINE("Converte para Real");
        } else {
            Generator.LODF("F", EXP_Addr6);
            Generator.CommentINLINE("REG <- Valor de Exp6");
        }
        Generator.LDI("A", ID_Addr);
        Generator.CommentINLINE("REG <- End. de ID:" + ID_Lex);
        Generator.RTR();
        Generator.ResetTemp();
    }

    void ESCALA() {
        int EXP_Type, EXP_Addr, ID_Addr;
        MatchToken(T_COMM_SCALE);
        MatchToken(TOKEN_TYPE_ID);
        Semantic_VerifyClass(CLASSE_OBJETO);
        ID_Addr = STable.GetAddress(LastReg.lexeme);
        MatchToken(T_COMMA);
        EXP(EXP_Type, EXP_Addr);
        if (EXP_Type == TIPO_REAL) {
            Generator.LODF("A", EXP_Addr);
        } else if (EXP_Type == TIPO_INTEIRO) {
            Generator.LOD("A", EXP_Addr);
            Generator.CNV("A", "A");
        } else {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        }
        Generator.LDI("A", ID_Addr);
        Generator.ESC("A", "A");
    }

    /**
     * Compara se diferente, menor, maior, igual, maior-igual ou menor-igual
     * @param Type Referência para armazenar tipo.
     * @param Addr Referência para armazenar endereço.
     */
    void EXP(int &Type, int &Addr) {
        //        DEBUG_Printf("EXP\n");
        int op;
        EXP_A(Type, Addr);
        while (Ref == T_LOGIC_DIFF || Ref == T_LOGIC_LESS || Ref == T_LOGIC_GREATER || Ref == T_LOGIC_EQUAL || Ref == T_LOGIC_GREATER_EQUAL || Ref == T_LOGIC_LESS_EQUAL) {
            if (Ref == T_LOGIC_DIFF)
                op = OP_DIFF;
            else if (Ref == T_LOGIC_LESS)
                op = OP_LESS;
            else if (Ref == T_LOGIC_GREATER)
                op = OP_GREATER;
            else if (Ref == T_LOGIC_EQUAL)
                op = OP_EQUAL;
            else if (Ref == T_LOGIC_GREATER_EQUAL)
                op = OP_GEQUAL;
            else if (Ref == T_LOGIC_LESS_EQUAL)
                op = OP_LEQUAL;
            MatchToken(Ref);
            int EXP_A_Type, Exp_A_Addr;
            EXP_A(EXP_A_Type, Exp_A_Addr);
            if (Type == TIPO_LOGICO && EXP_A_Type == TIPO_LOGICO) {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            } else if ((Type == TIPO_REAL || EXP_A_Type == TIPO_REAL) && op) {
                int Rot_True_Bin;
                int Rot_Fim_Bin;
                int Rot_True = Generator.NewLabel();
                int Rot_Fim = Generator.NewLabel();
                if (Type == TIPO_REAL) {
                    Generator.LODF("A", Addr);
                } else {
                    Generator.LOD("A", Addr);
                    Generator.CNV("A", "A");
                }
                if (EXP_A_Type == TIPO_REAL) {
                    Generator.LODF("B", Exp_A_Addr);
                } else {
                    Generator.LOD("B", Exp_A_Addr);
                    Generator.CNV("B", "B");
                }
                Generator.SUBF("A", "B");
                Addr = Generator.NewTemp();
                if (op == OP_LESS) {
                    Generator.BNGF("A", Rot_True);
                } else if (op == OP_GREATER) {
                    Generator.BPSF("A", Rot_True);
                } else if (op == OP_EQUAL) {
                    Generator.BZRF("A", Rot_True);
                } else if (op == OP_DIFF) {
                    Generator.BNZF("A", Rot_True);
                } else if (op == OP_LEQUAL) {
                    Generator.BNPF("A", Rot_True);
                } else if (op == OP_GEQUAL) {
                    Generator.BNNF("A", Rot_True);
                }
                Rot_True_Bin = Generator.GetBinCounter();
                Generator.LDI("A", 0);
                Generator.JMP(Rot_Fim);
                Rot_Fim_Bin = Generator.GetBinCounter();
                Generator.Label(Rot_True);
                Generator.UpdateBIN_Branch(Rot_True_Bin, Generator.GetBinCounter());
                Generator.LDI("A", 1);
                Generator.Label(Rot_Fim);
                Generator.UpdateBIN_Branch(Rot_Fim_Bin, Generator.GetBinCounter());
                Generator.STO("A", Addr);
                Type = TIPO_LOGICO;

            } else if ((Type == TIPO_INTEIRO && EXP_A_Type == TIPO_INTEIRO) && op) {
                int Rot_True_Bin;
                int Rot_Fim_Bin;
                int Rot_True = Generator.NewLabel();
                int Rot_Fim = Generator.NewLabel();

                Generator.LOD("A", Addr);
                Generator.LOD("B", Exp_A_Addr);
                Generator.SUB("A", "B");
                Addr = Generator.NewTemp();
                if (op == OP_LESS) {
                    Generator.BNG("A", Rot_True);
                } else if (op == OP_GREATER) {
                    Generator.BPS("A", Rot_True);
                } else if (op == OP_EQUAL) {
                    Generator.BZR("A", Rot_True);
                } else if (op == OP_DIFF) {
                    Generator.BNZ("A", Rot_True);
                } else if (op == OP_LEQUAL) {
                    Generator.BNP("A", Rot_True);
                } else if (op == OP_GEQUAL) {
                    Generator.BNN("A", Rot_True);
                }
                Rot_True_Bin = Generator.GetBinCounter();
                Generator.LDI("A", 0);
                Generator.JMP(Rot_Fim);
                Rot_Fim_Bin = Generator.GetBinCounter();
                Generator.Label(Rot_True);
                Generator.UpdateBIN_Branch(Rot_True_Bin, Generator.GetBinCounter());
                Generator.LDI("A", 1);
                Generator.Label(Rot_Fim);
                Generator.UpdateBIN_Branch(Rot_Fim_Bin, Generator.GetBinCounter());
                Generator.STO("A", Addr);
                Type = TIPO_LOGICO;
            }
        }
        //        DEBUG_Printf("EXP-> T:%d - A:%d\n", Type, Addr);
    }

    /**
     * Soma, Subtrai ou Compara (OU).
     * @param Type Referência para armazenar tipo.
     * @param Addr Referência para armazenar endereço.
     */
    void EXP_A(int &Type, int &Addr) {
        int neg = 0, op;
        if (Ref == T_SUB) {
            MatchToken(Ref);
            neg = 1;
        } else if (Ref == T_SUM) {
            MatchToken(Ref);
        }
        EXP_B(Type, Addr);
        //        DEBUG_Printf("EXP_1-> T:%d - A:%d\n", Type, Addr);
        if (Type != TIPO_LOGICO && neg == 1) {
            if (Type == TIPO_INTEIRO) {
                Generator.LOD("A", Addr);
                Addr = Generator.NewTemp();
                Generator.NEG("A");
                Generator.STO("A", Addr);
            } else if (Type == TIPO_REAL) {
                Generator.LODF("A", Addr);
                Addr = Generator.NewTempF();
                Generator.NEGF("A");
                Generator.STOF("A", Addr);
            }
            neg = 0;
        } else if (Type == TIPO_LOGICO && neg == 1) {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            neg = 0;
        }
        //        DEBUG_Printf("EXP_2-> T:%d - A:%d\n", Type, Addr);
        while (Ref == T_SUM || Ref == T_SUB || Ref == T_LOGIC_OR) {
            if (Ref == T_SUM && Type != TIPO_LOGICO) {
                op = OP_ADD;
            } else if (Ref == T_SUB && Type != TIPO_LOGICO) {
                op = OP_SUB;
            } else if (Ref == T_LOGIC_OR && Type == TIPO_LOGICO) {
                op = OP_OR;
            } else {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            }
            MatchToken(Ref);
            int EXP_B_Type, Exp_B_Addr;
            EXP_B(EXP_B_Type, Exp_B_Addr);
            if (Type == TIPO_LOGICO && EXP_B_Type != TIPO_LOGICO) {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            } else if (Type != TIPO_LOGICO && EXP_B_Type == TIPO_LOGICO) {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            } else if (Type == TIPO_LOGICO && EXP_B_Type == TIPO_LOGICO) {
                if (op != OP_OR) {
                    PrintError(Tokenizer->GetLine(), "tipos incompativeis");
                }
                int Rot_Dif_Bin;
                int Rot_Fim_Bin;
                int Rot_Dif = Generator.NewLabel();
                int Rot_Fim = Generator.NewLabel();
                Generator.LOD("A", Addr);
                Generator.LOD("B", Exp_B_Addr);
                Addr = Generator.NewTemp();
                Generator.ADD("A", "B");
                Generator.BZR("A", Rot_Dif);
                Rot_Dif_Bin = Generator.GetBinCounter();
                Generator.STI(1, Addr);
                Generator.JMP(Rot_Fim);
                Rot_Fim_Bin = Generator.GetBinCounter();
                Generator.Label(Rot_Dif);
                Generator.UpdateBIN_Branch(Rot_Dif_Bin, Generator.GetBinCounter());
                Generator.STI(0, Addr);
                Generator.Label(Rot_Fim);
                Generator.UpdateBIN_Branch(Rot_Fim_Bin, Generator.GetBinCounter());

            } else if (Type == TIPO_REAL || EXP_B_Type == TIPO_REAL) {

                if (Type == TIPO_REAL && EXP_B_Type == TIPO_INTEIRO) {
                    Generator.LODF("A", Addr);
                    Generator.LOD("B", Exp_B_Addr);
                    Generator.CNV("B", "B");
                } else if (Type == TIPO_INTEIRO && EXP_B_Type == TIPO_REAL) {
                    Generator.LOD("A", Addr);
                    Generator.LODF("B", Exp_B_Addr);
                    Generator.CNV("A", "A");
                } else {
                    Generator.LODF("A", Addr);
                    Generator.LODF("B", Exp_B_Addr);
                }
                if (op == OP_ADD) {
                    Generator.ADDF("A", "B");
                } else if (op == OP_SUB) {
                    Generator.SUBF("A", "B");
                } else {
                    PrintError(Tokenizer->GetLine(), "tipos incompativeis");
                }
                Addr = Generator.NewTempF();
                Generator.STOF("A", Addr);
                Type = TIPO_REAL;
            } else if (Type == TIPO_INTEIRO && EXP_B_Type == TIPO_INTEIRO) {
                Generator.LOD("A", Addr);
                Generator.LOD("B", Exp_B_Addr);
                if (op == OP_ADD) {
                    Generator.ADD("A", "B");
                } else if (op == OP_SUB) {
                    Generator.SUB("A", "B");
                } else {

                    PrintError(Tokenizer->GetLine(), "tipos incompativeis");
                }
                Addr = Generator.NewTemp();
                Generator.STO("A", Addr);
            }
        }
        //        DEBUG_Printf("EXP_A-> T:%d - A:%d\n", Type, Addr);
    }

    /**
     * Multiplica, Divide ou Compara (E) .
     * @param Type Referência para armazenar tipo.
     * @param Addr Referência para armazenar endereço.
     */
    void EXP_B(int &Type, int &Addr) {
        int op;
        EXP_C(Type, Addr);
        while (Ref == T_LOGIC_AND || Ref == T_MUL || Ref == T_DIV) {
            if (Ref == T_MUL && Type != TIPO_LOGICO) {
                op = OP_MUL;
            } else if (Ref == T_DIV && Type != TIPO_LOGICO) {
                op = OP_DIV;
            } else if (Ref == T_LOGIC_AND && Type == TIPO_LOGICO) {
                op = OP_AND;
            } else {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            }
            MatchToken(Ref);
            int EXP_C_Type, Exp_C_Addr;
            EXP_C(EXP_C_Type, Exp_C_Addr);
            if (Type == TIPO_LOGICO && EXP_C_Type != TIPO_LOGICO) {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            } else if (Type != TIPO_LOGICO && EXP_C_Type == TIPO_LOGICO) {
                PrintError(Tokenizer->GetLine(), "tipos incompativeis");
            } else if (Type == TIPO_LOGICO && EXP_C_Type == TIPO_LOGICO) {
                if (op != OP_AND) {
                    PrintError(Tokenizer->GetLine(), "tipos incompativeis");
                } else {
                    int Rot_Dif_Bin;
                    int Rot_Fim_Bin;
                    int Rot_Dif = Generator.NewLabel();
                    int Rot_Fim = Generator.NewLabel();
                    Generator.LOD("A", Addr);
                    Generator.LOD("B", Exp_C_Addr);
                    Addr = Generator.NewTemp();
                    Generator.ADD("A", "B");
                    Generator.LDI("B", 2);
                    Generator.SUB("A", "B");
                    Generator.BNZ("A", Rot_Dif);
                    Rot_Dif_Bin = Generator.GetBinCounter();
                    Generator.STI(1, Addr);
                    Generator.JMP(Rot_Fim);
                    Rot_Fim_Bin = Generator.GetBinCounter();
                    Generator.Label(Rot_Dif);
                    Generator.UpdateBIN_Branch(Rot_Dif_Bin, Generator.GetBinCounter());
                    Generator.STI(0, Addr);
                    Generator.Label(Rot_Fim);
                    Generator.UpdateBIN_Branch(Rot_Fim_Bin, Generator.GetBinCounter());
                }
            } else if (Type == TIPO_REAL || EXP_C_Type == TIPO_REAL) {
                //                DEBUG_Printf("EXP_B1-> T:%d - A:%d\n", Type, Addr);
                //                DEBUG_Printf("EXP_B2-> T:%d - A:%d\n", EXP_C_Type, Exp_C_Addr);
                if (Type == TIPO_REAL && EXP_C_Type == TIPO_INTEIRO) {
                    Generator.LODF("A", Addr);
                    Generator.LOD("B", Exp_C_Addr);
                    Generator.CNV("B", "B");
                } else if (Type == TIPO_INTEIRO && EXP_C_Type == TIPO_REAL) {
                    Generator.LOD("A", Addr);
                    Generator.LODF("B", Exp_C_Addr);
                    Generator.CNV("A", "A");
                } else {
                    Generator.LODF("A", Addr);
                    Generator.LODF("B", Exp_C_Addr);
                }
                if (op == OP_MUL) {
                    Generator.MULF("A", "B");
                } else if (op == OP_DIV) {
                    Generator.DIV("A", "B");
                } else {
                    PrintError(Tokenizer->GetLine(), "tipos incompativeis");
                }
                //                DEBUG_Printf("EXP_B3-> T:%d - A:%d\n", EXP_C_Type, Exp_C_Addr);
                Addr = Generator.NewTempF();
                Generator.STOF("A", Addr);
                Type = TIPO_REAL;
            } else if (Type == TIPO_INTEIRO && EXP_C_Type == TIPO_INTEIRO) {
                Generator.LOD("A", Addr);
                Generator.LOD("B", Exp_C_Addr);
                if (op == OP_MUL) {
                    Generator.MUL("A", "B");
                    Addr = Generator.NewTemp();
                    Generator.STO("A", Addr);
                } else if (op == OP_DIV) {
                    Generator.CNV("A", "A");
                    Generator.CNV("B", "B");
                    Generator.DIV("A", "B");
                    Addr = Generator.NewTempF();
                    Generator.STOF("A", Addr);
                    Type = TIPO_REAL;
                } else {
                    PrintError(Tokenizer->GetLine(), "tipos incompativeis");
                }
            }
        }
        //        DEBUG_Printf("EXP_B-> T:%d - A:%d\n", Type, Addr);
    }

    /**
     * Nega sub-expressão lógica.
     * @param Type Referência para armazenar tipo.
     * @param Addr Referência para armazenar endereço.
     */
    void EXP_C(int &Type, int &Addr) {
        Type = TIPO_VAZIO;
        if (Ref == T_LOGiC_NOT) {
            MatchToken(Ref);
            Type = TIPO_LOGICO;
        }
        int EXP_D_Type, Exp_D_Addr;
        EXP_D(EXP_D_Type, Exp_D_Addr);
        if (Type == TIPO_VAZIO) {
            Type = EXP_D_Type;
            Addr = Exp_D_Addr;
        } else if (Type == TIPO_LOGICO && EXP_D_Type == TIPO_LOGICO) {
            Generator.LOD("A", Exp_D_Addr);
            Generator.NEG("A");
            Generator.ADI("A", 1);
            Addr = Generator.NewTemp();
            Generator.STO("A", Addr);
        } else {
            PrintError(Tokenizer->GetLine(), "tipos incompativeis");
        }
        //        DEBUG_Printf("EXP_C-> T:%d - A:%d\n", Type, Addr);
    }

    /**
     * Carrega operandos para expressão.
     * @param Type Referência para armazenar tipo.
     * @param Addr Referência para armazenar endereço.
     */
    void EXP_D(int &Type, int &Addr) {
        if (Ref == T_PARENS_L) {
            MatchToken(T_PARENS_L);
            EXP(Type, Addr);
            MatchToken(T_PARENS_R);
        } else if (Ref == TOKEN_TYPE_ID) {
            //            DEBUG_Printf("EXP_D-ID\n");
            MatchToken(TOKEN_TYPE_ID);
            Semantic_VerifyDeclaration();
            Type = STable.GetType(LastReg.lexeme);
            Addr = STable.GetAddress(LastReg.lexeme);
            //            DEBUG_Printf("EXP_D-> T:%d - A:%d\n", Type, Addr);
        } else if (Ref == TOKEN_TYPE_CONST) {
            //            DEBUG_Printf("EXP_D-const\n");
            MatchToken(TOKEN_TYPE_CONST);
            Type = LastReg.data_type;
            if (Type == TIPO_REAL) {
                Addr = Generator.NewTempF();
                Generator.STIF("#" + LastReg.lexeme, Addr);
            } else {
                Addr = Generator.NewTemp();
                Generator.STI("#" + LastReg.lexeme, Addr);
            }
        } else {
            Error_Syntatic();
        }
    }
};

#endif

