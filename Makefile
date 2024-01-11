CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp) -lm

all: keygen encrypt decrypt 

keygen: keygen.o ss.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)
	
decrypt: decrypt.o ss.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)	
	
encrypt: encrypt.o ss.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f keygen encrypt decrypt *.o
	
cleankeys:
	rm -f *.{pub,priv}

format:
	clang-format -i -style=file *.[ch]

