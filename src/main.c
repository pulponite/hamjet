#include <SDL.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

typedef struct State {
	SDL_Renderer *renderer;
	lua_State *L;
} State;

int offset = 0;

void loop(void *r) {
	State *s = (State *)r;

	luaL_dostring(s->L, "return 1;");
	offset += lua_tointeger(s->L, -1);

	SDL_SetRenderDrawColor(s->renderer, 255, 0, 0, 255);
	SDL_RenderClear(s->renderer);

	SDL_SetRenderDrawColor(s->renderer, 0, 0, 0, 255); // the rect color (solid red)
	SDL_Rect rect = {offset % 500, 0, 100, 50}; // the rectangle
	SDL_RenderFillRect(s->renderer, &rect);

	SDL_RenderPresent(s->renderer);
}

int main(int argc, char** argv){
	printf("Hello World");
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("Error %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == 0){
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == 0){
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	lua_State *L = lua_open();

	State s = {ren, L};

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(loop, (void*)&s, 60, 1);
#else
	while(1) {
		loop((void*)&s);
	}
#endif

	lua_close(L);

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	SDL_Quit();
	return 0;
}
