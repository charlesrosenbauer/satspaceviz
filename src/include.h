#ifndef __HEADER__
#define __HEADER__


typedef struct{
	int8_t a, b, c;
}Clause;

typedef struct{
	uint64_t	state;
	Clause		cs[512];
	int			size, bit;
}Model;


uint64_t rng       ();
float    randFlt   ();

Model    makeModel (int, int);
void	 printModel(Model);

int      isMatch (uint64_t, Clause);
uint64_t maskIter(uint64_t, uint64_t);
uint64_t next    (uint64_t, Clause);


#endif
