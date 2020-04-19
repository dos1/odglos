/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

enum BallType {
	BALL_TYPE_NONE,
	BALL_TYPE_YELLOW,
	BALL_TYPE_RED,
	BALL_TYPE_ORANGE,
};

enum BoxType {
	BOX_TYPE_YELLOW,
	BOX_TYPE_RED,
	BOX_TYPE_ORANGE
};

struct GamestateResources {
	struct {
		enum BallType type;
		struct Character* character;
	} left, center, right;

	enum BallType stack[3];
	enum BoxType last;
	int stackpos;
	bool frames;
	bool won;
	ALLEGRO_BITMAP* mask;
};

int Gamestate_ProgressCount = 8;

static void SwitchAnimation(struct Game* game, struct GamestateResources* data, struct Character* character, char* name) {
	if (!GetSpritesheet(game, data->left.character, name)) {
		char path[255] = {};
		snprintf(path, 255, "sprites/pudelka/%s.awebp", name);
		RegisterStreamedSpritesheet(game, data->left.character, name, AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, path), false));
		data->center.character->spritesheets = data->left.character->spritesheets;
		data->right.character->spritesheets = data->left.character->spritesheets;
	}
	SelectSpritesheet(game, character, name);
}

static void EnqueueAnimation(struct Game* game, struct GamestateResources* data, struct Character* character, char* name) {
	if (!GetSpritesheet(game, data->left.character, name)) {
		char path[255] = {};
		snprintf(path, 255, "sprites/pudelka/%s.awebp", name);
		RegisterStreamedSpritesheet(game, data->left.character, name, AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, path), false));
		data->center.character->spritesheets = data->left.character->spritesheets;
		data->right.character->spritesheets = data->left.character->spritesheets;
	}
	EnqueueSpritesheet(game, character, name);
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	if (data->frames) {
		AnimateCharacter(game, data->left.character, delta, 1.0);
		AnimateCharacter(game, data->center.character, delta, 1.0);
		AnimateCharacter(game, data->right.character, delta, 1.0);
	} else {
		AnimateCharacter(game, data->center.character, delta, 1.3);
	}

	CheckMask(game, data->mask);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	int twenty = game->viewport.width * 0.015625; // 20px
	if (data->frames) {
		SetClippingRectangle(0, 0, game->viewport.width / 3.0 - twenty, game->viewport.height);
		SetCharacterPosition(game, data->left.character, 0, 0, 0);
		DrawCharacter(game, data->left.character);

		SetClippingRectangle(game->viewport.width / 3.0, 0, game->viewport.width / 3.0 - twenty, game->viewport.height);
		SetCharacterPosition(game, data->center.character, game->viewport.width * 0.328125, 0, 0);
		DrawCharacter(game, data->center.character);

		SetClippingRectangle(2 * game->viewport.width / 3.0, 0, game->viewport.width / 3.0, game->viewport.height);
		SetCharacterPosition(game, data->right.character, game->viewport.width * 0.65625, 0, 0);
		DrawCharacter(game, data->right.character);
	} else {
		SetCharacterPosition(game, data->center.character, 0, 0, 0);
		DrawCharacter(game, data->center.character);

		if (!data->won) {
			al_draw_filled_rectangle(game->viewport.width / 3.0 - twenty, 0, game->viewport.width / 3.0, game->viewport.height, al_map_rgb(0, 0, 0));
			al_draw_filled_rectangle(game->viewport.width * 0.65104166666666666667, 0, game->viewport.width * 0.65104166666666666667 + twenty, game->viewport.height, al_map_rgb(0, 0, 0));
		}
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	CheckMask(game, data->mask);

	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		if (!game->data->cursor || !game->data->hover) {
			return;
		}
		if (game->data->mouseX < 0.333) {
			data->last = BOX_TYPE_RED;
			if (data->left.type == BALL_TYPE_NONE) {
				PlaySound(game, "S LIST FX 13 58 55-001", 1.0);
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SwitchAnimation(game, data, data->left.character, "pudelko_1_czer_r");
						data->left.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SwitchAnimation(game, data, data->left.character, "pudelko_1_zolte_r");
						data->left.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SwitchAnimation(game, data, data->left.character, "pudelko_1_pom_r");
						data->left.type = BALL_TYPE_ORANGE;
						break;
					case BALL_TYPE_NONE:
						break;
				}
			} else {
				PlaySound(game, "S LIST FX 14 09 89-001", 1.0);
				switch (data->left.type) {
					case BALL_TYPE_RED:
						SwitchAnimation(game, data, data->left.character, "pudelko_1_czer");
						break;
					case BALL_TYPE_YELLOW:
						SwitchAnimation(game, data, data->left.character, "pudelko_1_zolte");
						break;
					case BALL_TYPE_ORANGE:
						SwitchAnimation(game, data, data->left.character, "pudelko_1_pom");
						break;
					case BALL_TYPE_NONE:
						break;
				}
				EnqueueAnimation(game, data, data->left.character, "pudelko_1_memlanie");
				HideMouse(game);
			}
		}

		if (game->data->mouseX > 0.333 && game->data->mouseX < 0.666) {
			data->last = BOX_TYPE_ORANGE;
			if (data->center.type == BALL_TYPE_NONE) {
				PlaySound(game, "S LIST FX 13 58 55-001", 1.0);
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SwitchAnimation(game, data, data->center.character, "pudelko_2_czer_r");
						data->center.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SwitchAnimation(game, data, data->center.character, "pudelko_2_zolte_r");
						data->center.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SwitchAnimation(game, data, data->center.character, "pudelko_2_pom_r");
						data->center.type = BALL_TYPE_ORANGE;
						break;
					case BALL_TYPE_NONE:
						break;
				}
			} else {
				PlaySound(game, "S LIST FX 14 09 89-001", 1.0);
				switch (data->center.type) {
					case BALL_TYPE_RED:
						SwitchAnimation(game, data, data->center.character, "pudelko_2_czer");
						break;
					case BALL_TYPE_YELLOW:
						SwitchAnimation(game, data, data->center.character, "pudelko_2_zolte");
						break;
					case BALL_TYPE_ORANGE:
						SwitchAnimation(game, data, data->center.character, "pudelko_2_pom");
						break;
					case BALL_TYPE_NONE:
						break;
				}
				EnqueueAnimation(game, data, data->center.character, "pudelko_2_memlanie");
				HideMouse(game);
			}
		}

		if (game->data->mouseX > 0.666) {
			data->last = BOX_TYPE_YELLOW;
			if (data->right.type == BALL_TYPE_NONE) {
				PlaySound(game, "S LIST FX 13 58 55-001", 1.0);
				switch (data->stack[--data->stackpos]) {
					case BALL_TYPE_RED:
						SwitchAnimation(game, data, data->right.character, "pudelko_3_czer_r");
						data->right.type = BALL_TYPE_RED;
						break;
					case BALL_TYPE_YELLOW:
						SwitchAnimation(game, data, data->right.character, "pudelko_3_zolte_r");
						data->right.type = BALL_TYPE_YELLOW;
						break;
					case BALL_TYPE_ORANGE:
						SwitchAnimation(game, data, data->right.character, "pudelko_3_pom_r");
						data->right.type = BALL_TYPE_ORANGE;
						break;
					case BALL_TYPE_NONE:
						break;
				}
			} else {
				PlaySound(game, "S LIST FX 14 09 89-001", 1.0);
				switch (data->right.type) {
					case BALL_TYPE_RED:
						SwitchAnimation(game, data, data->right.character, "pudelko_3_czer");
						break;
					case BALL_TYPE_YELLOW:
						SwitchAnimation(game, data, data->right.character, "pudelko_3_zolte");
						break;
					case BALL_TYPE_ORANGE:
						SwitchAnimation(game, data, data->right.character, "pudelko_3_pom");
						break;
					case BALL_TYPE_NONE:
						break;
				}
				EnqueueAnimation(game, data, data->right.character, "pudelko_3_memlanie");
				HideMouse(game);
			}
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		ChangeCurrentGamestate(game, "anim");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		ChangeCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->left.character = CreateCharacter(game, "pudelka");
	data->left.character->detailed_progress = true;

	char* anims[] = {
		"pudelka_poczatek",
		"pudelka_waz",
		"pudelko_1_czer",
		"pudelko_2_pom",
		"pudelko_3_zolte",
		"pudelko_1_czer_r",
		"pudelko_2_pom_r",
		"pudelko_3_zolte_r",
	};

	for (size_t i = 0; i < sizeof(anims) / sizeof(anims[0]); i++) {
		char path[255] = {};
		snprintf(path, 255, "sprites/pudelka/%s.awebp", anims[i]);
		RegisterStreamedSpritesheet(game, data->left.character, anims[i], AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, path), false));
		progress(game);
	}

	data->center.character = CreateCharacter(game, "pudelka");
	data->center.character->shared = true;
	data->center.character->spritesheets = data->left.character->spritesheets;

	data->right.character = CreateCharacter(game, "pudelka");
	data->right.character->shared = true;
	data->right.character->spritesheets = data->left.character->spritesheets;

	data->mask = al_load_bitmap(GetDataFilePath(game, "masks/pudelka_od_cioci_maska.mask"));

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyCharacter(game, data->right.character);
	DestroyCharacter(game, data->center.character);
	DestroyCharacter(game, data->left.character);
	al_destroy_bitmap(data->mask);
	free(data);
}

