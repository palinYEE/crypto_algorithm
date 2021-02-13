#include<stdio.h>

#define KEY_LENGTH 256
#define Nb 16

#define bitPLUS32(x,y) (x + y) % (0x100000000)
#define bitMINUS32(x,y) (x - y) % (0x100000000)

#if KEY_LENGTH == 128
	#define NK 16
	#define Nr 24
	unsigned char K[NK] = { 0x0f, 0x1e,0x2d,0x3c,0x4b,0x5a,0x69,0x78,0x87,0x96,0xa5,0xb4, 0xc3,0xd2,0xe1,0xf0 };

#elif KEY_LENGTH == 192
	#define NK 24
	#define Nr 28
	unsigned char K[NK] = { 0x0f, 0x1e,0x2d,0x3c,0x4b,0x5a,0x69,0x78,0x87,0x96,0xa5,0xb4, 0xc3,0xd2,0xe1,0xf0, 0xf0, 0xe1 , 0xd2 , 0xc3 , 0xb4 , 0xa5 , 0x96 , 0x87 };

#elif KEY_LENGTH == 256
	#define NK 32
	#define Nr 32
	unsigned char K[NK] = { 0x0f, 0x1e,0x2d,0x3c,0x4b,0x5a,0x69,0x78,0x87,0x96,0xa5,0xb4, 0xc3,0xd2,0xe1,0xf0, 0xf0, 0xe1 ,0xd2 ,0xc3 ,0xb4 ,0xa5 ,0x96 ,0x87,0x78, 0x69, 0x5a, 0x4b , 0x3c , 0x2d , 0x1e , 0x0f };

#endif

unsigned int RK_ENC[Nr][6] = { 0, };
unsigned char PlainText[16] = { 0x30,0x31 ,0x32 ,0x33 ,0x34 ,0x35 ,0x36 ,0x37 ,0x38 ,0x39 ,0x3a ,0x3b ,0x3c ,0x3d ,0x3e ,0x3f };
unsigned char CipherText[16] = { 0, };

unsigned int RK_CONST[8] = {
	0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec, 0x715ea49e, 0xc785da0a, 0xe04ef22a, 0xe5c40957
};

unsigned int ROL(int i, unsigned int x) {
	if (i == 0 || i == 32)
		return x;
	if (i > 32)
		i = i - 32;
	return (x << i) ^ (x >> (32 - i));
}
unsigned int ROR(int i, unsigned int x) {
	if (i == 0 || i == 32)
		return x;
	if (i > 32)
		i = i - 32;
	return (x >> i) ^ (x << (32 - i));
}

int Round_ENC(unsigned int X[4], unsigned int RK_ENC[Nr][6],int round) {
	unsigned int temp = X[0];
	X[0] = ROL(9, bitPLUS32((X[0] ^ RK_ENC[round][0]), (X[1] ^ RK_ENC[round][1])));
	X[1] = ROR(5, bitPLUS32((X[1] ^ RK_ENC[round][2]), (X[2] ^ RK_ENC[round][3])));
	X[2] = ROR(3, bitPLUS32((X[2] ^ RK_ENC[round][4]), (X[3] ^ RK_ENC[round][5])));
	X[3] = temp;

	return 0;
}

int Round_DEC(unsigned int X[4], unsigned int RK_ENC[Nr][6], int round) {
	unsigned int Next_X[4] = { 0, };
	Next_X[0] = X[3];
	Next_X[1] = (bitMINUS32(ROR(9, X[0]), (Next_X[0] ^ RK_ENC[round][0])) ^ RK_ENC[round][1]);
	Next_X[2] = (bitMINUS32(ROL(5, X[1]), (Next_X[1] ^ RK_ENC[round][2])) ^ RK_ENC[round][3]);
	Next_X[3] = (bitMINUS32(ROL(3, X[2]), (Next_X[2] ^ RK_ENC[round][4])) ^ RK_ENC[round][5]);

	for (int i = 0; i < 4; i++)
		X[i] = Next_X[i];

	return 0;
}

int KEYSCHEDULE_128(unsigned char K[NK],unsigned int RK_ENC[Nr][6]) {

	unsigned int T[4] = { 0, };
	for (int i = 0; i < 4; i++)
		T[i] = (K[4*i+3]<<24)^ (K[4 * i + 2] << 16)^ (K[4 * i + 1] << 8)^K[4*i];

	for (int i = 0; i < Nr; i++) {
		T[0] = ROL(1, bitPLUS32(T[0], ROL(i,RK_CONST[i % 4])));
		T[1] = ROL(3, bitPLUS32(T[1], ROL(i+1,RK_CONST[i % 4])));
		T[2] = ROL(6, bitPLUS32(T[2], ROL(i+2, RK_CONST[i % 4])));
		T[3] = ROL(11, bitPLUS32(T[3], ROL(i+3, RK_CONST[i % 4])));

		RK_ENC[i][0] = T[0];
		RK_ENC[i][1] = T[1]; 
		RK_ENC[i][2] = T[2]; 
		RK_ENC[i][3] = T[1]; 
		RK_ENC[i][4] = T[3]; 
		RK_ENC[i][5] = T[1];
	}
	return 0;
}

