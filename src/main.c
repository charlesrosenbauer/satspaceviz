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


void drawColors(uint32_t* pix, uint32_t* cs, Vec3* ps, int size){
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
			if(!((x < 0) || (x > 767) || (y < 0) || (y > 767))) ps[p] += (color >> 5) & 0x70707;
		}
	}else{					// from y1 -> y0
		m = 1.0 / m;
		int iy = (dy < 0)? -1 : 1;
		for(int i = 0; i < abs(dy); i++){
			y -= iy;
			c += m;
			x  = x1 - c;
			int p = (y * 768) + x;
			if(!((x < 0) || (x > 767) || (y < 0) || (y > 767))) ps[p] += (color >> 5) & 0x70707;
		}
	}
}


void drawGraph(Graph* g, uint32_t* pix, uint32_t* cs, Vec3* ps, int size){
	Vec3 camera = (Vec3){0, 0, 1};
	
	Vec3 center;
	float scale = 0.5 / getScale(ps, &center, size);
	Vec3* xs = alloca(sizeof(Vec3) * size);
	for(int i = 0; i < size; i++){
		Vec3 p =  mulVec(subVec(ps[i], center), scale);
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
	
	Model m = makeModel(18, 6);
	printModel(m);
	

	Vec3 points[1024];
	uint32_t cs[1024];
	for(int i = 0; i < 1024; i++) cs[i] = 0xff0000 + ((i / 4) * 256);
	
	Graph g = makeGraph(1024);
	
	for(int i = 0; i < 1024; i++){
		for(int j = 0; j < m.size; j++){
			uint64_t x = i;
			uint64_t y = next(x, m.cs[j]) % 1024;
			setGDist(&g, x, y, 1.0);
		}
	}
	
	listGStruct(&g);
	initialPos(points, &g);
	draw(pix, points, 1024);
	
	float anneal = 0.01;
	int cont = 1;
	int wave = 0;
	while(cont){
		SDL_FillRect(screen, 0, 0);
		
		for(int i = 0; i < 1024; i++){
			int j = (wave + i) % 1024;
			if(j < 32){
				cs[i] = 0xffffff;
			}else{
				cs[i] = 0xff0000 + ((i / 4) * 256);
			}
		}
	
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) cont = 0;
		}
		
		float pot = move(&g, points, anneal, -0.01, 1024);
		printf("%f\n", pot);
		drawGraph(&g, pix, cs, points, 1024);
		
		SDL_UpdateWindowSurface( w );
		SDL_Delay(16);
		
		wave += 4;
	}
	SDL_Quit();
}
