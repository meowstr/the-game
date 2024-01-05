#include <SDL.h>
#include <SDL_opengl.h>

#include <math.h>

#include "vector.h"

SDL_Event event;

#define WIDTH 640
#define HEIGHT 480

void InitGL()
{
	float ratio = (float)WIDTH/(float)HEIGHT;

	glClearColor(0.0f, 0.0f, 0.545f, 1.0f);
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,ratio,1.0,1024);

	//We are not on Windows lmao
	//GLenum error = glGetError();
	//if(error != GL_NO_ERROR){
	//	ShowError("Failed to initialize OpenGL", (const char*)gluErrorString(error), false);
	//}

	return;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	//This is where the UI goes
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//What UI lmao, there's none because WE ARE ON EMSCRIPTEN NOT WINDOWS

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	//This is where 3D stuff goes
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//The camera
	glPushMatrix();
	gluLookAt(5.0f, 5.0f, 5.0f,
			  0.0f, 1.0f, 0.0f,
			  0.0f, 1.0f, 0.0f);

	//3D objects

	glColor3f(0.9f, 0.9f, 0.9f);
	glPushMatrix();
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f, 100.0f);
		glVertex3f(100.0f, 0.0f, 100.0f);
		glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);

	//BRO RIP TEAPOT

	glPopMatrix();

	SDL_GL_SwapBuffers();
}

int main(int argc, char* argv[])
{
	bool quit = false;
	SDL_Surface* video = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;

	video = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL);

	if(!video)
		return -1;

	InitGL();

	//Information that it needs to catch for later
	//SDL_SysWMinfo wmInfo;
	//SDL_GetWMInfo(&wmInfo);
	//SDLWindow = wmInfo.window;

	//BRO WE ARE NOT ON WINDOWS WE ARE ON EMSCRIPTEN BECAUSE OF A GUY CALLED MEOWSTER

	//Here's where the magic happens

	//ALRIGHT COOL COOL LET'S GOOOOO
	while(!quit) {
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT)
				quit = true;
		}

		Render();
	}

	SDL_Quit();

	return 0;
}