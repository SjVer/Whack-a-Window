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
#include "SDL_syswm.h"
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
bool dragTimerRunning = true;
timer t;

vec2 Tmpl8::GetMonitorSize() {
	return monitorSize;
}

vec2 Tmpl8::GetWindowPos() {
	int x, y;
	SDL_GetWindowPosition(window, &x, &y);
	return vec2(x, y);
}

Surface* Tmpl8::GetWindowSurface() {
	return surface;
}

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

SDL_HitTestResult hitTestCallback(SDL_Window*, const SDL_Point*, void*) {
	return SDL_HITTEST_DRAGGABLE;
}

void tickGame();

#ifdef WINDOWS
int handleSyswmEvent(void*, SDL_Event* event) {
	// this handler is for windows system events only
	if (event->type == SDL_SYSWMEVENT) {
		// this, and other window-dragging related code adapted from
		// https://www.appsloveworld.com/cplus/100/61/keep-window-active-while-being-dragged-sdl-on-win32
		//
		// when dragging a window, windows pauses the main thread. so if we want to keep
		// the game loop running we need to bypass that. this event handler is called
		// outside of the main thread, and if there's a timer running windows will keep
		// sending timer events to this handler. we can abuse this to keep the game
		// loop running while the window is being dragged. cool right?

		const auto& winMessage = event->syswm.msg->msg.win;
		if (winMessage.msg == WM_ENTERSIZEMOVE) {
			// the user started dragging, so create the timer (with the minimum timeout)
			dragTimerRunning = SetTimer(GetActiveWindow(), 1, USER_TIMER_MINIMUM, NULL);
		}
		else if (winMessage.msg == WM_TIMER && winMessage.wParam == 1) {
			// user is dragging, update the game anyway
			vec2 newPos = GetWindowPos();
			vec2 diff = newPos - oldWindowPos;
			game->WindowMove(diff);
			oldWindowPos = newPos;
			tickGame();
		}
	}

	return 0;
}
#endif

void handleEvent(SDL_Event event) {
	switch (event.type)
	{
	case SDL_QUIT:
		game->state = Game::STATE_EXIT;
		break;
	case SDL_KEYDOWN:
		if (event.key.repeat == 0) 
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
			vec2 newPos = GetWindowPos();
			vec2 diff = newPos - oldWindowPos;
			game->WindowMove(diff);
			oldWindowPos = newPos;
		}
	}

	default: break;
	}
}

void tickGame() {
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
}

int main(int argc, char** argv)
{
	SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE);
	printf("application started.\n");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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

#ifdef WINDOWS
	// make sure the game keeps running while dragging the window
	SDL_AddEventWatch(handleSyswmEvent, NULL);
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
#endif

	// get and set monitor size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	monitorSize = vec2(dm.w, dm.h);

	// initialize game and window position
	game = new Game(surface);
	oldWindowPos = GetWindowPos();

	t.reset();
	while (game->state != Game::STATE_EXIT) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
#ifdef WINDOWS
			if (dragTimerRunning) {
				// this thread only continues to run after
				// the window dragging has ended, so if
				// we reach this we know the window was just
				// dragged and we can kill the timer.
				KillTimer(GetActiveWindow(), 1);
				dragTimerRunning = false;
			}
#endif
			handleEvent(event);
		}
		tickGame();
	}

	game->Shutdown();
	SDL_Quit();
	return 0;
}