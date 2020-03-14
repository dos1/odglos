#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

static bool IsCheckpoint(struct Game* game, int scene) {
	int checkpoints[] = {0, 7, 11, 14, 15, 17, 18, 23, 31, 34, 36, 37, 42, 45, 48, 57, 60, 62, 66, 68, 72, 73, 75, 84};
	for (unsigned int i = 0; i < sizeof(checkpoints) / sizeof(int); i++) {
		if (checkpoints[i] == scene) {
			return true;
		}
	}
	return false;
}

static bool Pergola(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	*x = 655 - 1355 / 2;
	*y = 353 - 762 / 2;
	*scale = 1355.0 / 1280.0;
	return false;
};

static bool Pergola2(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	*x = 0;
	*y = 0;
	*scale = 1.0;
	if ((frame >= 7 && frame <= 14) || (frame >= 20)) {
		*x = 655 - 1355 / 2;
		*y = 353 - 762 / 2;
		*scale = 1355.0 / 1280.0;
	}
	return false;
};

static bool Generator(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	*x = 142;
	*y = 136;
	return false;
}

static bool DuchPortalu(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	*x = 0;
	*y = -222;
	return false;
}

static bool RegalDmuchawa(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	*x = 550;
	*y = 60 - cos(frame / 4.0) * 10;
	SetCharacterPosition(game, character, frame * 30 - 800, 68, 0);
	return false;
}

static bool Zakochani(struct Game* game, struct Character* character, void** data) {
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02a_zakochani", .sounds = {{8, {SOUND, "lawka/10"}}, {12, {SOUND, "lawka/11"}}, {15, {SOUND, "lawka/12"}}}, .freezes = {{16, "DSCF7440_maska2_z_zakochana_para", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02b_muzykanci", .sounds = {{0, {SOUND, "pudelko2"}}}});

	return true;
}

static bool Muzykanci(struct Game* game, struct Character* character, void** data) {
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02b_muzykanci", .sounds = {{0, {SOUND, "pudelko2"}}}, .freezes = {{26, "DSCF7440_maska2_z_zakochana_para", .links = {{{0.0, 1.0, 0.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02a_zakochani", .sounds = {{8, {SOUND, "lawka/10"}}, {12, {SOUND, "lawka/11"}}, {15, {SOUND, "lawka/12"}}}});

	return true;
}

static bool Donice(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	if (!*data) {
		*data = malloc(sizeof(int) * 2);
		int* left = *data;
		int* right = left + 1;
		*left = 0;
		*right = 0;
	}
	return false;
}

static struct SceneDefinition anims_donica_left[] = {
	{"donice_18_samochod_duzy_wyjezdza_w_prawo", .freezes = {{13, "donice_w_ogrodzie_maski"}}},
	{"donice_03_samochod_duzy_wjezdza_do_donicy_z_lewej", .speed = 0.75, .freezes = {{6, "donice_w_ogrodzie_maski"}}},
	{"donice_17_samochod_duzy_wyjezdza_w_lewo", .freezes = {{7, "donice_w_ogrodzie_maski"}}},
};

static struct SceneDefinition anims_donica_right[] = {
	{"donice_06_samochod_maly_z_sowka_wyjezdza_w_przod", .freezes = {{14, "donice_w_ogrodzie_maski"}}},
	{"donice_19_samochod_maly_z_sowka_wjezdza_z_prawej", .freezes = {{9, "donice_w_ogrodzie_maski"}}},
	{"donice_05_samochod_maly_bez_sowki_wyjezdza_w_przod", .freezes = {{14, "donice_w_ogrodzie_maski"}}},
	{"donice_20_samochod_maly_bez_sowki_wjezdza_z_prawej", .freezes = {{8, "donice_w_ogrodzie_maski"}}},
	{"donice_04_samochod_maly_z_sowka_wyjezdza_w_prawo", .freezes = {{6, "donice_w_ogrodzie_maski"}}},
	{"donice_21_sowka_rudnik_wychodzi_w_prawo", .freezes = {{8, "donice_w_ogrodzie_maski"}}},
};

static bool DonicaLewa(struct Game* game, struct Character* character, void** data);
static bool DonicaPrawa(struct Game* game, struct Character* character, void** data);

static void DonicaSetup(struct Game* game, struct SceneDefinition* anim, void** data) {
	unsigned int* left = *data;
	unsigned int* right = left + 1;

	struct FreezeFrame* freeze = &anim->freezes[0];
	while (freeze->mask) {
		freeze->links[0].color = al_map_rgb_f(1.0, 0.0, 0.0);
		if (*left < (sizeof(anims_donica_left) / sizeof(struct SceneDefinition))) {
			freeze->links[0].callback = DonicaLewa;
		} else {
			freeze->links[0].ignore = true;
		}
		freeze->links[1].color = al_map_rgb_f(0.0, 1.0, 0.0);
		if (*right < (sizeof(anims_donica_right) / sizeof(struct SceneDefinition))) {
			freeze->links[1].callback = DonicaPrawa;
		} else {
			freeze->links[1].ignore = true;
		}
		freeze++;
	}

	if (anim->freezes[0].links[0].ignore && anim->freezes[0].links[1].ignore) {
		anim->freezes[0].mask = NULL;
		free(*data);
		*data = NULL;
	}

	anim->callback_data = *data;
}

