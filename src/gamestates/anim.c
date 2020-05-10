/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	struct Player* player;
	bool hidden;
};

int Gamestate_ProgressCount = IS_EMSCRIPTEN ? 1 : 7;

static bool HandleDispatch(struct Game* game, struct GamestateResources* data) {
	if (!Dispatch(game)) {
		ChangeCurrentGamestate(game, "end");
		return true;
	}

	StopLoops(game);
	if (game->data->scene.name[0] == '>') {
		HandleAudio(game, game->data->scene.audio);
		ChangeCurrentGamestate(game, game->data->scene.name + 1);
		return true;
	}

	LoadPlayerAnimation(game, data->player, &game->data->scene);
	return false;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) {
		return;
	}

	if (data->hidden) {
		return;
	}

	if (UpdatePlayer(game, data->player, delta)) {
		HandleDispatch(game, data);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	if (data->hidden) {
		return;
	}

	DrawPlayer(game, data->player);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }
	ProcessPlayerEvent(game, data->player, ev);

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		SkipPlayerAnim(game, data->player);
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		SkipPlayerAnim(game, data->player);
		game->data->queue_handled = 0;
		game->data->queue_pos = 0;
		game->data->sceneid--;
		if (game->data->sceneid < -1) {
			game->data->sceneid = -1;
		}
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->player = CreatePlayer(game);
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar

#ifndef __EMSCRIPTEN
	CacheSounds(game, progress);
#endif
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyPlayer(game, data->player);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	data->hidden = HandleDispatch(game, data);

#ifdef __EMSCRIPTEN__
	HideHTMLLoading(game);
#endif
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
