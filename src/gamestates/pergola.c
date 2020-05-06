/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct PergolaCharacter {
	struct AnimationDecoder* animation;
	int i;
	int j;
	ALLEGRO_BITMAP* hint;
	struct {
		int x, y;
	} offset;
};

struct GamestateResources {
	struct PergolaCharacter left, right;
	ALLEGRO_BITMAP *btn, *controls;
	struct {
		int x, y;
	} offset;
	double counter;
	bool mode;
	struct Timeline* timeline;
	double hint;
};

int Gamestate_ProgressCount = 4;

static float BRIGHTNESS[2][4][4][3] =
	{{{{0.429445,
			 0.422976,
			 0.423182},
			{0.420133,
				0.417356,
				0.423023},
			{0.422727,
				0.417609,
				0.420959},
			{0.422167,
				0.413627,
				0.417971}},
		 {{0.416591,
				0.421105,
				0.389477},
			 {0.397391,
				 0.397295,
				 0.390782},
			 {0.393090,
				 0.391678,
				 0.399707},
			 {0.399766,
				 0.367680,
				 0.365255}},
		 {{0.364033,
				0.364854,
				0.406830},
			 {0.358815,
				 0.362573,
				 0.402446},
			 {0.393973,
				 0.403466,
				 0.400498},
			 {0.399304,
				 0.398248,
				 0.399912}},
		 {{0.397074,
				0.393895,
				0.397157},
			 {0.361461,
				 0.398764,
				 0.389823},
			 {0.398074,
				 0.408518,
				 0.402227},
			 {0.406123,
				 0.400608,
				 0.397482}}},
		{{{0.397364,
				0.408421,
				0.389514},
			 {0.403829,
				 0.396523,
				 0.401344},
			 {0.411836,
				 0.418055,
				 0.406306},
			 {0.411300,
				 0.414891,
				 0.416158}},
			{{0.398842,
				 0.405234,
				 0.405471},
				{0.399524,
					0.397318,
					0.402588},
				{0.406852,
					0.404524,
					0.413710},
				{0.396889,
					0.404395,
					0.408580}},
			{{0.404804,
				 0.404335,
				 0.402160},
				{0.407603,
					0.402100,
					0.416626},
				{0.405895,
					0.405490,
					0.411845},
				{0.409412,
					0.413337,
					0.407946}},
			{{
				 0.409304,
				 0.400492,
				 0.404624,
			 },
				{0.403495,
					0.396108,
					0.410076},
				{0.408871,
					0.393835,
					0.407455},
				{0.404495,
					0.392346,
					0.401287}}}};

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
	ALLEGRO_COLOR color = CheckMaskSized(game, data->controls, c->offset.x, c->offset.y, game->viewport.width, game->viewport.height);
	bool hover = color.a > 0.5;
	color = CheckMaskSized(game, data->btn, data->offset.x, data->offset.y, game->viewport.width, game->viewport.height);
	game->data->hover = hover || color.a > 0.5;

	if (game->data->skip_requested) {
		UnsetSkip(game);
		ChangeCurrentGamestate(game, "anim");
	}
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

	struct PergolaCharacter* c = data->mode ? &data->right : &data->left;
	float brightness = 1.0 + ((BRIGHTNESS[data->mode][c->j][c->i][GetAnimationFrameNo(c->animation)] - 0.35881502787272146) / 0.07063002268473278) * 0.16 - 0.08;
	ALLEGRO_COLOR tint = al_map_rgba_f(brightness, brightness, brightness, 1.0);
	al_draw_tinted_bitmap(data->controls, tint, c->offset.x, c->offset.y, 0);
	al_draw_tinted_bitmap(data->btn, tint, data->offset.x, data->offset.y, 0);

	al_draw_tinted_bitmap(data->left.hint, al_map_rgba_f(hint, hint, hint, hint), 490, 116, 0);
	al_draw_tinted_scaled_rotated_bitmap(data->right.hint, al_map_rgba_f(hint, hint, hint, hint),
		1280 / 2.0 - 658, 720 / 2.0 - 314,
		655, 353 + (data->mode ? 0 : 7), 1355.0 / 1280.0, 1355.0 / 1280.0, 0, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	struct PergolaCharacter* c = data->mode ? &data->right : &data->left;
	ALLEGRO_COLOR color = CheckMaskSized(game, data->controls, c->offset.x, c->offset.y, game->viewport.width, game->viewport.height);
	bool hover = color.a > 0.5;
	color = CheckMaskSized(game, data->btn, data->offset.x, data->offset.y, game->viewport.width, game->viewport.height);
	game->data->hover = hover || color.a > 0.5;

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
					PlaySound(game, "nope_trickstar", 2.0);
				} else {
					PlaySound(game, "S LIST FX 08 40 33-001", 1.0);
				}
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_DOWN) {
				c->j += 1;
				if (c->j > 3) {
					data->hint = 255;
					c->j = 3;
					PlaySound(game, "nope_trickstar", 2.0);
				} else {
					PlaySound(game, "S LIST FX 08 00 42-001", 1.0);
				}
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
				c->i -= 1;
				if (c->i < 0) {
					data->hint = 255;
					c->i = 0;
					PlaySound(game, "nope_trickstar", 2.0);
				} else {
					PlaySound(game, "S LIST FX 08 07 74-001", 1.0);
				}
			} else if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				c->i += 1;
				if (c->i > 3) {
					data->hint = 255;
					c->i = 3;
					PlaySound(game, "nope_trickstar", 2.0);
				} else {
					PlaySound(game, "S LIST FX 08 18 20-001", 1.0);
				}
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

	data->controls = al_load_bitmap(GetDataFilePath(game, "pergola/pergola_strzalki.png"));
	data->left.offset.x = 200;
	data->left.offset.y = 57;
	data->right.offset.x = 1099;
	data->right.offset.y = 78;
	progress(game);
	data->left.hint = al_load_bitmap(GetDataFilePath(game, "pergola/tasma_lewa.png"));
	progress(game);
	data->right.hint = al_load_bitmap(GetDataFilePath(game, "pergola/DSCF7662_tasma_prawa.png"));
	progress(game);
	data->btn = al_load_bitmap(GetDataFilePath(game, "pergola/switch.png"));
	data->offset.x = 639;
	data->offset.y = 603;
	progress(game);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	if (data->left.animation) {
		DestroyAnimation(data->left.animation);
	}
	if (data->right.animation) {
		DestroyAnimation(data->right.animation);
	}
	al_destroy_bitmap(data->left.hint);
	al_destroy_bitmap(data->right.hint);
	al_destroy_bitmap(data->controls);
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
	game->data->skip_available = true;

	char filename[255];
	snprintf(filename, 255, "pergola/sowka_1/rzad_%d_kolumna_%d/anim.awebp", data->left.j + 1, data->left.i + 1);
	data->left.animation = CreateAnimation(game, GetDataFilePath(game, filename), false);
	snprintf(filename, 255, "pergola/sowka_2/rzad_%d_kolumna_%d/anim.awebp", data->right.j + 1, data->right.i + 1);
	data->right.animation = CreateAnimation(game, GetDataFilePath(game, filename), false);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	game->data->skip_available = false;
}
