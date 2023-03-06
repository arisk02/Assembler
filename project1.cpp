#ifndef __PROJECT1_CPP__
#define __PROJECT1_CPP__

#include "project1.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

std::unordered_map<std::string, int> labels;
std::unordered_map<std::string, int> static_memory;

int main(int argc, char* argv[]) {
    if (argc < 4) // Checks that at least 3 arguments are given in command line
    {
        std::cerr << "Expected Usage:\n ./assemble infile1.asm infile2.asm ... infilek.asm staticmem_outfile.bin instructions_outfile.bin\n" << std::endl;
        exit(1);
    }
    //Prepare output files
    std::ofstream inst_outfile, static_outfile;
    static_outfile.open(argv[argc - 2], std::ios::binary);
    inst_outfile.open(argv[argc - 1], std::ios::binary);
    std::vector<std::string> instructions;
    std::vector<std::string> static_values;
    std::vector<std::string> static_stuff;

    /**
     * Phase 1:
     * Read all instructions, clean them of comments and whitespace DONE
     * Determine the numbers for all static memory labels
     * (measured in bytes starting at 0) DONE
     * Determine the line numbers of all instruction line labels
     * (measured in instructions) starting at 0 DONE
    */

    int line_num = 0;
    int memory_counter = 0;

    //For each input file:
    for (int i = 1; i < argc - 2; i++) {
        std::ifstream infile(argv[i]); //  open the input file for reading
        if (!infile) { // if file can't be opened, need to let the user know
            std::cerr << "Error: could not open file: " << argv[i] << std::endl;
            exit(1);
        }

        std::string str;

        while (getline(infile, str)){ //Read a line from the file
            str = clean(str); // remove comments and whitespace
            int sep = str.find(" ");
            std::string first_word = str.substr(0,sep);   
            if (str == "") { //Ignore empty lines
                continue;
            }
            else if (str.find(".word")!=std::string::npos) {
                std::vector<std::string> reg_list = split(str, WHITESPACE+ ":");
                if (reg_list[0] == ".word") {
                    reg_list.erase(reg_list.begin()+0);
                }
                else if (reg_list[1] == ".word") {
                    reg_list.erase(reg_list.begin()+1);
                }
                static_memory[reg_list[0]] = memory_counter;
                for (int i = 1; i < reg_list.size(); i++) {
                    static_values.push_back(reg_list[i]);
                    memory_counter += 4;
                }
            }
            else if (first_word.at(0) == '.'){
                continue;
            }
            else if (str.find(":")!=std::string::npos) {  // Check if the line is a label
                int sep = str.find(":");
                std::string label = str.substr(0,sep);
                labels[label] = line_num; // Add the line number of the label into the labels map
            }
            else {
                instructions.push_back(str); // This will need to change for labels DONE
                line_num++;
            }
        }
        infile.close();
    }

    /** Phase 2
     * Process all static memory, output to static memory file
     * All of this DONE
     */
    for (int i = 0; i < static_values.size(); i++) {
        if (labels.count(static_values[i]) == 1) {
            //Grab line number, multiply, then write.
            int value = labels[static_values[i]] * 4;
            write_binary(value, static_outfile);
        }
        else {
            write_binary(stoi(static_values[i]), static_outfile);
        }
    }

    /** Phase 3
     * Process all instructions, output to instruction memory file
     * Almost all of this, it only works for adds DONE
     */

    int inst_number = 0;
    for(std::string inst : instructions) {
        int sep = inst.find(" ");
        std::string inst_type = inst.substr(0,sep);
        if (inst_type == "add") {
            write_binary(process_add(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "addi") {
            write_binary(process_addi(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "sub") {
            write_binary(process_sub(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "mult") {
            write_binary(process_mult(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "div") {
            write_binary(process_div(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "mflo") {
            write_binary(process_mflo(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "mfhi") {
            write_binary(process_mfhi(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "sll") {
            write_binary(process_sll(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "srl") {
            write_binary(process_srl(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "lw") {
            inst.pop_back();
            write_binary(process_lw(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "sw") {
            inst.pop_back();
            write_binary(process_sw(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "slt") {
            write_binary(process_slt(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "beq") {
            write_binary(process_beq(inst.substr(sep+1), inst_number), inst_outfile);
        }
        else if (inst_type == "bne") {
            write_binary(process_bne(inst.substr(sep+1), inst_number), inst_outfile);
        }
        else if (inst_type == "j") {
            write_binary(process_j(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "jal") {
            write_binary(process_jal(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "jr") {
            write_binary(process_jr(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "jalr") {
            write_binary(process_jalr(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "la") {
            write_binary(process_la(inst.substr(sep+1)), inst_outfile);
        }
        // start bonus instructions
        else if (inst_type == "and") {
            write_binary(process_and(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "or") {
            write_binary(process_or(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "xor") {
            write_binary(process_xor(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "not") {
            write_binary(process_not(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "nor") {
            write_binary(process_nor(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "andi") {
            write_binary(process_andi(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "ori") {
            write_binary(process_ori(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "xori") {
            write_binary(process_xori(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "lui") {
            write_binary(process_lui(inst.substr(sep+1)), inst_outfile);
        }
        else if (inst_type == "move") {
            write_binary(process_move(inst.substr(sep+1)), inst_outfile);
        }
        // end bonus instructions
        else if (inst_type == "syscall") {
            write_binary((26<<11) + 12, inst_outfile);
        }
        inst_number++;
    }
}

int process_add(const std::string &inst) { //opcode = 0, funccode = 32 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 32); 
}

int process_addi(const std::string &inst) { //opcode = 8 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int imm = stoi(reg_list[2]);
    if (imm < 0) {
        imm += 65536;
    }
    return process_Itype(8, registers[reg_list[1]], registers[reg_list[0]], imm);
}

int process_sub(const std::string &inst) { //opcode = 0, funccode = 34 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 34); 
}

int process_mult(const std::string &inst) { //opcode = 0, funccode = 24 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[0]], registers[reg_list[1]], 0, 0, 24); 
}

int process_div(const std::string &inst) { //opcode = 0, funccode = 26 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[0]], registers[reg_list[1]], 0, 0, 26); 
}

int process_mflo(const std::string &inst) { //opcode = 0, funccode = 18  DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, 0, 0, registers[reg_list[0]], 0, 18); 
}

int process_mfhi(const std::string &inst) { //opcode = 0, funccode = 16 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, 0, 0, registers[reg_list[0]], 0, 16); 
}

int process_sll(const std::string &inst) { //opcode = 0, funccode = 0 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, 0, registers[reg_list[1]], registers[reg_list[0]], stoi(reg_list[2]), 0); 
}

int process_srl(const std::string &inst) { //opcode = 0, funccode = 2 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, 0, registers[reg_list[1]], registers[reg_list[0]], stoi(reg_list[2]), 2); 
}

int process_lw(const std::string &inst) { //opcode = 35 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+"," + "(");
    return process_Itype(35, registers[reg_list[2]], registers[reg_list[0]], stoi(reg_list[1])); 
}

int process_sw(const std::string &inst) { //opcode = 43 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+"," + "(");
    return process_Itype(43, registers[reg_list[2]], registers[reg_list[0]], stoi(reg_list[1])); 
}

int process_slt(const std::string &inst) { //opcode = 0, funccode = 42 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 42); 
}

int process_beq(const std::string &inst, int line_num) { //opcode = 4 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int value = labels[reg_list[2]] - line_num - 1;
    if (value < 0) {
        value += 65536;
    }
    return process_Itype(4, registers[reg_list[0]], registers[reg_list[1]], value); 
}

int process_bne(const std::string &inst, int line_num) { //opcode = 5 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int value = labels[reg_list[2]] - line_num - 1;
    if (value < 0) {
        value += 65536;
    }
    return process_Itype(5, registers[reg_list[0]], registers[reg_list[1]], value); 
}

int process_j(const std::string &inst) { //opcode = 2 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Jtype(2, labels[reg_list[0]]);
}

int process_jal(const std::string &inst) { //opcode = 3 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Jtype(3, labels[reg_list[0]]); 
}

int process_jr(const std::string &inst) { //opcode = 0, funccode = 8 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[0]], 0, 0, 0, 8); 
}

int process_jalr(const std::string &inst) { //opcode = 0, funccode = 9 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[0]], 0, registers["$ra"], 0, 9);
}

int process_la(const std::string &inst) { //opcode = 8 - modified addi instruction DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Itype(8, 0, registers[reg_list[0]], static_memory[reg_list[1]]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//**************************** B O N U S   I N S T R U C T I O N S ***************************************

int process_and(const std::string &inst) { //opcode = 0, funccode = 36 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 36); 
}

int process_or(const std::string &inst) { //opcode = 0, funccode = 37 DONE 
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 37);
}

int process_nor(const std::string &inst) { //opcode = 0, funccode = 39 DONE 
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 39);
}

int process_not(const std::string &inst) { //opcode = 0, funccode = 39 - modified nor instruction DONE 
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[1]], registers[reg_list[0]], 0, 39);
}

int process_xor(const std::string &inst) { //opcode = 0, funccode = 38 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], registers[reg_list[2]], registers[reg_list[0]], 0, 38);
}

int process_andi(const std::string &inst) { //opcode = 12 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int value = registers[reg_list[2]];
    if (value < 0) {
        value += 65536;
    }
    return process_Itype(12, registers[reg_list[1]], registers[reg_list[0]], value);
}

int process_xori(const std::string &inst) { //opcode = 14 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int value = registers[reg_list[2]];
    if (value < 0) {
        value += 65536;
    }
    return process_Itype(14, registers[reg_list[1]], registers[reg_list[0]], value);
}

int process_ori(const std::string &inst) { //opcode = 13 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int value = registers[reg_list[2]];
    if (value < 0) {
        value += 65536;
    }
    return process_Itype(13, registers[reg_list[1]], registers[reg_list[0]], value);
}

int process_lui(const std::string &inst) { //opcode = 15 DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    int value = registers[reg_list[1]];
    if (value < 0) {
        value += 65536;
    }
    return process_Itype(15, 0, registers[reg_list[0]], value);
}

int process_move(const std::string &inst) { //opcode = 0, funccode = 36 - modified add instruction DONE
    std::vector<std::string> reg_list = split(inst, WHITESPACE+",");
    return process_Rtype(0, registers[reg_list[1]], 0, registers[reg_list[0]], 0, 36); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int process_Rtype(int opcode, int rs, int rt, int rd, int shftamt, int funccode) {
    return (opcode << 26) + (rs << 21) + (rt << 16) + (rd << 11) + (shftamt << 6) + funccode;
}

int process_Itype(int opcode, int rs, int rt, int imm) {
    return (opcode << 26) + (rs << 21) + (rt << 16) + imm;
}

int process_Jtype(int opcode, int pseudo_address) {
    return (opcode << 26) + pseudo_address;
}
#endif