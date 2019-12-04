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
	bool finished;
	void (*callback)(struct Game*, int, int*, int*, struct Character*);
	struct Character* character;

	int freezeno;
	ALLEGRO_BITMAP* mask;
	bool frozen;
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
	if (data->mask) {
		al_destroy_bitmap(data->mask);
		data->mask = NULL;
	}
	if (data->character) {
		DestroyCharacter(game, data->character);
		data->character = NULL;
	}
	data->anim = CreateAnimation(GetDataFilePath(game, path));
	if (progress) {
		progress(game);
	}
	if (game->data->scene->bg) {
		char p[255] = {0};
		snprintf(p, 255, "%s.png", game->data->scene->bg);
		data->bg = al_load_bitmap(GetDataFilePath(game, p));
	}
	if (game->data->scene->fg) {
		char p[255] = {0};
		snprintf(p, 255, "%s.png", game->data->scene->fg);
		data->fg = al_load_bitmap(GetDataFilePath(game, p));
	}
	data->repeats = game->data->scene->repeats;
	data->callback = game->data->scene->callback;
	if (game->data->scene->character.name) {
		data->character = CreateCharacter(game, game->data->scene->character.name);
		char p[255] = {};
		snprintf(p, 255, "sprites/%s/%s.awebp", game->data->scene->character.name, game->data->scene->character.spritesheet);
		RegisterStreamedSpritesheet(game, data->character, game->data->scene->character.spritesheet, AnimationStream, DestroyStream, CreateAnimation(GetDataFilePath(game, p)));
		if (game->data->scene->character.preload) {
			PreloadStreamedSpritesheet(game, data->character, game->data->scene->character.spritesheet);
		}
		SelectSpritesheet(game, data->character, game->data->scene->character.spritesheet);
		SetCharacterPosition(game, data->character, 0, 0, 0);
	}
	data->x = 0;
	data->y = 0;
	data->freezeno = 0;
	if (data->callback) {
		data->callback(game, 0, &data->x, &data->y, data->character);
	}
	ResetAnimation(data->anim);
	PrintConsole(game, "Loaded: %s", path);
}

static void HandleDispatch(struct Game* game, struct GamestateResources* data, void (*progress)(struct Game*)) {
	if (!Dispatch(game)) {
		SwitchCurrentGamestate(game, "end");
	} else {
		if (game->data->scene->name[0] == '>') {
			ChangeCurrentGamestate(game, game->data->scene->name + 1);
		} else {
			LoadAnimation(game, data, progress);
			data->finished = false;
		}
	}
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	float modifier = 1.25 * ((game->data->scene->speed == 0.0) ? 1.0 : game->data->scene->speed);

	game->data->debuginfo = GetAnimationFrameNo(data->anim);

	if (!data->frozen && game->data->scene->freezes[data->freezeno].mask && game->data->scene->freezes[data->freezeno].frame == GetAnimationFrameNo(data->anim)) {
		data->frozen = true;
		ShowMouse(game);
		if (!data->mask) {
			char path[255] = {0};
			snprintf(path, 255, "masks/%s.png", game->data->scene->freezes[data->freezeno].mask);
			data->mask = al_load_bitmap(GetDataFilePath(game, path));
		}
		PrintConsole(game, "Freeze: [%d] %s (frame: %d)", data->freezeno, game->data->scene->freezes[data->freezeno].mask, GetAnimationFrameNo(data->anim));
	}

	if (data->frozen) {
		CheckMask(game, data->mask);
		return;
	}

	if (data->character) {
		AnimateCharacter(game, data->character, delta, modifier);
	}

	if (data->delay > 0) {
		data->delay -= delta;
		if (data->delay <= 0) {
			data->delay = 0;
			if (!data->repeats) {
				HandleDispatch(game, data, NULL);
			} else {
				ResetAnimation(data->anim);
				data->repeats--;
			}
		}
	} else {
		if (!UpdateAnimation(data->anim, delta * modifier)) {
			data->delay = GetAnimationFrameDuration(data->anim) / modifier;
			data->finished = true;
		}
	}

	if (data->callback) {
		data->callback(game, GetAnimationFrameNo(data->anim) + GetAnimationFrameCount(data->anim) * (game->data->scene->repeats - data->repeats) + 1, &data->x, &data->y, data->character);
	} else {
		data->x = 0;
		data->y = 0;
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bitmap;
	if (data->bg) {
		bitmap = data->bg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
	bitmap = GetAnimationFrame(data->anim);
	al_draw_bitmap(bitmap, data->x, data->y, 0);

	if (data->character) {
		DrawCharacter(game, data->character);
	}

	if (data->fg) {
		bitmap = data->fg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->hover && (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) || (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN))) {
		if (data->frozen) {
			data->freezeno++;
			al_destroy_bitmap(data->mask);
			data->mask = NULL;
			data->frozen = false;
			HideMouse(game);
			PrintConsole(game, "Unfreeze!");
		}
	}

	if (game->_priv.showconsole && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		data->delay = 0.01;
		data->finished = true;
		data->frozen = false;
		data->freezeno++;
		data->repeats = 0;
	}
	if (game->_priv.showconsole && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		data->delay = 0.01;
		data->finished = true;
		game->data->sceneid--;
		if (game->data->sceneid < -1) {
			game->data->sceneid = -1;
		}
		data->frozen = false;
		data->freezeno++;
		data->repeats = 0;
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	HandleDispatch(game, data, progress);
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
	if (data->mask) {
		al_destroy_bitmap(data->mask);
	}
	if (data->character) {
		DestroyCharacter(game, data->character);
	}
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	if (data->finished) {
		HandleDispatch(game, data, NULL);
	}
	data->delay = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
