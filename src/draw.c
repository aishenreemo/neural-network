#include <SDL2/SDL.h>
#include <stdlib.h>

#include "application.h"
#include "draw.h"

void draw_circle(int center_x, int center_y, uint radius) {
	uint diameter = (radius * 2);
	int x = radius - 1;
	int y = 0;

	int t_x = 1;
	int t_y = 1;

	int error = t_x - diameter;

	while (x >= y) {
		SDL_RenderDrawPoint(app.renderer, center_x + x, center_y - y);
		SDL_RenderDrawPoint(app.renderer, center_x + x, center_y + y);
		SDL_RenderDrawPoint(app.renderer, center_x - x, center_y - y);
		SDL_RenderDrawPoint(app.renderer, center_x - x, center_y + y);
		SDL_RenderDrawPoint(app.renderer, center_x + y, center_y - x);
		SDL_RenderDrawPoint(app.renderer, center_x + y, center_y + x);
		SDL_RenderDrawPoint(app.renderer, center_x - y, center_y - x);
		SDL_RenderDrawPoint(app.renderer, center_x - y, center_y + x);

		if (error <= 0) {
			y += 1;
			error += t_y;
			t_y += 2;
		}

		if (error > 0) {
			x -= 1;
			t_x += 2;
			error += t_x - diameter;
		}
	}
}
