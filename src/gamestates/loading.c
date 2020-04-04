/*! \file loading.c
 *  \brief Loading screen.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../common.h"
#include <libsuperderpy.h>

/*! \brief Resources used by Loading state. */
struct GamestateResources {
	ALLEGRO_BITMAP* bg;
	ALLEGRO_BITMAP* fg;
};

int Gamestate_ProgressCount = -1;

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev){};

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta){};

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	float progress = game->loading.progress;
	float val = 0.75;
	ALLEGRO_COLOR tint = al_map_rgba_f(0.5, 0.5, 0.5, 0.25);
	if (game->data->dark_loading) {
		tint = al_map_rgba_f(0.2, 0.2, 0.2, 0.1);
		val = 0.0;
#ifdef __EMSCRIPTEN__
		progress = 1.0;
#endif
	} else {
#ifdef __EMSCRIPTEN__
		progress *= 0.2;
		progress += 0.2;
		val = progress;
#endif
	}
	al_draw_tinted_scaled_bitmap(data->bg, al_map_rgba_f(val, val, val, val), 0, 0, al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg), 0, 0, game->viewport.width, game->viewport.height, 0);

	al_draw_tinted_scaled_rotated_bitmap(data->fg, tint, 0, 0,
		464, 341, game->viewport.width / 1280.0, game->viewport.height / 720.0, 0, 0);

	SetClippingRectangle(464, 341, al_get_bitmap_width(data->fg) * progress * game->viewport.width / 1280.0, al_get_bitmap_height(data->fg) * game->viewport.height / 720.0);
	al_draw_tinted_scaled_rotated_bitmap(data->fg, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0,
		464, 341, game->viewport.width / 1280.0, game->viewport.height / 720.0, 0, 0);
	ResetClippingRectangle();
};

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));
	data->bg = al_load_bitmap(GetDataFilePath(game, "ekran_startowy_tlo_przyciete.png"));
	data->fg = al_load_bitmap(GetDataFilePath(game, "ekran_startowy_kostki16.png"));
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_bitmap(data->bg);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// HACK
#ifdef LIBSUPERDERPY_IMGUI
	ImGuiIO* io = igGetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#endif
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
