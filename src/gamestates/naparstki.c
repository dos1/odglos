/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

static struct SceneDefinition ANIMS[] = {
	{"naparstki_01_kapelusz_jaszczurka_wersja3_najlepsza", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{7, {SOUND, "K OLD FX 08 14 79-001", .volume = 1.75}}}},
	{"naparstki_02_jaszczurka_kredki", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{9, {SOUND, "SWOOSH 4S LIST FX 00 07 10-001 "}}}},
	{"naparstki_03_paski_okno", .audio = {SOUND, "K STUD 01 39 48-001"}, .speed = 0.84, .sounds = {{5, {SOUND, "MADAT S LIST FX 14 09 89-001 R"}}}},
	{"naparstki_04_dzwoneczek_kwiatki", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{9, {SOUND, "SWARM SZ ENTER L 06 22 11", .volume = 0.8}}}},
	{"naparstki_05_kwiatki_czarno_biale", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{9, {SOUND, "POLTERGEIST SZ PULS L 00 06 00 31 - 02 14", .volume = 1.2}}}},
	{"naparstki_06_czarno_bialy_bambus", .audio = {SOUND, "K STUD 01 39 48-001"}, .speed = 0.9, .sounds = {{4, {SOUND, "happy_pointslines"}}, {9, {SOUND, "S RHAP FX 01 08 03-001", .volume = 0.75}}}},
	{"naparstki_07_panda_babuszka_wersja_krotsza2_lepsza", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{9, {SOUND, "ija_agentorange", .volume = 0.7}}}},
	{"naparstki_08_babuszka_zlote", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{9, {SOUND, "S LIST FX 00 43 16-001", .volume = 0.8}}}},
	{"naparstki_09_zloty_statki", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{9, {SOUND, "noise"}}}},
};

static char* ANIMS_EMPTY[] = {
	NULL,
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

static int KOKARDKA_OFFSETS[][2] = {
	{647, 189},
	{765, 420},
	{519, 121},
	{801, 61},
	{456, 430},
	{624, 352},
	{906, 326},
	{332, 359},
	{640, 68},
	{309, 179},
};

static int TASIEMKA_OFFSETS[][2] = {
	{616, 198},
	{754, 483},
	{512, 179},
	{743, 137},
	{445, 480},
	{604, 403},
	{881, 366},
	{353, 403},
	{629, 146},
};

struct GamestateResources {
	struct Player* player;
	ALLEGRO_BITMAP* mask;
	ALLEGRO_BITMAP *kokardki[9], *tasiemki[9];
	int enabled;
	int jaszczur;
};

int Gamestate_ProgressCount = 20;

static void GoForward(struct Game* game, struct GamestateResources* data) {
	Enqueue(game, (struct SceneDefinition){"naparstki_10_latarnia_zbija_wszystko", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{14, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {19, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {23, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {27, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {31, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {35, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {39, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {44, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {48, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {53, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {58, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {62, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {67, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {72, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {77, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}, {82, {SOUND, "K OLD P 04 47 86", .volume = 2.0}}}, .freezes = {{85, "naparstki2", .links = {{{1.0, -1.0, -1.0}, .skip = true}, {{0.0, -1.0, -1.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"naparstki_10a_latarnia_pusta_sowka_piana_czesc1", .audio = {SOUND, "K STUD 01 36 84-001"}, .freezes = {{25, "naparstki2", .links = {{{1.0, -1.0, -1.0}, .skip = true}, {{0.0, -1.0, -1.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"naparstki_10a_latarnia_pusta_sowka_piana_czesc2", .audio = {SOUND, "BIG BOING S LIST FX 00 10 32-001 "}, .freezes = {{25, "naparstki2", .links = {{{-1.0, -1.0, 1.0}, .skip = true}, {{-1.0, -1.0, 0.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"naparstki_10a_latarnia_pusta_sowka_piana_czesc3", .audio = {SOUND, "piana_lapis", .stop_music = true}});
	ChangeCurrentGamestate(game, "anim");
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	UpdatePlayer(game, data->player, delta);
	ALLEGRO_COLOR color = CheckMask(game, data->mask);
	int nr = round(((color.r * 255) + (color.g * 255) + (color.b * 255)) / 40.0);
	game->data->hover = nr < 17 && nr >= data->enabled - 1;

	if (PlayerIsFinished(game, data->player)) {
		ShowMouse(game);
	}

	if (game->data->skip_requested) {
		for (int i = data->enabled; i < 9; i++) {
			Enqueue(game, ANIMS[i]);
		}
		GoForward(game, data);
		UnsetSkip(game);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	DrawPlayer(game, data->player);
	if (game->data->cursor && data->enabled > 0) {
		for (int i = 0; i < data->enabled - 1; i++) {
			al_draw_bitmap(data->tasiemki[i], TASIEMKA_OFFSETS[i][0], TASIEMKA_OFFSETS[i][1], 0);
		}
		al_draw_bitmap(data->kokardki[data->enabled - 1], KOKARDKA_OFFSETS[data->enabled - 1][0], KOKARDKA_OFFSETS[data->enabled - 1][1], 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	if (game->data->cursor &&
		((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN))) {
		ALLEGRO_COLOR color = CheckMask(game, data->mask);
		int nr = round(((color.r * 255) + (color.g * 255) + (color.b * 255)) / 40.0);
		if (nr < 17 && nr >= data->enabled - 1) {
			if ((nr != data->enabled) && (nr != data->enabled - 1)) {
				LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){ANIMS_EMPTY[nr]});
				PlaySound(game, "K STUD 01 25 13-001", 1.0);
			} else {
				if (nr == 9) {
					GoForward(game, data);
					return;
				}
				if (nr == data->enabled) {
					data->enabled++;
				}
				if (nr == 0) {
					if (data->jaszczur == 1) {
						LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){"naparstki_01_kapelusz_jaszczurka_wersja1", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{7, {SOUND, "K OLD FX 08 14 79-001", .volume = 1.75}}}});
					} else if (data->jaszczur == 2) {
						LoadPlayerAnimation(game, data->player, &(struct SceneDefinition){"naparstki_01_kapelusz_jaszczurka_wersja2", .audio = {SOUND, "K STUD 01 39 48-001"}, .sounds = {{7, {SOUND, "K OLD FX 08 14 79-001", .volume = 1.75}}}});
					} else {
						LoadPlayerAnimation(game, data->player, &ANIMS[nr]);
					}
					data->jaszczur++;
					if (data->jaszczur == 3) {
						data->jaszczur = 0;
					}
				} else {
					LoadPlayerAnimation(game, data->player, &ANIMS[nr]);
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

	data->mask = LoadMemoryBitmap(GetDataFilePath(game, "masks/naparstki.mask"));
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
	EnsureMusic(game, "smok2_lapis", 0.75);
	game->data->skip_available = true;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	game->data->skip_available = false;
}
