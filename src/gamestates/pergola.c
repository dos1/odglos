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

struct PergolaCharacter {
	struct AnimationDecoder* animations[4][4];
	int i;
	int j;
	ALLEGRO_BITMAP *controls, *hint;
};

struct GamestateResources {
	struct PergolaCharacter left, right;
	double counter;
	bool mode;
	bool locked;
	struct Timeline* timeline;
	int changed;
	double hint;
};

int Gamestate_ProgressCount = 36;

static bool IsCompleted(struct Game* game, struct GamestateResources* data) {
	return data->left.i == 3 && data->left.j == 3 && data->right.i == 0 && data->right.j == 0;
}

static TM_ACTION(Switch) {
	TM_RunningOnly;
	data->mode = !data->mode;
	if (!IsCompleted(game, data)) {
		data->hint = 255;
	}
	if (data->mode) {
		ResetAnimation(data->right.animations[data->right.j][data->right.i]);
	} else {
		ResetAnimation(data->left.animations[data->left.j][data->left.i]);
	}
	return TM_END;
}

static TM_ACTION(Unlock) {
	TM_RunningOnly;
	data->locked = false;
	ShowMouse(game);
	return TM_END;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	TM_Process(data->timeline, delta);
	data->counter += delta;
	data->hint -= delta * 100;
	if (data->hint < 0) {
		data->hint = 0;
	}
	if (data->locked) {
		if (data->mode) {
			UpdateAnimation(data->right.animations[data->right.j][data->right.i], delta);
		} else {
			UpdateAnimation(data->left.animations[data->left.j][data->left.i], delta);
		}
	}
	if (!data->locked && IsCompleted(game, data)) {
		SwitchCurrentGamestate(game, "anim");
	}

	struct PergolaCharacter* c = data->mode ? &data->right : &data->left;
	ALLEGRO_COLOR color = CheckMask(game, c->controls);
	game->data->hover = color.a > 0.5;
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bmp = NULL;
	if (data->mode) {
		bmp = GetAnimationFrame(data->right.animations[data->right.j][data->right.i]);
		al_draw_scaled_rotated_bitmap(bmp, al_get_bitmap_width(bmp) / 2.0, al_get_bitmap_height(bmp) / 2.0,
			655, 353, 1355.0 / 1280.0, 1355.0 / 1280.0, 0, 0);
	} else {
		bmp = GetAnimationFrame(data->left.animations[data->left.j][data->left.i]);
		al_draw_scaled_bitmap(bmp, 0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
			0, 0, game->viewport.width, game->viewport.height, 0);
	}

	float hint = pow(data->hint / 255.0, 0.9);

	if (data->mode) {
		int frame = GetAnimationFrameNo(data->right.animations[data->right.j][data->right.i]) + data->right.j + data->right.i;
		al_draw_tinted_scaled_bitmap(data->right.controls, al_map_rgba(255 - frame * 4, 255 - frame * 4, 255 - frame * 4, 255),
			0, 0, al_get_bitmap_width(data->right.controls), al_get_bitmap_height(data->right.controls),
			0, 0, game->viewport.width, game->viewport.height, 0);

		al_draw_tinted_scaled_rotated_bitmap(data->right.hint, al_map_rgba_f(hint, hint, hint, hint),
			al_get_bitmap_width(data->right.hint) / 2.0, al_get_bitmap_height(data->right.hint) / 2.0,
			655, 353, 1355.0 / 1280.0, 1355.0 / 1280.0, 0, 0);
	} else {
		int frame = GetAnimationFrameNo(data->left.animations[data->left.j][data->left.i]) + data->left.j + data->left.i;
		al_draw_tinted_scaled_bitmap(data->left.controls, al_map_rgba(255 - frame * 4, 255 - frame * 4, 255 - frame * 4, 255),
			0, 0, al_get_bitmap_width(data->left.controls), al_get_bitmap_height(data->left.controls),
			0, 0, game->viewport.width, game->viewport.height, 0);

		al_draw_tinted_scaled_bitmap(data->left.hint, al_map_rgba_f(hint, hint, hint, hint),
			0, 0, al_get_bitmap_width(data->left.hint), al_get_bitmap_height(data->left.hint),
			0, 0, game->viewport.width, game->viewport.height, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		if (!data->locked && game->data->hover) {
			ev->keyboard.type = ALLEGRO_EVENT_KEY_DOWN;
			int pos = game->data->mouseY * 720;
			if (pos > 410) {
				ev->keyboard.keycode = ALLEGRO_KEY_LEFT;
			} else if (pos > 305) {
				ev->keyboard.keycode = ALLEGRO_KEY_DOWN;
			} else if (pos > 210) {
				ev->keyboard.keycode = ALLEGRO_KEY_RIGHT;
			} else {
				ev->keyboard.keycode = ALLEGRO_KEY_UP;
			}
		}
	}

	if (!data->locked && !IsCompleted(game, data)) {
		struct PergolaCharacter* c = data->mode ? &data->right : &data->left;

		if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev->keyboard.keycode == ALLEGRO_KEY_UP) {
				c->j -= 1;
				if (c->j < 0) {
					c->j = 0;
					return;
				}
				data->changed++;
			}
			if (ev->keyboard.keycode == ALLEGRO_KEY_DOWN) {
				c->j += 1;
				if (c->j > 3) {
					c->j = 3;
					return;
				}
				data->changed++;
			}
			if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
				c->i -= 1;
				if (c->i < 0) {
					c->i = 0;
					return;
				}
				data->changed++;
			}
			if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				c->i += 1;
				if (c->i > 3) {
					c->i = 3;
					return;
				}
				data->changed++;
			}

			if (data->changed == 3) {
				data->locked = true;
				HideMouse(game);
				data->changed = 0;
				TM_AddDelay(data->timeline, 0.5);
				TM_AddAction(data->timeline, Unlock, NULL);
				TM_AddAction(data->timeline, Switch, NULL);
			}
		}
	}

	if (game->_priv.showconsole && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		SwitchCurrentGamestate(game, "anim");
	}
	if (game->_priv.showconsole && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		SwitchCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));

	data->timeline = TM_Init(game, data, "timeline");

	char filename[255] = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			snprintf(filename, 255, "pergola/sowka_1/rzad_%d_kolumna_%d/anim.awebp", i + 1, j + 1);
			data->left.animations[i][j] = CreateAnimation(game, GetDataFilePath(game, filename), false);
			progress(game);
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			snprintf(filename, 255, "pergola/sowka_2/rzad_%d_kolumna_%d/anim.awebp", i + 1, j + 1);
			data->right.animations[i][j] = CreateAnimation(game, GetDataFilePath(game, filename), false);
			progress(game);
		}
	}

	data->left.controls = al_load_bitmap(GetDataFilePath(game, "pergola/pergola_strzalki_lewe.png"));
	progress(game);
	data->right.controls = al_load_bitmap(GetDataFilePath(game, "pergola/pergola_strzalki_prawe.png"));
	progress(game);
	data->left.hint = al_load_bitmap(GetDataFilePath(game, "pergola/tasma_lewa.png"));
	progress(game);
	data->right.hint = al_load_bitmap(GetDataFilePath(game, "pergola/DSCF7662_tasma_prawa.png"));
	progress(game);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DestroyAnimation(data->left.animations[i][j]);
			DestroyAnimation(data->right.animations[i][j]);
		}
	}
	al_destroy_bitmap(data->left.hint);
	al_destroy_bitmap(data->right.hint);
	al_destroy_bitmap(data->left.controls);
	al_destroy_bitmap(data->right.controls);
	TM_Destroy(data->timeline);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	ShowMouse(game);
	data->left.i = 0;
	data->left.j = 0;
	data->right.i = 3;
	data->right.j = 3;
	data->mode = false;
	data->hint = 255;
	data->locked = false;
	data->counter = 0;
	data->changed = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
