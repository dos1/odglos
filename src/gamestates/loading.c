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
};

int Gamestate_ProgressCount = -1;

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev){};

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta){};

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	al_draw_tinted_scaled_bitmap(data->bg, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 0, 0, al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg), 0, 0, game->viewport.width, game->viewport.height, 0);
	al_draw_filled_rectangle(0, game->viewport.height * 0.49, game->viewport.width, game->viewport.height * 0.51, al_map_rgba(32, 32, 32, 32));
	al_draw_filled_rectangle(0, game->viewport.height * 0.49, game->loading.progress * game->viewport.width, game->viewport.height * 0.51, al_map_rgba(128, 128, 128, 128));
};

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));
	data->bg = al_load_bitmap(GetDataFilePath(game, "ekran_startowy_tlo_przyciete.png"));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_bitmap(data->bg);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
#ifdef __EMSCRIPTEN__
	HideHTMLLoading(game);
#endif

	// HACK
#ifdef LIBSUPERDERPY_IMGUI
	ImGuiIO* io = igGetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#endif
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
