#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <gmp.h>

#include "randstate.h"
#include "ss.h"
#include "numtheory.h"

#define OPTIONS "b:i:n:d:s:vh"

void help(void) {
    fprintf(stderr,
        "Synopsis\n"
        "   Generates Key, produces SS public and private key pairs.\n"
        "Usage\n"
        "run './keygen [command line option]' in terminal opened to directory asgn5 \n"
        "Command Line Options\n"
        "   -b      specifies minimum bits needed for the public modulus n\n"
        "   -i      specifies the number of Miller-Rabin iterattions for testing primes (default: "
        "50)\n"
        "   -n pbfile                   specifies the public key file (default: ss.pub)\n"
        "   -d pvfile                   specifies the private key file (default: ss.priv)\n"
        "   -s	specifies the random seed for the random state initialization (default: seconds "
        "since the UNIX epoch, time(NULL)"
        "   -v      enables verbose output"
        "   -h      displays program synopsis and usage\n");
}

int main(int argc, char **argv) {
    uint32_t iters = 50; //defaults
    uint32_t def_seed = time(NULL);
    uint64_t nbits = 256;
    bool verbose = false;
    int opt = 0;

    // 2. "open public and private keys using fopen, generate error if needed"
    FILE *pbfile = fopen("./ss.pub", "w");
    FILE *pvfile = fopen("./ss.priv", "w");
    // 1. "parse command line options using getopt()"
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': nbits = strtol(optarg, NULL, 10); break;
        case 'i': iters = strtol(optarg, NULL, 10); break;
        case 'n': pbfile = fopen(optarg, "w"); break;
        case 'd': pvfile = fopen(optarg, "w"); break;
        case 's': def_seed = strtol(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        case 'h':
            help();
            exit(1);
            break;
        default:
            pbfile = fopen("./ss.pub", "w");
            pvfile = fopen("./ss.priv", "w");
            break;
        }
    }

    // 3. "using fchmod() and fileno(), make sure that the private key file perms are set to 0600, no perms for anyone other than user"
    fchmod(fileno(pvfile), 0600);
    // 4. "initialize randstate using randstate_init()"
    randstate_init(def_seed);

    mpz_t p, q, n, pq, d, username;
    mpz_inits(p, q, n, pq, d, username, NULL);

    // 5. "make public and private keys using ss_make_pub() / ss_make_priv()"
    do {
        ss_make_pub(p, q, n, nbits, iters);
    } while (mpz_sizeinbase(n, 2) < nbits);
    ss_make_priv(d, pq, p, q);
    // 6. "get current user's name as a string, using getenv()"
    char *user = getenv("USER");
    mpz_set_str(username, user, 62);
    // 7. write the computed public and private key to their respective files

    // 8. "if verbose output is enabled print with trailing newline, username, first large prime p, second large prime q, public key n, private exponent d, private modulus pq"
    if (verbose) {
        fprintf(stderr, "user = %s\n", user);
        gmp_fprintf(stderr, "p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_fprintf(stderr, "q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_fprintf(stderr, "n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
        gmp_fprintf(stderr, "pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
    }

    // 9. "close the public and private key files, clear randstate and clear mpz_t vars"
    fclose(pvfile);
    fclose(pbfile);
    randstate_clear();
    mpz_clears(p, q, n, d, pq, username, NULL);
    return 0;
}