int KEYSCHEDULE_192(unsigned char K[NK], unsigned int RK_ENC[Nr][6]) {
	unsigned int T[6] = { 0, };
	for (int i = 0; i < 6; i++)
		T[i] = (K[4 * i + 3] << 24) ^ (K[4 * i + 2] << 16) ^ (K[4 * i + 1] << 8) ^ K[4 * i];

	for (int i = 0; i < Nr; i++) {
		T[0] = ROL(1, bitPLUS32(T[0], ROL(i, RK_CONST[i % 6])));
		T[1] = ROL(3, bitPLUS32(T[1], ROL(i + 1, RK_CONST[i % 6])));
		T[2] = ROL(6, bitPLUS32(T[2], ROL(i + 2, RK_CONST[i % 6])));
		T[3] = ROL(11, bitPLUS32(T[3], ROL(i + 3, RK_CONST[i % 6])));
		T[4] = ROL(13, bitPLUS32(T[4], ROL(i + 4, RK_CONST[i % 6])));
		T[5] = ROL(17, bitPLUS32(T[5], ROL(i + 5, RK_CONST[i % 6])));

		RK_ENC[i][0] = T[0]; RK_ENC[i][1] = T[1]; RK_ENC[i][2] = T[2];
		RK_ENC[i][3] = T[3]; RK_ENC[i][4] = T[4]; RK_ENC[i][5] = T[5];
	}
	return 0;
}

int KEYSCHEDULE_256(unsigned char K[NK], unsigned int RK_ENC[Nr][6]) {
	unsigned int T[8] = { 0, };
	for (int i = 0; i < 8; i++)
		T[i] = (K[4 * i + 3] << 24) ^ (K[4 * i + 2] << 16) ^ (K[4 * i + 1] << 8) ^ K[4 * i];

	for (int i = 0; i < Nr; i++) {
		T[(6 * i) % 8] = ROL(1, bitPLUS32(T[(6 * i) % 8], ROL(i, RK_CONST[i % 8])));
		T[(6 * i + 1) % 8] = ROL(3, bitPLUS32(T[(6 * i + 1) % 8], ROL(i + 1, RK_CONST[i % 8])));
		T[(6 * i + 2) % 8] = ROL(6, bitPLUS32(T[(6 * i + 2) % 8], ROL(i + 2, RK_CONST[i % 8])));
		T[(6 * i + 3) % 8] = ROL(11, bitPLUS32(T[(6 * i + 3) % 8], ROL(i + 3, RK_CONST[i % 8])));
		T[(6 * i + 4) % 8] = ROL(13, bitPLUS32(T[(6 * i + 4) % 8], ROL(i + 4, RK_CONST[i % 8])));
		T[(6 * i + 5) % 8] = ROL(17, bitPLUS32(T[(6 * i + 5) % 8], ROL(i + 5, RK_CONST[i % 8])));

		RK_ENC[i][0] = T[(6 * i) % 8]; RK_ENC[i][1] = T[(6 * i + 1) % 8]; RK_ENC[i][2] = T[(6 * i + 2) % 8];
		RK_ENC[i][3] = T[(6 * i + 3) % 8]; RK_ENC[i][4] = T[(6 * i + 4) % 8]; RK_ENC[i][5] = T[(6 * i + 5) % 8];
	}
	return 0;
}

int ENCRYPT(unsigned char PlainText[16], unsigned int RK[Nr][6], unsigned char CipherText[16]) {
	unsigned int X[4] = { 0, };
	for (int i = 0; i < 4; i++)
		X[i] = (PlainText[4 * i + 3] << 24) ^ (PlainText[4 * i + 2] << 16) ^ (PlainText[4 * i + 1] << 8) ^ PlainText[4 * i];

	for (int i = 0; i < Nr; i++) {
		Round_ENC(X, RK_ENC, i);
	}
	for (int i = 0; i < 4; i++) {
		CipherText[4 * i] = X[i] & 0xff;
		CipherText[4 * i + 1] = (X[i] >> 8) & 0xff;
		CipherText[4 * i + 2] = (X[i] >> 16) & 0xff;
		CipherText[4 * i + 3] = (X[i] >> 24) & 0xff;
	}
	return 0;
}
int DECRYPT(unsigned char CipherText[16], unsigned int RK[Nr][6], unsigned char PlainText[16]) {
	unsigned int X[4] = { 0, };
	for (int i = 0; i < 4; i++)
		X[i] = (CipherText[4 * i + 3] << 24) ^ (CipherText[4 * i + 2] << 16) ^ (CipherText[4 * i + 1] << 8) ^ CipherText[4 * i];

	for (int i = Nr-1; i >= 0; i--) {
		Round_DEC(X, RK_ENC, i);
	}
	for (int i = 0; i < 4; i++) {
		PlainText[4 * i] = X[i] & 0xff;
		PlainText[4 * i + 1] = (X[i] >> 8) & 0xff;
		PlainText[4 * i + 2] = (X[i] >> 16) & 0xff;
		PlainText[4 * i + 3] = (X[i] >> 24) & 0xff;
	}
	return 0;
}
int main() {
	KEYSCHEDULE_256(K, RK_ENC);
	ENCRYPT(PlainText, RK_ENC, CipherText);
	printf("\n===================CipherText=================\n");
	for (int i = 0; i < 16; i++)
		printf("%02x ", CipherText[i]);

	DECRYPT(CipherText, RK_ENC, PlainText);
	printf("\n===================PlainText=================\n");
	for (int i = 0; i < 16; i++)
		printf("%02x ", PlainText[i]);
}