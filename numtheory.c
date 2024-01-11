#include "numtheory.h"
#include "randstate.h"

#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
// all based off of asgn pdf pseudocode
// use temp vars for const vars, otherwise type interference
void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t t, temp_a, temp_b;
    mpz_inits(t, temp_a, temp_b, NULL);
    mpz_set_ui(t, 0);
    mpz_set(temp_a, a);
    mpz_set(temp_b, b);

    while (mpz_cmp_ui(temp_b, 0) != 0) {
        mpz_set(t, temp_b);
        mpz_mod(temp_b, temp_a, temp_b);
        mpz_set(temp_a, t);
    }

    mpz_set(g, temp_a);
    mpz_clears(t, temp_a, temp_b, NULL);
    return;
}

//

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r, r_prime, t, t_prime, q, z, mul;
    mpz_inits(r, r_prime, t, t_prime, q, z, mul, NULL);

    mpz_set(r, n);
    mpz_set(r_prime, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);
    while (mpz_cmp_ui(r_prime, 0) != 0) {
        mpz_fdiv_q(q, r, r_prime);
        mpz_set(z, r);
        mpz_set(r, r_prime);
        mpz_mul(mul, q, r_prime);
        mpz_sub(r_prime, z, mul);
        mpz_set(z, t);
        mpz_set(t, t_prime);
        mpz_mul(mul, q, t_prime);
        mpz_sub(t_prime, z, mul);
    }

    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(o, 0);
        mpz_clears(r, r_prime, t, t_prime, q, z, mul, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(o, t);

    mpz_clears(r, r_prime, t, t_prime, q, z, mul, NULL);
    return;
}
// v < 1
// p < a
// while d > 0
//	if Odd(d)
//		v < (v x p) mod n
//	p < (p x p) mod n
//	d < [d/2]
// return v
//
void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, z, temp_d, temp_n;
    mpz_inits(v, p, z, a, d, n, temp_d, temp_n, NULL);
    mpz_set(temp_d, d);
    mpz_set(temp_n, n);
    mpz_set_ui(v, 1);
    mpz_set(p, a);
    while (mpz_cmp_ui(temp_d, 0) > 0) {
        if (mpz_odd_p(temp_d) != 0) {
            mpz_mul(z, v, p);
            mpz_mod(v, z, temp_n);
        }
        mpz_mul(z, p, p);
        mpz_mod(p, z, temp_n);
        mpz_fdiv_q_ui(temp_d, temp_d, 2);
    }
    mpz_set(o, v);
    mpz_clears(v, p, z, a, d, temp_n, temp_d, NULL);
    return;
}

// write n-1 = 2^s such that r is odd
// for i < 1 to k
// 	choose random a is on {2,3,...,n-2}
// 	y = POWER-MOD(a,r,n)
//	if y != 1 and y != n-1
//		j < 1
// 		while j is less than or equal to s -1 and y != n - 1
// 			y < POWER-MOD(y, 2, n)
// 			if == 1
// 				return FALSE
//			j < j + 1
// 		if y != n - 1
// 			return FALSE
// return TRUE

bool is_prime(const mpz_t n, uint64_t iters) {
    if ((mpz_cmp_ui(n, 1) <= 0) || (mpz_cmp_ui(n, 4) == 0)) {
        return false;
    }
    if ((mpz_cmp_ui(n, 3) <= 0)) {
        return true;
    }

    mpz_t temp_n;
    mpz_init(temp_n);
    mpz_set(temp_n, n);
    mpz_t a, x, z, y, n_minus_1, j, s_minus_1, two, r;
    mpz_inits(a, x, z, y, n_minus_1, j, s_minus_1, two, r, NULL);
    mpz_sub_ui(n_minus_1, temp_n, 1);
    mpz_set_ui(two, 2);
    mpz_set_ui(r, 0);
    mp_bitcnt_t s = 0;

    do {
        mpz_fdiv_q_2exp(r, n_minus_1, s);
        s++;
    } while (!mpz_even_p(r));
    s--;

    mpz_fdiv_q_2exp(r, n_minus_1, s);

    // finally test if prime
    for (uint64_t i = 1; i < iters; ++i) {

        mpz_urandomm(a, state, n);
        mpz_sub_ui(z, temp_n, 3);
        mpz_mod(a, a, z);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_minus_1) != 0)) {
            mpz_set_ui(j, 1);
            while ((mpz_cmp(j, s_minus_1) <= 0) && (mpz_cmp(y, n_minus_1) != 0)) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(a, x, z, y, n_minus_1, j, s_minus_1, two, temp_n, r, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, n_minus_1) != 0) {
                mpz_clears(a, x, z, y, n_minus_1, j, s_minus_1, two, temp_n, r, NULL);
                return false;
            }
        }
    }
    mpz_clears(a, x, z, y, n_minus_1, j, s_minus_1, two, temp_n, r, NULL);
    return true;
}

// should be at least bits number of bits long
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_init(p);

    do {
        mpz_urandomb(p, state, bits);
    } while (!is_prime(p, iters));
    mpz_clear(p);
    return;
}
