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

	enum BallType stack[3];
	int stackpos;
	bool frames;
	bool won;
	ALLEGRO_BITMAP* mask;
};

int Gamestate_ProgressCount = 24;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	if (data->frames) {
		AnimateCharacter(game, data->left.character, delta, 1.0);
		AnimateCharacter(game, data->center.character, delta, 1.0);
		AnimateCharacter(game, data->right.character, delta, 1.0);
	} else {
		AnimateCharacter(game, data->center.character, delta, 1.3);
	}

	CheckMask(game, data->mask);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	int twenty = game->viewport.width * 0.015625; // 20px
	if (data->frames) {
		SetClippingRectangle(0, 0, game->viewport.width / 3.0 - twenty, game->viewport.height);
		SetCharacterPosition(game, data->left.character, 0, 0, 0);
		DrawCharacter(game, data->left.character);

		SetClippingRectangle(game->viewport.width / 3.0, 0, game->viewport.width / 3.0 - twenty, game->viewport.height);
		SetCharacterPosition(game, data->center.character, game->viewport.width * 0.328125, 0, 0);
		DrawCharacter(game, data->center.character);

		SetClippingRectangle(2 * game->viewport.width / 3.0, 0, game->viewport.width / 3.0, game->viewport.height);
		SetCharacterPosition(game, data->right.character, game->viewport.width * 0.65625, 0, 0);
		DrawCharacter(game, data->right.character);
	} else {
		SetCharacterPosition(game, data->center.character, 0, 0, 0);
		DrawCharacter(game, data->center.character);

		if (!data->won) {
			al_draw_filled_rectangle(game->viewport.width / 3.0 - twenty, 0, game->viewport.width / 3.0, game->viewport.height, al_map_rgb(0, 0, 0));
			al_draw_filled_rectangle(game->viewport.width * 0.65104166666666666667, 0, game->viewport.width * 0.65104166666666666667 + twenty, game->viewport.height, al_map_rgb(0, 0, 0));
		}
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		if (!game->data->cursor || !game->data->hover) {
			return;
		}
		if (game->data->mouseX < 0.333) {
			if (data->left.type == BALL_TYPE_NONE) {
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SelectSpritesheet(game, data->left.character, "pudelko_1_czer_r");
						data->left.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->left.character, "pudelko_1_zolte_r");
						data->left.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->left.character, "pudelko_1_pom_r");
						data->left.type = BALL_TYPE_ORANGE;
						break;
					case BALL_TYPE_NONE:
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
					case BALL_TYPE_NONE:
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
						SelectSpritesheet(game, data->center.character, "pudelko_2_czer_r");
						data->center.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->center.character, "pudelko_2_zolte_r");
						data->center.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->center.character, "pudelko_2_pom_r");
						data->center.type = BALL_TYPE_ORANGE;
						break;
					case BALL_TYPE_NONE:
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
					case BALL_TYPE_NONE:
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
						SelectSpritesheet(game, data->right.character, "pudelko_3_czer_r");
						data->right.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SelectSpritesheet(game, data->right.character, "pudelko_3_zolte_r");
						data->right.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SelectSpritesheet(game, data->right.character, "pudelko_3_pom_r");
						data->right.type = BALL_TYPE_ORANGE;
						break;
					case BALL_TYPE_NONE:
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
					case BALL_TYPE_NONE:
						break;
				}
				EnqueueSpritesheet(game, data->right.character, "pudelko_3_memlanie");
				HideMouse(game);
			}
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		SwitchCurrentGamestate(game, "anim");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		SwitchCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->left.character = CreateCharacter(game, "pudelka");
	data->left.character->detailed_progress = true;

	char* anims[] = {
		"pudelka_poczatek",
		"pudelka_tancowanie",
		"pudelka_waz",
		"pudelko_1_memlanie",
		"pudelko_2_memlanie",
		"pudelko_3_memlanie",
		"pudelko_1_czer",
		"pudelko_1_pom",
		"pudelko_1_zolte",
		"pudelko_2_czer",
		"pudelko_2_pom",
		"pudelko_2_zolte",
		"pudelko_3_czer",
		"pudelko_3_pom",
		"pudelko_3_zolte",
		"pudelko_1_czer_r",
		"pudelko_1_pom_r",
		"pudelko_1_zolte_r",
		"pudelko_2_czer_r",
		"pudelko_2_pom_r",
		"pudelko_2_zolte_r",
		"pudelko_3_czer_r",
		"pudelko_3_pom_r",
		"pudelko_3_zolte_r",
	};

	for (size_t i = 0; i < sizeof(anims) / sizeof(anims[0]); i++) {
		char path[255] = {};
		snprintf(path, 255, "sprites/pudelka/%s.awebp", anims[i]);
		RegisterStreamedSpritesheet(game, data->left.character, anims[i], AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, path), false));
		progress(game);
	}

	data->center.character = CreateCharacter(game, "pudelka");
	data->center.character->shared = true;
	data->center.character->spritesheets = data->left.character->spritesheets;

	data->right.character = CreateCharacter(game, "pudelka");
	data->right.character->shared = true;
	data->right.character->spritesheets = data->left.character->spritesheets;

	data->mask = al_load_bitmap(GetDataFilePath(game, "masks/pudelka_od_cioci_maska.mask"));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyCharacter(game, data->right.character);
	DestroyCharacter(game, data->center.character);
	DestroyCharacter(game, data->left.character);
	al_destroy_bitmap(data->mask);
	free(data);
}

