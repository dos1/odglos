#include "../common.h"
#include <libsuperderpy.h>

enum Myszol {
	MYSZOL_LEFT = 0,
	MYSZOL_RIGHT = 1,
	MYSZOL_BOTTOM = 2,
	MYSZOL_BOTTOM_LEFT = 3,
	MYSZOL_TOP = 4,
	MYSZOL_TOP_RIGHT = 5
};

#define SCALE (1.0 / LIBSUPERDERPY_IMAGE_SCALE)

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	ALLEGRO_BITMAP* myszka;

	struct {
		ALLEGRO_BITMAP *lewo[2], *prawo[3], *gora[3], *dol[4], *lewodol, *prawogora;
	} myszole;

	enum Myszol myszol;
	int counter;
	double pos;
	double angle;
	double rand;
	int con;
};

int Gamestate_ProgressCount = 14; // number of loading steps as reported by Gamestate_Load; 0 when missing

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Here you should do all your game logic as if <delta> seconds have passed.
	if (game->data->footnote) { return; }
}

void Gamestate_Tick(struct Game* game, struct GamestateResources* data) {
	// Here you should do all your game logic as if <delta> seconds have passed.
	data->counter++;
	if (data->counter % 6 == 0) {
		data->pos += 0.04; //al_get_audio_stream_position_secs(data->music) / al_get_audio_stream_length_secs(data->music);
		data->angle = rand() / (double)RAND_MAX;
	}
	if (data->pos >= 0.92) {
		data->con++;
		if (data->con > 20) {
			ChangeCurrentGamestate(game, "anim");
		}
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.
	if (data->con) {
		return;
	}
	switch (data->myszol) {
		case MYSZOL_RIGHT:
			al_draw_scaled_rotated_bitmap(data->myszka, al_get_bitmap_width(data->myszka) / 2.0, al_get_bitmap_height(data->myszka) / 2.0,
				(2500 * SCALE * data->pos) * 0.6666, (1080 * SCALE / 2.0 + data->rand * 1080 * SCALE) * 0.666, 0.25, 0.25, data->angle * 0.2 - 0.1, 0);
			break;
		case MYSZOL_LEFT:
			al_draw_scaled_rotated_bitmap(data->myszka, al_get_bitmap_width(data->myszka) / 2.0, al_get_bitmap_height(data->myszka) / 2.0,
				(2500 * SCALE * (1.0 - data->pos) - 500 * SCALE) * 0.666, (1080 * SCALE / 2.0 + data->rand * 1080 * SCALE) * 0.666, 0.25, 0.25, data->angle * 0.2 - 0.1, 0);
			break;
		case MYSZOL_TOP:
			al_draw_scaled_rotated_bitmap(data->myszka, al_get_bitmap_width(data->myszka) / 2.0, al_get_bitmap_height(data->myszka) / 2.0,
				(1920 * SCALE / 2.0 + 1920 * data->rand) * 0.666, (1300 * SCALE * (1.0 - data->pos) - 200 * SCALE) * 0.666, 0.25, 0.25, data->angle * 0.2 - 0.1, 0);
			break;
		case MYSZOL_BOTTOM:
			al_draw_scaled_rotated_bitmap(data->myszka, al_get_bitmap_width(data->myszka) / 2.0, al_get_bitmap_height(data->myszka) / 2.0,
				(1920 * SCALE / 2.0 + 1920 * data->rand) * 0.666, (1300 * SCALE * data->pos) * 0.666, 0.25, 0.25, data->angle * 0.2 - 0.1, 0);
			break;
		case MYSZOL_BOTTOM_LEFT:
			al_draw_scaled_rotated_bitmap(data->myszka, al_get_bitmap_width(data->myszka) / 2.0, al_get_bitmap_height(data->myszka) / 2.0,
				(2500 * SCALE * (1.0 - data->pos) - 500 * SCALE) * 0.666, (1300 * SCALE * data->pos) * 0.666, 0.25, 0.25, data->angle * 0.2 - 0.1, 0);
			break;
		case MYSZOL_TOP_RIGHT:
			al_draw_scaled_rotated_bitmap(data->myszka, al_get_bitmap_width(data->myszka) / 2.0, al_get_bitmap_height(data->myszka) / 2.0,
				(2500 * SCALE * data->pos) * 0.666, (1300 * SCALE * (1.0 - data->pos) - 200 * SCALE) * 0.666, 0.25, 0.25, data->angle * 0.2 - 0.1, 0);
			break;
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
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
	// NOTE: There's no OpenGL context available here. If you want to prerender something,
	// create VBOs, etc. do it in Gamestate_PostLoad.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	progress(game);

	for (int i = 0; i < 2; i++) {
		data->myszole.lewo[i] = al_load_bitmap(GetDataFilePath(game, PunchNumber(game, "myszki/lewoX.webp", 'X', i)));
		progress(game);
	}
	for (int i = 0; i < 3; i++) {
		data->myszole.prawo[i] = al_load_bitmap(GetDataFilePath(game, PunchNumber(game, "myszki/prawoX.webp", 'X', i)));
		progress(game);
	}
	for (int i = 0; i < 3; i++) {
		data->myszole.gora[i] = al_load_bitmap(GetDataFilePath(game, PunchNumber(game, "myszki/goraX.webp", 'X', i)));
		progress(game);
	}
	for (int i = 0; i < 4; i++) {
		data->myszole.dol[i] = al_load_bitmap(GetDataFilePath(game, PunchNumber(game, "myszki/dolX.webp", 'X', i)));
		progress(game);
	}
	data->myszole.lewodol = al_load_bitmap(GetDataFilePath(game, "myszki/lewodol.webp"));
	progress(game);

	data->myszole.prawogora = al_load_bitmap(GetDataFilePath(game, "myszki/prawogora.webp"));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	for (int i = 0; i < 2; i++) {
		al_destroy_bitmap(data->myszole.lewo[i]);
	}
	for (int i = 0; i < 3; i++) {
		al_destroy_bitmap(data->myszole.prawo[i]);
	}
	for (int i = 0; i < 3; i++) {
		al_destroy_bitmap(data->myszole.gora[i]);
	}
	for (int i = 0; i < 4; i++) {
		al_destroy_bitmap(data->myszole.dol[i]);
	}
	al_destroy_bitmap(data->myszole.lewodol);
	al_destroy_bitmap(data->myszole.prawogora);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	HideMouse(game);
	StopMusic(game);
	PlaySound(game, "przejscie2", 1.5);
	data->counter = 0;
	data->con = 0;
	data->pos = 0;
	data->angle = 0;

	data->myszol = rand() % 6;

	data->rand = (rand() / (double)RAND_MAX) * 0.5 - 0.25;

	switch (data->myszol) {
		case MYSZOL_LEFT:
			data->myszka = data->myszole.lewo[rand() % 2];
			break;
		case MYSZOL_RIGHT:
			data->myszka = data->myszole.prawo[rand() % 3];
			break;
		case MYSZOL_TOP:
			data->myszka = data->myszole.gora[rand() % 3];
			break;
		case MYSZOL_BOTTOM:
			data->myszka = data->myszole.dol[rand() % 4];
			break;
		case MYSZOL_BOTTOM_LEFT:
			data->myszka = data->myszole.lewodol;
			break;
		case MYSZOL_TOP_RIGHT:
			data->myszka = data->myszole.prawogora;
			break;
	}
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	StopSound(game, "przejscie2");
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
