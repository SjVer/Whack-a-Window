// Template, BUAS version https://www.buas.nl/games
// IGAD/BUAS(NHTV)/UU - Jacco Bikker - 2006-2020

// Note:
// this version of the template uses SDL2 for all frame buffer interaction
// see: https://www.libsdl.org

#ifdef _MSC_VER
#pragma warning (disable : 4530) // complaint about exception handler
#pragma warning (disable : 4311) // pointer truncation from HANDLE to long
#endif

//#define FULLSCREEN
//#define ADVANCEDGL

#include "game.h"

#include <fcntl.h>
#include <stdio.h>
#include "template.h"
#include <math.h>
#include <SDL.h>
#include "surface.h"
#include <cstdio>
#include <iostream>
#include <ctime>

#ifdef ADVANCEDGL
#define GLEW_BUILD
extern "C"
{
#include "glew.h" 
}
#include "gl.h"
#ifdef WINDOWS
#include "wglext.h"
#else
#include "glext.h"
#endif
#endif

namespace Tmpl8 {

	timer::timer() : start(get())
	{
		init();
	}

	float timer::elapsed() const
	{
		// return (float)((get() - start) * inv_freq);
		return (float)(get() - start);
	}

	timer::value_type timer::get()
	{
		return clock();
	}

	double timer::to_time(const value_type vt)
	{
		// return double(vt) * inv_freq;
		return vt;
	}

	void timer::reset()
	{
		start = get();
	}

	void timer::init()
	{
		// timespec ts;
		// clock_gettime(CLOCK_MONOTONIC, &ts);
		// inv_freq = 1000. / double(ts.tv_nsec);

	}

	// Math Stuff
	// ----------------------------------------------------------------------------
	vec3 normalize(const vec3& v) { return v.normalized(); }
	vec3 cross(const vec3& a, const vec3& b) { return a.cross(b); }
	float dot(const vec3& a, const vec3& b) { return a.dot(b); }
	vec3 operator * (const float& s, const vec3& v) { return vec3(v.x * s, v.y * s, v.z * s); }
	vec3 operator * (const vec3& v, const float& s) { return vec3(v.x * s, v.y * s, v.z * s); }
	vec4 operator * (const float& s, const vec4& v) { return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
	vec4 operator * (const vec4& v, const float& s) { return vec4(v.x * s, v.y * s, v.z * s, v.w * s); }
	vec4 operator * (const vec4& v, const mat4& M)
	{
		vec4 mx(M.cell[0], M.cell[4], M.cell[8], M.cell[12]);
		vec4 my(M.cell[1], M.cell[5], M.cell[9], M.cell[13]);
		vec4 mz(M.cell[2], M.cell[6], M.cell[10], M.cell[14]);
		vec4 mw(M.cell[3], M.cell[7], M.cell[11], M.cell[15]);
		return v.x * mx + v.y * my + v.z * mz + v.w * mw;
	}

	mat4::mat4()
	{
		memset(cell, 0, 64);
		cell[0] = cell[5] = cell[10] = cell[15] = 1;
	}

	mat4 mat4::identity()
	{
		mat4 r;
		memset(r.cell, 0, 64);
		r.cell[0] = r.cell[5] = r.cell[10] = r.cell[15] = 1.0f;
		return r;
	}

	mat4 mat4::rotate(const vec3 l, const float a)
	{
		// http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation
		mat4 M;
		const float u = l.x, v = l.y, w = l.z, ca = cosf(a), sa = sinf(a);
		M.cell[0] = u * u + (v * v + w * w) * ca, M.cell[1] = u * v * (1 - ca) - w * sa;
		M.cell[2] = u * w * (1 - ca) + v * sa, M.cell[4] = u * v * (1 - ca) + w * sa;
		M.cell[5] = v * v + (u * u + w * w) * ca, M.cell[6] = v * w * (1 - ca) - u * sa;
		M.cell[8] = u * w * (1 - ca) - v * sa, M.cell[9] = v * w * (1 - ca) + u * sa;
		M.cell[10] = w * w + (u * u + v * v) * ca;
		M.cell[3] = M.cell[7] = M.cell[11] = M.cell[12] = M.cell[13] = M.cell[14] = 0, M.cell[15] = 1;
		return M;
	}
	mat4 mat4::rotatex(const float a)
	{
		mat4 M;
		const float ca = cosf(a), sa = sinf(a);
		M.cell[5] = ca, M.cell[6] = -sa;
		M.cell[9] = sa, M.cell[10] = ca;
		return M;
	}
	mat4 mat4::rotatey(const float a)
	{
		mat4 M;
		const float ca = cosf(a), sa = sinf(a);
		M.cell[0] = ca, M.cell[2] = sa;
		M.cell[8] = -sa, M.cell[10] = ca;
		return M;
	}
	mat4 mat4::rotatez(const float a)
	{
		mat4 M;
		const float ca = cosf(a), sa = sinf(a);
		M.cell[0] = ca, M.cell[1] = -sa;
		M.cell[4] = sa, M.cell[5] = ca;
		return M;
	}

	void NotifyUser(char* s)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", s, NULL);
		exit(0);
	}

}

