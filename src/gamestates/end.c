/*! \file example.c
 *  \brief Example gamestate.
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
	LoadGamestate(game, "myszka");
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
