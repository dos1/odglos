/*! \file empty.c
 *  \brief Empty gamestate.
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

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	ALLEGRO_BITMAP* fg;
	float starting_point;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load; 0 when missing

static float GetProgress(struct Game* game, struct GamestateResources* data) {
	int n = 0;
	float val = 0.0;
	for (int i = 0; i <= game->data->download.requested; i++) {
		val += game->data->download.progress[i];
		n += 1;
	}
	val /= (float)n;
	if (val == 1.0) {
		return 1.0;
	}
	return (val - data->starting_point) / (1.0 - data->starting_point);
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Here you should do all your game logic as if <delta> seconds have passed.
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.
	float progress = GetProgress(game, data);

	al_draw_tinted_scaled_rotated_bitmap(data->fg, al_map_rgba_f(0.2, 0.2, 0.2, 0.1), 0, 0,
		464, 341, game->viewport.width / 1280.0, game->viewport.height / 720.0, 0, 0);

	SetClippingRectangle(464, 341, al_get_bitmap_width(data->fg) * progress * game->viewport.width / 1280.0, al_get_bitmap_height(data->fg) * game->viewport.height / 720.0);
	al_draw_tinted_scaled_rotated_bitmap(data->fg, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0,
		464, 341, game->viewport.width / 1280.0, game->viewport.height / 720.0, 0, 0);
	ResetClippingRectangle();

#ifdef __EMSCRIPTEN__
	SetHTMLLoadingValue(game, progress);
#endif

	if (game->data->download.loaded[game->data->download.requested]) {
		StartDownloadPacks(game);
		StopCurrentGamestate(game);
		StartInitialGamestate(game, !game->data->download.additional);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	//
	// Keep in mind that there's no OpenGL context available here. If you want to prerender something,
	// create VBOs, etc. do it in Gamestate_PostLoad.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->fg = al_load_bitmap(GetDataFilePath(game, "ekran_startowy_kostki16.png"));
	progress(game);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	// HACK
	data->starting_point = 0.0;
	data->starting_point = GetProgress(game, data);
	if (!game->data->download.loaded[game->data->download.requested]) {
		StopMusic(game);
	}
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	game->data->dark_loading = true;
	game->data->download.additional = true;
}

// Optional endpoints:

void Gamestate_PostLoad(struct Game* game, struct GamestateResources* data) {
	// This is called in the main thread after Gamestate_Load has ended.
	// Use it to prerender bitmaps, create VBOs, etc.
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic nor ProcessEvent)
	// Pause your timers and/or sounds here.
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers and/or sounds here.
}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {
	// Called when the display gets lost and not preserved bitmaps need to be recreated.
	// Unless you want to support mobile platforms, you should be able to ignore it.
}
