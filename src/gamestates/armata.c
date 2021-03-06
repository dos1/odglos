/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	struct AnimationDecoder* anim;
	ALLEGRO_BITMAP *bg, *dol, *drzewo, *maska;
	struct Character* makieta;
	bool clicked, finished;
	double angle, counter;
};

int Gamestate_ProgressCount = 7;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }
	data->counter -= delta;
	if (data->counter < 0) {
		data->angle = sin(game->time / 2.0) / 16.0;
		data->counter = 0.1;
	}

	if (data->clicked) {
		if (!UpdateAnimation(data->anim, delta * 1.0)) {
			//ResetAnimation(data->anim);
			data->finished = true;
		}
	}
	if (data->finished) {
		int pos = data->makieta->pos;
		AnimateCharacter(game, data->makieta, delta, 1.0);
		while (pos != data->makieta->pos && data->makieta->pos) {
			MoveCharacter(game, data->makieta, -600 * delta * (1.0 + fmax(0.0, data->makieta->x) / 4.0) * 6, -69 * delta * 6, 0);
			data->makieta->scaleX += 6 * delta / 2.0;
			data->makieta->scaleY += 6 * delta / 2.0;
			pos++;
		}

		if (pos && !data->makieta->pos) { // XXX: this may break!
			ChangeCurrentGamestate(game, "anim");
		}
	}

	ALLEGRO_COLOR color = CheckMask(game, GetAnimationFrame(data->anim));
	game->data->hover = color.a > 0.5;
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	al_draw_scaled_bitmap(data->bg, 0, 0, al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg), 0, 0, game->viewport.width, game->viewport.height, 0);
	al_draw_bitmap(data->dol, 815, 236, 0);

	ALLEGRO_BITMAP* bitmap = GetAnimationFrame(data->anim);
	al_draw_scaled_rotated_bitmap(bitmap, 888, 269, 888, 269, game->viewport.width / (float)al_get_bitmap_width(bitmap), game->viewport.height / (float)al_get_bitmap_height(bitmap), data->angle, 0);

	if (data->finished) {
		DrawCharacter(game, data->makieta);
		al_draw_scaled_rotated_bitmap(data->maska, 888 - 659, 269 - 202, 888, 269, 1.0, 1.0, data->angle, 0);
	}

	al_draw_bitmap(data->drzewo, 125, 0, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	ALLEGRO_COLOR color = CheckMask(game, GetAnimationFrame(data->anim));
	game->data->hover = color.a > 0.5;

	if (game->data->hover && ((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN))) {
		data->clicked = true;
		HideMouse(game);
		PlayMusic(game, "odwilz_trickstar5", 1.0);
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
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->anim = CreateAnimation(game, GetDataFilePath(game, "armata/armata_strzela.awebp"), false);
	progress(game);

	data->bg = al_load_bitmap(GetDataFilePath(game, "armata/armata_zszopowane_tlo_UZYC.png"));
	progress(game);
	data->drzewo = al_load_bitmap(GetDataFilePath(game, "armata/armata_drzewo.png"));
	progress(game);
	data->dol = al_load_bitmap(GetDataFilePath(game, "armata/armata_dol.png"));
	progress(game);
	data->maska = al_load_bitmap(GetDataFilePath(game, "armata/armata_gora_1_maska.png"));
	progress(game);

	data->makieta = CreateCharacter(game, "makieta");
	RegisterStreamedSpritesheet(game, data->makieta, "makieta", AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, "sprites/makieta/makieta.awebp"), false));

	//RegisterSpritesheet(game, data->makieta, "makieta");
	LoadSpritesheets(game, data->makieta, progress);

	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->anim);
	DestroyCharacter(game, data->makieta);
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->dol);
	al_destroy_bitmap(data->drzewo);
	al_destroy_bitmap(data->maska);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	ShowMouse(game);
	SetCharacterPosition(game, data->makieta, 685, 235, 0);
	data->makieta->scaleX = 0.333;
	data->makieta->scaleY = 0.333;
	data->angle = sin(game->time / 2.0) / 16.0;
	data->counter = 0.1;
	data->clicked = false;
	data->finished = false;
	SelectSpritesheet(game, data->makieta, "makieta");
	ResetAnimation(data->anim, true);
	PlayMusic(game, "odwilz_trickstar3", 1.0);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