using namespace Tmpl8;
using namespace std;

#ifdef ADVANCEDGL

PFNGLGENBUFFERSPROC glGenBuffers = 0;
PFNGLBINDBUFFERPROC glBindBuffer = 0;
PFNGLBUFFERDATAPROC glBufferData = 0;
PFNGLMAPBUFFERPROC glMapBuffer = 0;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = 0;
typedef GLboolean(GLAPIENTRY* PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
unsigned int framebufferTexID[2];
GLuint fbPBO[2];
unsigned char* framedata = 0;

#endif

int ACTWIDTH, ACTHEIGHT;

Surface* surface = 0;
Game* game = 0;
SDL_Window* window = 0;
vec2 oldWindowPos;
vec2 monitorSize;

vec2 Tmpl8::GetMonitorSize() {
	return monitorSize;
}

vec2 Tmpl8::GetWindowPos() {
	return oldWindowPos;
}

Surface* Tmpl8::GetWindowSurface() {
	return surface;
}

vec2 GetRealWindowPos() {
	int x, y;
	SDL_GetWindowPosition(window, &x, &y);
	return vec2(x, y);
}

#undef _MSC_VER
#ifdef _MSC_VER
bool redirectIO()
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 500;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	HANDLE h1 = GetStdHandle(STD_OUTPUT_HANDLE);
	int h2 = _open_osfhandle((intptr_t)h1, _O_TEXT);
	FILE* fp = _fdopen(h2, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
	h1 = GetStdHandle(STD_INPUT_HANDLE), h2 = _open_osfhandle((intptr_t)h1, _O_TEXT);
	fp = _fdopen(h2, "r"), * stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);
	h1 = GetStdHandle(STD_ERROR_HANDLE), h2 = _open_osfhandle((intptr_t)h1, _O_TEXT);
	fp = _fdopen(h2, "w"), * stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);
	ios::sync_with_stdio();
	FILE* stream;
	if ((stream = freopen("CON", "w", stdout)) == NULL)
		return false;
	if ((stream = freopen("CON", "w", stderr)) == NULL)
		return false;
	return true;
}
#endif

#ifdef ADVANCEDGL

bool createFBtexture()
{
	glGenTextures(2, framebufferTexID);
	if (glGetError()) return false;
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, framebufferTexID[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (glGetError()) return false;
	}
	const int sizeMemory = 4 * ScreenWidth * ScreenHeight;
	glGenBuffers(2, fbPBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, sizeMemory, NULL, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, sizeMemory, NULL, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexID[0]);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[0]);
	framedata = (unsigned char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	if (!framedata) return false;
	memset(framedata, 0, ScreenWidth * ScreenHeight * 4);
	return (glGetError() == 0);
}

bool init()
{
	fbPBO[0] = fbPBO[1] = -1;
	glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffersARB");
	glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBufferARB");
	glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferDataARB");
	glMapBuffer = (PFNGLMAPBUFFERPROC)SDL_GL_GetProcAddress("glMapBufferARB");
	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)SDL_GL_GetProcAddress("glUnmapBufferARB");
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)SDL_GL_GetProcAddress("wglSwapIntervalEXT");
	if ((!glGenBuffers) || (!glBindBuffer) || (!glBufferData) || (!glMapBuffer) || (!glUnmapBuffer)) return false;
	if (glGetError()) return false;
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	if (!createFBtexture()) return false;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	if (wglSwapIntervalEXT) wglSwapIntervalEXT(0);
	surface = new Surface(ScreenWidth, ScreenHeight, 0, ScreenWidth);
	return true;
}

