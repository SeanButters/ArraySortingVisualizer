#include "SDL.h"
#include <iostream>





int main(int argc, char* argv[]) {
	// init
	if (SDL_Init(SDL_INIT_VIDEO) != 0) return -1;
	init("test", 1960, 1080);


	// game running
	while (application.isRunning) {

		// render loop
		handleEvents();
		update();
		render();
	}

	// exit
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(frame);
	SDL_Quit();

	return 0;
}