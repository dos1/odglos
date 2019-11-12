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

enum BallType {
	BALL_TYPE_NONE,
	BALL_TYPE_YELLOW,
	BALL_TYPE_RED,
	BALL_TYPE_ORANGE,
};

struct GamestateResources {
	struct {
		enum BallType type;
		struct Character* character;
	} left, center, right;

	struct Character* character;

	enum BallType stack[3];
	int stackpos;
};

int Gamestate_ProgressCount = 160;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	AnimateCharacter(game, data->left.character, delta, 1.0);
	AnimateCharacter(game, data->center.character, delta, 1.0);
	AnimateCharacter(game, data->right.character, delta, 1.0);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	SetClippingRectangle(0, 0, game->viewport.width / 3.0 - 20, game->viewport.height);
	DrawCharacter(game, data->left.character);

	SetClippingRectangle(game->viewport.width / 3.0, 0, game->viewport.width / 3.0 - 20, game->viewport.height);
	DrawCharacter(game, data->center.character);

	SetClippingRectangle(2 * game->viewport.width / 3.0, 0, game->viewport.width / 3.0 - 20, game->viewport.height);
	DrawCharacter(game, data->right.character);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if (ev->type == ALLEGRO_EVENT_KEY_DOWN && ev->keyboard.keycode == ALLEGRO_KEY_SPACE) {
		SelectSpritesheet(game, data->left.character, "pudelko_1_czer");
		SelectSpritesheet(game, data->center.character, "pudelko_2_pom");
		SelectSpritesheet(game, data->right.character, "pudelko_3_zolte");
		data->left.character->delta = 0;
		data->center.character->delta = 0;
		data->right.character->delta = 0;

		EnqueueSpritesheet(game, data->left.character, "pudelka_tancowanie");
		EnqueueSpritesheet(game, data->center.character, "pudelka_tancowanie");
		EnqueueSpritesheet(game, data->right.character, "pudelka_tancowanie");
	}

	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		if (!game->data->cursor) {
			return;
		}
		if (game->data->mouseX < 0.333) {
			if (data->left.type == BALL_TYPE_NONE) {
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->left.character, "-pudelko_1_czer");
						data->left.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->left.character, "-pudelko_1_zolte");
						data->left.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->left.character, "-pudelko_1_pom");
						data->left.type = BALL_TYPE_ORANGE;
						break;
				}
			} else {
				switch (data->left.type) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->left.character, "pudelko_1_czer");
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->left.character, "pudelko_1_zolte");
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->left.character, "pudelko_1_pom");
						break;
				}
				EnqueueSpritesheet(game, data->left.character, "pudelko_1_memlanie");
				HideMouse(game);
			}
		}

		if (game->data->mouseX > 0.333 && game->data->mouseX < 0.666) {
			if (data->center.type == BALL_TYPE_NONE) {
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->center.character, "-pudelko_2_czer");
						data->center.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->center.character, "-pudelko_2_zolte");
						data->center.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->center.character, "-pudelko_2_pom");
						data->center.type = BALL_TYPE_ORANGE;
						break;
				}
			} else {
				switch (data->center.type) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->center.character, "pudelko_2_czer");
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->center.character, "pudelko_2_zolte");
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->center.character, "pudelko_2_pom");
						break;
				}
				EnqueueSpritesheet(game, data->center.character, "pudelko_2_memlanie");
				HideMouse(game);
			}
		}

		if (game->data->mouseX > 0.666) {
			if (data->right.type == BALL_TYPE_NONE) {
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->right.character, "-pudelko_3_czer");
						data->right.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->right.character, "-pudelko_3_zolte");
						data->right.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->right.character, "-pudelko_3_pom");
						data->right.type = BALL_TYPE_ORANGE;
						break;
				}
			} else {
				switch (data->right.type) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->right.character, "pudelko_3_czer");
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->right.character, "pudelko_3_zolte");
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->right.character, "pudelko_3_pom");
						break;
				}
				EnqueueSpritesheet(game, data->right.character, "pudelko_3_memlanie");
				HideMouse(game);
			}
		}
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->left.character = CreateCharacter(game, "pudelka");
	data->left.character->detailedProgress = true;

	RegisterSpritesheet(game, data->left.character, "pudelka_poczatek");
	RegisterSpritesheet(game, data->left.character, "pudelka_tancowanie");
	RegisterSpritesheet(game, data->left.character, "pudelka_waz");
	RegisterSpritesheet(game, data->left.character, "pudelko_1_czer");
	RegisterSpritesheet(game, data->left.character, "pudelko_1_memlanie");
	RegisterSpritesheet(game, data->left.character, "pudelko_1_pom");
	RegisterSpritesheet(game, data->left.character, "pudelko_1_zolte");
	RegisterSpritesheet(game, data->left.character, "pudelko_2_czer");
	RegisterSpritesheet(game, data->left.character, "pudelko_2_memlanie");
	RegisterSpritesheet(game, data->left.character, "pudelko_2_pom");
	RegisterSpritesheet(game, data->left.character, "pudelko_2_zolte");
	RegisterSpritesheet(game, data->left.character, "pudelko_3_czer");
	RegisterSpritesheet(game, data->left.character, "pudelko_3_memlanie");
	RegisterSpritesheet(game, data->left.character, "pudelko_3_pom");
	RegisterSpritesheet(game, data->left.character, "pudelko_3_zolte");
	LoadSpritesheets(game, data->left.character, progress);

	data->center.character = CreateCharacter(game, "pudelka");
	data->center.character->shared = true;
	data->center.character->spritesheets = data->left.character->spritesheets;

	data->right.character = CreateCharacter(game, "pudelka");
	data->right.character->shared = true;
	data->right.character->spritesheets = data->left.character->spritesheets;
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyCharacter(game, data->right.character);
	DestroyCharacter(game, data->center.character);
	DestroyCharacter(game, data->left.character);
	free(data);
}

