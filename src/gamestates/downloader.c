/*! \file empty.c
 *  \brief Empty gamestate.
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

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Here you should do all your game logic as if <delta> seconds have passed.
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.
	float progress = GetDownloadProgress(game);
	if (data->starting_point < 1.0) {
		progress = (progress - data->starting_point) / (1.0 - data->starting_point);
	}

	al_draw_tinted_scaled_rotated_bitmap(data->fg, al_map_rgba_f(0.2, 0.2, 0.2, 0.1), 0, 0,
		464, 341, game->viewport.width / 1280.0, game->viewport.height / 720.0, 0, 0);

	SetClippingRectangle(464, 341, al_get_bitmap_width(data->fg) * progress * game->viewport.width / 1280.0, al_get_bitmap_height(data->fg) * game->viewport.height / 720.0);
	al_draw_tinted_scaled_rotated_bitmap(data->fg, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0,
		464, 341, game->viewport.width / 1280.0, game->viewport.height / 720.0, 0, 0);
	ResetClippingRectangle();

	if (game->data->download.pack[game->data->download.requested].loaded) {
		MountDataPacks(game);
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
	data->starting_point = GetDownloadProgress(game);
	if (!game->data->download.pack[game->data->download.requested].loaded) {
		StopMusic(game);
	}
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
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
