#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	ALLEGRO_BITMAP *logo, *chodnik, *gradient, *by;
	ALLEGRO_AUDIO_STREAM* music;

	int counter;
};

int Gamestate_ProgressCount = 5; // number of loading steps as reported by Gamestate_Load; 0 when missing

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Here you should do all your game logic as if <delta> seconds have passed.
}

void Gamestate_Tick(struct Game* game, struct GamestateResources* data) {
	// Here you should do all your game logic as if <delta> seconds have passed.
	data->counter++;
	if (data->counter > 460) {
		SwitchScene(game, "anim");
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.

	if (data->counter < 320) {
		al_draw_scaled_bitmap(data->chodnik,
			0, 0, al_get_bitmap_width(data->chodnik), al_get_bitmap_height(data->chodnik),
			0, 0, game->viewport.width, game->viewport.height, 0);

		if ((data->counter > 60) && (data->counter < 65)) {
			al_draw_tinted_scaled_bitmap(data->logo, al_map_rgba(100, 100, 100, 100),
				0, 0, al_get_bitmap_width(data->logo), al_get_bitmap_height(data->logo),
				200, -200, game->viewport.width, game->viewport.height, 0);
		}

		if ((data->counter > 140) && (data->counter < 155)) {
			al_draw_tinted_scaled_bitmap(data->logo, al_map_rgba(50, 50, 50, 50),
				0, 0, al_get_bitmap_width(data->logo), al_get_bitmap_height(data->logo),
				-500, 300, game->viewport.width, game->viewport.height, 0);
		}

		al_draw_tinted_scaled_bitmap(data->logo, al_map_rgba(200, 200, 200, 200),
			0, 0, al_get_bitmap_width(data->logo), al_get_bitmap_height(data->logo),
			0, 0, game->viewport.width, game->viewport.height, 0);

		if (data->counter > 180) {
			al_draw_tinted_scaled_rotated_bitmap(data->by, al_map_rgba(222, 222, 222, 222),
				al_get_bitmap_width(data->by) / 2.0, al_get_bitmap_height(data->by) / 2.0,
				game->viewport.width / 2.0, game->viewport.height - al_get_bitmap_height(data->by) / LIBSUPERDERPY_IMAGE_SCALE,
				0.5 / LIBSUPERDERPY_IMAGE_SCALE, 0.5 / LIBSUPERDERPY_IMAGE_SCALE, 0.0, 0);
		}

		al_draw_scaled_bitmap(data->gradient,
			0, 0, al_get_bitmap_width(data->gradient), al_get_bitmap_height(data->gradient),
			0, 0, game->viewport.width, game->viewport.height, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		SwitchScene(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	//
	// NOTE: There's no OpenGL context available here. If you want to prerender something,
	// create VBOs, etc. do it in Gamestate_PostLoad.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar

	data->chodnik = al_load_bitmap(GetDataFilePath(game, "chodnik.png"));
	progress(game);
	data->gradient = al_load_bitmap(GetDataFilePath(game, "gradient.png"));
	progress(game);
	data->logo = al_load_bitmap(GetDataFilePath(game, "logo.png"));
	progress(game);
	data->by = al_load_bitmap(GetDataFilePath(game, "byholypangolin.png"));
	progress(game);

	data->music = al_load_audio_stream(GetDataFilePath(game, "logo.flac"), 4, 2048);
	al_set_audio_stream_playing(data->music, false);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->chodnik);
	al_destroy_bitmap(data->gradient);
	al_destroy_bitmap(data->logo);
	al_destroy_bitmap(data->by);
	al_destroy_audio_stream(data->music);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	al_set_audio_stream_playing(data->music, true);
	data->counter = 0;
	HideMouse(game);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_set_audio_stream_playing(data->music, false);
}

// Optional endpoints:

void Gamestate_PostLoad(struct Game* game, struct GamestateResources* data) {
	// This is called in the main thread after Gamestate_Load has ended.
	// Use it to prerender bitmaps, create VBOs, etc.
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic nor ProcessEvent)
	// Pause your timers and/or sounds here.
	al_set_audio_stream_playing(data->music, false);
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers and/or sounds here.
	al_set_audio_stream_playing(data->music, true);
}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {
	// Called when the display gets lost and not preserved bitmaps need to be recreated.
	// Unless you want to support mobile platforms, you should be able to ignore it.
}