static CHARACTER_CALLBACK(GoForward) {
	//SwitchCurrentGamestate(game, "samochod_kominek");
}

static CHARACTER_CALLBACK(HandleLeft) {
	struct GamestateResources* d = data;
	if (old && new) {
		if (strcmp("pudelko_1_memlanie", new->name) == 0) {
			d->stack[d->stackpos++] = d->left.type;
			d->left.type = BALL_TYPE_NONE;
			PrintConsole(game, "memlam lewe");
			ShowMouse(game);
		}
	}
}

static CHARACTER_CALLBACK(HandleCenter) {
	struct GamestateResources* d = data;
	if (old && new) {
		if (strcmp("pudelko_2_memlanie", new->name) == 0) {
			d->stack[d->stackpos++] = d->center.type;
			d->center.type = BALL_TYPE_NONE;
			PrintConsole(game, "memlam srodkowe");
			ShowMouse(game);
		}
	}
}

static CHARACTER_CALLBACK(HandleRight) {
	struct GamestateResources* d = data;
	if (old && new) {
		if (strcmp("pudelko_3_memlanie", new->name) == 0) {
			d->stack[d->stackpos++] = d->right.type;
			d->right.type = BALL_TYPE_NONE;
			PrintConsole(game, "memlam prawe");
			ShowMouse(game);
		}
	}
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	SetCharacterPosition(game, data->left.character, 0, 0, 0);
	SetCharacterPosition(game, data->center.character, 0, 0, 0);
	SetCharacterPosition(game, data->right.character, 0, 0, 0);
	SelectSpritesheet(game, data->left.character, "pudelka_poczatek");
	SelectSpritesheet(game, data->center.character, "pudelka_poczatek");
	SelectSpritesheet(game, data->right.character, "pudelka_poczatek");
	EnqueueSpritesheet(game, data->left.character, "-pudelko_1_zolte");
	EnqueueSpritesheet(game, data->center.character, "-pudelko_2_czer");
	EnqueueSpritesheet(game, data->right.character, "-pudelko_3_pom");
	data->left.character->pos = 1;
	data->left.character->delta = 120;
	data->right.character->pos = 2;
	data->right.character->delta = 33;

	data->left.character->callback = HandleLeft;
	data->left.character->callback_data = data;
	data->center.character->callback = HandleCenter;
	data->center.character->callback_data = data;
	data->right.character->callback = HandleRight;
	data->right.character->callback_data = data;

	data->left.type = BALL_TYPE_YELLOW;
	data->center.type = BALL_TYPE_RED;
	data->right.type = BALL_TYPE_ORANGE;

	data->stackpos = 0;
	data->stack[0] = BALL_TYPE_NONE;
	data->stack[1] = BALL_TYPE_NONE;
	data->stack[2] = BALL_TYPE_NONE;

	ShowMouse(game);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
