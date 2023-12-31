#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#define ROUND 10
#define KEYSIZE 16
#define BLOCKSIZE 16
#define KEY_SCHEDULE_ROUND 15

const uint32_t rc[ROUND] = {0x01000000, 0x02000000,0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};
const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
const uint8_t invSbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

uint32_t* KeyExpansion(const uint8_t* key);
uint8_t* encrypt(const char* plaintext, uint8_t* key);
void subBytes(uint8_t state[4][4]);
void shiftRows(uint8_t state[4][4]);
void mixColumns(uint8_t state[4][4]);
void addRoundKey(int round, uint8_t state[4][4], uint32_t* key);
void invSubBytes(uint8_t state[4][4]);
void invMixColumn(uint8_t state[4][4]);
void invShiftRows(uint8_t state[4][4]);
void pad(const char* s, size_t size);
uint32_t RotWord(uint32_t words);
uint32_t SubWord(uint32_t words);
uint8_t x_time(uint8_t b, uint8_t n);

uint8_t x_time(uint8_t b, uint8_t n) {
	int i;
	uint8_t temp = 0, mask = 0x01;

	for (i = 0; i < 8; i++) {
		if (n & mask)
			temp ^= b;

		if (b & 0x80)
			b = (b << 1) ^ 0x1B;
		else
			b <<= 1;

		mask <<= 1;
	}

	return temp;
}

uint32_t RotWord(uint32_t words) {
    uint8_t tmp[4] = {(words >> 24) & 0xff, (words >> 16) & 0xff, (words >> 8) & 0xff, words & 0xff};
    uint8_t retBlock[4] = {0, };
    uint32_t ret = 0;
    retBlock[3] = tmp[0];
    retBlock[2] = tmp[3];
    retBlock[1] = tmp[2];
    retBlock[0] = tmp[1];
    for(int i = 0; i < 4; i++)
        ret |= (retBlock[i] << (8 * (3-(i%4))));
    return ret;
}

uint32_t SubWord(uint32_t words) {
    uint8_t retBlock[4] = {0, };
    uint32_t ret = 0;
    retBlock[3] = sbox[words & 0xff];
    retBlock[2] = sbox[(words >> 8) & 0xff];
    retBlock[1] = sbox[(words >> 16) & 0xff];
    retBlock[0] = sbox[(words >> 24) & 0xff];
    for(int i = 0; i < 4; i++)
        ret |= (retBlock[i] << (8 * (3-(i%4))));
    return ret;
}

uint32_t* KeyExpansion(const uint8_t * key) {
    uint32_t K[4] = {0, };
    static uint32_t W[4*ROUND + 5] = {0, };
    for(int i = 0; i < 4; i++) {
        uint32_t t = 0;
        for(int j = i*4; j < i*4+4; j++) 
            t |= ((uint8_t) key[j] << (8 * (3-(j%4))));
        K[i] = t;
    }
    for(int i = 0; i < 4*(ROUND + 1); i++) {
        if(i < 4) W[i] = K[i];
        else if(i >= 4 && (i % 4 == 0)) W[i] = (((W[i - 4] ^ SubWord(RotWord(W[i - 1])) & 0xffffffff) & 0xffffffff) ^ rc[i/4 - 1]) & 0xffffffff;
        else W[i] = (W[i - 4] ^ W[i - 1]);
    }
    return W;
}

void addRoundKey(int round, uint8_t state[4][4], uint32_t* key) { 
    for(int i = 0; i < 4 ; i++) {
        uint32_t shift = 24;
        uint32_t mask = 0xff000000;
        for(int j = 0; j < 4; j++) {
            state[i][j] = (((key[4*round + i] & mask) >> shift) ^ state[i][j]) & 0xff;
            mask >>= 8;
            shift -= 8;
        }
    }
}

void subBytes(uint8_t state[4][4]) {
    for(int i = 0; i < 4; i++) 
        for(int j = 0; j < 4; j++)
            state[i][j] = sbox[state[i][j]];
}
void invSubBytes(uint8_t state[4][4]) {
    for(int i = 0; i < 4; i++) 
        for(int j = 0; j < 4; j++)
            state[i][j] = invSbox[state[i][j]];
}

