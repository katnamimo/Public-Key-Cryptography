#include "ss.h"
#include "numtheory.h"
#include "randstate.h"

#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    mpz_t p_prime, q_prime;
    gmp_randstate_t state;
    gmp_randinit_default(state);
    mpz_inits(p_prime, q_prime, NULL);
    do {
        uint64_t bits = (gmp_urandomb_ui(state, (nbits / 5)) + (nbits / 5));
        uint64_t prime_bits = nbits - (2 * bits);
        make_prime(p, bits, iters);
        make_prime(q, prime_bits, iters);

        mpz_sub_ui(p_prime, p, 1);
        mpz_sub_ui(q_prime, q, 1);

        mpz_mul(n, p, q);
    } while (mpz_divisible_p(q, p_prime) || mpz_divisible_p(p, q_prime));
    mpz_clears(p_prime, q_prime, NULL);
    gmp_randclear(state);
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    char *hexstring = mpz_get_str(NULL, 16, n);
    gmp_fprintf(pbfile, "%s\n", hexstring);
    gmp_fprintf(pbfile, "%s\n", username);
    free(hexstring);
    return;
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    char hexstring[mpz_sizeinbase(n, 16) + 2];
    fgets(hexstring, sizeof(hexstring), pbfile);
    hexstring[strcspn(hexstring, "\n")] = '\0';
    mpz_set_str(n, hexstring, 16);
    fgets(username, 256, pbfile);
    return;
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t p_prime, q_prime, pq_prime, x, y;
    mpz_inits(p_prime, q_prime, pq_prime, x, y, NULL);
    mpz_sub_ui(p_prime, p, 1);
    mpz_sub_ui(q_prime, q, 1);
    mpz_mul(pq_prime, p_prime, q_prime);
    mpz_mul(pq, p, q);
    gcd(x, p_prime, q_prime);
    mpz_fdiv_q(x, pq_prime, x);
    mpz_mul(y, p, pq);
    mod_inverse(d, y, x);
    mpz_clears(x, y, p_prime, q_prime, pq_prime, NULL);
    return;
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", pq);
    gmp_fprintf(pvfile, "%Zx\n", d);
    return;
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", pq);
    gmp_fscanf(pvfile, "%Zx", d);
    return;
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
    return;
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t x, y, msg;
    mpz_inits(y, msg, NULL);
    mpz_init_set_ui(x, (log2(sqrt(mpz_get_ui(n))) - 1 / 4 * 2));
    uint8_t *array = (uint8_t *) malloc(mpz_get_ui(x));
    array[0] = 0xFF; //buffer
    while (!feof(infile)) {
        uint64_t t = fread(array + 1, 1, mpz_get_ui(x) - 1, infile);
        mpz_import(y, t + 1, 1, 1, 1, 0, array);
        ss_encrypt(msg, y, n);
        gmp_fprintf(outfile, "%Zx\n", msg);
    }

    mpz_clears(x, y, msg, NULL);
    free(array);
    array = NULL;
    return;
}
void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
    return;
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t x, y, msg;
    mpz_init_set_ui(x, (log2(mpz_get_ui(pq)) - 1) / (4 * 2));
    mpz_inits(y, msg, NULL);
    uint8_t *array = (uint8_t *) malloc(mpz_get_ui(x));
    while (!feof(infile)) {
        gmp_fscanf(infile, "%Zx\n", y);
        uint64_t t;
        ss_decrypt(msg, y, d, pq);
        mpz_export(array, &t, 1, 1, 1, 0, msg);
        fwrite(array + 1, 1, t - 1, outfile);
    }

    mpz_clears(x, y, msg, NULL);
    free(array);
    array = NULL;
    return;
}
