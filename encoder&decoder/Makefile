# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c 
LDFLAGS=-lcrypto

all: convertHap2Set

convertHap2Set : convertHap2Set.o
	$(CC)  convertHap2Set.o -o convertHap2Set $(LDFLAGS)

convertHap2Set.o : convertHap2Set.cxx 
	$(CC) $(CFLAGS) convertHap2Set.cxx

clean:
	rm -rf *o *.o