static bool DonicaLewa(struct Game* game, struct Character* character, void** data) {
	unsigned int* left = *data;

	struct SceneDefinition anim = anims_donica_left[*left];
	(*left)++;

	DonicaSetup(game, &anim, data);
	Enqueue(game, anim);
	PlaySound(game, "S LIST FX 00 50 07-001", 1.0);

	return true;
}

static bool DonicaPrawa(struct Game* game, struct Character* character, void** data) {
	unsigned int* left = *data;
	unsigned int* right = left + 1;

	struct SceneDefinition anim = anims_donica_right[*right];
	(*right)++;

	DonicaSetup(game, &anim, data);
	Enqueue(game, anim);
	PlaySound(game, "S LIST FX 00 51 84-001", 1.0);

	return true;
}

static bool Dzwieki(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	if (!*data) {
		*data = malloc(sizeof(bool) * 3);
		bool* d = *data;
		*d = false;
		*(d + 1) = false;
		*(d + 2) = false;
	}
	return false;
}

static bool Dzwiek1(struct Game* game, struct Character* character, void** data);
static bool Dzwiek2(struct Game* game, struct Character* character, void** data);
static bool Dzwiek3(struct Game* game, struct Character* character, void** data);

static bool Dzwiek1(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;

	*d = true;

	PlaySound(game, "lawka/19", 1.0);

	if (!(*d && *(d + 1) && *(d + 2))) {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek1", .freezes = {{25, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}, .callback_data = *data});
	} else {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek1"});
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool Dzwiek2(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;

	*(d + 1) = true;

	PlaySound(game, "lawka/6", 1.0);

	if (!(*d && *(d + 1) && *(d + 2))) {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek2", .freezes = {{45, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}, .callback_data = *data});
	} else {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek2"});
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool Dzwiek3(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;

	*(d + 2) = true;

	PlaySound(game, "lawka/7", 1.0);

	if (!(*d && *(d + 1) && *(d + 2))) {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek3", .freezes = {{25, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}, .callback_data = *data});
	} else {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek3"});
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool Ul(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	if (!*data) {
		*data = malloc(sizeof(bool) * 3);
		bool* d = *data;
		*d = false;
		*(d + 1) = false;
		*(d + 2) = false;
	}
	return false;
}

static bool UlLewo(struct Game* game, struct Character* character, void** data);
static bool UlGora(struct Game* game, struct Character* character, void** data);
static bool UlDol(struct Game* game, struct Character* character, void** data);

static void UlSetup(struct Game* game, struct SceneDefinition* scene, bool* data) {
	*scene = (struct SceneDefinition){"ul_duzy_pusty_mozna_dac_tez_sama_pierwsza_klatke", .callback = Ul, .freezes = {{0, "IMG_0053_maska", .links = {{{0.0, 1.0, 0.0}, .callback = UlLewo}, {{1.0, 0.0, 0.0}, .callback = UlGora}, {{0.0, 0.0, 1.0}, .callback = UlDol}}}}, .callback_data = data};
	if (*data) {
		scene->freezes[0].links[0].ignore = true;
	}
	if (*(data + 1)) {
		scene->freezes[0].links[1].ignore = true;
	}
	if (*(data + 2)) {
		scene->freezes[0].links[2].ignore = true;
	}
}

static bool UlLewo(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;

	*d = true;

	Enqueue(game, (struct SceneDefinition){"ul_duzy_lewo"});

	if (!(*d && *(d + 1) && *(d + 2))) {
		struct SceneDefinition scene;
		UlSetup(game, &scene, d);
		Enqueue(game, scene);
	} else {
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool UlGora(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;

	*(d + 1) = true;

	Enqueue(game, (struct SceneDefinition){"ul_duzy_gora"});

	if (!(*d && *(d + 1) && *(d + 2))) {
		struct SceneDefinition scene;
		UlSetup(game, &scene, d);
		Enqueue(game, scene);
	} else {
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool UlDol(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;

	*(d + 2) = true;

	Enqueue(game, (struct SceneDefinition){"ul_duzy_dol"});

	if (!(*d && *(d + 1) && *(d + 2))) {
		struct SceneDefinition scene;
		UlSetup(game, &scene, d);
		Enqueue(game, scene);
	} else {
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool SwiecznikLewy(struct Game* game, struct Character* character, void** data);
static bool SwiecznikPrawy(struct Game* game, struct Character* character, void** data);

static void Swiecznik(struct Game* game, void** data) {
	bool* d = *data;
	char* name;
	if (!*d && !*(d + 1)) {
		name = "DSCF2449";
	} else if (*d && !*(d + 1)) {
		name = "DSCF2305";
	} else if (!*d && *(d + 1)) {
		name = "DSCF2311";
	} else {
		free(*data);
		*data = NULL;
		Enqueue(game, (struct SceneDefinition){"DSCF2320"});
		return;
	}
	Enqueue(game, (struct SceneDefinition){name, .callback_data = *data, .freezes = {{0, "DSCF2296_maska_oczy_ewentualnie", .links = {{{10.0 / 255.0, 0.0, 0.0}, .callback = SwiecznikLewy, .audio = {SOUND, "switch"}}, {{20.0 / 255.0, 0.0, 0.0}, .callback = SwiecznikPrawy, .audio = {SOUND, "switch"}}}}}});
}

static bool SwiecznikLewy(struct Game* game, struct Character* character, void** data) {
	if (!*data) {
		*data = malloc(sizeof(bool) * 2);
		bool* d = *data;
		*d = false;
		*(d + 1) = false;
	}
	bool* d = *data;
	*d = !*d;
	Swiecznik(game, data);
	return true;
}

static bool SwiecznikPrawy(struct Game* game, struct Character* character, void** data) {
	if (!*data) {
		*data = malloc(sizeof(bool) * 2);
		bool* d = *data;
		*d = false;
		*(d + 1) = false;
	}
	bool* d = *data;
	*(d + 1) = !*(d + 1);

	Swiecznik(game, data);
	return true;
}

static bool Credits(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	*x = 50;
	*y = 180;
	*scale = 0.5;
	return false;
}

static void DrawCredits(struct Game* game, int frame, void** data) {
	if (frame < 15) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "ODGŁOS");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 1, ALLEGRO_ALIGN_LEFT, "(" LIBSUPERDERPY_GAME_GIT_REV "-" LIBSUPERDERPY_GIT_REV ")");
	} else if (frame < 30) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "by Holy Pangolin");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 2, ALLEGRO_ALIGN_LEFT, "Agata Nawrot");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 3, ALLEGRO_ALIGN_LEFT, "Sebastian Krzyszkowiak");
	} else if (frame < 50) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "SPECIAL THANKS");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 2, ALLEGRO_ALIGN_LEFT, "Handicraft:");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 4, ALLEGRO_ALIGN_LEFT, "Fundacja Pogotowie Społeczne");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 5, ALLEGRO_ALIGN_LEFT, "Aleksandra Nawrot");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 6, ALLEGRO_ALIGN_LEFT, "Leszek Puchalski");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 7, ALLEGRO_ALIGN_LEFT, "Zela Monika Pytko");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 8, ALLEGRO_ALIGN_LEFT, "Zośka Koszałkowska");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 9, ALLEGRO_ALIGN_LEFT, "Lisa Slater");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 11, ALLEGRO_ALIGN_LEFT, "PRES model:");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 12, ALLEGRO_ALIGN_LEFT, "Museum of Modern Art in Warsaw");

	} else if (frame < 70) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "SPECIAL THANKS");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 2, ALLEGRO_ALIGN_LEFT, "Instruments:");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 4, ALLEGRO_ALIGN_LEFT, "Bartosz Izbicki");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 5, ALLEGRO_ALIGN_LEFT, "Małgorzata Izbicka");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 6, ALLEGRO_ALIGN_LEFT, "Bartosz Żłobiński");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 7, ALLEGRO_ALIGN_LEFT, "Leszek Pelc");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 8, ALLEGRO_ALIGN_LEFT, "Zbigniew Butryn");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 9, ALLEGRO_ALIGN_LEFT, "Hermann Knoch");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 10, ALLEGRO_ALIGN_LEFT, "Béla Szerényi");
	} else if (frame < 85) {
		al_draw_multiline_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180, 500, 30, ALLEGRO_ALIGN_LEFT, "All music and sounds created in Polish Radio Experimental Studio by Krzysztof Knittel, Elżbieta Sikora and Ryszard Szeremeta.");
	} else if (frame < 100) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "© 2019 Adam Mickiewicz Insitute");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 1, ALLEGRO_ALIGN_LEFT, "       Vitruvio Foundation");

		al_draw_multiline_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 3, 500, 30, ALLEGRO_ALIGN_LEFT, "Financed by the Ministry of Culture and National Heritage of the Republic of Poland as part of the multi-annual programme NIEPODLEGŁA 2017–2022.");

		al_draw_bitmap(game->data->banner, 720, 490, 0);
	}
}