void swap()
{
	static int index = 0;
	int nextindex;
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
	glBindTexture(GL_TEXTURE_2D, framebufferTexID[index]);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[index]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ScreenWidth, ScreenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	nextindex = (index + 1) % 2;
	index = (index + 1) % 2;
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, fbPBO[nextindex]);
	framedata = (unsigned char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_GL_SwapWindow(window);
}

#endif

SDL_HitTestResult hitTestCallback(SDL_Window*, const SDL_Point*, void* game) {
	return SDL_HITTEST_DRAGGABLE;
}

int main(int argc, char** argv)
{
	SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE);
#ifdef _MSC_VER
	if (!redirectIO())
		return 1;
#endif
	printf("application started.\n");
	SDL_Init(SDL_INIT_VIDEO);
#ifdef ADVANCEDGL
#ifdef FULLSCREEN
	window = SDL_CreateWindow(WindowName, 50, 50, ScreenWidth, ScreenHeight, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
#else
	window = SDL_CreateWindow(
		WindowName,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		ScreenWidth, ScreenHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
	);
#endif
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	SDL_SetWindowHitTest(window, hitTestCallback, NULL);
	init();
	// SDL_ShowCursor(false);
#else
#ifdef FULLSCREEN
	window = SDL_CreateWindow(WindowName, 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_FULLSCREEN);
#else
	window = SDL_CreateWindow(WindowName, 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
#endif
	surface = new Surface(ScreenWidth, ScreenHeight);
	surface->Clear(0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture* frameBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, ScreenWidth, ScreenHeight);
#endif

	// get monitor size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	monitorSize = vec2(dm.w, dm.h);

	int exitapp = 0;
	game = new Game(surface);
	timer t;
	t.reset();
	oldWindowPos = GetRealWindowPos();
	while (!exitapp)
	{
#ifdef ADVANCEDGL
		swap();
		surface->SetBuffer((Pixel*)framedata);
#else
		void* target = 0;
		int pitch;
		SDL_LockTexture(frameBuffer, NULL, &target, &pitch);
		if (pitch == (surface->GetWidth() * 4))
		{
			memcpy(target, surface->GetBuffer(), ScreenWidth * ScreenHeight * 4);
		}
		else
		{
			unsigned char* t = (unsigned char*)target;
			for (int i = 0; i < ScreenHeight; i++)
			{
				memcpy(t, surface->GetBuffer() + i * ScreenWidth, ScreenWidth * 4);
				t += pitch;
			}
		}
		SDL_UnlockTexture(frameBuffer);
		SDL_RenderCopy(renderer, frameBuffer, NULL, NULL);
		SDL_RenderPresent(renderer);

#endif
		// calculate frame time and pass it to game->Tick
		float elapsedTime = t.elapsed();
		t.reset();

		game->Tick(elapsedTime);
		// event loop
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				exitapp = 1;
				break;
			case SDL_KEYDOWN:
				game->KeyDown(event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				game->KeyUp(event.key.keysym.scancode);
				break;
			case SDL_MOUSEMOTION: {
				vec2 pos(event.motion.x, event.motion.y);
				game->MouseMove(pos);
				game->mousePos = pos;
				break;
			}
			case SDL_MOUSEBUTTONUP:
				game->MouseUp(event.button.button);
				break;
			case SDL_MOUSEBUTTONDOWN:
				game->MouseDown(event.button.button);
				break;

			case SDL_WINDOWEVENT: {
				if (event.window.event == SDL_WINDOWEVENT_MOVED) {
					vec2 new_pos = GetRealWindowPos();
					game->WindowMove(new_pos);
					oldWindowPos = new_pos;
				}
			}

			default:
				break;
			}
		}

		if (game->state == Game::STATE_EXIT) exitapp = 1;
	}
	game->Shutdown();
	SDL_Quit();
	return 0;
}