#include "core.h"

void HASH256(const char *msg, uint8_t *digest) {

	core::hash256 sh256;

	core::HASH256_init(&sh256);
	for (int i = 0; msg[i] != 0; i++) core::HASH256_process(&sh256, msg[i]);
	core::HASH256_hash(&sh256, digest);
}

/*
#include "hash.hpp"
#include <stdio.h>

char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

int main()
{
	char digest[32];
	int i;

	HASH256(test256, digest);

	for (i=0;i<32;i++) printf("%02x",(unsigned char)digest[i]);
	printf("\n");

	return 0;
}
*/