#include "stdint.h"
#include "stdio.h"

#include "include.h"



uint64_t rstate = 6789175398151;

uint64_t rng(){
	rstate  = (rstate * 9395819399113) - 49458174917981;
	int rt  =  rstate % 64;
	rstate  = (rstate >> rt) | (rstate << (64-rt));
	rstate ^=  rstate >> 17;
	
	return rstate ^ 135891753891;
}

float randFlt(){
	float  n = rng() % 1048576;
	return n / 1048576;
}

int abs(int x){
	return (x < 0)? -x : x;
}






Model    makeModel(int size, int bit){
	Model ret;
	bit  = (size >  64)?  64 :  bit;
	size = (size > 512)? 512 : size;
	for(int i = 0; i < size; i++){
		int a, b, c;
		a = rng() % (bit * 2);
		b = rng() % (bit * 2);
		c = rng() % (bit * 2);
		a = (a >= bit)? a-(bit+bit) : a+1;
		b = (b >= bit)? b-(bit+bit) : b+1;
		c = (c >= bit)? c-(bit+bit) : c+1;
		
		ret.cs[i] = (Clause){a, b, c};
	}
	ret.size = size;
	ret.bit  = bit;
	return ret;
}


void printModel(Model m){
	printf("M : %i, %i\n", m.size, m.bit);
	for(int i = 0; i < m.size; i++)
		printf("%i %i %i\n", m.cs[i].a, m.cs[i].b, m.cs[i].c);
}

uint64_t clauseMask(Clause c){
	uint64_t am = 1l << (abs(c.a)-1);
	uint64_t bm = 1l << (abs(c.b)-1);
	uint64_t cm = 1l << (abs(c.c)-1);
	return am | bm | cm;
}

uint64_t clauseVal(Clause c){
	uint64_t am = 1l << (abs(c.a)-1);
	uint64_t bm = 1l << (abs(c.b)-1);
	uint64_t cm = 1l << (abs(c.c)-1);

	uint64_t av = am & ((c.a < 0)? 0 : -1);
	uint64_t bv = bm & ((c.b < 0)? 0 : -1);
	uint64_t cv = cm & ((c.c < 0)? 0 : -1);
	return av | bv | cv;
}

int isMatch(uint64_t x, Clause c){
	uint64_t m = clauseMask(c);
	uint64_t v = clauseVal (c);
	
	return (~(x ^ v) & m) != 0;
}


uint64_t maskIter(uint64_t x, uint64_t m){
	return m & (x - m);
}


uint64_t next(uint64_t x, Clause c){
	if(isMatch(x, c)) return x;
	
	uint64_t m = clauseMask(c);
	uint64_t v = clauseVal (c);
	
	uint64_t y = x & m;
	for(int i = 0; i < 8; i++){
		y = maskIter(y, m);
		if((~(y ^ v) & m)) break;
	}
	
	
	//printf("%08lx, %08lx, %08lx\n", m, v, d);
	
	return y;
}






