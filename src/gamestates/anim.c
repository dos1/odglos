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
	int repeats, all_repeats;
	int x, y;
	double scale;
	struct FreezeFrame* freezes;
	struct AudioFrame* sounds;
	bool finished;
	bool stay;
	bool (*callback)(struct Game*, int, int*, int*, double*, struct Character*, void**);
	void (*draw)(struct Game*, int, void**);
	struct Character* character;

	int freezeno, soundno;
	ALLEGRO_BITMAP* mask;
	bool frozen, linked;
	char *bgname, *fgname, *maskname;

	void* callback_data;
};

int Gamestate_ProgressCount = 2;

static void LoadAnimation(struct Game* game, struct GamestateResources* data, void (*progress)(struct Game*)) {
	char path[255] = {0};
	snprintf(path, 255, "animations/%s.awebp", game->data->scene.name);
	HideMouse(game);

	if (data->anim) {
		DestroyAnimation(data->anim);
	}
	if (data->bg && game->data->scene.bg != data->bgname) {
		al_destroy_bitmap(data->bg);
		data->bg = NULL;
	}
	if (data->fg && game->data->scene.fg != data->fgname) {
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

	data->anim = CreateAnimation(game, GetDataFilePath(game, path), false);
	if (progress) {
		progress(game);
	}
	if (!data->bg && game->data->scene.bg) {
		char p[255] = {0};
		snprintf(p, 255, "%s.png", game->data->scene.bg);
		data->bg = al_load_bitmap(GetDataFilePath(game, p));
		data->bgname = game->data->scene.bg;
	}
	if (!data->fg && game->data->scene.fg) {
		char p[255] = {0};
		snprintf(p, 255, "%s.png", game->data->scene.fg);
		data->fg = al_load_bitmap(GetDataFilePath(game, p));
		data->fgname = game->data->scene.fg;
	}
	data->repeats = game->data->scene.repeats;
	data->all_repeats = game->data->scene.repeats;
	data->callback = game->data->scene.callback;
	data->draw = game->data->scene.draw;
	data->freezes = game->data->scene.freezes;
	data->sounds = game->data->scene.sounds;
	if (game->data->scene.character.name) {
		data->character = CreateCharacter(game, game->data->scene.character.name);
		char** spritesheet = game->data->scene.character.spritesheets;
		while (*spritesheet) {
			char p[255] = {};
			snprintf(p, 255, "sprites/%s/%s.awebp", game->data->scene.character.name, *spritesheet);
			RegisterStreamedSpritesheet(game, data->character, *spritesheet, AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, p), game->data->scene.character.repeat));
			if (game->data->scene.character.preload) {
				PreloadStreamedSpritesheet(game, data->character, *spritesheet);
			}
			spritesheet++;
		}
		SelectSpritesheet(game, data->character, game->data->scene.character.spritesheets[0]);
		SetCharacterPosition(game, data->character, 0, 0, 0);

		if (game->data->scene.character.hidden) {
			HideCharacter(game, data->character);
		}
	}

	data->x = 0;
	data->y = 0;
	data->scale = 1.0;
	data->freezeno = 0;
	data->soundno = 0;
	data->linked = false;
	data->callback_data = game->data->scene.callback_data;
	if (data->callback) {
		if (data->callback(game, 0, &data->x, &data->y, &data->scale, data->character, &data->callback_data)) {
			data->finished = true;
		}
	}

	HandleAudio(game, game->data->scene.audio);

	ResetAnimation(data->anim);
	//PrintConsole(game, "Loaded: %s", path);
}

