#ifndef CODEGEN_H
#define	CODEGEN_H

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "Util.h"

class CodeGen {
private:

    typedef struct Instruct {
        std::string name, param_1, param_2, comment;
        int16_t opCode, op_param_1, op_param_2, op_param_3;
    } Instruct;

    int f_debug, f_optimize;
    std::string o_filename;
    std::ofstream file_bin, file_asm, file_asm_o;

    int asm_comment_counter = 0;
    int asm_code_counter = -1;
    int bin_code_counter = 0;
    int label_counter = 0;
    int address_base = 0;
    int address = 0;

    std::vector<Instruct> gen_asm;
    std::vector<int16_t> gen_bin;
    std::vector<pair<int, int16_t>> branchs_bin;
    std::map<int, pair<int, int>> branchs_asm;

    void Peephole_ASM(std::vector<Instruct> *gen_asm_o) {
        int index;
        std::vector<Instruct>::iterator it1, it2;
        for (it1 = gen_asm_o->begin(); it1 < gen_asm_o->end(); it1++) {
            it2 = it1;
            if (it1 + 1 <= gen_asm_o->end())
                it2 = it1 + 1;
            else break;
            index = it1 - gen_asm_o->begin();
            if (it1->name == "STOF" && it2->name == "LODF") {
                if ((it1->param_1 == it2->param_1) && (it1->param_2 == it2->param_2)) {
                    gen_asm_o->erase(it1, it1 + 2);
                    it1--;
                    for (auto it = branchs_asm.begin(); it != branchs_asm.end(); it++) {
                        if (it->second.first > index) it->second.first -= 2;
                        if (it->second.second > index) it->second.second -= 2;
                    }
                }
            } else if (it1->name == "STO" && it2->name == "LOD") {
                if ((it1->param_1 == it2->param_1) && (it1->param_2 == it2->param_2)) {
                    gen_asm_o->erase(it1, it1 + 2);
                    it1--;
                    for (auto it = branchs_asm.begin(); it != branchs_asm.end(); it++) {
                        if (it->second.first > index) it->second.first -= 2;
                        if (it->second.second > index) it->second.second -= 2;
                    }
                }
            }
        }

        if (f_debug == 2 || f_debug == 3) {
            file_asm_o.open(o_filename + "_o1.asm");
            file_asm_o << "; Peephole\n";
            for (std::vector<Instruct>::iterator it = gen_asm_o->begin(); it < gen_asm_o->end(); it++) {
                if (it->name == "ROT") {
                    file_asm_o << " " << it->param_1 << it->param_2;
                } else {
                    file_asm_o << "      " << it->name;
                    file_asm_o << " " << it->param_1;
                    if (it->param_2.size() > 0) file_asm_o << " , " << it->param_2;
                }
                file_asm_o << "\n";
            }
            file_asm_o.close();
        }
    }