static void CheckWin(struct Game* game, struct GamestateResources* data) {
	if (data->won) {
		return;
	}
	if (data->left.type == BALL_TYPE_RED && data->center.type == BALL_TYPE_ORANGE && data->right.type == BALL_TYPE_YELLOW) {
		data->won = true;
		HideMouse(game);
		SwitchAnimation(game, data, data->left.character, "pudelko_1_czer");
		SwitchAnimation(game, data, data->center.character, "pudelko_2_pom");
		SwitchAnimation(game, data, data->right.character, "pudelko_3_zolte");
		data->left.character->delta = 0;
		data->center.character->delta = 0;
		data->right.character->delta = 0;
		PlayMusic(game, "pudelka_metrograph", 1.0);

		EnqueueAnimation(game, data, data->center.character, "pudelka_tancowanie");
	}
}

static CHARACTER_CALLBACK(HandleLeft) {
	struct GamestateResources* d = data;
	if (old && new) {
		if (strcmp("pudelko_1_memlanie", new->name) == 0) {
			d->stack[d->stackpos++] = d->left.type;
			d->left.type = BALL_TYPE_NONE;
			PrintConsole(game, "memlam lewe");
			ShowMouse(game);
		}
	}
	if (old && strcmp("pudelko_1_czer_r", old->name) == 0 && !new) {
		if (d->last == BOX_TYPE_RED) {
			CheckWin(game, data);
		}
	}
}

