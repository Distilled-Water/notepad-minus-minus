#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>


void cleanup(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font) {
	if (window) SDL_DestroyWindow(window);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (font) TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	

	// Creates a window
	SDL_Window* window = SDL_CreateWindow("Notepad--", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
	if (!window) { 
		std::cout << "Failed to create window! Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		std::cout << "Failed to create renderer! Error: " << SDL_GetError() << std::endl;
		cleanup(window, nullptr, nullptr);
		return -1;
	}

	TTF_Init();

	// Create a font and colour
	TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
	if (!font) {
		std::cout << "Failed to load font! Error:" << TTF_GetError() << std::endl;
		cleanup(window, renderer, nullptr);
		return -1;
	}
	SDL_Color green = { 0, 0, 255, 255 };
	SDL_Texture* textTexture = nullptr;


	// MAIN LOOP
	std::vector<std::string> lines = {""};
	int lineSpacing = 0;
	int maxWidth = 570;

	bool running = true;
	SDL_Event e;
	while (running) {

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				// If any key is pressed, convert that to a string and concat that with initial text
				if (e.key.keysym.sym >= SDLK_SPACE && e.key.keysym.sym <= SDLK_z) {
					// appends letter to last line
					char keyPressed = static_cast<char>(e.key.keysym.sym);
					lines.back() += keyPressed;

					// create new line if width>maxWidth
					int textWidth;
					int textHeight;
					TTF_SizeText(font, lines.back().c_str(), &textWidth, &textHeight);

					if (textWidth > maxWidth) {
						lines.push_back("");
					}
				}
				else if (e.key.keysym.sym == SDLK_BACKSPACE)
				{
					if (lines.back().size() >= 1) {
						lines.back().pop_back();
					}
					else if (lines.size() >= 2) {
						lines.pop_back();
					}

				}
			default:
				break;
			}
		}


		// draws background
		SDL_SetRenderDrawColor(renderer, 188, 255, 255, 255);
		SDL_RenderClear(renderer);

		// draws char on each line of vector
		int yOffset = 10;
		for (const std::string& line : lines) {

			// Renders text, then converts that into a surface
			SDL_Surface* textSurface = TTF_RenderText_Blended(font, line.c_str(), green);
			if (textSurface) {
				SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

				// Renders Text into a box
				SDL_Rect textRect = { 10, yOffset, textSurface->w, textSurface->h };
				SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

				yOffset += textSurface->h + lineSpacing;

				SDL_DestroyTexture(textTexture);
			}
			SDL_FreeSurface(textSurface);
		}

		// updates screen
		SDL_RenderPresent(renderer);
	}

	cleanup(window, renderer, font);
	return 0;
}