void invShiftRows(uint8_t state[4][4]) {
    uint8_t t;
    t = state[3][1];
    state[3][1] = state[2][1];
    state[2][1] = state[1][1];
    state[1][1] = state[0][1];
    state[0][1] = t;

    t = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = t;

    t = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = t;

    t = state[0][3];
    state[0][3] = state[1][3];
    state[1][3] = state[2][3];
    state[2][3] = state[3][3];
    state[3][3] = t;
}

void invMixColumns(uint8_t state[4][4]) {
    int i, j, k;
	uint8_t a[4][4] = { 
        {0x0e, 0x0b, 0x0d, 0x09},
        {0x09, 0x0e, 0x0b, 0x0d},
        {0x0d, 0x09, 0x0e, 0x0b},
        {0x0b, 0x0d, 0x09, 0x0e}
    };

	for (i = 0; i < 4; i++) {
		uint8_t temp[4] = { 0, };

		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				temp[j] ^= x_time(state[i][k], a[j][k]);

		state[i][0] = temp[0];
		state[i][1] = temp[1];
		state[i][2] = temp[2];
		state[i][3] = temp[3];
	}
}
void shiftRows(uint8_t state[4][4]) { 
    uint8_t t = state[0][1];
    state[0][1] = state[1][1];
    state[1][1] = state[2][1];
    state[2][1] = state[3][1];
    state[3][1] = t;

    t = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = t;

    t = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = t;

    t = state[0][3];
    state[0][3] = state[3][3];
    state[3][3] = state[2][3];
    state[2][3] = state[1][3];
    state[1][3] = t;
}

void mixColumns(uint8_t state[4][4]) {
	int i, j, k;
	uint8_t a[4][4] = { 0x2, 0x3 , 0x1, 0x1,
		0x1, 0x2, 0x3, 0x1,
		0x1, 0x1, 0x2, 0x3,
		0x3, 0x1, 0x1, 0x2 };

	for (i = 0; i < 4; i++) {
		uint8_t temp[4] = { 0, };

		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				temp[j] ^= x_time(state[i][k], a[j][k]);

		state[i][0] = temp[0];
		state[i][1] = temp[1];
		state[i][2] = temp[2];
		state[i][3] = temp[3];
	}
}

/* PKCS#7 */
void pad(const char* s, size_t size) {
    size_t padLength = size - (strlen(s) % size);
    char *tmp = malloc(sizeof(char) * size);
    memset(tmp, padLength, padLength);
    strcat(s, tmp);
    free(tmp);
}

uint8_t* encrypt(const char* plaintext, uint8_t* key) {
    uint32_t* roundKey = KeyExpansion(key);
    uint8_t state[4][4] = {0, };
    for(int i = 0; i < 4; i++) 
        for(int j = 0; j < 4; j++) 
            state[i][j] = plaintext[i*4 + j];
    addRoundKey(0, state, roundKey);
    for(int round = 1; round < ROUND; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(round, state, roundKey);
    }
    subBytes(state);
    shiftRows(state);
    addRoundKey(ROUND, state, roundKey);
    static uint8_t ciphertext[16];
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            ciphertext[4*i + j] = state[i][j];
    return ciphertext;
}

uint8_t* decrypt(const char* ciphertext, uint8_t* key) {
    uint32_t* roundKey = KeyExpansion(key);
    uint8_t state[4][4] = {0, };
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            state[i][j] =ciphertext[i*4 + j];
    addRoundKey(ROUND, state, roundKey); 
    for(int round = ROUND - 1; round > 0; round--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(round, state, roundKey);
        invMixColumns(state);
    }
    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(0, state, roundKey);
    static uint8_t plaintext[16];
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            plaintext[4*i + j] = state[i][j];
    return plaintext;
}

int main() {
    uint8_t* key = "BBBBBBBBBBBBBBBB";
    uint8_t* plaintext = "AAAAAAAAAAAAAAAA";
    uint8_t* ciphertext = "";
    ciphertext = encrypt(plaintext, key);
    printf("[*] ciphertext (hex) : ");
    for(int i = 0; i < 16; i++)
        printf("%x", ciphertext[i]);
    printf("\n");
    uint8_t* plaintext2 = "";
    plaintext2 = decrypt(ciphertext, key);
    printf("[*] plaintext (hex) : ");
    for(int i = 0; i < 16; i++)
        printf("%x", plaintext2[i]);
}