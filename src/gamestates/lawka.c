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
	bool footnoted;

	ALLEGRO_AUDIO_STREAM* stream[16];

	int user;
};

int Gamestate_ProgressCount = 3;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	if (delta > GetAnimationFrameDuration(data->animation)) {
		delta = GetAnimationFrameDuration(data->animation);
	}

	UpdateAnimation(data->animation, delta);

	if (GetAnimationFrameNo(data->animation) != data->current) {
		data->current = GetAnimationFrameNo(data->animation);
		//PrintConsole(game, "%d", data->current);
		data->bmps[data->current] = al_clone_bitmap(GetAnimationFrame(data->animation));
		if (data->current < 16) {
			al_rewind_audio_stream(data->stream[data->current]);
			al_set_audio_stream_playing(data->stream[data->current], true);
		}
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
			} else if (data->position < 3) {
				HideMouse(game);
				data->delay = 0.4;
				data->play = data->sequence[data->position++];
			} else if (data->position == 3) {
				data->delay = 6.0;
				data->position = 0;
				data->play = 16;
				ShowMouse(game);
			}

			if (data->play < 16) {
				al_rewind_audio_stream(data->stream[data->play]);
				al_set_audio_stream_playing(data->stream[data->play], true);
			}
		}
		if (data->success) {
			data->counter += delta;
		}
	}
	if (data->counter > 4.0) {
		if (!data->footnoted) {
			data->footnoted = true;
			ShowFootnote(game, 5);
		} else {
			SwitchCurrentGamestate(game, "anim");
		}
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
	if (game->data->footnote) { return; }

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
			al_rewind_audio_stream(data->stream[data->play]);
			al_set_audio_stream_playing(data->stream[data->play], true);

			if (data->sequence[data->user] == data->play) {
				data->user++;
			} else {
				data->user = 0;
			}
			if (data->user == 3) {
				data->success = true;
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

	data->animation = CreateAnimation(game, GetDataFilePath(game, "lawka_w_parku/lawka.awebp"), false);
	progress(game);
	data->bmps[0] = al_clone_bitmap(GetAnimationFrame(data->animation));
	progress(game);

	for (int i = 0; i < 16; i++) {
		char path[255] = {};
		snprintf(path, 255, "sounds/lawka/%d.flac.opus", i + 1);
		data->stream[i] = al_load_audio_stream(GetDataFilePath(game, path), 4, 2048);
		al_set_audio_stream_playing(data->stream[i], false);
		al_attach_audio_stream_to_mixer(data->stream[i], game->audio.fx);
	}

	data->mask = al_load_bitmap(GetDataFilePath(game, "masks/lawka_w_parku_maski.mask"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->animation);
	for (int i = 0; i < 17; i++) {
		al_destroy_bitmap(data->bmps[i]);
	}
	al_destroy_bitmap(data->mask);
	for (int i = 0; i < 16; i++) {
		//al_destroy_audio_stream(data->stream[i]);
	}
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	data->current = 0;
	data->playing = false;
	data->counter = 0;
	data->footnoted = false;
	for (int i = 0; i < 4; i++) {
		data->sequence[i] = rand() % 16;
		if (i > 0) {
			while (data->sequence[i - 1] == data->sequence[i]) {
				data->sequence[i] = rand() % 16;
			}
		}
	}
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
