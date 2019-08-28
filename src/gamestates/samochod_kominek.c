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
	struct Character* character;
};

int Gamestate_ProgressCount = 23;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	AnimateCharacter(game, data->character, delta, 1.0);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	DrawCharacter(game, data->character);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->character = CreateCharacter(game, "samochod_kominek");
	data->character->detailedProgress = true;
	RegisterSpritesheet(game, data->character, "anim");
	LoadSpritesheets(game, data->character, progress);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyCharacter(game, data->character);
	free(data);
}

static CHARACTER_CALLBACK(GoForward) {
	SwitchCurrentGamestate(game, "samochod_lazienka");
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	SetCharacterPosition(game, data->character, 0, 0, 0);
	data->character->callback = GoForward;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
