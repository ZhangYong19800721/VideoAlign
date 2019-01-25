/*
 * hamming.cpp
 *
 *  compute the hamming distance of a 64bit unsigned integer
 */

unsigned int hamming(const unsigned long A) {
	unsigned long B,Ch,D;
	static const unsigned long AA = 0x5555555555555555; //01010101
	static const unsigned long BB = 0x3333333333333333; //00110011
	static const unsigned long CC = 0x0F0F0F0F0F0F0F0F; //00001111
	static const unsigned long DD = 0x00FF00FF00FF00FF; //00001111
	static const unsigned long EE = 0x0000FFFF0000FFFF;
	static const unsigned long FF = 0x00000000FFFFFFFF;

	B = A & AA;
	Ch = (A >> 1) & AA; //(1) sum of 2 neighbor
	D = B + Ch;

	B = D & BB;
	Ch = (D >> 2) & BB; //(2) sum of 4 neighbor
	D = B + Ch;

	B = D & CC;
	Ch = (D >> 4) & CC; //(3) sum of 8 neighbor
	D = B + Ch;

	B = D & DD;
	Ch = (D >> 8) & DD; //(4) sum of 16 neighbor
	D = B + Ch;

	B = D & EE;
	Ch = (D >> 16) & EE; //(5) sum of 32 neighbor
	D = B + Ch;

	B = D & FF;
	Ch = (D >> 32) & FF; //(6) sum of 64 neighbor
	D = B + Ch;
	return D;
}

