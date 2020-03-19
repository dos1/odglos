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
	struct Character* bg;
	ALLEGRO_BITMAP* mask;
	int step;
};

int Gamestate_ProgressCount = 11;

static CHARACTER_CALLBACK(ShowMouseCb) {
	ShowMouse(game);
}

static CHARACTER_CALLBACK(GoForwardCb) {
	ChangeCurrentGamestate(game, "anim");
}

static CHARACTER_CALLBACK(OutroCb) {
	PlayMusic(game, "odwilz_trickstar", 1.0);
	SelectSpritesheet(game, character, "naparstki_10b_KONCOWKA_chodaki_owce");
	character->callback = GoForwardCb;
}

static void CheckHover(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_COLOR color = CheckMask(game, data->mask);
	int val = 0.0;
	if (data->step == 0 || data->step == 1) {
		val = color.r;
	} else if (data->step == 2) {
		val = color.b;
	} else if (data->step == 3) {
		val = color.g;
	}
	game->data->hover = val > 0.5;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }
	AnimateCharacter(game, data->bg, delta, 1.0);
	CheckHover(game, data);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	SetCharacterPosition(game, data->bg, game->viewport.width / 2.0, game->viewport.height / 2.0, 0);
	DrawCharacter(game, data->bg);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	CheckHover(game, data);

	if (game->data->cursor &&
		(((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE)) ||
			(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) ||
			(ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
			(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN))) {
		if (game->data->hover) {
			if (data->step == 0) {
				SelectSpritesheet(game, data->bg, "naparstki_10a_latarnia_pusta_sowka_piana_czesc1");
				PlaySound(game, "K STUD 01 39 48-001", 1.0);
			}
			if (data->step == 1) {
				SelectSpritesheet(game, data->bg, "naparstki_10a_latarnia_pusta_sowka_piana_czesc2");
				PlaySound(game, "pudelko2", 1.0);
			}
			HideMouse(game);
			data->bg->callback = ShowMouseCb;
			if (data->step == 2) {
				SelectSpritesheet(game, data->bg, "naparstki_10a_latarnia_pusta_sowka_piana_czesc3");
				data->bg->callback = OutroCb;
				PlaySound(game, "pudelko3", 1.0);
			}
			data->step++;
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		ChangeCurrentGamestate(game, "anim");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid++;
		ChangeCurrentGamestate(game, "naparstki");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->bg = CreateCharacter(game, "naparstki");

	char* anims[] = {
		"naparstki_10_latarnia_zbija_wszystko",
		"naparstki_10a_latarnia_pusta_sowka_piana_czesc1",
		"naparstki_10a_latarnia_pusta_sowka_piana_czesc2",
		"naparstki_10a_latarnia_pusta_sowka_piana_czesc3",
		"naparstki_10b_KONCOWKA_chodaki_owce",
	};

	for (size_t i = 0; i < sizeof(anims) / sizeof(anims[0]); i++) {
		char path[255] = {};
		snprintf(path, 255, "sprites/naparstki/%s.awebp", anims[i]);
		RegisterStreamedSpritesheet(game, data->bg, anims[i], AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, path), false));
		progress(game);
	}
	LoadSpritesheets(game, data->bg, progress);

	data->mask = al_load_bitmap(GetDataFilePath(game, "naparstki2.png"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyCharacter(game, data->bg);
	al_destroy_bitmap(data->mask);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	SelectSpritesheet(game, data->bg, "naparstki_10_latarnia_zbija_wszystko");
	data->bg->callback = ShowMouseCb;
	data->step = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