    void Peephole_BIN() {
        int index;
        std::vector<int16_t>::iterator it1, it2, it3, it4, it5, it6;
        for (it1 = gen_bin.begin(); it1 < gen_bin.end(); it1 += 3) {
            it2 = it3 = it4 = it5 = it6 = it1;
            if (it1 + 1 <= gen_bin.end())
                it2 = it1 + 1;
            else break;
            if (it1 + 2 <= gen_bin.end())
                it3 = it1 + 2;
            else break;
            if (it1 + 3 <= gen_bin.end())
                it4 = it1 + 3;
            else break;
            if (it1 + 4 <= gen_bin.end())
                it5 = it1 + 4;
            else break;
            if (it1 + 5 <= gen_bin.end())
                it6 = it1 + 5;
            else break;

            index = it1 - gen_bin.begin();

            if (*it1 == 37 && *it4 == 26) { //STOF - LODF
                if ((*it2 == *it5) && (*it3 == *it6)) {
                    gen_bin.erase(it1, it1 + 6);
                    it1 -= 3;
                    for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
                        if (it->first > index) it->first -= 6;
                        if (it->second > index) it->second -= 6;
                    }

                }
            } else if (*it1 == 36 && *it4 == 25) { //STO - LOD
                if ((*it2 == *it5) && (*it3 == *it6)) {
                    gen_bin.erase(it1, it1 + 6);
                    it1 -= 3;
                    for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
                        if (it->first > index) it->first -= 6;
                        if (it->second > index) it->second -= 6;
                    }
                }
            }

        }
        // Reescrever Endereços
        for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
            gen_bin.at(it->first) = it->second;
        }
    }

    void RedundantBranch_ASM(std::vector<Instruct> *gen_asm_o) {

        for (auto it = branchs_asm.begin(); it != branchs_asm.end(); it++) {
            if (gen_asm_o->at(it->second.second + 1).name == "ROT") {
                if (gen_asm_o->at(it->second.first).name == "JMP")
                    gen_asm_o->at(it->second.first).param_1 = gen_asm_o->at(it->second.second + 1).param_1;
                else gen_asm_o->at(it->second.first).param_2 = gen_asm_o->at(it->second.second + 1).param_1;
                gen_asm_o->erase(gen_asm_o->begin() + it->second.second);
                for (auto it2 = branchs_asm.begin(); it2 != branchs_asm.end(); it2++) {
                    if (it2->second.second > it->second.second) it2->second.second -= 1;
                    if (it2->second.first > it->second.second) it2->second.first -= 1;
                }
                it = branchs_asm.begin();
            }
        }
        for (auto it = branchs_asm.begin(); it != branchs_asm.end(); it++) {
            if (gen_asm_o->at(it->second.second + 1).name == "JMP") {
                if (gen_asm_o->at(it->second.first).name == "JMP")
                    gen_asm_o->at(it->second.first).param_1 = gen_asm_o->at(it->second.second + 1).param_1;
                else gen_asm_o->at(it->second.first).param_2 = gen_asm_o->at(it->second.second + 1).param_1;
                gen_asm_o->erase(gen_asm_o->begin() + it->second.second);
                for (auto it2 = branchs_asm.begin(); it2 != branchs_asm.end(); it2++) {
                    if (it2->second.second > it->second.second) it2->second.second -= 1;
                    if (it2->second.first > it->second.second) it2->second.first -= 1;
                }
                it = branchs_asm.begin();
            }
        }

        //        for (auto it = branchs_asm.begin(); it != branchs_asm.end(); it++) {
        //            DEBUG_Printf(">> Instruct:%d on line %d jump to %d]\n", it->first, it->second.first + 2, it->second.second + 2);
        //        }

        if (f_debug == 2 || f_debug == 3) {
            file_asm_o.open(o_filename + "_o2.asm");
            file_asm_o << "; Redundancia de Desvios\n";
            for (std::vector<Instruct>::iterator it = gen_asm_o->begin(); it < gen_asm_o->end(); it++) {
                if (it->name == "ROT") {
                    file_asm_o << " " << it->param_1 << it->param_2;
                } else {
                    file_asm_o << "      " << it->name;
                    file_asm_o << " " << it->param_1;
                    if (it->param_2.size() > 0) file_asm_o << " , " << it->param_2;
                }
                file_asm_o << "\n";
            }
            file_asm_o.close();
        }
    }

    void RedundantBranch_BIN() {
        for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
            if (gen_bin.at(it->second) == 21) { // Jump
                it->second = gen_bin.at(it->second + 1);
                gen_bin.at(it->first) = it->second;
                it = branchs_bin.begin();
            }
        }
        //        for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
        //            DEBUG_Printf(">> Line %d jump to %d\n", it->first, it->second);
        //        }
    }

    void DeadCode_ASM(std::vector<Instruct> *gen_asm_o) {
        int found = 0;
        int index;
        std::vector<Instruct>::iterator it1, it2;
        for (it1 = gen_asm_o->begin(); it1 < gen_asm_o->end(); it1++) {
            it2 = it1;
            if (it1 + 1 <= gen_asm_o->end())
                it2 = it1 + 1;
            else break;
            index = it1 - gen_asm_o->begin();
            if (it1->name == "JMP" && it2->name == "JMP") {
                found = 1;
                gen_asm_o->erase(it2);
                it1 = gen_asm_o->begin();
                for (auto it = branchs_asm.begin(); it != branchs_asm.end(); it++) {
                    if (it->second.first > index) it->second.first -= 1;
                    if (it->second.second > index) it->second.second -= 1;
                }

            }
        }

        if ((f_debug == 2 || f_debug == 3) && found == 1) {
            file_asm_o.open(o_filename + "_o2.asm");
            file_asm_o << "; Redundancia de Desvios e DeadCode(Jumps subsequentes)\n";
            for (std::vector<Instruct>::iterator it = gen_asm_o->begin(); it < gen_asm_o->end(); it++) {
                if (it->name == "ROT") {
                    file_asm_o << " " << it->param_1 << it->param_2;
                } else {
                    file_asm_o << "      " << it->name;
                    file_asm_o << " " << it->param_1;
                    if (it->param_2.size() > 0) file_asm_o << " , " << it->param_2;
                }
                file_asm_o << "\n";
            }
            file_asm_o.close();
        }
    }

    void DeadCode_BIN() {
        int index;
        std::vector<int16_t>::iterator it1, it2, it3, it4;
        for (it1 = gen_bin.begin(); it1 < gen_bin.end(); it1 += 2) {
            it2 = it3 = it4 = it1;
            if (it1 + 1 <= gen_bin.end())
                it2 = it1 + 1;
            else break;
            if (it1 + 2 <= gen_bin.end())
                it3 = it1 + 2;
            else break;
            if (it1 + 3 <= gen_bin.end())
                it4 = it1 + 3;
            else break;

            index = it1 - gen_bin.begin();

            if (*it1 == 21 && *it3 == 21) { //JMP
                if (*it2 == *it4) {
                    gen_bin.erase(it1, it1 + 4);
                    it1 -= 2;
                    for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
                        if (it->first > index) it->first -= 4;
                        if (it->second > index) it->second -= 4;
                    }
                }
            }

        }
        // Reescrever Endereços
        for (auto it = branchs_bin.begin(); it < branchs_bin.end(); it++) {
            gen_bin.at(it->first) = it->second;
        }
    }