static void HandleDispatch(struct Game* game, struct GamestateResources* data, void (*progress)(struct Game*)) {
	if (!Dispatch(game)) {
		SwitchCurrentGamestate(game, "end");
	} else {
		StopLoops(game);
		if (game->data->scene.name[0] == '>') {
			HandleAudio(game, game->data->scene.audio);
			ChangeCurrentGamestate(game, game->data->scene.name + 1);
		} else {
			LoadAnimation(game, data, progress);
			data->finished = false;
		}
	}
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	float modifier = 1.25 * ((game->data->scene.speed == 0.0) ? 1.0 : game->data->scene.speed);

	int frame = GetAnimationFrameNo(data->anim) + GetAnimationFrameCount(data->anim) * (data->all_repeats - data->repeats);
	game->data->debuginfo = frame;

	if (data->sounds[data->soundno].frame == frame) {
		HandleAudio(game, data->sounds[data->soundno].audio);
		data->soundno++;
	}

	if (!data->frozen && data->freezes[data->freezeno].frame == frame) {
		if (data->freezes[data->freezeno].mask) {
			data->frozen = true;
			ShowMouse(game);
			if (!data->mask) {
				if (data->freezes[data->freezeno].mask && data->freezes[data->freezeno].mask[0] != 0) {
					char path[255] = {0};
					snprintf(path, 255, "masks/%s.mask", data->freezes[data->freezeno].mask);
					data->mask = al_load_bitmap(GetDataFilePath(game, path));
				}
			}
			PrintConsole(game, "Freeze: [%d] %s (frame: %d)", data->freezeno, data->freezes[data->freezeno].mask, frame);
		}
		if (data->freezes[data->freezeno].footnote) {
			ShowFootnote(game, data->freezes[data->freezeno].footnote);
			data->freezeno++;
			PrintConsole(game, "Footnote: %d (frame: %d)", data->freezes[data->freezeno].footnote, frame);
		}
	}

	if (data->frozen) {
		if (data->mask) {
			ALLEGRO_COLOR color = CheckMask(game, data->mask);
			for (int i = 0; i < 8; i++) {
				if (al_color_distance_ciede2000(data->freezes[data->freezeno].links[i].color, color) < 0.01) {
					if (data->freezes[data->freezeno].links[i].ignore) {
						game->data->hover = false;
					}
					break;
				}
			}
		} else {
			ALLEGRO_BITMAP* bitmap = GetAnimationFrame(data->anim);
			int x = game->data->mouseX * game->viewport.width;
			int y = game->data->mouseY * game->viewport.height;
			x -= data->x;
			y -= data->y;

			ALLEGRO_COLOR color = al_get_pixel(bitmap, x, y);
			game->data->hover = (color.a > 0.5);
		}

		if (data->character && data->linked) {
			AnimateCharacter(game, data->character, delta, modifier);
		}
	} else {
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
				if (!data->delay) {
					data->delay = 0.01;
				}
				data->finished = !data->stay;
			}
		}
	}

	if (data->callback) {
		if (data->callback(game, GetAnimationFrameNo(data->anim) + GetAnimationFrameCount(data->anim) * (data->all_repeats - data->repeats), &data->x, &data->y, &data->scale, data->character, &data->callback_data)) {
			data->finished = true;
			data->freezeno++;
			al_destroy_bitmap(data->mask);
			data->mask = NULL;
			data->frozen = false;
		}
	} else {
		data->x = 0;
		data->y = 0;
		data->scale = 1.0;
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bitmap;
	if (data->bg) {
		bitmap = data->bg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
	bitmap = GetAnimationFrame(data->anim);
	al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), data->x, data->y, al_get_bitmap_width(bitmap) * data->scale, al_get_bitmap_height(bitmap) * data->scale, 0);

	if (data->draw) {
		data->draw(game, GetAnimationFrameNo(data->anim) + GetAnimationFrameCount(data->anim) * (data->all_repeats - data->repeats), &data->callback_data);
	}

	if (data->character) {
		DrawCharacter(game, data->character);
	}

	if (data->fg) {
		bitmap = data->fg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	if (game->data->hover && (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) || (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN))) {
		if (data->frozen && !data->linked) {
			HandleAudio(game, data->freezes[data->freezeno].audio);

			for (int i = 0; i < 8; i++) {
				if (data->freezes[data->freezeno].links[i].callback || data->freezes[data->freezeno].links[i].name) {
					if (al_color_distance_ciede2000(data->freezes[data->freezeno].links[i].color, CheckMask(game, data->mask)) < 0.01) {
						PrintConsole(game, "Linked!");
						if (data->character && data->freezes[data->freezeno].links[i].name) {
							SelectSpritesheet(game, data->character, data->freezes[data->freezeno].links[i].name);
							ShowCharacter(game, data->character);
							HideMouse(game);
							data->linked = true;
							return;
						}
						if (data->freezes[data->freezeno].links[i].callback) {
							if (!data->freezes[data->freezeno].links[i].callback(game, data->character, &data->callback_data)) {
								data->linked = true;
								return;
							}
						}
						HandleAudio(game, data->freezes[data->freezeno].links[i].audio);
					}
				}
			}
			if (data->freezes[data->freezeno].callback) {
				data->freezes[data->freezeno].callback(game, data->character, &data->callback_data);
			}
			data->freezeno++;
			al_destroy_bitmap(data->mask);
			data->mask = NULL;
			data->frozen = false;
			HideMouse(game);
			PrintConsole(game, "Unfreeze!");
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		data->delay = 0.01;
		data->finished = true;
		data->frozen = false;
		data->freezeno++;
		data->repeats = 0;
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
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
	if (data->finished) {
		HandleDispatch(game, data, NULL);
	}
	data->delay = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