static struct SceneDefinition SCENES[] = {
	{"kostki_animacja02_cwierc_obrotu_zapetlic", .repeats = 3, .freezes = {{23, ""}}, .bg = "ekran_startowy_tlo_przyciete"}, //
	{"kostki_animacja03_waz", .audio = {MUSIC, "odlot"}, .bg = "ekran_startowy_tlo_przyciete"}, //
	{">logo"}, //
	{"wedrowka_rodzinki_po_trawce", .audio = {MUSIC, "wedrowka_lapis"}, .freezes = {{30, .footnote = 9}}}, //
	{"rodzinka_jak_wiewiorki", .freezes = {{20, "DSCF8146_maska", .audio = {MUSIC, "wedrowka2_lapis"}}}}, //
	{"sowka_wchodzi_na_drzewo"}, //
	{"sowka_pokazuje_mordke_i_wraca"}, //
	{"buty_drewniane", .audio = {STOP_MUSIC}, .sounds = {{0, {SOUND, "buciki_trickstar"}}}}, //
	{"regal_animacja_sam", .freezes = {{0, "", .audio = {MUSIC, "dmuchawa_metrographfuture"}}}, .repeats = 1, .bg = "regal_dmuchawa_100_9254_tlo_przyciete", .callback = RegalDmuchawa, .character = {"dmuchawa", {"dmuchawa_ptaszor_sam"}, .repeat = true}}, //
	{"generator_animacja_wstepna", .audio = {STOP_MUSIC}, .freezes = {{0, "", .audio = {LOOP, "generator_metrographfuture"}}}, .callback = Generator, .bg = "generator_tlo_liscie_przyciete"}, //
	{"sucha_trawa_aksamitki_samochod_stary", .audio = {SOUND, "K ROB FX 03 26 00-001"}}, //
	{"sowka_konfrontacja_z_rzezba", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF8160_maska", .audio = {MUSIC, "BIRDY K K LAP L 05 29 08"}}}}, //
	{"wrzosy_kuzyn_i_sowka2", .audio = {MUSIC, "DIGI DOGZ K NOR L 04 16 61"}}, //
	{"samochod_kominek", .audio = {MUSIC, "LASER SHOWER S LIST L 12 35 45"}}, //
	{">byk"}, //
	{"swiecznik_hover_ewentualnie", .audio = {MUSIC, "ambient"}, .freezes = {{2, "DSCF2296_maska_oczy_ewentualnie", .links = {{{10.0 / 255.0, 0.0, 0.0}, .callback = SwiecznikLewy, .audio = {SOUND, "switch"}}, {{20.0 / 255.0, 0.0, 0.0}, .callback = SwiecznikPrawy, .audio = {SOUND, "switch"}}}}}}, //
	{"swiecznik3_TAK", .audio = {LOOP, "MAD CASPER SZ AGNT L 00 02 10 45 - 13 89"}}, //
	{"samochody_w_lesie", .audio = {MUSIC, "silence"}, .freezes = {{0, "DSCF2433_maska", .audio = {LOOP, "MEGAFLANGE SZ POINTS1 L 00 04 48- 51 "}}}, .sounds = {{24, {SOUND, "S RHAP FX 05 56 09-001", .volume = 0.69}}, {37, {LOOP, "ULTRAFLANGE"}}}}, //
	{"aksamitki_samochod_sowka", .speed = 1.25, .audio = {LOOP, "PIXEL BUBBLES K ROB L 05 29 10 ", .stop_music = true}}, //
	{"donice_02_samochod_duzy_jedzie_w_lewo", .audio = {SOUND, "donice1_points"}, .speed = 0.5}, //
	{"donice_10_sowka_srednia_wjezdza_do_donicy_z_prawej", .audio = {SOUND, "donice2_points"}}, //

	{"donice_01_samochod_duzy_jedzie_w_prawo", .audio = {SOUND, "donice3_points"}, .freezes = {{18, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}}, //
	{"donice_08_mala_sowka_z_samochodem_wyjezdza_w_przod", .audio = {SOUND, "donice4_points"}}, //
	//{">skrzypce"},
	{"skrzypce2_animacja_przerywnikowa", .audio = {SOUND, "S RHAP FX 05 56 09-001"}}, //
	{"skrzypce2_dzwiek1", .freezes = {{0, "skrzypce_maski_DSCF9053", .audio = {SOUND, "skrzypce1_orange"}}}}, //
	{"skrzypce2_dzwiek2", .audio = {SOUND, "skrzypce2_orange"}}, //
	{"skrzypce2_dzwiek3", .audio = {SOUND, "skrzypce3_orange"}}, //
	{"skrzypce2_dzwiek4", .audio = {SOUND, "skrzypce5_orange"}}, //
	{"skrzypce2_dzwiek5", .audio = {SOUND, "skrzypce6_orange"}}, //
	{"skrzypce2_dzwiek6", .audio = {SOUND, "skrzypce4_orange"}}, //
	{"skrzypce2_animacja_koncowa", .repeats = 1, .freezes = {{0, .footnote = 3}}, .sounds = {{1, {SOUND, "skrzypce_orange"}}}}, //
	{"gawron_i_drewniany_medrzec",
		.audio = {MUSIC, "gawron_poko"},
		.sounds = {
			{4, {LOOP, "CRICKET GOSSIP S VIEW L 03 49 48", .volume = 0.9}},
			{18, {STOP_LOOP, "CRICKET GOSSIP S VIEW L 03 49 48"}},
			{20, {LOOP, "CRICKET GOSSIP 2S VIEW L 03 54 93", .volume = 0.8}},
			{31, {STOP_LOOP, "CRICKET GOSSIP 2S VIEW L 03 54 93"}},
			{97, {LOOP, "CRICKET GOSSIP S VIEW L 03 49 48", .volume = 0.9}},
			{107, {STOP_LOOP, "CRICKET GOSSIP S VIEW L 03 49 48"}},
			{108, {SOUND, "drop_poko", .volume = 1.5}},
			{115, {SOUND, "lawka/13", .volume = 1.5}},
			{123, {SOUND, "lawka/14"}}, //
			{124, {LOOP, "DRUNK DRONE K NOR L 15 31 2"}},
			{187, {SOUND, "lawka/16"}},
			{188, {SOUND, "lawka/19"}}, //
			{189, {SOUND, "lawka/5"}}, //
			{190, {SOUND, "lawka/25"}}, //
			{191, {SOUND, "lawka/29"}}, //
			{192, {SOUND, "lawka/32"}},
			{193, {SOUND, "lawka/33"}}, //
			{194, {SOUND, "lawka/21"}}, //
			{195, {SOUND, "lawka/31"}},
			{199, {STOP_LOOP, "DRUNK DRONE K NOR L 15 31 2"}},
			{200, {SOUND, "lawka/27"}},
			//
			{210, {SOUND, "lawka/35"}},
			{212, {SOUND, "lawka/36"}},
			{213, {SOUND, "lawka/34"}},
			{215, {SOUND, "lawka/37"}},
			{216, {SOUND, "lawka/35"}},
			{217, {SOUND, "lawka/36"}},
			{219, {SOUND, "lawka/34"}},
			{220, {SOUND, "lawka/37"}},
			{221, {SOUND, "lawka/35"}},
			{223, {SOUND, "lawka/34"}},
			{225, {SOUND, "lawka/36"}},
			{226, {SOUND, "lawka/35"}},
			{227, {SOUND, "lawka/34"}},
			{228, {SOUND, "lawka/37"}},
			{248, {LOOP, "LASER SHOWER S LIST L 12 35 45"}},
			{291, {LOOP, "LASER SHOWER S LIST L 12 35 45", .volume = 0.25}},
			{296, {STOP_LOOP, "LASER SHOWER S LIST L 12 35 45"}},
		},
		.freezes = {
			{87, .footnote = 7},
			{107, "DSCF2982_maska", .audio = {MUSIC, "gawron_poko_mini"}},
			{290, "DSCF3781_maska", .pre_audio = {STOP_LOOP, "LASER SHOWER S LIST L 12 35 45"}, .audio = {LOOP, "BIRDY K K LAP L 05 29 08"}},
		}}, //
	//{">przyciski"},
	//{"przyciski_na_stacji_przycisk1", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk1_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk1_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk2", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk2_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk2_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk3", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk3_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_stacji_przycisk3_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	//{"przyciski_na_kominie_przycisk_1_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_1_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_1_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_2_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_2_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_2_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_3_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_3_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	//{"przyciski_na_kominie_przycisk_3_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},

	{"ciemna_trawa_samochod_sowka", .sounds = {{1, {SOUND, "S LIST FX 01 37 00-001 S LIST FX 01 53 86-001"}}}, .speed = 1.25}, //
	{"ciemna_trawa_waz", .sounds = {{2, {SOUND, "S LIST FX 07 17 05-001"}}}, .speed = 1.25}, //
	{"wchodzenie_po_schodach_samochod_sowka", .audio = {MUSIC, "dwor"}, .freezes = {{19, "maska_schodek1", .audio = {SOUND, "pac"}}, {23, "maska_schodek2", .audio = {SOUND, "pac"}}, {26, "maska_schodek3", .audio = {SOUND, "pac"}}, {29, "maska_schodek4", .audio = {SOUND, "pac"}}}}, //
	{"schodzenie_ze_schodow_waz"}, //

	{">lawka"}, //
	{"animacja_silacz1", .sounds = {{59, {SOUND, "S LIST FX 07 17 05-001"}}}, .audio = {MUSIC, "silence"}, .freezes = {{0, "silacz_maska", .audio = {SOUND, "LASER SHOWER S LIST L 12 35 45"}}, {22, "silacz_maska", .audio = {SOUND, "S RHAP FX 05 56 09-001"}}, {46, "silacz_maska", .audio = {SOUND, "S HEAD FX 17 06 27-001"}}}}, //
	{"donice_14_samochod_nadjezdza_z_prawej_i_wjezdza_do_donicy_z_lewej", .audio = {SOUND, "DIGI DOGZ K NOR L 04 16 61"}, .callback = Donice, .freezes = {{16, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .callback = DonicaLewa}, {{0.0, 1.0, 0.0}, .callback = DonicaPrawa}}}}}, //
	{"donice_12_waz_idzie_w_prawo_i_wchodzi_do_prawej_donicy", .audio = {SOUND, "S LIST FX 07 17 05-001"}}, //
	{"silacz2_maly_samochod_z_sowka_opuszcz_cien_rozny", .freezes = {{0, "silacz_maska", .audio = {SOUND, "K ROB FX 03 26 00-001"}}}}, //
	{"silacz3_maly_samochod_sam", .audio = {SOUND, "LASER SHOWER S LIST L 12 35 45"}}, //
	//.freezes = {{0, "silacz_maska"}}},
	{"lira_korbowa", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF8976_maska", .audio = {MUSIC, "CENTAURI SZ MIRAC L 06 20 79 - 24 61"}}}}, //
	{"male_dziwne_cos", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF8646_maska", .audio = {MUSIC, "breath"}}}, .repeats = 1}, //
	{"turkusowe_cos", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF9030_maska", .audio = {MUSIC, "rave"}}}}, //
	{"rzezby_w_lazience_2_wyciszenie_sznureczka", .audio = {MUSIC, "points"}}, //
	{"sowka_i_rzezby_01_sowka_przejezdza", .sounds = {{12, {SOUND, "lawka/13"}}}, .freezes = {{8, .footnote = 6}, {18, "DSCF7440_maska2_z_zakochana_para", .links = {{{1.0, 0.0, 0.0}, .callback = Zakochani}, {{0.0, 1.0, 0.0}, .callback = Muzykanci}}}}}, //
	{"031_donice_dom1", .audio = {SOUND, "S LIST FX 10 39 39-001", .stop_music = true}}, //
	{"donice_16_samochod_kartonowy_duzy_wjezdza_z_prawej", .freezes = {{0, "donice_w_ogrodzie_maski", .audio = {SOUND, "donice4_points"}, .links = {{{0.0, 1.0, 0.0}, .ignore = true}}}}}, //
	{"donice_22_sowka_srednia_whodzi_do_duzej_donicy_z_lewej", .audio = {SOUND, "donice3_points"}, .freezes = {{7, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}}, //
	{"donice_15_maly_samochodzik_kartonowy_wyjezdza", .audio = {SOUND, "donice1_points"}}, //
	{"donice_23_sowka_mala_wychodzi_w_przod", .audio = {SOUND, "donice2_points"}}, //
	{"donice_24_sowka_mala_wchodzi_z_prawej", .audio = {SOUND, "donice3_points"}}, //
	{"donice_25_sowka_srednia_wychodzi_z_lewej_donicy", .audio = {SOUND, "donice4_points"}}, //
	{"donice_26_sowka_srednia_wchodzi_do_prawej_donicy", .audio = {SOUND, "donice2_points"}}, //
	{"donice_27_sowka_duza_wychodzi_z_lewej_donicy", .audio = {SOUND, "donice1_points"}}, //
	{"donice_11_waz_buszuje_w_prawo_w_lewo_i_wchodzi_z_lewej", .audio = {SOUND, "donice3_points"}}, //
	{"donica_w_hortensjach_06_waz", .freezes = {{0, "donica_w_hortensjach_maska", .audio = {SOUND, "S LIST FX 07 17 05-001"}}, {10, "donica_w_hortensjach_maska", .audio = {SOUND, "pac"}}}}, //

	{"aksamitki_waz", .speed = 1.25, .audio = {MUSIC, "waz_poko"}}, //
	{"waz_zmienia_sie_w_kostke", .freezes = {{14, "IMG_0770_maska", .audio = {SOUND, "ELVES S LIST L 08 57 95"}}}, .audio = {MUSIC, "waz2_poko"}}, //
	{"sowka_wchodzi_do_miski_ciemniejsze", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF1595_maska", .audio = {MUSIC, "chill"}}}}, //
	{"duza_sowka_na_drewnianym_kole", .audio = {SOUND, "K RESZT FX 03 27 28-001"}, .speed = 0.8, .sounds = {{12, {STOP_MUSIC}}}, .freezes = {{13, "IMG_1010_maska", .audio = {SOUND, "dryndryn"}}}}, //
	{"animacja_poczatkowa", .audio = {SOUND, "pergola_trickstar"}, .repeats = 2, .callback = Pergola}, //
	{">pergola"}, //
	{"pergola_animacja_koncowa2", .callback = Pergola2, .audio = {MUSIC, "pergola2_trickstar"}}, //
	{"pergola_animacja_koncowa6"}, //, .freezes = {{9, .footnote = 3}}}, //
	{"ul_duzy_pusty_mozna_dac_tez_sama_pierwsza_klatke", .audio = {MUSIC, "dwor"}, .callback = Ul, .freezes = {{0, "IMG_0053_maska", .links = {{{0.0, 1.0, 0.0}, .callback = UlLewo}, {{1.0, 0.0, 0.0}, .callback = UlGora}, {{0.0, 0.0, 1.0}, .callback = UlDol}}}}}, //
	{"ul_duzy_animacja_koncowa_samochod", .sounds = {{0, {SOUND, "K ROB FX 03 26 00-001"}}}}, //
	{"pudelko_w_ogrodzie", .audio = {MUSIC, "pienki"}, .freezes = {{22, "pudelko_w_ogrodzie_maska1", .audio = {SOUND, "pac"}}, {56, "pudelko_w_ogrodzie_maska3", .audio = {SOUND, "pac"}}, {93, "pudelko_w_ogrodzie_maska2", .audio = {SOUND, "pac"}}, {119, "pudelko_w_ogrodzie_maska3", .audio = {SOUND, "pac"}}, {157, "pudelko_w_ogrodzie_maska2", .audio = {SOUND, "pac"}}, {183, "pudelko_w_ogrodzie_maska3", .audio = {SOUND, "pac"}}, {195, .footnote = 8}}}, //
	{"portal_ze_stolika_bialego", .freezes = {{9, "DSCF8382_maska", .audio = {SOUND, "pac"}}, {14, "DSCF8387_maska", .audio = {SOUND, "pac"}}}}, //
	{"siatka_na_drzewie_myszka", .audio = {MUSIC, "myszki"}}, //
	{"drzewko_kolorowe1_maskotki_podwojne_moze_lepsze_TAK"}, //
	{">pudelka"}, //
	{"pudelko_wypluwa_szczypczyki_smok_bez_dyn_TAK", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF5025_maska", .audio = {MUSIC, "JAMMIN K LAP L 18 10 23"}}}}, //
	{">naparstki"}, //
	{"01statki_szyszki_tasmy_animacja1", .audio = {ENSURE_MUSIC, "odwilz_trickstar", 1.0}}, //
	{"02statki_szyszki_tasmy_animacja2"}, // .freezes = {{11, .footnote = 4}}}, //
	{"03statki_szyszki_tasmy_animacja3", .callback = Dzwieki, .freezes = {{40, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}}, //
	{"05statki_szyszki_tasmy_animacja4", .freezes = {{69, "DSCF4999_maska"}}}, //
	{"06statki_szyszki_tasmy_animacja5"}, //
	{"magnetofon2_bez_myszek", .freezes = {{0, "DSCF9467_maska_magnetofon"}}}, //
	{"duch_portalu_animacja2_zlozona_TAK", .callback = DuchPortalu}, //
	{">armata"}, //

	{"podniebny_generator_z_kosmosem", .freezes = {{0, "podniebny_generator_z_kosmosem00_maska"}}}, //
	{"makieta_w_kosmosie_bez_tla", .audio = {MUSIC, "kosmos_metrograph"}, .bg = "kosmos", .freezes = {{28, .footnote = 2}}}, //
	{"makieta_pusta"}, //
	{"krzeslo_w_lesie_czesc1", .freezes = {{12, "krzeslo_w_lesie08_maska"}}}, //
	{"krzeslo_w_lesie_czesc2"}, //
	{"sowka1_wchodzi_na_stol_z_bliska_pojawia_sie_TAK"}, // fade?
	{"sowka2_zaluzje_pojawia_sie2_TAK", .speed = 0.15}, // fade?
	{"sowki_zamieniaja_sie_krzeslami_po_dwa_i_nie_znikaja_TAK", .freezes = {{85, "DSCF0566_maska_obszary"}}}, //
	{"sowka1_wlacza_konsole_z_daleka2", .bg = "kosmos"}, //
	{"sowka1_wlacza_konsole_z_bliska_lewa_konsola", .freezes = {{0, "DSCF0067_maska_ze_stolem"}}}, //
	{"sowka1_wlacza_konsole_z_bliska_srodkowa_konsola", .freezes = {{0, "DSCF0067_maska_ze_stolem"}}}, //
	//{"altanka_samochod"},
	//{"zamiana_myszki_w_bramie"},
	//{"sowka1_wchodzi_na_stol_z_bliska_nie_znika_TAK"},
	{"sowka2_klika_konsole_prawa", .freezes = {{0, "DSCF0286_maska"}}}, {"sowka2_klika_konsole_lewa", .freezes = {{0, "DSCF0286_maska"}}}, //
	//{"buzia_01_bez_niczego"},
	//{"buzia_02_sowa"},
	//{"buzia_03_kuzyn"},
	//{"buzia_04_myszka"},
	//{"wiklinowy_cyrk_po_dwa_bez_myszki"},
	//{"wiklinowy_cyrk_sama_myszka"},
	//{"wiklinowe_kolo1_samochod"},
	//{"wiklinowe_kolo2_pilka"},
	//{"wiklinowe_kolo3_myszka"},
	{"drzwi_zamykaja_sie_same", .audio = {MUSIC, "koniec_lapis"}, .bg = "kosmos"}, //
	{"okna_sie_otwieraja_z_sowka2", .bg = "kosmos"}, //
	{"sowka2_zaluzje_nie_znika_TAK"}, //
	{"sowka1_zaluzje"}, //
	{"animacja_koncowa", .bg = "kosmos", .freezes = {{29, .footnote = 1}}}, //
	{">myszka"}, //
	{">blank"}, //
	{"animacje_koncowe_rodzinki", .audio = {SOUND, "napisy_metrograph"}, .callback = Credits, .draw = DrawCredits, .speed = 0.5}, //
	{">blank", .audio = {STOP_SOUND, "napisy_metrograph"}}, //
	{"donice_13_tasma", .speed = 0.5, .freezes = {{0, "donice_w_ogrodzie_maski", .audio = {SOUND, "pudelko3"}, .links = {{{0.0, 1.0, 0.0}, .ignore = true}}}}}, //
	//
	//{"sowka_na_trawie"},
	//{"kuzyn_na_galeziach_podwojne"},
	//{"winorosle1_ciemnoniebieski_TAK"},
	//{"winorosle2_jasnoniebieski_bez_samych_lisciTAK"},
	//{"winorosle3_zolty_TAK"},
	//{"ule_jesienne"},
	//{"czerwona_sciana_z_winorosli"},
	//{"donica_w_hortensjach_05_samochod_z_sowka_wjezdza"},
	//{"donica_w_hortensjach_03_sowka_srednia_wychodzi"},
	//{"donica_w_hortensjach_02_sowka_srednia_wchodzi"},
	//{"donica_w_hortensjach_04_samochod_z_sowka_wyjezdza"},
	//{"myszkowanie_w_wiklinie"},
	//{"duch_portalu_animacja_oczu_osobno_lewe_TAK_DO_GRY", .callback = DuchPortalu},
	//{"duch_portalu_animacja_oczu_osobno_prawe_TAK_DO_GRY", .callback = DuchPortalu},
	//{"stolik_animacja", .freezes = {{0, ""}}, .bg = "stolik_tlo", .fg = "stolik_drzewa_na_przedzie"},
	//{"donice_28_sowka_duza_wchodzi_do_lewej_donicy"},
	//{"sucha_trawa_aksamitki_waz_stary"},
	//{"donice_29_sowka_srednia_wychodzi_z_prawej_donicy"},
	//{"sowka_idzie_w_lesie"},
	//{"kuzyn_w_lesie"},
	//{"donice_02_samochod_duzy_jedzie_w_lewo", .speed = 0.75},
	//{"donice_07_sowka_srednia_wjezdza_do_donicy_z_lewej"},
	//{"donice_20_samochod_maly_bez_sowki_wjezdza_z_prawej"},
	//{"donica_w_hortensjach_01_donica_sie_pojawia"},
};

bool Dispatch(struct Game* game) {
	if (game->data->queue_pos) {
		game->data->scene = game->data->queue[game->data->queue_handled];
		game->data->queue_handled++;
		PrintConsole(game, "Dispatch (from queue): %s", game->data->scene.name);
		if (game->data->queue_handled == game->data->queue_pos) {
			game->data->queue_handled = 0;
			game->data->queue_pos = 0;
		}
		return true;
	}
	do {
		game->data->sceneid++;
		if ((size_t)game->data->sceneid >= sizeof(SCENES) / sizeof(struct SceneDefinition)) {
			return false;
		}
	} while (!SCENES[game->data->sceneid].name);
	game->data->scene = SCENES[game->data->sceneid];
	PrintConsole(game, "Dispatch: %s", game->data->scene.name);
	if (game->data->sceneid) {
		char val[255];
		snprintf(val, 255, "%d", game->data->sceneid);
		SetConfigOption(game, LIBSUPERDERPY_GAMENAME, "scene", val);
	} else {
		DeleteConfigOption(game, LIBSUPERDERPY_GAMENAME, "scene");
	}
	return true;
}

void StartInitialGamestate(struct Game* game) {
	if (game->data->sceneid < 0) {
		game->data->sceneid = -1;
	}
	if (game->data->sceneid >= (int)((sizeof(SCENES) / sizeof(struct SceneDefinition)) - 1)) {
		game->data->sceneid = sizeof(SCENES) / sizeof(struct SceneDefinition) - 2;
	}
	while (!IsCheckpoint(game, game->data->sceneid + 1)) {
		game->data->sceneid--;
	}
	if (game->data->sceneid > 0) {
		game->data->menu_requested = true;
	}
	if (SCENES[game->data->sceneid + 1].name[0] == '>') {
		StartGamestate(game, SCENES[game->data->sceneid + 1].name + 1);
		Dispatch(game);
		return;
	}
	StartGamestate(game, "anim");
}

void Enqueue(struct Game* game, struct SceneDefinition scene) {
	PrintConsole(game, "Enqueue: %s", scene.name);
	game->data->queue[game->data->queue_pos] = scene;
	game->data->queue_pos++;
}
