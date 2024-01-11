#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <gmp.h>

#include "randstate.h"
#include "ss.h"
#include "numtheory.h"

#define OPTIONS "i:o:n:vh"

void help(void) {
    fprintf(stderr,
        "Synopsis\n"
        "   Encrypts files using a public key.\n"
        "run './encrypt [command line option]' in terminal opened to directory asgn5 \n"
        "Command Line Options\n"
        "   -i infile       specifies the input file to encrypt (default: stdin)\n"
        "   -o outfile      specifies the output file to encrypt (default: stdout)\n"
        "   -n pbfile       specifies the file containing the public key (default: ss.pub)\n"
        "   -v              enables verbose output\n"
        "   -h              displays program synopsis and usage\n");
}

int main(int argc, char **argv) {
    bool verbose = false;
    int opt = 0;
    // 2. "open public key file using fopen(), print failure and exit"
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pbfile = fopen("./ss.pub", "r");
    if ((infile == NULL) || (outfile == NULL)) {
        fprintf(stderr, "File Opening Error.\n");
        exit(1);
    }
    // 1. "parse command line options using getopt()"
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'n': pbfile = fopen(optarg, "r"); break;
        case 'v': verbose = true; break;
        case 'h':
            help();
            exit(1);
            break;
        default:
            infile = stdin;
            outfile = stdout;
            pbfile = fopen("./ss.pub", "r");
            break;
        }
    }

    mpz_t n, username;
    mpz_inits(n, username, NULL);
    char user[1024]; //buffer for username

    // 3. "read public key from open public key file"
    ss_read_pub(n, user, pbfile);

    // 4. "if verbose output is enabled print the following, each with trailing newline in order username and public key n"
    if (verbose) {
        fprintf(stderr, "user = %s\n", user);
        gmp_fprintf(stderr, "n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
    }

    // 5. "encrypt the file using ss_encrypt_file()"
    ss_encrypt_file(infile, outfile, n);

    // 6. "close the public key file and clear and mpz_t vars used"
    fclose(infile);
    fclose(outfile);
    fclose(pbfile);
    mpz_clears(n, username, NULL);

    return 0;
}
