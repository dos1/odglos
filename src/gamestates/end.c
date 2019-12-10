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
	bool empty;
};

int Gamestate_ProgressCount = 1;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	LoadGamestate(game, "logo");
	LoadGamestate(game, "anim");
	LoadGamestate(game, "naparstki");
	LoadGamestate(game, "naparstki2");
	LoadGamestate(game, "end");
	LoadGamestate(game, "lawka");
	LoadGamestate(game, "byk");
	LoadGamestate(game, "blank");
	LoadGamestate(game, "pudelka");
	LoadGamestate(game, "pergola");
	LoadGamestate(game, "armata");
	game->data->sceneid = -1;
	ChangeCurrentGamestate(game, "anim");
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	progress(game);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
