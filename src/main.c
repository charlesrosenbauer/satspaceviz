#include "SDL/SDL.h"

#include "stdint.h"
#include "stdlib.h"
#include "math.h"

#include "include.h"



typedef struct{
	float x, y, z;
}Vec3;

typedef struct{
	Vec3 x, y, z;
}Mat3;

float distSqr(Vec3 a, Vec3 b){
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return (dx * dx) + (dy * dy) + (dz * dz);
}

Vec3 addVec(Vec3 a, Vec3 b){
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vec3 subVec(Vec3 a, Vec3 b){
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

Vec3 mulVec(Vec3 a, float b){
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}

Vec3 absVec(Vec3 a){
	a.x = (a.x < 0)? -a.x : a.x;
	a.y = (a.y < 0)? -a.y : a.y;
	a.z = (a.z < 0)? -a.z : a.z;
	return a;
}

Vec3 matmul(Vec3 a, Mat3 m){
	Vec3 ret;
	ret.x = (a.x * m.x.x) + (a.y * m.y.x) + (a.z * m.z.x);
	ret.y = (a.x * m.x.y) + (a.y * m.y.y) + (a.z * m.z.y);
	ret.z = (a.x * m.x.z) + (a.y * m.y.z) + (a.z * m.z.z);
	return ret;
}

/*
	I'm not going to bother with full matrix multiplication now
*/
Mat3 rotX(float x){
	Mat3 ret;
	ret.x = (Vec3){      1,      0,       0};
	ret.y = (Vec3){      0,  cos(x),  sin(x)};
	ret.z = (Vec3){      0, -sin(x),  cos(x)};
	return ret;
}

Mat3 rotY(float y){
	Mat3 ret;
	// transpose
	ret.x = (Vec3){ cos(y),      0,  sin(y)};
	ret.y = (Vec3){      0,      1,       0};
	ret.z = (Vec3){-sin(y),      0,  cos(y)};
	return ret;
}

Mat3 rotZ(float z){
	Mat3 ret;
	// transpose
	ret.x = (Vec3){ cos(z), -sin(z),       0};
	ret.y = (Vec3){ sin(z),  cos(z),       0};
	ret.z = (Vec3){      0,       0,       0};
	return ret;
}



typedef struct{
	float*	dist;
	int		size;
}Graph;

Graph makeGraph(int size){
	Graph ret;
	ret.dist  = malloc(sizeof(float) * size * size);
	ret.size  = size;
	for(int i = 0; i < (size*size); i++) ret.dist[i] = 0;
	return ret;
}

void setGDist(Graph* g, int a, int b, float d){
	g->dist[(a * g->size) + b] = d;
	g->dist[(b * g->size) + a] = d;
}





Vec3 randStep(Vec3 v, float f){
	float x = randFlt() + 0.0001;
	float y = randFlt() + 0.0001;
	float z = randFlt() + 0.0001;
	float d = sqrt(distSqr((Vec3){x, y, z}, (Vec3){0, 0, 0}));
	x      *= (f / d);
	y      *= (f / d);
	z      *= (f / d);
	return (Vec3){v.x+x, v.y+y, v.z+z};
}


void listGStruct(Graph* g){
	for(int i = 0; i < g->size; i++){
		printf("V %i : ", i);
		for(int j = 0; j < g->size; j++){
			if(g->dist[(i*g->size)+j] > 0.001){
				printf("%i, ", j);
			}
		}
		printf("\n");
	}
}


void initialPos(Vec3* ps, Graph* g){
	for(int i = 0; i < g->size; i++) ps[i] = (Vec3){0.0, 0.0, 0.0};
	
	for(int i = 0; i < g->size; i++){
		if(i == 0){
			
		}else if((ps[i].x == 0.0) && (ps[i].y == 0.0) && (ps[i].z == 0.0)){
			ps[i] = (Vec3){randFlt(), randFlt(), randFlt()};
			for(int j = 1; j < g->size; j++){
				int ix = (i*g->size)+j;
				if(g->dist[ix] > 0.0001){
					if((ps[j].x == 0.0) && (ps[j].y == 0.0) && (ps[j].z == 0.0)){
						ps[j] = randStep(ps[i], g->dist[ix]);
					}
				}
			}
		}
	}
}


float getScale(Vec3* ps, Vec3* center, int size){
	*center = (Vec3){0, 0, 0};
	for(int i = 0; i < size; i++){
		*center = addVec(*center, ps[i]);
	}
	*center = mulVec(*center, 1.0 / size);
	
	float dev  = 0;
	for(int i  = 0; i < size; i++){
		Vec3 d = absVec(subVec(*center, ps[i]));
		dev    = (d.x > dev)? d.x : dev;
		dev    = (d.y > dev)? d.y : dev;
		dev    = (d.z > dev)? d.z : dev;
	}
	return dev;
}


void draw(uint32_t* pix, Vec3* ps, int size){
	Vec3 camera = (Vec3){0, 0, 1};

	Vec3 center;
	float scale = 0.5 / getScale(ps, &center, size);
	for(int i = 0; i < size; i++){
		Vec3 p =  mulVec(subVec(ps[i], center), scale);
		p      =  subVec(p, camera);
		p.x   /=  p.z;
		p.y   /=  p.z;
		int x  = (p.x * 384) + 384;
		int y  = (p.y * 384) + 384;
		if((x >= 0) && (x < 768) && (y >= 0) && (y < 768)) pix[(y * 768) + x] = 0xffffff;
	}
}


float move(Graph* g, Vec3* ps, float t, int size){
	float pot = 0;
	Vec3*  ds = alloca(sizeof(Vec3) * size);
	for(int i = 0; i < size; i++) ds[i] = (Vec3){0, 0, 0};
	
	for(int i = 0; i < size; i++){
		for(int j = i+1; j < size; j++){
			int ix = (i * size) + j;
			float s = g->dist[ix];
			if(s > 0.0001){
				float dist = sqrt(distSqr(ps[i], ps[j]));
				pot       += fabs(dist - s);
				Vec3  diff = mulVec(subVec(ps[i], ps[j]), dist);
				ds[i]      = addVec(ds[i], mulVec(diff, -t));
				ds[j]      = addVec(ds[j], mulVec(diff,  t));
			}
		}
	}
	for(int i = 0; i < size; i++)
		ps[i] = addVec(ps[i], ds[i]);
	
	return pot;
}







int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen = SDL_SetVideoMode(512, 512, 32, 0);
	
	uint32_t* pix = screen->pixels;
	
	Model m = makeModel(64, 18);
	//printModel(m);
	
	{
		uint64_t x = 0;
		Clause   c = (Clause){1, 2, 4};
		for(int  i = 0; i < 64; i++){
			uint64_t y = next(x, c);
			x = (x == y)? y+1 : y;
			if(x != y) printf("%i %08lx\n", i, y);
		}
	}
	/*
	for(int t = 0; t < 64; t++){
		SDL_FillRect(screen, 0, 0);
	
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) t = 64;
		}
	
		m.size = t;
		for(int i = 0; i < 262144; i++){
			uint64_t x = i;
			int   pass = 1;
			for(int j = 0; j < m.size; j++){
				if(!isMatch(x, m.cs[j])){
					pass = 0;
					break;
				}
			}
			if(pass){
				pix[i] = 0xffffff;
			}
		}
		SDL_Flip(screen);
		SDL_Delay(100);
	}*/
	
	/*
	Vec3 points[1024];
	Graph g = makeGraph(1024);
	
	for(int i = 0; i < 1024; i++){
		for(int j = 0; j < 5; j++){
			if(i) setGDist(&g, i-1, i, 1.0);
			int a = i - (rng() % 16);
			if(a > 0){
				setGDist(&g, a, i, 1.0);
			}
		}
	}
	
	listGStruct(&g);
	initialPos(points, &g);
	draw(pix, points, 1024);
	
	float anneal = 0.1;
	int cont = 1;
	while(cont){
		SDL_FillRect(screen, 0, 0);
	
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) cont = 0;
		}
		
		if(anneal > 0.16){
			anneal -= 0.005;
		}else if(anneal > 0.10){
			anneal -= 0.003;
		}else if(anneal > 0.01){
			anneal -= 0.001;
		}
		
		float pot = move(&g, points, anneal, 1024);
		printf("%f\n", pot);
		draw(pix, points, 1024);
		
		SDL_Flip(screen);
		SDL_Delay(16);
	}
	SDL_Quit();*/
}