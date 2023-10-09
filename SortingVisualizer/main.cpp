#include "SDL.h"
#include <iostream>
#include <string>
const int ARRAYSIZE = 1000;		//Contains array size;


//
// Application Struct
//
struct application
{
	//Variables
	bool isRunning;				// Keeps track if user closes window
	bool endOperation;			// Ends current opertaion;
	bool arrayUnderOperation;	// Prevents array operations happening simulataneously
	int arr[ARRAYSIZE];			// Array of integers
	float timeElapsed;			// Keeps track of how long operations take

	int numComparisons;
	int numSwaps;

	//SDL objects
	SDL_Window* window;			// SDL2 window
	SDL_Renderer* renderer;		// SDL2 renderer
	SDL_Texture* texture;		// array of rgb pixel data

	//Methods
	void init(const char* title, int width, int height);	//Initialize window and variables
	void initArray();					// Initialize Values in array
	void initTexture();					// Create texture from array
	void display();						// Refresh texture to window
	void renderSwap(int x1, int x2);	// Update texture when a swap happens
	void handleEvents();				// Handle user inputs

	//Array Sorting Methods
	void randomize();
	void bubbleSort();

	void endDiagnostics();
	void startDiagnostics();

	void update() {
		handleEvents();
		display();
	}
	void update(int x1, int x2) {
		handleEvents();
		renderSwap(x1, x2);
	}
	void swap(int x1, int x2) {
		int temp = arr[x1];
		arr[x1] = arr[x2];
		arr[x2] = temp;
		numSwaps++;

		update(x1, x2);
	}
};


//
// Main Method
//
int main(int argc, char* argv[]) {
	// init
	if (SDL_Init(SDL_INIT_VIDEO) != 0) return -1;
	application app;

	app.init("Array Sort Visualizer", 1960, 1080);

	// render loop
	while (app.isRunning) {
		app.update();
	}

	// exit
	SDL_DestroyWindow(app.window);
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyTexture(app.texture);
	SDL_Quit();

	return 0;
}


//
// Array Sort Definitons
//

void application::randomize() {
	std::cout << "Randomizing Array..." << std::endl;
	startDiagnostics();

	//initialize random seed
	srand(time(NULL));

	for (int j = 0; j < ARRAYSIZE; j++) {
		if (!isRunning || endOperation) break;

		swap(j, rand() % ARRAYSIZE);
	}

	endDiagnostics();
}

void application::bubbleSort() {
	std::cout << "Sorting Array with Bubble Sort..." << std::endl;
	startDiagnostics();
	int temp;

	for (int i = ARRAYSIZE; i >= 1; i--) {
		if (!isRunning || endOperation) break;

		for (int j = 0; j < (i- 1); j++) {
			if (!isRunning || endOperation) break;

			if (arr[j] > arr[j+1]) {	//swap
				swap(j, j + 1);
			}
			numComparisons++;
		}
	}

	endDiagnostics();
}

void application::startDiagnostics() {
	arrayUnderOperation = true;
	endOperation = false;
	timeElapsed = SDL_GetTicks();
	numSwaps = 0;
	numComparisons = 0;
}

void application::endDiagnostics() {
	timeElapsed = (SDL_GetTicks() - timeElapsed) / 1000.0f;
	arrayUnderOperation = false;
	if (endOperation) std::cout << "Operation Stopped" << std::endl;
	else std::cout << "Operation complete" << std::endl;
	std::cout << "Time Elapsed: " << timeElapsed << "s" << std::endl;
	std::cout << "Number of Comparisons: " << numComparisons << std::endl;
	std::cout << "Number of Swaps: " << numSwaps << std::endl;
	std::cout << "Number of Operations: " << numSwaps + numComparisons << std::endl << std::endl << std::endl;
	endOperation = false;
}


//
// Function Definitions
//

void application::init(const char* title, int width, int height) {
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	if (window) renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer) texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, ARRAYSIZE, ARRAYSIZE);
	if (texture) isRunning = true;

	initArray();
	initTexture();
}

void application::initArray() {
	for (int x = 0; x < ARRAYSIZE; x++) {
		arr[x] = x;
	}
	arrayUnderOperation = false;
}

void application::display() {
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void application::initTexture() {
	void* data;
	int pitch;
	SDL_LockTexture(texture, NULL, &data, &pitch);

	// clear to black background
	SDL_memset(data, 0, pitch * ARRAYSIZE);

	//Draw array to texture
	for (int x = 0; x < ARRAYSIZE; x++) {
		for (int y = 0; y < arr[x]; y++) {
			unsigned int* row = (unsigned int*)((char*)data + pitch * (ARRAYSIZE - y));
			row[x] = 0xFFFFFF00;
		}
	}

	SDL_UnlockTexture(texture);

	display();
}

void application::renderSwap(int x1, int x2) {
	void* data;
	int pitch;
	unsigned int* row;
	SDL_LockTexture(texture, NULL, &data, &pitch);

	for (int j = 0; j < 2; j++) {
		for (int y = 0; y < ARRAYSIZE; y++) {
			row = (unsigned int*)((char*)data + pitch * (ARRAYSIZE - y));
			if(y < arr[x1]) row[x1] = 0xFFFFFF00;
			else row[x1] = 0;
			if (y < arr[x2]) row[x2] = 0xFFFFFF00;
			else row[x2] = 0;
		}
	}

	SDL_UnlockTexture(texture);

	display();
}

void application::handleEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:			// Exit application
			isRunning = false;
			break;
		case SDL_KEYDOWN:		//Key Down
			if (event.key.keysym.sym == SDLK_x) {
				endOperation = true;
			}
			else if (event.key.keysym.sym == SDLK_r && !arrayUnderOperation) {	//press r
				randomize();
			}
			else if (event.key.keysym.sym == SDLK_1 && !arrayUnderOperation) {
				bubbleSort();
			}
			break;
		default:
			break;
		}
	}
}