public:

    CodeGen() {
        f_debug = 0;
        f_optimize = 0;
        o_filename = "output";
    }

    void SetArgs(std::string o_filename, int f_debug = 0, int f_optimize = 0) {
        this->o_filename = o_filename;
        this->f_debug = f_debug;
        this->f_optimize = f_optimize;
    }

    int ADD(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "ADD";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 1;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int ADDF(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "ADDF";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 2;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int ADI(std::string Reg1, int immed) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "ADI";
        temp.param_1 = Reg1;
        temp.param_2 = "#" + patch::to_string(immed);

        temp.opCode = 3;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(immed));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int ADIF(std::string Reg1, std::string immed) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "ADIF";
        temp.param_1 = Reg1;
        temp.param_2 = "#" + immed;

        temp.opCode = 4;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(immed));
        temp.op_param_3 = NumToInt16_DecimalPart(patch::to_string(immed));

        bin_code_counter += 4;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);
        gen_bin.push_back(temp.op_param_3);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNG(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNG";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 5;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNGF(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNGF";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 6;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNN(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNN";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 7;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNNF(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNNF";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 8;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNP(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNP";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 9;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNPF(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNPF";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 10;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNZ(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNZ";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 11;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BNZF(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BNZF";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 12;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BPS(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BPS";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 13;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BPSF(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BPSF";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 14;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BZR(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BZR";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 15;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int BZRF(std::string Reg1, int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "BZRF";
        temp.param_1 = Reg1;
        temp.param_2 = "ROT" + patch::to_string(label);
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 16;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int CNV(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "CNV";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 17;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int DIV(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "DIV";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 18;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int ESC(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "ESC";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 19;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int HLT() {
        asm_code_counter++;
        Instruct temp;
        temp.name = "HLT";
        temp.param_1 = "";
        temp.param_2 = "";

        temp.opCode = 20;

        bin_code_counter += 1;
        gen_bin.push_back(temp.opCode);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int JMP(int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "JMP";
        temp.param_1 = "ROT" + patch::to_string(label);
        temp.param_2 = "";
        branchs_asm[label].first = asm_code_counter - asm_comment_counter;

        temp.opCode = 21;
        temp.op_param_1 = NumToInt16_IntergerPart(patch::to_string(label));

        bin_code_counter += 2;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int LDI(std::string Reg1, int immed) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "LDI";
        temp.param_1 = Reg1;
        temp.param_2 = "#" + patch::to_string(immed);

        temp.opCode = 22;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(immed));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int LGT(std::string Reg1) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "LGT";
        temp.param_1 = Reg1;
        temp.param_2 = "";

        temp.opCode = 24;
        temp.op_param_1 = RegCod(Reg1);

        bin_code_counter += 2;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int LOD(std::string Reg1, int mem) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "LOD";
        temp.param_1 = Reg1;
        temp.param_2 = patch::to_string(mem) + "(DS)";

        temp.opCode = 25;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(mem));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int LODF(std::string Reg1, int mem) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "LODF";
        temp.param_1 = Reg1;
        temp.param_2 = patch::to_string(mem) + "(DS)";

        temp.opCode = 26;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(mem));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int MUL(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "MUL";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 29;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int MULF(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "MULF";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 30;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int NEG(std::string Reg1) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "NEG";
        temp.param_1 = Reg1;
        temp.param_2 = "";

        temp.opCode = 31;
        temp.op_param_1 = RegCod(Reg1);

        bin_code_counter += 2;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int NEGF(std::string Reg1) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "NEGF";
        temp.param_1 = Reg1;
        temp.param_2 = "";

        temp.opCode = 32;
        temp.op_param_1 = RegCod(Reg1);

        bin_code_counter += 2;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int RTR() {
        asm_code_counter++;
        Instruct temp;
        temp.name = "RTR";
        temp.param_1 = "";
        temp.param_2 = "";

        temp.opCode = 33;

        bin_code_counter += 1;
        gen_bin.push_back(temp.opCode);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int STI(std::string p1, int p2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "STI";
        temp.param_1 = p1;
        temp.param_2 = patch::to_string(p2) + "(DS)";
        temp.opCode = 34;
        temp.op_param_1 = NumToInt16_IntergerPart(p1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(p2));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int STI(int p1, int p2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "STI";
        temp.param_1 = "#" + patch::to_string(p1);
        temp.param_2 = patch::to_string(p2) + "(DS)";
        temp.opCode = 34;
        temp.op_param_1 = NumToInt16_IntergerPart(patch::to_string(p1));
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(p2));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int STIF(std::string p1, int p2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "STIF";
        temp.param_1 = RealToStr(p1);
        temp.param_2 = patch::to_string(p2) + "(DS)";

        temp.opCode = 35;
        temp.op_param_1 = NumToInt16_IntergerPart(p1);
        temp.op_param_2 = NumToInt16_DecimalPart(p1);
        temp.op_param_3 = NumToInt16_IntergerPart(patch::to_string(p2));

        bin_code_counter += 4;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);
        gen_bin.push_back(temp.op_param_3);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int STO(std::string Reg1, int mem) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "STO";
        temp.param_1 = Reg1;
        temp.param_2 = patch::to_string(mem) + "(DS)";

        temp.opCode = 36;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(mem));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int STOF(std::string Reg1, int mem) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "STOF";
        temp.param_1 = Reg1;
        temp.param_2 = patch::to_string(mem) + "(DS)";

        temp.opCode = 37;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = NumToInt16_IntergerPart(patch::to_string(mem));

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int SUB(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "SUB";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 38;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int SUBF(std::string Reg1, std::string Reg2) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "SUBF";
        temp.param_1 = Reg1;
        temp.param_2 = Reg2;

        temp.opCode = 39;
        temp.op_param_1 = RegCod(Reg1);
        temp.op_param_2 = RegCod(Reg2);

        bin_code_counter += 3;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);
        gen_bin.push_back(temp.op_param_2);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int TME(std::string Reg1) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "TME";
        temp.param_1 = Reg1;
        temp.param_2 = "";

        temp.opCode = 40;
        temp.op_param_1 = RegCod(Reg1);

        bin_code_counter += 2;
        gen_bin.push_back(temp.opCode);
        gen_bin.push_back(temp.op_param_1);

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int Label(int label) {
        asm_code_counter++;
        Instruct temp;
        temp.name = "ROT";
        temp.param_1 = "ROT" + patch::to_string(label);
        temp.param_2 = ":";

        branchs_asm[label].second = asm_code_counter - asm_comment_counter;

        gen_asm.push_back(temp);
        return asm_code_counter;
    }

    int GetBinCounter() {
        return bin_code_counter - 1;
    }

    void UpdateBIN_Branch(int position, int jump_to) {
        gen_bin.at(position) = (int16_t) jump_to + 1;
        branchs_bin.push_back(std::make_pair(position, (int16_t) jump_to + 1));
    }

    int NewData() {
        return address;
    }

    int NewTempF() {
        int atual_adress = address_base + (address);
        address += 2;
        return atual_adress;
    }

    int NewTemp() {
        int atual_adress = address_base + (address);
        address += 1;
        return atual_adress;
    }

    int NewLabel() {
        return label_counter++;
    }

    void SetTempBase() {
        address_base = address;
        address = 0;
    }

    void ResetTemp() {
        address = 0;
    }

    void Comment(std::string comment) {
        if (f_debug == 1 || f_debug == 3) {
            asm_comment_counter++;
            asm_code_counter++;
            Instruct temp;
            temp.name = "COMM";
            temp.param_1 = "; ";
            temp.param_2 = comment;
            gen_asm.push_back(temp);
        }
    }

    void CommentI(std::string comment) {
        if (f_debug == 1 || f_debug == 3) {
            asm_comment_counter++;
            asm_code_counter++;
            Instruct temp;
            temp.name = "COMM";
            temp.param_1 = "     ;";
            temp.param_2 = comment;
            gen_asm.push_back(temp);
        }
    }

    void CommentINLINE(std::string comment) {
        if (f_debug == 1 || f_debug == 3) {
            Instruct temp;
            temp = gen_asm.at(asm_code_counter);
            temp.comment = comment;
            gen_asm.at(asm_code_counter) = temp;
        }
    }

    void FlushCode() {
        if (f_optimize > 0) {
            std::vector<Instruct> *gen_asm_o;
            if (f_debug > 0) {
                gen_asm_o = new std::vector<Instruct>();
                for (std::vector<Instruct>::iterator it = gen_asm.begin(); it < gen_asm.end(); it++)
                    if (it->name != "COMM") {
                        gen_asm_o->push_back(*it);
                        gen_asm_o->back().comment.clear();
                    }
            } else {
                gen_asm_o = &gen_asm;
            }
            if (f_optimize == 1 || f_optimize == 3) {
                Peephole_ASM(gen_asm_o);
                Peephole_BIN();
            }
            if (f_optimize == 2 || f_optimize == 3) {
                RedundantBranch_ASM(gen_asm_o);
                DeadCode_ASM(gen_asm_o);
                RedundantBranch_BIN();
                DeadCode_BIN();
            }
        }

        file_asm.open(o_filename + ".asm");
        file_bin.open(o_filename + ".exe", std::ofstream::binary);

        //Binário
        for (std::vector<int16_t>::iterator it = gen_bin.begin(); it < gen_bin.end(); it++) {
            file_bin.write(reinterpret_cast<const char *> (&(*it)), sizeof (int16_t));
        }

        //Assembly
        for (std::vector<Instruct>::iterator it = gen_asm.begin(); it < gen_asm.end(); it++) {
            if (it->name == "COMM") {
                if (f_debug == 1 || f_debug == 3) {
                    if (it->param_2.size() > 0) {
                        file_asm << it->param_1;
                        file_asm << it->param_2;
                    }
                    file_asm << "\n";
                }
            } else {
                if (it->name == "ROT") {
                    file_asm << " " << it->param_1 << it->param_2;
                } else {
                    file_asm << "      " << it->name;
                    file_asm << " " << it->param_1;
                    if (it->param_2.size() > 0) file_asm << " , " << it->param_2;
                }
                if ((it->comment.size() > 0))
                    file_asm << " ;" << it->comment;
                file_asm << "\n";
            }
        }

        file_bin.close();
        file_asm.close();
    }

};


#endif