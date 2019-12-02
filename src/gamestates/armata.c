/*! \file example.c
 *  \brief Example gamestate.
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
	struct AnimationDecoder* anim;
	ALLEGRO_BITMAP *bg, *dol, *drzewo, *maska;
	struct Character* makieta;
	bool clicked, finished;
};

int Gamestate_ProgressCount = 7;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
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
			SwitchCurrentGamestate(game, "anim");
		}
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	al_draw_scaled_bitmap(data->bg, 0, 0, al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg), 0, 0, game->viewport.width, game->viewport.height, 0);
	al_draw_scaled_bitmap(data->dol, 0, 0, al_get_bitmap_width(data->dol), al_get_bitmap_height(data->dol), 0, 0, game->viewport.width, game->viewport.height, 0);

	ALLEGRO_BITMAP* bitmap = GetAnimationFrame(data->anim);
	al_draw_scaled_rotated_bitmap(bitmap, 888, 269, 888, 269, game->viewport.width / (float)al_get_bitmap_width(bitmap), game->viewport.height / (float)al_get_bitmap_height(bitmap), sin(game->time / 2.0) / 16.0, 0);

	if (data->finished) {
		DrawCharacter(game, data->makieta);
		al_draw_scaled_rotated_bitmap(data->maska, 888, 269, 888, 269, game->viewport.width / (float)al_get_bitmap_width(data->maska), game->viewport.height / (float)al_get_bitmap_height(data->maska), sin(game->time / 2.0) / 16.0, 0);
		//al_draw_scaled_bitmap(data->maska, 0, 0, al_get_bitmap_width(data->maska), al_get_bitmap_height(data->maska), 0, 0, game->viewport.width, game->viewport.height, 0);
	}

	al_draw_scaled_bitmap(data->drzewo, 0, 0, al_get_bitmap_width(data->drzewo), al_get_bitmap_height(data->drzewo), 0, 0, game->viewport.width, game->viewport.height, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) ||
		(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
		(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
		data->clicked = true;
		HideMouse(game);
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->anim = CreateAnimation(GetDataFilePath(game, "armata/armata_strzela.awebp"));
	progress(game);

	data->bg = al_load_bitmap(GetDataFilePath(game, "armata/armata_zszopowane_tlo_UZYC.jpg"));
	progress(game);
	data->drzewo = al_load_bitmap(GetDataFilePath(game, "armata/armata_drzewo.png"));
	progress(game);
	data->dol = al_load_bitmap(GetDataFilePath(game, "armata/armata_dol.png"));
	progress(game);
	data->maska = al_load_bitmap(GetDataFilePath(game, "armata/armata_gora_1_maska.png"));
	progress(game);

	data->makieta = CreateCharacter(game, "makieta");
	RegisterStreamedSpritesheet(game, data->makieta, "makieta", AnimationStream, DestroyStream, CreateAnimation(GetDataFilePath(game, "sprites/makieta/makieta.awebp")));

	//RegisterSpritesheet(game, data->makieta, "makieta");
	LoadSpritesheets(game, data->makieta, progress);

	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->anim);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	ShowMouse(game);
	SetCharacterPosition(game, data->makieta, 685, 235, 0);
	data->makieta->scaleX = 0.333;
	data->makieta->scaleY = 0.333;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
