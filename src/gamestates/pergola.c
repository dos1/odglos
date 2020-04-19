/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct PergolaCharacter {
	struct AnimationDecoder* animation;
	int i;
	int j;
	ALLEGRO_BITMAP *controls, *hint;
};

struct GamestateResources {
	struct PergolaCharacter left, right;
	double counter;
	bool mode;
	struct Timeline* timeline;
	double hint;
};

int Gamestate_ProgressCount = 6;

static bool IsCompleted(struct Game* game, struct GamestateResources* data) {
	return data->left.i == 3 && data->left.j == 3 && data->right.i == 0 && data->right.j == 0;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }
	TM_Process(data->timeline, delta);
	data->counter += delta;
	data->hint -= delta * 100;
	if (data->hint < 0) {
		data->hint = 0;
	}
	if (data->mode) {
		UpdateAnimation(data->right.animation, delta);
	} else {
		UpdateAnimation(data->left.animation, delta);
	}
	if (IsCompleted(game, data)) {
		ChangeCurrentGamestate(game, "anim");
	}

	struct PergolaCharacter* c = data->mode ? &data->right : &data->left;
	ALLEGRO_COLOR color = CheckMask(game, c->controls);
	game->data->hover = color.a > 0.5;
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bmp = NULL;
	if (data->mode) {
		bmp = GetAnimationFrame(data->right.animation);
		al_draw_scaled_rotated_bitmap(bmp, al_get_bitmap_width(bmp) / 2.0, al_get_bitmap_height(bmp) / 2.0,
			655, 353, 1355.0 / 1280.0, 1355.0 / 1280.0, 0, 0);
	} else {
		bmp = GetAnimationFrame(data->left.animation);
		al_draw_scaled_bitmap(bmp, 0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
			0, 0, game->viewport.width, game->viewport.height, 0);
	}

	float hint = pow(data->hint / 255.0, 0.9);

	if (data->mode) {
		int frame = GetAnimationFrameNo(data->right.animation) + data->right.j + data->right.i;
		al_draw_tinted_scaled_bitmap(data->right.controls, al_map_rgba(255 - frame * 4, 255 - frame * 4, 255 - frame * 4, 255),
			0, 0, al_get_bitmap_width(data->right.controls), al_get_bitmap_height(data->right.controls),
			0, 0, game->viewport.width, game->viewport.height, 0);
	} else {
		int frame = GetAnimationFrameNo(data->left.animation) + data->left.j + data->left.i;
		al_draw_tinted_scaled_bitmap(data->left.controls, al_map_rgba(255 - frame * 4, 255 - frame * 4, 255 - frame * 4, 255),
			0, 0, al_get_bitmap_width(data->left.controls), al_get_bitmap_height(data->left.controls),
			0, 0, game->viewport.width, game->viewport.height, 0);
	}

	al_draw_tinted_scaled_bitmap(data->left.hint, al_map_rgba_f(hint, hint, hint, hint),
		0, 0, al_get_bitmap_width(data->left.hint), al_get_bitmap_height(data->left.hint),
		0, 0, game->viewport.width, game->viewport.height, 0);
	al_draw_tinted_scaled_rotated_bitmap(data->right.hint, al_map_rgba_f(hint, hint, hint, hint),
		al_get_bitmap_width(data->right.hint) / 2.0, al_get_bitmap_height(data->right.hint) / 2.0,
		655, 353, 1355.0 / 1280.0, 1355.0 / 1280.0, 0, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	struct PergolaCharacter* c = data->mode ? &data->right : &data->left;
	ALLEGRO_COLOR color = CheckMask(game, c->controls);
	game->data->hover = color.a > 0.5;

	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		if (game->data->hover) {
			ev->keyboard.type = ALLEGRO_EVENT_KEY_DOWN;
			int pos = game->data->mouseY * 720;
			if (!data->mode) {
				if (pos > 450) {
					ev->keyboard.keycode = ALLEGRO_KEY_RSHIFT;
				} else if (pos > 345) {
					ev->keyboard.keycode = ALLEGRO_KEY_DOWN;
				} else if (pos > 250) {
					ev->keyboard.keycode = ALLEGRO_KEY_RIGHT;
				} else if (pos > 155) {
					ev->keyboard.keycode = ALLEGRO_KEY_LEFT;
				} else {
					ev->keyboard.keycode = ALLEGRO_KEY_UP;
				}
			} else {
				if (pos > 475) {
					ev->keyboard.keycode = ALLEGRO_KEY_RSHIFT;
				} else if (pos > 365) {
					ev->keyboard.keycode = ALLEGRO_KEY_DOWN;
				} else if (pos > 270) {
					ev->keyboard.keycode = ALLEGRO_KEY_RIGHT;
				} else if (pos > 175) {
					ev->keyboard.keycode = ALLEGRO_KEY_LEFT;
				} else {
					ev->keyboard.keycode = ALLEGRO_KEY_UP;
				}
			}
		}
	}

	if (!IsCompleted(game, data)) {
		if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
			bool action = true;
			if (ev->keyboard.keycode == ALLEGRO_KEY_UP) {
				c->j -= 1;
				if (c->j < 0) {
					data->hint = 255;
					c->j = 0;
				}
				PlaySound(game, "S LIST FX 08 40 33-001", 1.0);
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_DOWN) {
				c->j += 1;
				if (c->j > 3) {
					data->hint = 255;
					c->j = 3;
				}
				PlaySound(game, "S LIST FX 08 00 42-001", 1.0);
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
				c->i -= 1;
				if (c->i < 0) {
					data->hint = 255;
					c->i = 0;
				}
				PlaySound(game, "S LIST FX 08 07 74-001", 1.0);
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				c->i += 1;
				if (c->i > 3) {
					data->hint = 255;
					c->i = 3;
				}
				PlaySound(game, "S LIST FX 08 18 20-001", 1.0);
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_RSHIFT) {
				data->mode = !data->mode;
				PlaySound(game, "S LIST FX 09 42 19-001", 1.0);
				if (!IsCompleted(game, data)) {
					data->hint = 255;
				}
			} else {
				action = false;
			}

			if (action) {
				char filename[255];
				if (!data->mode) {
					snprintf(filename, 255, "pergola/sowka_1/rzad_%d_kolumna_%d/anim.awebp", data->left.j + 1, data->left.i + 1);
					DestroyAnimation(data->left.animation);
					data->left.animation = CreateAnimation(game, GetDataFilePath(game, filename), false);
				} else {
					snprintf(filename, 255, "pergola/sowka_2/rzad_%d_kolumna_%d/anim.awebp", data->right.j + 1, data->right.i + 1);
					DestroyAnimation(data->right.animation);
					data->right.animation = CreateAnimation(game, GetDataFilePath(game, filename), false);
				}
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

	data->timeline = TM_Init(game, data, "timeline");

	data->left.animation = CreateAnimation(game, GetDataFilePath(game, "pergola/sowka_1/rzad_1_kolumna_1/anim.awebp"), false);
	progress(game);
	data->right.animation = CreateAnimation(game, GetDataFilePath(game, "pergola/sowka_2/rzad_4_kolumna_4/anim.awebp"), false);
	progress(game);
	data->left.controls = al_load_bitmap(GetDataFilePath(game, "pergola/pergola_strzalki_lewe.png"));
	progress(game);
	data->right.controls = al_load_bitmap(GetDataFilePath(game, "pergola/pergola_strzalki_prawe.png"));
	progress(game);
	data->left.hint = al_load_bitmap(GetDataFilePath(game, "pergola/tasma_lewa.png"));
	progress(game);
	data->right.hint = al_load_bitmap(GetDataFilePath(game, "pergola/DSCF7662_tasma_prawa.png"));
	progress(game);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->left.animation);
	DestroyAnimation(data->right.animation);
	al_destroy_bitmap(data->left.hint);
	al_destroy_bitmap(data->right.hint);
	al_destroy_bitmap(data->left.controls);
	al_destroy_bitmap(data->right.controls);
	TM_Destroy(data->timeline);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	PlayMusic(game, "silence", 1.0);
	ShowMouse(game);
	data->left.i = 0;
	data->left.j = 0;
	data->right.i = 3;
	data->right.j = 3;
	data->mode = false;
	data->hint = 255;
	data->counter = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
