#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <gmp.h>

#include "randstate.h"
#include "ss.h"
#include "numtheory.h"

#define OPTIONS "i:o:n:vh"

void help(void) {
    fprintf(stderr,
        "Synopsis\n"
        "   Decrypts the encrypted files using the corresponding private key.\n"
        "run './decrypt [command line option]' in terminal opened to directory asgn5 \n"
        "Command Line Options\n"
        "   -i infile       specifies the input file to decrypt (default: stdin)\n"
        "   -o outfile      specifies the output file to decrypt (default: stdout)\n"
        "   -n pbfile       specifies the file containing the private key (default: ss.priv)\n"
        "   -v              enables verbose output\n"
        "   -h              displays program synopsis and usage\n");
}

int main(int argc, char **argv) {
    bool verbose = false;
    int opt = 0;
    // 2. "open priv key using fopen(), print error and exit if failure"
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile = fopen("./ss.priv", "r");
    if ((infile == NULL) || (outfile == NULL)) {
        fprintf(stderr, "File Opening Error.\n");
        exit(1);
    }
    // switch cases "1, parse command line options using getopt()"xa
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'n': pvfile = fopen(optarg, "r"); break;
        case 'v': verbose = true; break;
        case 'h':
            help();
            exit(1);
            break;
        default:
            infile = stdin;
            outfile = stdout;
            pvfile = fopen("./ss.priv", "r");
            break;
        }
    }
    mpz_t pq, d;
    mpz_inits(pq, d, NULL);

    // 3. "read private key from opened private key file"
    ss_read_priv(pq, d, pvfile);

    // 4. "if verbose is enabled print priv modulus pq and d with num of bits and decimal values
    if (verbose) {
        gmp_fprintf(stderr, "pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_fprintf(stderr, "d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // 5. "decrypt the file using ss_decrypt_file()
    ss_decrypt_file(infile, outfile, pq, d);

    // 6. "close private key file and clear any mpz_t vars used"
    fclose(infile);
    fclose(outfile);
    fclose(pvfile);
    mpz_clears(pq, d, NULL);

    return 0;
}
