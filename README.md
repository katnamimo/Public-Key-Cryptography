Kat Pe Benito kpebenit

**Description:** This program encrypts files using a public key,
decrypts files using a private key, and generates said SS public and
private keys. It makes use of the GNU multiple precision arithmetic
library, a library containing mathematical functions for SS, and a
random state module.

**How to Run Program:** In a terminal opened to the directory with asgn5
files, enter 'make' to format C files and executables. 'make clean' may
also be used to delete executables.

**Command Line Options:** **Encrypt:** - -i: specifies the input file to
encrypt (default stdin) - -o: specifies the output file to encrypt
(default stdout) - -n: specifies the file containing the public key
(default: ss.pub) - -v: enables verbose output - -h: displays program
synopsis and usage. **Decrypt:** - -i: specifies the input file to
decrypt (default stdin) - -o: specifies the output file to decrypt
(default stdout) - -n: specifies the file containing the private key
(default: ss.priv) - -v: enables verbose output - -h: displays program
synopsis and usage. **Keygen:** - -b : specifies the minimum bits needed
for the public modulus n. - -i : specifies the number of Miller-Rabin
iterations for testing primes (default: 50). - -n pbfile : specifies the
public key file (default: ss.pub). - -d pvfile : specifies the private
key file (default: ss.priv). - -s : specifies the random seed for the
random state initialization (default: the seconds since the UNIX epoch,
given by time(NULL)). • -v : enables verbose output. • -h : displays
program synopsis and usage.
