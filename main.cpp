#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include "Def.h"
#include "Parser.h"

void Usage() {
    printf(" Usage: %s [file] -o[0:1:2:3] -d[0:1:2:3] [-v] [-h] \n", P_SHORT_NAME);
    printf("  Required\n");
    printf("   file : Input file.\n");
    printf("  Optimize\n");
    printf("    -o0 : Does not optimize. Default.\n");
    printf("    -o1 : Peephole.\n");
    printf("    -o2 : Redundant Branchs and DeadCode(subsequent jumps)\n");
    printf("    -o3 : '-o1' e '-o2' .\n");
    printf("  Debug\n");
    printf("    -d0 : No debug. Default.\n");
    printf("    -d1 : Comments on assembly without optimization.\n");
    printf("    -d2 : Save optimized assembly files separately.\n");
    printf("    -d3 : '-d1' e '-d2' .\n");
    printf("  Others\n");
    printf("     -v : About.\n");
    printf("     -h : Help.\n");
}

int main(int argc, char* argv[]) {

    int option = 0, option_index = 0, f_optimize = 0, f_debug = 0;
    string source_file;

    static struct option long_options[] = {
        {"optimize", required_argument, 0, 'o'},
        {"debug", required_argument, 0, 'd'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    opterr = 0;
    while ((option = getopt_long(argc, argv, "o:d:vh", long_options, &option_index)) != -1) {
        switch (option) {
            case 0:
                break;
            case 'o':
                if (optarg) f_optimize = atoi(optarg);
                f_optimize = (f_optimize > 3) ? 3 : f_optimize;
//                printf("Optimize Level '%d'.\n\n", f_optimize);
                break;
            case 'd':
                if (optarg) f_debug = atoi(optarg);
                f_debug = (f_debug > 3) ? 3 : f_debug;
//                printf("Debug Level '%d'.\n\n", f_debug);
                break;
            case 'h':
                Usage();
                exit(EXIT_SUCCESS);
            case 'v':
                printf("%s %s \n", P_NAME, P_VERSION);
                printf("%s\n", P_ABOUT);
                exit(EXIT_SUCCESS);
            case '?':
                if (optopt == 'o' || optopt == 'd')
                    printf("%s: error: missing argument to '-%c' \n", P_SHORT_NAME, optopt);
                else
                    printf("%s: error: invalid option '-%c' \n", P_SHORT_NAME, optopt);
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);
        }
    }

    int index = optind;
    if (argc > index + 1) {
        printf("%s: fatal error: Invalid file name [%s", P_SHORT_NAME, argv[index++]);
        while (index < argc) printf(" %s", argv[index++]);
        printf("].\n");
        exit(EXIT_FAILURE);
    } else if (index == argc) {
        printf("%s: fatal error: No input file.\n", P_SHORT_NAME);
        exit(EXIT_FAILURE);
    } else {
        source_file = argv[index];
        string temp = source_file.substr(source_file.size()-3, source_file.size());
        if(toLowerCase(temp) != ".gl"){
            printf("%s: error: Invalid file extension '%s' != '.gl' .\n", P_SHORT_NAME, temp.c_str());
            exit(EXIT_FAILURE);
        }
    }

    Parser Parser(source_file, f_optimize, f_debug);
    Parser.Start();

    return 0;
}

