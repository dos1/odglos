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
	struct AnimationDecoder* animation;
	ALLEGRO_BITMAP *bmps[17], *mask;
	int current;

	bool playing;
	bool buffered;
	int play;
	double delay;
	double counter;

	double timeout;
	int sequence[4];
	bool success;
	int position;
	bool pressed;

	int user;
};

int Gamestate_ProgressCount = 3;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (delta > GetAnimationFrameDuration(data->animation)) {
		delta = GetAnimationFrameDuration(data->animation);
	}

	UpdateAnimation(data->animation, delta);

	if (GetAnimationFrameNo(data->animation) != data->current) {
		data->current = GetAnimationFrameNo(data->animation);
		//PrintConsole(game, "%d", data->current);
		data->bmps[data->current] = al_clone_bitmap(GetAnimationFrame(data->animation));
	}

	if (IsAnimationComplete(data->animation) && !data->buffered) {
		ShowMouse(game);
		data->buffered = true;
	}

	if (data->playing) {
		data->delay -= delta;
		if (data->delay < 0.0) {
			if (data->success) {
				data->delay = 0.1 + ((rand() % 50) / 1000.0);
				data->play = rand() % 17;
			} else if (data->pressed) {
				data->delay = data->user ? 8.0 : 2.0;
				data->pressed = false;
				data->play = 16;
				data->position = 0;
			} else if (data->position < 4) {
				HideMouse(game);
				data->delay = 0.4;
				data->play = data->sequence[data->position++];
			} else if (data->position == 4) {
				data->delay = 6.0;
				data->position = 0;
				data->play = 16;
				ShowMouse(game);
			}
		}
		if (data->success) {
			data->counter += delta;
		}
	}
	if (data->counter > 4.0) {
		SwitchCurrentGamestate(game, "anim");
	}

	CheckMask(game, data->mask);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bitmap;
	if (data->playing) {
		bitmap = data->bmps[data->play];
	} else {
		bitmap = GetAnimationFrame(data->animation);
	}
	al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (IsAnimationComplete(data->animation) && game->data->hover) {
		if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) ||
			(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
			(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
			ALLEGRO_COLOR color = CheckMask(game, data->mask);

			int pos = round(color.r * 255 / 10.0);
			PrintConsole(game, "%d", pos);
			if (pos > 15) {
				return;
			}
			data->play = 15 - pos;
			data->delay = 0.2;
			data->pressed = true;
			data->playing = true;

			if (data->sequence[data->user] == data->play) {
				data->user++;
			} else {
				data->user = 0;
			}
			if (data->user == 4) {
				data->success = true;
				HideMouse(game);
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

	data->animation = CreateAnimation(game, GetDataFilePath(game, "lawka_w_parku/lawka.awebp"), false);
	progress(game);
	data->bmps[0] = al_clone_bitmap(GetAnimationFrame(data->animation));
	progress(game);

	data->mask = al_load_bitmap(GetDataFilePath(game, "masks/lawka_w_parku_maski.png"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->animation);
	for (int i = 0; i < 17; i++) {
		al_destroy_bitmap(data->bmps[i]);
	}
	al_destroy_bitmap(data->mask);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	data->current = 0;
	data->playing = false;
	data->counter = 0;
	for (int i = 0; i < 4; i++) {
		data->sequence[i] = rand() % 16;
	}
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
