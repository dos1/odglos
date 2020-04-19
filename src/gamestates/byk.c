#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	struct Character* byk;

	ALLEGRO_BITMAP *bg, *gradient, *leaf;

	ALLEGRO_SAMPLE_INSTANCE* sound;
	ALLEGRO_SAMPLE* sample;

	int state;
	bool volup;
	float vol;

	float delay;

	bool munching;
	bool footnoted;
};

int Gamestate_ProgressCount = 6; // number of loading steps as reported by Gamestate_Load; 0 when missing

#define SCALE 0.66666

static void UpdateState(struct Game* game, struct GamestateResources* data) {
	int state = data->state;
	data->state = floor((sqrt(pow(game->data->mouseX * game->viewport.width - 1024 * SCALE, 2) + pow(game->data->mouseY * game->viewport.height - 900 * SCALE, 2)) + 75 * SCALE) / (150.0 * SCALE));

	if (data->state < 0) {
		data->state = 0;
	}
	game->data->hover = data->state == 0;

	if (data->state > 4) {
		data->state = 4;
	}

	if (data->state != state) {
		data->volup = true;
	}
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) {
		al_set_sample_instance_gain(data->sound, 0.0);
		return;
	}
	if (data->munching) {
		AnimateCharacter(game, data->byk, delta, 1.0);
		data->delay += delta;
		if (data->delay > 6.0) {
			if (!data->footnoted) {
				ShowFootnote(game, 4);
				data->footnoted = true;
			} else {
				ChangeCurrentGamestate(game, "anim");
			}
		} else {
			al_set_sample_instance_gain(data->sound, 0.75);
		}
		return;
	}

	if (data->volup) {
		data->vol += delta * 4.0;
	} else {
		data->vol -= delta * 4.0;
	}
	if (data->vol <= 0) {
		data->vol = 0;
	}
	if (data->vol >= 2.0) {
		data->volup = false;
	}

	UpdateState(game, data);

	if (data->vol > 1.5) {
		data->byk->pos = 4 - data->state;
		data->byk->frame = &data->byk->spritesheet->frames[data->byk->pos];
	}

	al_set_sample_instance_gain(data->sound, Clamp(0.0, 1.0, data->vol));
	//PrintConsole(game, "state %d vol %f volup %d", data->state, data->vol, data->volup);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, 0, 0, 0);
	DrawCharacter(game, data->byk);

	if (!data->munching) {
		al_draw_bitmap(data->leaf, game->data->mouseX * game->viewport.width - (game->data->touch ? 55 : 5), game->data->mouseY * game->viewport.height + (game->data->touch ? 0 : 30), 0);
	} else {
		al_draw_tinted_bitmap(data->gradient, al_map_rgba(128, 128, 128, 128), 0, 0, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.

	if (ev->type == ALLEGRO_EVENT_MOUSE_AXES || ev->type == ALLEGRO_EVENT_TOUCH_BEGIN || ev->type == ALLEGRO_EVENT_TOUCH_MOVE) {
		UpdateState(game, data);
	}

	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev->type == ALLEGRO_EVENT_TOUCH_END) {
		if (data->state == 0 && !data->munching) {
			SelectSpritesheet(game, data->byk, "chew");
			data->munching = true;
			PlayLoop(game, "funky", 1.0, false);
			HideMouse(game);
		}
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
	// NOTE: There's no OpenGL context available here. If you want to prerender something,
	// create VBOs, etc. do it in Gamestate_PostLoad.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));

	data->bg = al_load_bitmap(GetDataFilePath(game, "byk/byk.png"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar

	data->gradient = al_load_bitmap(GetDataFilePath(game, "gradient.png"));
	progress(game);

	data->leaf = al_load_bitmap(GetDataFilePath(game, "byk/lisc.png"));
	progress(game);

	data->sample = al_load_sample(GetDataFilePath(game, "byk/crunch.flac"));
	data->sound = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->sound, game->audio.fx);
	al_set_sample_instance_gain(data->sound, 0.0);
	al_set_sample_instance_speed(data->sound, 0.5);
	al_set_sample_instance_playmode(data->sound, ALLEGRO_PLAYMODE_LOOP);
	progress(game);

	data->byk = CreateCharacter(game, "byk");
	RegisterStreamedSpritesheet(game, data->byk, "open", AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, "sprites/byk/open.awebp"), false));
	PreloadStreamedSpritesheet(game, data->byk, "open");
	RegisterStreamedSpritesheet(game, data->byk, "chew", AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, "sprites/byk/chew.awebp"), true));
	LoadSpritesheets(game, data->byk, progress);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	DestroyCharacter(game, data->byk);
	al_destroy_sample_instance(data->sound);
	al_destroy_sample(data->sample);
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->gradient);
	al_destroy_bitmap(data->leaf);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	ShowMouse(game);
	StopMusic(game);
	PlayMusic(game, "dwor", 1.0);
	al_play_sample_instance(data->sound);
	SetCharacterPosition(game, data->byk, 775 * SCALE, 775 * SCALE, 0);
	SelectSpritesheet(game, data->byk, "open");
	data->state = 0;
	data->vol = 0;
	data->volup = false;
	data->byk->scaleX = SCALE;
	data->byk->scaleY = SCALE;
	data->delay = 0;
	data->munching = false;
	data->footnoted = false;
	game->data->mouseX = -1.0;
	game->data->mouseY = -1.0;
	UpdateState(game, data);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_set_sample_instance_playing(data->sound, false);
}

// Optional endpoints:

void Gamestate_PostLoad(struct Game* game, struct GamestateResources* data) {
	// This is called in the main thread after Gamestate_Load has ended.
	// Use it to prerender bitmaps, create VBOs, etc.
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic nor ProcessEvent)
	// Pause your timers and/or sounds here.
	al_set_sample_instance_playing(data->sound, false);
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers and/or sounds here.
	al_set_sample_instance_playing(data->sound, true);
}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {
	// Called when the display gets lost and not preserved bitmaps need to be recreated.
	// Unless you want to support mobile platforms, you should be able to ignore it.
}
