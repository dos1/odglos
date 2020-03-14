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

static char* ANIMS[] = {
	"naparstki_01_kapelusz_jaszczurka_wersja3_najlepsza",
	"naparstki_02_jaszczurka_kredki",
	"naparstki_03_paski_okno",
	"naparstki_04_dzwoneczek_kwiatki",
	"naparstki_05_kwiatki_czarno_biale",
	"naparstki_06_czarno_bialy_bambus",
	"naparstki_07_panda_babuszka_wersja_krotsza2_lepsza",
	"naparstki_08_babuszka_zlote",
	"naparstki_09_zloty_statki",
};

static char* ANIMS_EMPTY[] = {
	"naparstki_PUSTE_01_kapelusz",
	"naparstki_PUSTE_02_jaszczurka",
	"naparstki_PUSTE_03_paski",
	"naparstki_PUSTE_04_dzwoneczek",
	"naparstki_PUSTE_05_kwiatek",
	"naparstki_PUSTE_06_czarno_bialy",
	"naparstki_PUSTE_07_panda",
	"naparstki_PUSTE_08_babuszka",
	"naparstki_PUSTE_09_zloty",
	"naparstki_PUSTE_10_latarnia",
	"naparstki_PUSTE_11_niebieskie_cos",
	"naparstki_PUSTE_12_Krakow",
	"naparstki_PUSTE_13_Mallorca",
	"naparstki_PUSTE_14_sztuka_wspolczesna",
	"naparstki_PUSTE_15_Toscania",
	"naparstki_PUSTE_16_maluszek",
	"naparstki_PUSTE_17_Praha",
};

struct GamestateResources {
	struct Character* bg;
	ALLEGRO_BITMAP* mask;
	int enabled;
	int jaszczur;
	bool first;
};

int Gamestate_ProgressCount = 61;

static CHARACTER_CALLBACK(ShowMouseCb) {
	ShowMouse(game);
	PlayMusic(game, "ambient", 1.0);
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	AnimateCharacter(game, data->bg, delta, 1.0);
	ALLEGRO_COLOR color = CheckMask(game, data->mask);
	int nr = round(((color.r * 255) + (color.g * 255) + (color.b * 255)) / 40.0);
	game->data->hover = nr < 17;

	if (data->first && game->data->cursor) {
		data->first = false;
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	SetCharacterPosition(game, data->bg, game->viewport.width / 2.0, game->viewport.height / 2.0, 0);
	DrawCharacter(game, data->bg);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->cursor &&
		(((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE)) ||
			(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) ||
			(ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
			(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN))) {
		ALLEGRO_COLOR color = CheckMask(game, data->mask);
		int nr = round(((color.r * 255) + (color.g * 255) + (color.b * 255)) / 40.0);
		if (nr < 17) {
			if (nr > data->enabled) {
				SelectSpritesheet(game, data->bg, ANIMS_EMPTY[nr]);
				PlaySound(game, "K STUD 01 25 13-001", 1.0);
			} else {
				if (nr == 9) {
					SwitchCurrentGamestate(game, "naparstki2");
					return;
				}
				SelectSpritesheet(game, data->bg, ANIMS[nr]);
				if (nr == data->enabled) {
					data->enabled++;
				}
				PlaySound(game, "K STUD 01 39 48-001", 1.0);
				if (nr == 0) {
					if (data->jaszczur == 1) {
						SelectSpritesheet(game, data->bg, "naparstki_01_kapelusz_jaszczurka_wersja1");
					}
					if (data->jaszczur == 2) {
						SelectSpritesheet(game, data->bg, "naparstki_01_kapelusz_jaszczurka_wersja2");
					}
					data->jaszczur++;
					if (data->jaszczur == 3) {
						data->jaszczur = 0;
					}
				}
			}
			HideMouse(game);
			data->bg->callback = ShowMouseCb;
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		SwitchCurrentGamestate(game, "naparstki2");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		SwitchCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->bg = CreateCharacter(game, "naparstki");

	char* anims[] = {
		"naparstki_00_poczatek",
		"naparstki_01_kapelusz_jaszczurka_wersja1",
		"naparstki_01_kapelusz_jaszczurka_wersja2",
		"naparstki_01_kapelusz_jaszczurka_wersja3_najlepsza",
		"naparstki_02_jaszczurka_kredki",
		//naparstki_02_jaszczurka_statki_DO_SKROCONEJ_GRY_W_1.ini
		"naparstki_03_paski_okno",
		//naparstki_03_paski_statki_DO_SKROCONEJ_GRY_W_2.ini
		"naparstki_04_dzwoneczek_kwiatki",
		"naparstki_05_kwiatki_czarno_biale",
		"naparstki_06_czarno_bialy_bambus",
		"naparstki_07_panda_babuszka_wersja_krotsza2_lepsza",
		"naparstki_08_babuszka_zlote",
		"naparstki_09_zloty_statki",
		"naparstki_10_latarnia_zbija_wszystko",
		//"naparstki_10a_latarnia_pusta_sowka_piana_czesc1",
		//"naparstki_10a_latarnia_pusta_sowka_piana_czesc2",
		//"naparstki_10a_latarnia_pusta_sowka_piana_czesc3",
		//"naparstki_10b_KONCOWKA_chodaki_owce",
		"naparstki_PUSTE_01_kapelusz",
		"naparstki_PUSTE_02_jaszczurka",
		"naparstki_PUSTE_03_paski",
		"naparstki_PUSTE_04_dzwoneczek",
		"naparstki_PUSTE_05_kwiatek",
		"naparstki_PUSTE_06_czarno_bialy",
		"naparstki_PUSTE_07_panda",
		"naparstki_PUSTE_08_babuszka",
		"naparstki_PUSTE_09_zloty",
		"naparstki_PUSTE_10_latarnia",
		"naparstki_PUSTE_11_niebieskie_cos",
		"naparstki_PUSTE_12_Krakow",
		"naparstki_PUSTE_13_Mallorca",
		"naparstki_PUSTE_14_sztuka_wspolczesna",
		"naparstki_PUSTE_15_Toscania",
		"naparstki_PUSTE_16_maluszek",
		"naparstki_PUSTE_17_Praha",
	};

	for (size_t i = 0; i < sizeof(anims) / sizeof(anims[0]); i++) {
		char path[255] = {};
		snprintf(path, 255, "sprites/naparstki/%s.awebp", anims[i]);
		RegisterStreamedSpritesheet(game, data->bg, anims[i], AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, path), false));
		progress(game);
	}
	LoadSpritesheets(game, data->bg, progress);

	data->mask = al_load_bitmap(GetDataFilePath(game, "naparstki.png"));
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
	SelectSpritesheet(game, data->bg, "naparstki_00_poczatek");
	data->bg->callback = ShowMouseCb;
	data->enabled = 0;
	data->first = true;
	EnsureMusic(game, "JAMMIN K LAP L 18 10 23", 1.0);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
