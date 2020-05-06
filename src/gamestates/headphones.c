/*! \file headphones.c
 *  \brief Headphones notice.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	ALLEGRO_BITMAP* sowka;
	ALLEGRO_FONT *font, *bold;
	double time;
};

int Gamestate_ProgressCount = 3; // number of loading steps as reported by Gamestate_Load; 0 when missing

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Here you should do all your game logic as if <delta> seconds have passed.
	if (game->data->footnote) { return; }
	data->time += delta;
	if (data->time > 6.0) {
		ChangeCurrentGamestate(game, "anim");
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.
	al_draw_scaled_rotated_bitmap(data->sowka, 0, 0, 290 * 1280.0 / 1920.0, 760 * 1280.0 / 1920.0, 1280.0 / 1920.0, 1280.0 / 1920.0, 0, 0);

	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_scale_transform(&transform, 0.5, 0.5);
	PushTransform(game, &transform);

	al_draw_text(data->font, al_map_rgb(255, 255, 255), 850 + 120, 1280, ALLEGRO_ALIGN_LEFT, "For the best experience, please put your");
	al_draw_text(data->bold, al_map_rgb(255, 255, 255), 1666 + 120, 1280, ALLEGRO_ALIGN_LEFT, "headphones");
	al_draw_text(data->font, al_map_rgb(255, 255, 255), 1933 + 120, 1280, ALLEGRO_ALIGN_LEFT, "on.");

	PopTransform(game);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	if ((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (game->data->cursor && ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
		ChangeCurrentGamestate(game, "anim");
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		ChangeCurrentGamestate(game, "anim");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		ChangeCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	//
	// Keep in mind that there's no OpenGL context available here. If you want to prerender something,
	// create VBOs, etc. do it in Gamestate_PostLoad.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->sowka = al_load_bitmap(GetDataFilePath(game, "sowka_w_sluchawkach.png"));
	progress(game);
	data->font = al_load_font(GetDataFilePath(game, "fonts/SourceSansPro-Regular.ttf"), 48, 0);
	progress(game);
	data->bold = al_load_font(GetDataFilePath(game, "fonts/SourceSansPro-Semibold.ttf"), 48, 0);
	progress(game);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->sowka);
	al_destroy_font(data->bold);
	al_destroy_font(data->font);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	StopMusic(game);
	data->time = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
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
