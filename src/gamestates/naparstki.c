/*! \file example.c
 *  \brief Example gamestate.
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
	struct Player* player;
	ALLEGRO_BITMAP* mask;
	ALLEGRO_BITMAP *kokardki[9], *tasiemki[9];
	int enabled;
	int jaszczur;
	bool first;
};

int Gamestate_ProgressCount = 20;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	UpdatePlayer(game, data->player, delta);
	ALLEGRO_COLOR color = CheckMask(game, data->mask);
	int nr = round(((color.r * 255) + (color.g * 255) + (color.b * 255)) / 40.0);
	game->data->hover = nr < 17 && nr >= data->enabled - 1;

	if (data->first && game->data->cursor) {
		data->first = false;
		PlayMusic(game, "ambient", 1.0);
	}

	if (PlayerIsFinished(game, data->player)) {
		ShowMouse(game);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	DrawPlayer(game, data->player);
	if (game->data->cursor && data->enabled > 0) {
		for (int i = 0; i < data->enabled - 1; i++) {
			al_draw_bitmap(data->tasiemki[i], 0, 0, 0);
		}
		al_draw_bitmap(data->kokardki[data->enabled - 1], 0, 0, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	if (game->data->cursor &&
		(((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE)) ||
			(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) ||
			(ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
			(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN))) {
		ALLEGRO_COLOR color = CheckMask(game, data->mask);
		int nr = round(((color.r * 255) + (color.g * 255) + (color.b * 255)) / 40.0);
		if (nr < 17 && nr >= data->enabled - 1) {
			if ((nr != data->enabled) && (nr != data->enabled - 1)) {
				LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){ANIMS_EMPTY[nr]});
				PlaySound(game, "K STUD 01 25 13-001", 1.0);
			} else {
				if (nr == 9) {
					ChangeCurrentGamestate(game, "anim");
					return;
				}
				LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){ANIMS[nr]});
				if (nr == data->enabled) {
					data->enabled++;
				}
				PlaySound(game, "K STUD 01 39 48-001", 1.0);
				if (nr == 0) {
					if (data->jaszczur == 1) {
						LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){"naparstki_01_kapelusz_jaszczurka_wersja1"});
					}
					if (data->jaszczur == 2) {
						LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){"naparstki_01_kapelusz_jaszczurka_wersja2"});
					}
					data->jaszczur++;
					if (data->jaszczur == 3) {
						data->jaszczur = 0;
					}
				}
			}
			HideMouse(game);
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		ChangeCurrentGamestate(game, "anim");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		game->data->queue_handled = 0;
		game->data->queue_pos = 0;
		ChangeCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	data->player = CreatePlayer(game);
	progress(game);

	for (int i = 0; i < 9; i++) {
		data->kokardki[i] = al_load_bitmap(GetDataFilePath(game, PunchNumber(game, "naparstki/kokardkaX.png", 'X', i + 1)));
		progress(game);
		data->tasiemki[i] = al_load_bitmap(GetDataFilePath(game, PunchNumber(game, "naparstki/tasiemkaX.png", 'X', i + 1)));
		progress(game);
	}

	data->mask = al_load_bitmap(GetDataFilePath(game, "masks/naparstki.mask"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyPlayer(game, data->player);
	al_destroy_bitmap(data->mask);
	for (int i = 0; i < 9; i++) {
		al_destroy_bitmap(data->kokardki[i]);
		al_destroy_bitmap(data->tasiemki[i]);
	}
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){"naparstki_00_poczatek"});
	data->enabled = 0;
	data->first = true;
	EnsureMusic(game, "JAMMIN K LAP L 18 10 23", 1.0);
	Enqueue(game, (struct SceneDefinition){"naparstki_10_latarnia_zbija_wszystko", .freezes = {{93, "naparstki2", .links = {{{1.0, -1.0, -1.0}, .skip = true}, {{0.0, -1.0, -1.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"naparstki_10a_latarnia_pusta_sowka_piana_czesc1", .audio = {SOUND, "K STUD 01 39 48-001"}, .freezes = {{25, "naparstki2", .links = {{{1.0, -1.0, -1.0}, .skip = true}, {{0.0, -1.0, -1.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"naparstki_10a_latarnia_pusta_sowka_piana_czesc2", .audio = {SOUND, "pudelko2"}, .freezes = {{25, "naparstki2", .links = {{{-1.0, -1.0, 1.0}, .skip = true}, {{-1.0, -1.0, 0.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"naparstki_10a_latarnia_pusta_sowka_piana_czesc3", .audio = {SOUND, "pudelko3"}});
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