static CHARACTER_CALLBACK(HandleCenter) {
	struct GamestateResources* d = data;
	if (old && new) {
		if (strcmp("pudelko_2_memlanie", new->name) == 0) {
			d->stack[d->stackpos++] = d->center.type;
			d->center.type = BALL_TYPE_NONE;
			PrintConsole(game, "memlam srodkowe");
			ShowMouse(game);
		}
		if (strcmp("pudelka_poczatek", old->name) == 0 && old != new) {
			ShowMouse(game);
			d->frames = true;
			SwitchAnimation(game, data, d->left.character, "pudelko_1_zolte_r");
			SwitchAnimation(game, data, d->center.character, "pudelko_2_czer_r");
			SwitchAnimation(game, data, d->right.character, "pudelko_3_pom_r");
		}
		if (strcmp("pudelka_tancowanie", new->name) == 0) {
			d->frames = false;
		}
	}
	if (old && strcmp("pudelko_2_pom_r", old->name) == 0 && !new) {
		if (d->last == BOX_TYPE_ORANGE) {
			CheckWin(game, data);
		}
	}
	if (old && strcmp("pudelka_waz", old->name) == 0) {
		ChangeCurrentGamestate(game, "anim");
	}
}

static CHARACTER_CALLBACK(HandleRight) {
	struct GamestateResources* d = data;
	if (old && new) {
		if (strcmp("pudelko_3_memlanie", new->name) == 0) {
			d->stack[d->stackpos++] = d->right.type;
			d->right.type = BALL_TYPE_NONE;
			PrintConsole(game, "memlam prawe");
			ShowMouse(game);
		}
	}
	if (old && strcmp("pudelko_3_zolte_r", old->name) == 0 && !new) {
		if (d->last == BOX_TYPE_YELLOW) {
			CheckWin(game, data);
		}
	}
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	StopMusic(game);
	PlaySound(game, "K OLD O 00 13 75-001", 1.0);
	SetCharacterPosition(game, data->left.character, 0, 0, 0);
	SetCharacterPosition(game, data->center.character, 0, 0, 0);
	SetCharacterPosition(game, data->right.character, 0, 0, 0);
	SwitchAnimation(game, data, data->center.character, "pudelka_poczatek");
	EnqueueAnimation(game, data, data->center.character, "pudelko_2_czer_r");
	//data->left.character->pos = 1;
	//data->left.character->delta = 120;
	//data->right.character->pos = 2;
	//data->right.character->delta = 33;

	data->left.character->callback = HandleLeft;
	data->left.character->callback_data = data;
	data->center.character->callback = HandleCenter;
	data->center.character->callback_data = data;
	data->right.character->callback = HandleRight;
	data->right.character->callback_data = data;

	data->left.type = BALL_TYPE_YELLOW;
	data->center.type = BALL_TYPE_RED;
	data->right.type = BALL_TYPE_ORANGE;

	data->stackpos = 0;
	data->stack[0] = BALL_TYPE_NONE;
	data->stack[1] = BALL_TYPE_NONE;
	data->stack[2] = BALL_TYPE_NONE;

	data->frames = false;

	data->won = false;

	HideMouse(game);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
