#include "SDL2/SDL.h"

#include "stdio.h"
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
	float d  = (dx * dx) + (dy * dy) + (dz * dz);
	if(d < 0.00001) return 0.0001;
	return d;
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

Vec3 matMul(Vec3 a, Mat3 m){
	Vec3 ret;
	ret.x = (a.x * m.x.x) + (a.y * m.x.y) + (a.z * m.x.z);
	ret.y = (a.x * m.y.x) + (a.y * m.y.y) + (a.z * m.y.z);
	ret.z = (a.x * m.z.x) + (a.y * m.z.y) + (a.z * m.z.z);
	return ret;
}

/*
	I'm not going to bother with full matrix multiplication now
*/
Mat3 rotX(float x){
	Mat3 ret;
	ret.x = (Vec3){      1,      0,       0};
	ret.y = (Vec3){      0,  cos(x), -sin(x)};
	ret.z = (Vec3){      0,  sin(x),  cos(x)};
	return ret;
}

Mat3 rotY(float y){
	Mat3 ret;
	ret.x = (Vec3){ cos(y),      0,  sin(y)};
	ret.y = (Vec3){      0,      1,       0};
	ret.z = (Vec3){-sin(y),      0,  cos(y)};
	return ret;
}

Mat3 rotZ(float z){
	Mat3 ret;
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


uint32_t spark = 0xffffff;

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


void drawColors(uint32_t* pix, uint32_t* cs, Vec3* ps, int size, int rx, int ry, int rz){
	Vec3 camera = (Vec3){0, 0, 1};
	
	Mat3 mx = rotX(rx * 0.1);
	Mat3 my = rotY(ry * 0.1);
	Mat3 mz = rotZ(rz * 0.1);

	Vec3 center;
	float scale = 0.5 / getScale(ps, &center, size);
	for(int i = 0; i < size; i++){
		Vec3 p =  mulVec(subVec(ps[i], center), scale);
		p      =  matMul(p, mx);
		p      =  matMul(p, my);
		p      =  matMul(p, mz);
		
		p      =  subVec(p, camera);
		p.x   /=  p.z;
		p.y   /=  p.z;
		int x  = (p.x * 384) + 384;
		int y  = (p.y * 384) + 384;
		if((x >= 1) && (x < 767) && (y >= 1) && (y < 767)){
			pix[(y * 768) + x] = cs[i];
			if(cs[i] == 0xffffff){
				y--;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x  ] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
				
				y++;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
				
				y++;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x  ] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
			}else{
				y--;
				pix[(y * 768) + x  ] = cs[i];
				
				y++;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
				
				y++;
				pix[(y * 768) + x  ] = cs[i];
			}
		}
	}
}

void drawLine(uint32_t* ps, int x0, int y0, int x1, int y1, uint32_t color){
	int dx  = x1 - x0;
	int dy  = y1 - y0;
	float m = ((float)abs(dy)) / ((float)dx);
	
	int   x = x1;
	int   y = y1;
	float c = 0.0;
	if(abs(dx) > abs(dy)){	// from x1 -> x0
		int  ix = (dx < 0)? -1 : 1;
		float c = 0.0;
		if((dx ^ dy) > 0) m = -m;
		for(int i = 0; i < abs(dx); i++){
			x -= ix;
			c -= m;
			y  = y1 - c;
			int p = (y * 768) + x;
			if(!((x < 0) || (x > 767) || (y < 0) || (y > 767))) ps[p] += (color >> 2) & 0x3f3f3f;
		}
	}else{					// from y1 -> y0
		m = 1.0 / m;
		int iy = (dy < 0)? -1 : 1;
		for(int i = 0; i < abs(dy); i++){
			y -= iy;
			c += m;
			x  = x1 - c;
			int p = (y * 768) + x;
			if(!((x < 0) || (x > 767) || (y < 0) || (y > 767))) ps[p] += (color >> 2) & 0x3f3f3f;
		}
	}
}


void drawGraph(Graph* g, uint32_t* pix, uint32_t* cs, Vec3* ps, int size, int rx, int ry, int rz){
	Vec3 camera = (Vec3){0, 0, 0.5};
		
	Mat3 mx = rotX(rx * 0.05);
	Mat3 my = rotY(ry * 0.05);
	Mat3 mz = rotZ(rz * 0.05);
	
	Vec3 center;
	float scale = 0.5 / getScale(ps, &center, size);
	Vec3* xs = alloca(sizeof(Vec3) * size);
	for(int i = 0; i < size; i++){
		Vec3 p =  mulVec(subVec(ps[i], center), scale);
		p      =  matMul(p, mx);
		p      =  matMul(p, my);
		p      =  matMul(p, mz);
		
		p      =  subVec(p, camera);
		p.x   /=  p.z;
		p.y   /=  p.z;
		xs[i]  =  p;
	}
	
	for(int i = 0; i < g->size; i++){
		for(int j = i+1; j < g->size; j++){
			int ix = (i * g->size) + j;
			if(g->dist[ix] > 0.001){
				int x0 = (xs[i].x * 384) + 384;
				int y0 = (xs[i].y * 384) + 384;
				int x1 = (xs[j].x * 384) + 384;
				int y1 = (xs[j].y * 384) + 384;
			
				drawLine(pix, x0, y0, x1, y1, cs[i]);
			}
		}
	}

	
	for(int i = 0; i < size; i++){
		int x  = (xs[i].x * 384) + 384;
		int y  = (xs[i].y * 384) + 384;
		if((x >= 1) && (x < 767) && (y >= 1) && (y < 767)){
			pix[(y * 768) + x] = cs[i];
			if(cs[i] == spark){
				y--;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x  ] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
				
				y++;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
				
				y++;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x  ] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
			}else{
				y--;
				pix[(y * 768) + x  ] = cs[i];
				
				y++;
				pix[(y * 768) + x-1] = cs[i];
				pix[(y * 768) + x+1] = cs[i];
				
				y++;
				pix[(y * 768) + x  ] = cs[i];
			}
		}
	}
}





