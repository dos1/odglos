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
	ALLEGRO_BITMAP *bg, *fg;
	double delay;
	int repeats;
	int x, y;
};

int Gamestate_ProgressCount = 2;

static void LoadAnimation(struct Game* game, struct GamestateResources* data, void (*progress)(struct Game*)) {
	char path[255] = {0};
	snprintf(path, 255, "animations/%s.awebp", game->data->scene->name);
	if (data->anim) {
		DestroyAnimation(data->anim);
	}
	if (data->bg) {
		al_destroy_bitmap(data->bg);
		data->bg = NULL;
	}
	if (data->fg) {
		al_destroy_bitmap(data->fg);
		data->fg = NULL;
	}
	data->anim = CreateAnimation(GetDataFilePath(game, path));
	if (progress) {
		progress(game);
	}
	if (game->data->scene->bg) {
		data->bg = al_load_bitmap(GetDataFilePath(game, game->data->scene->bg));
	}
	if (game->data->scene->fg) {
		data->fg = al_load_bitmap(GetDataFilePath(game, game->data->scene->fg));
	}
	data->repeats = game->data->scene->repeats;
	data->x = game->data->scene->x;
	data->y = game->data->scene->y;
	ResetAnimation(data->anim);
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	float modifier = 1.25 * ((game->data->scene->speed == 0.0) ? 1.0 : game->data->scene->speed);

	if (data->delay > 0) {
		data->delay -= delta * modifier;
		if (data->delay <= 0) {
			data->delay = 0;
			if (!data->repeats) {
				if (!Dispatch(game)) {
					SwitchCurrentGamestate(game, "end");
					return;
				}
				if (game->data->scene->name[0] == '>') {
					ChangeCurrentGamestate(game, game->data->scene->name + 1);
				} else {
					LoadAnimation(game, data, NULL);
				}
			} else {
				ResetAnimation(data->anim);
				data->repeats--;
			}
		}
	} else {
		if (!UpdateAnimation(data->anim, delta * modifier)) {
			data->delay = GetAnimationFrameDuration(data->anim);
		}
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bitmap;
	if (data->bg) {
		bitmap = data->bg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
	bitmap = GetAnimationFrame(data->anim);
	al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), data->x, data->y, game->viewport.width, game->viewport.height, 0);
	if (data->fg) {
		bitmap = data->fg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	Dispatch(game);
	LoadAnimation(game, data, progress);
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->anim);
	if (data->bg) {
		al_destroy_bitmap(data->bg);
	}
	if (data->fg) {
		al_destroy_bitmap(data->fg);
	}
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	data->delay = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