static void CheckWin(struct Game* game, struct GamestateResources* data) {
	if (data->won) {
		return;
	}
	if (data->left.type == BALL_TYPE_RED && data->center.type == BALL_TYPE_ORANGE && data->right.type == BALL_TYPE_YELLOW) {
		data->won = true;
		HideMouse(game);
		SelectSpritesheet(game, data->left.character, "pudelko_1_czer");
		SelectSpritesheet(game, data->center.character, "pudelko_2_pom");
		SelectSpritesheet(game, data->right.character, "pudelko_3_zolte");
		data->left.character->delta = 0;
		data->center.character->delta = 0;
		data->right.character->delta = 0;

		EnqueueSpritesheet(game, data->center.character, "pudelka_tancowanie");
	}
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
	if (old && strcmp("pudelko_1_czer_r", old->name) == 0 && !new) {
		CheckWin(game, data);
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
		if (strcmp("pudelka_poczatek", old->name) == 0 && old != new) {
			ShowMouse(game);
			d->frames = true;
			SelectSpritesheet(game, d->left.character, "pudelko_1_zolte_r");
			SelectSpritesheet(game, d->center.character, "pudelko_2_czer_r");
			SelectSpritesheet(game, d->right.character, "pudelko_3_pom_r");
		}
		if (strcmp("pudelka_tancowanie", new->name) == 0) {
			d->frames = false;
		}
	}
	if (old && strcmp("pudelko_2_pom_r", old->name) == 0 && !new) {
		PrintConsole(game, "checkwin");
		CheckWin(game, data);
	}
	if (old && strcmp("pudelka_waz", old->name) == 0) {
		SwitchCurrentGamestate(game, "anim");
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
	if (old && strcmp("pudelko_3_zolte_r", old->name) == 0 && !new) {
		CheckWin(game, data);
	}
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	SetCharacterPosition(game, data->left.character, 0, 0, 0);
	SetCharacterPosition(game, data->center.character, 0, 0, 0);
	SetCharacterPosition(game, data->right.character, 0, 0, 0);
	//SelectSpritesheet(game, data->left.character, "pudelka_poczatek");
	SelectSpritesheet(game, data->center.character, "pudelka_poczatek");
	//SelectSpritesheet(game, data->right.character, "pudelka_poczatek");
	EnqueueSpritesheet(game, data->center.character, "pudelko_2_czer_r");
	//data->left.character->pos = 1;
	//data->left.character->delta = 120;
	//data->right.character->pos = 2;
	//data->right.character->delta = 33;

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

	data->frames = false;

	HideMouse(game);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