float move(Graph* g, Vec3* ps, float t, float r, int size){
	float pot = 0;
	Vec3*  ds = alloca(sizeof(Vec3) * size);
	for(int i = 0; i < size; i++) ds[i] = (Vec3){0, 0, 0};
	
	float mindist = 0.0000001;
	
	for(int i = 0; i < size; i++){
		for(int j = i+1; j < size; j++){
			int ix = (i * size) + j;
			float s = g->dist[ix];
			if(s > 0.0001){
				float dist = sqrt(distSqr(ps[i], ps[j]));
				pot       += fabs(dist - s);
				Vec3  diff = mulVec(subVec(ps[i], ps[j]), (dist-s));
				ds[i]      = addVec(ds[i], mulVec(diff, -t));
				ds[j]      = addVec(ds[j], mulVec(diff,  t));
			}else{
				float dsqr = distSqr(ps[i], ps[j]);
				float    f = -r / dsqr;
				f          = (fabs(f) > mindist)? ((f < 0)? -mindist : mindist) : f;
				pot       += f;
				Vec3  diff = mulVec(subVec(ps[i], ps[j]), f);
				ds[i]      = addVec(ds[i], mulVec(diff, -t));
				ds[j]      = addVec(ds[j], mulVec(diff,  t));
			}
		}
	}
	for(int i = 0; i < size; i++)
		ps[i] = addVec(ps[i], ds[i]);
	
	return pot;
}



void updateColorWave(uint32_t* cs, int wave, int size){
	for(int i = 0; i < size; i++){
		int j = (wave + i) % size;
		if(j < 32){
			cs[i] = spark;
		}else{
			cs[i] = 0xff0000 + ((i / 4) * 256);
		}
	}
}


void updateColorFlow(Graph* g, uint32_t* cs, int size, int particles){
	int bitlen   = (size / 64) + ((size % 64) != 0);

	uint64_t* fs = alloca(sizeof(uint64_t) * bitlen);
	for(int i = 0; i < bitlen; i++) fs[i] = 0;
	
	int fill  = 0;
	for(int i = 0; i < size; i++){
		if(cs[i] == spark){
			int ix = rng() & (size-1);
			for(int j = 0; j < size; j++){
				int n = (i * size) + ix;
				if(g->dist[n] > 0.001){
					fs[ix / 64] |= 1l << (ix % 64);
					j  = size;
					fill++;
				}else{
					ix = (ix+1 >= size)? 0 : ix+1;
				}
			}
		}
	}
	
	for(int i = 0; i < size; i++) cs[i] = 0xff0000 + ((i / 4) * 256);
	
	int count = 0;
	for(int i = 0; i < bitlen; i++){
		if(fs[i]){
			for(int j = 0; j < 64; j++){
				if(fs[i] & (1l << j)){
					cs[(i*64)+j] = spark;
					count++;
				}
			}
		}
	}
	
	if(count < particles){
		for(int i = count; i < particles; i++){
			cs[rng() & (size-1)] = spark;
		}
	}
}





int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Window* w = SDL_CreateWindow("satspace",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		768,
		768,
		SDL_WINDOW_SHOWN );
	SDL_Surface* screen = SDL_GetWindowSurface( w );
	
	uint32_t* pix = screen->pixels;
	
	rngseed(78371598375);
	Model m = makeModel(20, 10);
	printModel(m);
	

	Vec3 points[1024];
	uint32_t cs[1024];
	for(int i = 0; i < 1024; i++) cs[i] = 0xff0000 + ((i / 4) * 256);
	
	
	
	
	
	Graph g = makeGraph(1024);
	
	for(int i = 0; i < 1024; i++){
		uint64_t max = 0;
		for(int j = 0; j < m.size; j++){
			uint64_t x = i;
			uint64_t y = next(x, m.cs[j]) % 1024;
			max = (max > y)? max : y;
			//setGDist(&g, x, y, 0.05);
		}
		setGDist(&g, i, max, 0.05);
	}
	
	//listGStruct(&g);
	initialPos(points, &g);
	draw(pix, points, 1024);
	
	float anneal =  0.008;
	float repel  = -0.8;
	int cont = 1;
	int wave = 0;
	
	int rotX = 0;
	int rotY = 0;
	int rotZ = 0;
	
	const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
	
	while(cont){
		SDL_FillRect(screen, 0, 0);
		
		
	
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) cont = 0;
		}
		
		if(currentKeyStates[ SDL_SCANCODE_W ]) rotZ++;
		if(currentKeyStates[ SDL_SCANCODE_S ]) rotZ--;
		if(currentKeyStates[ SDL_SCANCODE_Q ]) rotX++;
		if(currentKeyStates[ SDL_SCANCODE_E ]) rotX--;
		if(currentKeyStates[ SDL_SCANCODE_A ]) rotY++;
		if(currentKeyStates[ SDL_SCANCODE_D ]) rotY--;
		if(currentKeyStates[ SDL_SCANCODE_L ]){ rotX = 0; rotY = 0; rotZ = 0; }
		
		
		float pot = move(&g, points, anneal, repel, 1024);
		printf("%f\n", pot);
		drawGraph(&g, pix, cs, points, 1024, rotX, rotY, rotZ);
		
		SDL_UpdateWindowSurface( w );
		SDL_Delay(10);
		
		updateColorFlow(&g, cs, 1024, 64);
		
		wave += 4;
	}
	SDL_Quit();
}
