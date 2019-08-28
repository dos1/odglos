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
	ALLEGRO_BITMAP* bitmaps[4][4];
	int i;
	int j;
};

struct GamestateResources {
	ALLEGRO_FONT* font;
	struct PergolaCharacter left, right;
	double counter;
	ALLEGRO_BITMAP* anim[2];
	bool mode;
	bool locked;
	struct Timeline* timeline;
};

int Gamestate_ProgressCount = 34;

static TM_ACTION(Switch) {
	TM_RunningOnly;
	data->mode = !data->mode;
	return TM_END;
}

static TM_ACTION(Unlock) {
	TM_RunningOnly;
	data->locked = false;
	return TM_END;
}

static bool IsCompleted(struct Game* game, struct GamestateResources* data) {
	return data->left.i == 3 && data->left.j == 3 && data->right.i == 0 && data->right.j == 0;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	TM_Process(data->timeline, delta);
	if (!data->locked && IsCompleted(game, data)) {
		data->counter += delta;
	}
	if (data->counter > 4.0) {
		SwitchCurrentGamestate(game, "schodki_i_sowka");
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bmp = NULL;
	if (!data->locked && IsCompleted(game, data)) {
		bmp = data->anim[(int)floor(fmod(data->counter * 4.0, 2))];
	} else {
		if (data->mode) {
			bmp = data->right.bitmaps[data->right.j][data->right.i];
		} else {
			bmp = data->left.bitmaps[data->left.j][data->left.i];
		}
	}

	al_draw_scaled_bitmap(bmp, 0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
		0, 0, game->viewport.width, game->viewport.height, 0);

	DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), 2, game->viewport.height - 20, ALLEGRO_ALIGN_LEFT, "strzalki na klawiaturze");
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		if (game->data->mouseX < 0.33) {
			if ((game->data->mouseY > 0.33) && (game->data->mouseY < 0.66)) {
				ev->type = ALLEGRO_EVENT_KEY_DOWN;
				ev->keyboard.keycode = ALLEGRO_KEY_LEFT;
			}
		}
		if (game->data->mouseX > 0.66) {
			if ((game->data->mouseY > 0.33) && (game->data->mouseY < 0.66)) {
				ev->type = ALLEGRO_EVENT_KEY_DOWN;
				ev->keyboard.keycode = ALLEGRO_KEY_RIGHT;
			}
		}
		if (game->data->mouseY < 0.33) {
			if ((game->data->mouseX > 0.33) && (game->data->mouseX < 0.66)) {
				ev->type = ALLEGRO_EVENT_KEY_DOWN;
				ev->keyboard.keycode = ALLEGRO_KEY_UP;
			}
		}
		if (game->data->mouseY > 0.66) {
			if ((game->data->mouseX > 0.33) && (game->data->mouseX < 0.66)) {
				ev->type = ALLEGRO_EVENT_KEY_DOWN;
				ev->keyboard.keycode = ALLEGRO_KEY_DOWN;
			}
		}
	}

	if (!data->locked && !IsCompleted(game, data)) {
		if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
			bool changed = false;
			struct PergolaCharacter* c = data->mode ? &data->right : &data->left;
			if (ev->keyboard.keycode == ALLEGRO_KEY_UP) {
				c->j -= 1;
				changed = true;
				if (c->j < 0) {
					c->j = 0;
					return;
				}
			}
			if (ev->keyboard.keycode == ALLEGRO_KEY_DOWN) {
				c->j += 1;
				changed = true;
				if (c->j > 3) {
					c->j = 3;
					return;
				}
			}
			if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
				c->i -= 1;
				changed = true;
				if (c->i < 0) {
					c->i = 0;
					return;
				}
			}
			if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				c->i += 1;
				changed = true;
				if (c->i > 3) {
					c->i = 3;
					return;
				}
			}

			if (changed) {
				data->locked = true;
				TM_AddDelay(data->timeline, 0.5);
				TM_AddAction(data->timeline, Switch, NULL);
				TM_AddAction(data->timeline, Unlock, NULL);
			}
		}
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_MAG_LINEAR); // disable linear scaling for pixelarty appearance

	data->timeline = TM_Init(game, data, "timeline");

	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	al_set_new_bitmap_flags(flags);

	int x = 0;
	char filename[255] = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (x == 9) {
				x = 10;
			}
			snprintf(filename, 255, "pergola/pergola__%04d_sowka1_rzad%d_kolumna%d.webp", x, i + 1, j + 1);
			x++;
			data->left.bitmaps[i][j] = al_load_bitmap(GetDataFilePath(game, filename));
			progress(game);
		}
	}
	x = 21;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (x == 22) {
				x = 27;
			}
			snprintf(filename, 255, "pergola/pergola__%04d_sowka2_rzad%d_kolumna%d.webp", x, i + 1, j + 1);
			x++;
			data->right.bitmaps[i][j] = al_load_bitmap(GetDataFilePath(game, filename));
			progress(game);
		}
	}
	data->anim[0] = al_load_bitmap(GetDataFilePath(game, "pergola/pergola__0042_obie_sowki1.webp"));
	progress(game);
	data->anim[1] = al_load_bitmap(GetDataFilePath(game, "pergola/pergola__0043_obie_sowki2.webp"));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_font(data->font);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			al_destroy_bitmap(data->left.bitmaps[i][j]);
			al_destroy_bitmap(data->right.bitmaps[i][j]);
		}
	}
	al_destroy_bitmap(data->anim[0]);
	al_destroy_bitmap(data->anim[1]);
	TM_Destroy(data->timeline);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	data->left.i = 0;
	data->left.j = 0;
	data->right.i = 3;
	data->right.j = 3;
	data->mode = false;
	data->locked = false;
	data->counter = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
