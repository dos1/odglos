#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

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
	*y = 136 - sin(frame / ALLEGRO_PI / 2.0) * 10;
	SetCharacterPosition(game, character, *x, *y, 0);
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

static bool Skrzypce(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	if (!*data) {
		*data = malloc(sizeof(bool) * 6);
		bool* d = *data;
		for (int i = 0; i < 6; i++) {
			*(d + i) = false;
		}
	}
	return false;
}

static bool Skrzypce1(struct Game* game, struct Character* character, void** data);
static bool Skrzypce2(struct Game* game, struct Character* character, void** data);
static bool Skrzypce3(struct Game* game, struct Character* character, void** data);
static bool Skrzypce4(struct Game* game, struct Character* character, void** data);
static bool Skrzypce5(struct Game* game, struct Character* character, void** data);
static bool Skrzypce6(struct Game* game, struct Character* character, void** data);

static bool SkrzypceCheck(struct Game* game, void** data) {
	bool* d = *data;
	for (int i = 0; i < 6; i++) {
		PrintConsole(game, "checking skrzypce %d", i);
		if (!*(d + i)) {
			PrintConsole(game, "skrzypce %d is false", i);
			Enqueue(game, (struct SceneDefinition) //
				{"skrzypce2_animacja_przerywnikowa", //
					.freezes = {{3, "skrzypce_maski_DSCF9053", //
						.links = {
							{{0.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce1},
							{{10.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce2},
							{{20.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce3},
							{{30.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce4},
							{{40.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce5},
							{{50.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce6},
						}}},
					.callback_data = *data});
			return false;
		}
	}
	free(*data);
	*data = NULL;
	Enqueue(game, (struct SceneDefinition){"skrzypce2_animacja_przerywnikowa", .freezes = {{3, .footnote = 3}}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek1", .audio = {SOUND, "skrzypce1_orange"}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek2", .audio = {SOUND, "skrzypce2_orange"}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek3", .audio = {SOUND, "skrzypce3_orange"}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek4", .audio = {SOUND, "skrzypce5_orange"}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek5", .audio = {SOUND, "skrzypce6_orange"}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek6", .audio = {SOUND, "skrzypce4_orange"}});
	Enqueue(game, (struct SceneDefinition){"skrzypce2_animacja_koncowa", .repeats = 1, .sounds = {{1, {SOUND, "skrzypce_orange"}}}});
	return true;
}

static bool Skrzypce1(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;
	*(d + 0) = true;

	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek1", .callback_data = *data, .audio = {SOUND, "skrzypce1_orange"}});
	SkrzypceCheck(game, data);

	return true;
}

static bool Skrzypce2(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;
	*(d + 1) = true;

	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek2", .callback_data = *data, .audio = {SOUND, "skrzypce2_orange"}});
	SkrzypceCheck(game, data);

	return true;
}

static bool Skrzypce3(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;
	*(d + 2) = true;

	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek3", .callback_data = *data, .audio = {SOUND, "skrzypce3_orange"}});
	SkrzypceCheck(game, data);

	return true;
}

static bool Skrzypce4(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;
	*(d + 3) = true;

	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek4", .callback_data = *data, .audio = {SOUND, "skrzypce5_orange"}});
	SkrzypceCheck(game, data);

	return true;
}

static bool Skrzypce5(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;
	*(d + 4) = true;

	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek5", .callback_data = *data, .audio = {SOUND, "skrzypce6_orange"}});
	SkrzypceCheck(game, data);

	return true;
}

static bool Skrzypce6(struct Game* game, struct Character* character, void** data) {
	bool* d = *data;
	*(d + 5) = true;

	Enqueue(game, (struct SceneDefinition){"skrzypce2_dzwiek6", .callback_data = *data, .audio = {SOUND, "skrzypce4_orange"}});
	SkrzypceCheck(game, data);

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
	if (frame < 12) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "ODGŁOS");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 1, ALLEGRO_ALIGN_LEFT, "by Holy Pangolin");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 3, ALLEGRO_ALIGN_LEFT, "Agata Nawrot");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 4, ALLEGRO_ALIGN_LEFT, "Sebastian Krzyszkowiak");
	} else if (frame < 30) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "Production:");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 2, ALLEGRO_ALIGN_LEFT, "Jakub Marszałkowski");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 3, ALLEGRO_ALIGN_LEFT, "Vitruvio Foundation");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 5, ALLEGRO_ALIGN_LEFT, "Informational texts:");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 7, ALLEGRO_ALIGN_LEFT, "Bartek Chaciński");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 8, ALLEGRO_ALIGN_LEFT, "Jacek Hawryluk");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 9, ALLEGRO_ALIGN_LEFT, "Michał Mendyk");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 11, ALLEGRO_ALIGN_LEFT, "Translation:");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 12, ALLEGRO_ALIGN_LEFT, "Anna Kwapiszewska");
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

	} else if (frame < 69) {
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
		al_draw_multiline_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180, 500, 30, ALLEGRO_ALIGN_LEFT, "All music and sounds created in Polish Radio Experimental Studio by:");

		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 4, ALLEGRO_ALIGN_LEFT, "Krzysztof Knittel");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 5, ALLEGRO_ALIGN_LEFT, "Elżbieta Sikora");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 6, ALLEGRO_ALIGN_LEFT, "Ryszard Szeremeta");

		al_draw_multiline_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 10 - 5, 500, 30, ALLEGRO_ALIGN_LEFT, "The game originates from the Geek Jam held during the Game Industry Conference 2018.");
	} else if (frame < 100) {
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 0, ALLEGRO_ALIGN_LEFT, "© 2019 Adam Mickiewicz Institute");
		al_draw_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 1, ALLEGRO_ALIGN_LEFT, "       Vitruvio Foundation");

		al_draw_multiline_text(game->data->creditsfont, al_map_rgb(255, 255, 255), 720, 180 + 28 * 3, 500, 30, ALLEGRO_ALIGN_LEFT, "Financed by the Ministry of Culture and National Heritage of the Republic of Poland as part of the multi-annual programme NIEPODLEGŁA 2017–2022.");

		al_draw_bitmap(game->data->banner, 720, 490, 0);
	}
}

struct KosmosData {
	bool krzatanie;
	int buzia;
	int cyrk;
	int kolo;
	int samochod;
	int myszkowanie;
};

static bool Kosmos(struct Game* game, int frame, int* x, int* y, double* scale, struct Character* character, void** data) {
	if (!*data) {
		*data = calloc(1, sizeof(struct KosmosData));
	}
	return false;
}

static bool KosmosFinish(struct Game* game, struct KosmosData* data) {
	return data->krzatanie && data->buzia && data->cyrk && data->kolo && data->samochod && data->myszkowanie;
}

static bool KosmosBack(struct Game* game, struct Character* character, void** d);

static bool KosmosSowka(struct Game* game, struct Character* character, void** d);

static bool KosmosSowkaLeft(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	Enqueue(game, (struct SceneDefinition){"sowka1_wlacza_konsole_z_bliska_lewa_konsola", .sounds = {{7, {SOUND, "komputer_points", .volume = 0.5}}}});

	if (data->buzia % 2 == 0) {
		Enqueue(game, (struct SceneDefinition){"buzia_01_bez_niczego", .speed = 0.75, .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	} else {
		Enqueue(game, (struct SceneDefinition){"zamiana_myszki_w_bramie", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	}

	data->buzia++;
	return KosmosSowka(game, character, d);
}

static bool KosmosSowkaMiddle(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	Enqueue(game, (struct SceneDefinition){"sowka1_wlacza_konsole_z_bliska_srodkowa_konsola", .sounds = {{5, {SOUND, "K ROB FX 02 40 06-001", .volume = 0.5}}}});

	if (data->cyrk % 2 == 0) {
		Enqueue(game, (struct SceneDefinition){"wiklinowy_cyrk_po_dwa_bez_myszki", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	} else {
		Enqueue(game, (struct SceneDefinition){"wiklinowy_cyrk_sama_myszka", .speed = 0.666, .repeats = 1, .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	}

	data->cyrk++;
	return KosmosSowka(game, character, d);
}

static bool KosmosSowkaRight(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	Enqueue(game, (struct SceneDefinition){"sowka1_wchodzi_na_stol_z_bliska_nie_znika_TAK", .speed = 0.75, .sounds = {{6, {SOUND, "SPACE SMS S VOY FX 08 22 22-001", .volume = 0.5}}, {11, {SOUND, "SPACE SMS S VOY FX 08 22 22-001", .volume = 0.5}}}});

	if (data->kolo % 2 == 0) {
		Enqueue(game, (struct SceneDefinition){"wiklinowe_kolo1_samochod", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	} else {
		Enqueue(game, (struct SceneDefinition){"wiklinowe_kolo3_myszka", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	}

	data->kolo++;
	return KosmosSowka(game, character, d);
}

static bool KosmosSowka(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	if (!data->krzatanie) {
		data->krzatanie = true;
		Enqueue(game, (struct SceneDefinition){"sowka1_wlacza_konsole_z_daleka2", .bg = "kosmos", .sounds = {{7, {LOOP, "NEEDLES SZ ENTER L 04 03 11", .volume = 0.5}}, {40, {SOUND, "S LIST FX 00 36 10-001", .volume = 0.5}}}});
	}
	Enqueue(game, (struct SceneDefinition){"sowka1_wlacza_konsole_z_bliska_lewa_konsola", .callback_data = data, .freezes = {{0, "DSCF0067_maska_ze_stolem", .skip = true, .links = {{{0.0, 0.0, 1.0}, .callback = KosmosBack}, {{1.0, 0.0, 0.0}, .callback = KosmosSowkaLeft}, {{0.0, 1.0, 0.0}, .callback = KosmosSowkaMiddle}, {{0.0, 0.0, 0.0}, .callback = KosmosSowkaRight}}}}});
	return true;
}

static bool KosmosRudnik(struct Game* game, struct Character* character, void** d);

static bool KosmosRudnikLeft(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	Enqueue(game, (struct SceneDefinition){"sowka2_klika_konsole_lewa", .speed = 0.75, .sounds = {{10, {SOUND, "kompurer_bad_points", .volume = 0.75}}}});

	if (data->samochod % 2 == 0) {
		Enqueue(game, (struct SceneDefinition){"altanka_samochod", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	} else {
		Enqueue(game, (struct SceneDefinition){"altanka", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	}

	data->samochod++;
	return KosmosRudnik(game, character, d);
}

static bool KosmosRudnikRight(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	Enqueue(game, (struct SceneDefinition){"sowka2_klika_konsole_prawa", .speed = 0.75, .sounds = {{7, {SOUND, "SAD ENGINE K ROB L 05 48 10 short", .volume = 2.25}}}});

	if (data->myszkowanie % 2 == 0) {
		Enqueue(game, (struct SceneDefinition){"myszkowanie_w_wiklinie_puste", .repeats = 1, .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	} else {
		Enqueue(game, (struct SceneDefinition){"myszkowanie_w_wiklinie2", .fg = "gradient", .audio = {LOOP, "ambient", .volume = 0.1}, .sounds = {{0, {LOOP, "dwor", .volume = 0.1}}}});
	}

	data->myszkowanie++;
	return KosmosRudnik(game, character, d);
}

static bool KosmosRudnik(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	Enqueue(game, (struct SceneDefinition){"sowka2_klika_konsole_prawa", .callback_data = data, .freezes = {{0, "DSCF0286_maska", .skip = true, .links = {{{0.0, 0.0, 1.0}, .callback = KosmosBack}, {{1.0, 0.0, 0.0}, .callback = KosmosRudnikLeft}, {{0.0, 1.0, 0.0}, .callback = KosmosRudnikRight}}}}});
	return true;
}

static bool KosmosBack(struct Game* game, struct Character* character, void** d) {
	struct KosmosData* data = *d;
	if (KosmosFinish(game, *d)) {
		free(*d);
		return true;
	}
	Enqueue(game, (struct SceneDefinition){"sowki_zamieniaja_sie_krzeslami_po_dwa_freeze", .callback_data = data, .freezes = {{0, "DSCF0566_maska_obszary", .links = {{{1.0, 0.0, 0.0}, .callback = KosmosSowka}, {{0.0, 1.0, 0.0}, .callback = KosmosRudnik}}}}});
	return true;
}

static char* PACK_POSTLOAD[DOWNLOAD_PARTS][3] = {
	{"byk"},
	{"lawka"},
	{},
	{"pergola"},
	{"pudelka"},
	{},
	{},
	{"naparstki"},
	{"armata"},
	{},
	{},
};

void StartDownloadPacks(struct Game* game) {
	for (int i = 0; i < DOWNLOAD_PARTS; i++) {
		if (game->data->download.started[i] || !game->data->download.loaded[i]) {
			continue;
		}
		int j = 0;
		while (PACK_POSTLOAD[i][j]) {
			LoadGamestate(game, PACK_POSTLOAD[i][j]);
			j++;
		}
		game->data->download.started[i] = true;
	}
}

static struct SceneDefinition SCENES[] = {
	{"kostki_animacja02_cwierc_obrotu_zapetlic", .repeats = 3, .freezes = {{23, ""}}, .bg = "ekran_startowy_tlo_przyciete"}, //
	{"kostki_animacja03_waz", .audio = {LOOP, "odlot"}, .bg = "ekran_startowy_tlo_przyciete"}, //
	{">logo"}, //
	{"wedrowka_rodzinki_po_trawce", .audio = {MUSIC, "wedrowka_lapis"}, .freezes = {{30, .footnote = 9}}}, //
	{"rodzinka_jak_wiewiorki", .freezes = {{20, "DSCF8146_maska", .audio = {MUSIC, "wedrowka2_lapis"}}}}, //
	{"sowka_wchodzi_na_drzewo"}, //
	{"sowka_pokazuje_mordke_i_wraca"}, //
	{"buty_drewniane", .audio = {STOP_MUSIC}, .sounds = {{0, {SOUND, "buciki_trickstar"}}}, .checkpoint = true}, //
	{"regal_animacja_sam", .freezes = {{0, "", .audio = {MUSIC, "dmuchawa_metrographfuture"}}}, .repeats = 1, .bg = "regal_dmuchawa_100_9254_tlo_przyciete", .callback = RegalDmuchawa, .character = {"dmuchawa", {"dmuchawa_ptaszor_sam"}, .repeat = true}}, //
	{"generator_animacja_wstepna", .audio = {STOP_MUSIC}, .freezes = {{0, "", .audio = {LOOP, "generator_metrographfuture"}}}, .callback = Generator, .bg = "generator_tlo_liscie_przyciete", .character = {"generator", {"generator_wloski"}, .repeat = true}}, //
	{"sucha_trawa_aksamitki_samochod_stary", .audio = {SOUND, "K ROB FX 03 26 00-001"}}, //
	{"sowka_konfrontacja_z_rzezba", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF8160_maska", .audio = {MUSIC, "BIRDY K K LAP L 05 29 08"}}}, .checkpoint = true}, //
	{"wrzosy_kuzyn_i_sowka2", .audio = {MUSIC, "DIGI DOGZ K NOR L 04 16 61"}}, //
	{"samochod_kominek", .audio = {MUSIC, "LASER SHOWER S LIST L 12 35 45"}}, //
	{">byk", .checkpoint = true}, //
	{"swiecznik_hover_ewentualnie", .audio = {MUSIC, "ambient"}, .freezes = {{2, "DSCF2296_maska_oczy_ewentualnie", .links = {{{10.0 / 255.0, 0.0, 0.0}, .callback = SwiecznikLewy, .audio = {SOUND, "switch"}}, {{20.0 / 255.0, 0.0, 0.0}, .callback = SwiecznikPrawy, .audio = {SOUND, "switch"}}}}}, .checkpoint = true}, //
	{"swiecznik3_TAK", .audio = {LOOP, "MAD CASPER SZ AGNT L 00 02 10 45 - 13 89"}}, //
	{"samochody_w_lesie", .audio = {MUSIC, "silence"}, .freezes = {{0, "DSCF2433_maska", .audio = {LOOP, "MEGAFLANGE SZ POINTS1 L 00 04 48- 51 "}}}, .sounds = {{24, {SOUND, "S RHAP FX 05 56 09-001", .volume = 0.5}}, {37, {LOOP, "ULTRAFLANGE"}}}, .checkpoint = true, .pack = 1}, //
	{"aksamitki_samochod_sowka", .speed = 1.25, .audio = {LOOP, "PIXEL BUBBLES K ROB L 05 29 10 ", .stop_music = true}, .checkpoint = true}, //
	{"donice_02_samochod_duzy_jedzie_w_lewo", .audio = {SOUND, "donice1_points"}, .speed = 0.75}, //
	{"donice_10_sowka_srednia_wjezdza_do_donicy_z_prawej", .audio = {SOUND, "donice2_points"}}, //

	{"donice_01_samochod_duzy_jedzie_w_prawo", .audio = {SOUND, "donice3_points"}, .freezes = {{18, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}}, //
	{"donice_08_mala_sowka_z_samochodem_wyjezdza_w_przod", .audio = {SOUND, "donice4_points"}}, //

	{"skrzypce2_animacja_przerywnikowa",
		.callback = Skrzypce,
		.audio = {SOUND, "S RHAP FX 05 56 09-001"},
		.freezes = {{3, "skrzypce_maski_DSCF9053",
			.links = {
				{{0.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce1},
				{{10.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce2},
				{{20.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce3},
				{{30.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce4},
				{{40.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce5},
				{{50.0 / 255.0, 0.0, 0.0}, .callback = Skrzypce6},
			}}},
		.checkpoint = true}, //

	{"gawron_i_drewniany_medrzec", .audio = {MUSIC, "gawron_poko"}, //
		.sounds = {
			{4, {LOOP, "CRICKET GOSSIP S VIEW L 03 49 48", .volume = 0.8}},
			{18, {STOP_LOOP, "CRICKET GOSSIP S VIEW L 03 49 48"}},
			{20, {LOOP, "CRICKET GOSSIP 2S VIEW L 03 54 93", .volume = 0.7}},
			{31, {STOP_LOOP, "CRICKET GOSSIP 2S VIEW L 03 54 93"}},
			{97, {LOOP, "CRICKET GOSSIP S VIEW L 03 49 48", .volume = 0.8}},
			{107, {STOP_LOOP, "CRICKET GOSSIP S VIEW L 03 49 48"}},
			{108, {SOUND, "drop_poko", .volume = 1.5}},
			{115, {SOUND, "lawka/13", .volume = 1.5}},
			{123, {SOUND, "lawka/14"}},
			{124, {LOOP, "DRUNK DRONE K NOR L 15 31 2"}},
			{187, {SOUND, "lawka/16"}},
			{188, {SOUND, "lawka/19"}},
			{189, {SOUND, "lawka/5"}},
			{190, {SOUND, "lawka/25"}},
			{191, {SOUND, "lawka/29"}},
			{192, {SOUND, "lawka/32"}},
			{193, {SOUND, "lawka/33"}},
			{194, {SOUND, "lawka/21"}},
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
		},
		.checkpoint = true}, //

	{"ciemna_trawa_samochod_sowka", .audio = {MUSIC, "silence"}, .sounds = {{4, {SOUND, "S LIST FX 01 37 00-001"}}, {20, {SOUND, "S LIST FX 01 53 86-001"}}}, .speed = 1.35}, //
	{"ciemna_trawa_waz", .sounds = {{2, {SOUND, "S LIST FX 07 17 05-001"}}}, .speed = 1.25}, //
	{"wchodzenie_po_schodach_samochod_sowka", .audio = {MUSIC, "dwor"}, .freezes = {{19, "maska_schodek1", .audio = {SOUND, "pac"}}, {23, "maska_schodek2", .audio = {SOUND, "pac"}}, {26, "maska_schodek3", .audio = {SOUND, "pac"}}, {29, "maska_schodek4", .audio = {SOUND, "pac"}}}, .checkpoint = true}, //
	{"schodzenie_ze_schodow_waz"}, //

	{">lawka", .checkpoint = true}, //
	{"animacja_silacz1", .sounds = {{59, {SOUND, "S LIST FX 07 17 05-001"}}}, .audio = {MUSIC, "silence"}, .freezes = {{0, "silacz_maska", .audio = {SOUND, "LASER SHOWER S LIST L 12 35 45"}}, {22, "silacz_maska", .audio = {SOUND, "S RHAP FX 05 56 09-001"}}, {46, "silacz_maska", .audio = {SOUND, "S HEAD FX 17 06 27-001"}}}, .checkpoint = true, .pack = 2}, //
	{"donice_14_samochod_nadjezdza_z_prawej_i_wjezdza_do_donicy_z_lewej", .audio = {SOUND, "DIGI DOGZ K NOR L 04 16 61"}, .callback = Donice, .freezes = {{16, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .callback = DonicaLewa}, {{0.0, 1.0, 0.0}, .callback = DonicaPrawa}}}}}, //
	{"donice_12_waz_idzie_w_prawo_i_wchodzi_do_prawej_donicy", .audio = {SOUND, "S LIST FX 07 17 05-001"}}, //
	{"silacz2_maly_samochod_z_sowka_opuszcz_cien_rozny", .freezes = {{0, "silacz_maska", .audio = {SOUND, "K ROB FX 03 26 00-001"}}}}, //
	{"silacz3_maly_samochod_sam", .audio = {SOUND, "LASER SHOWER S LIST L 12 35 45"}}, //
	{"lira_korbowa", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF8976_maska", .audio = {MUSIC, "CENTAURI SZ MIRAC L 06 20 79 - 24 61"}}}, .checkpoint = true}, //
	{"male_dziwne_cos", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF8646_maska", .audio = {MUSIC, "breath"}}}, .repeats = 1}, //
	{"turkusowe_cos", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF9030_maska", .audio = {MUSIC, "rave"}}}}, //
	{"rzezby_w_lazience_2_wyciszenie_sznureczka", .audio = {MUSIC, "points"}, .checkpoint = true}, //
	{"sowka_i_rzezby_01_sowka_przejezdza", .sounds = {{12, {SOUND, "lawka/13", .volume = 2.0}}}, .freezes = {{8, .footnote = 6}, {18, "DSCF7440_maska2_z_zakochana_para", .links = {{{1.0, 0.0, 0.0}, .callback = Zakochani}, {{0.0, 1.0, 0.0}, .callback = Muzykanci}}}}}, //
	{"031_donice_dom1", .audio = {STOP_MUSIC}, .sounds = {{4, {SOUND, "S LIST FX 10 39 39-001"}}}, .speed = 1.25}, //
	{"donice_16_samochod_kartonowy_duzy_wjezdza_z_prawej", .freezes = {{0, "donice_w_ogrodzie_maski", .audio = {SOUND, "donice4_points"}, .links = {{{0.0, 1.0, 0.0}, .ignore = true}}}}, .checkpoint = true}, //
	{"donice_22_sowka_srednia_whodzi_do_duzej_donicy_z_lewej", .audio = {SOUND, "donice3_points"}, .freezes = {{7, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}}, //
	{"donice_15_maly_samochodzik_kartonowy_wyjezdza", .audio = {SOUND, "donice1_points"}}, //
	{"donice_23_sowka_mala_wychodzi_w_przod", .audio = {SOUND, "donice2_points"}}, //
	{"donice_24_sowka_mala_wchodzi_z_prawej", .audio = {SOUND, "donice3_points"}}, //
	{"donice_25_sowka_srednia_wychodzi_z_lewej_donicy", .audio = {SOUND, "donice4_points"}}, //
	{"donice_26_sowka_srednia_wchodzi_do_prawej_donicy", .audio = {SOUND, "donice2_points"}}, //
	{"donice_27_sowka_duza_wychodzi_z_lewej_donicy", .audio = {SOUND, "donice1_points"}}, //
	{"donice_11_waz_buszuje_w_prawo_w_lewo_i_wchodzi_z_lewej", .audio = {SOUND, "donice3_points"}}, //

	{"donica_w_hortensjach_06_waz", .freezes = {{0, "donica_w_hortensjach_maska", .audio = {SOUND, "S LIST FX 07 17 05-001"}}, {10, "donica_w_hortensjach_maska", .audio = {SOUND, "pac"}}}, .checkpoint = true}, //

	{"aksamitki_waz", .speed = 1.25, .audio = {MUSIC, "waz_poko"}}, //
	{"waz_zmienia_sie_w_kostke", .freezes = {{14, "IMG_0770_maska", .audio = {LOOP, "ELVES S LIST L 08 57 95"}}}, .audio = {MUSIC, "waz2_poko"}, .speed = 1.2}, //
	{"sowka_wchodzi_do_miski_ciemniejsze", .audio = {STOP_MUSIC}, .freezes = {{0, "DSCF1595_maska", .audio = {MUSIC, "chill"}}}, .checkpoint = true, .pack = 3}, //
	{"duza_sowka_na_drewnianym_kole", .audio = {SOUND, "K RESZT FX 03 27 28-001"}, .speed = 0.8, .sounds = {{12, {STOP_MUSIC}}}, .freezes = {{13, "IMG_1010_maska", .audio = {SOUND, "dryndryn"}}}}, //

	{"animacja_poczatkowa", .audio = {SOUND, "pergola_trickstar"}, .repeats = 2, .callback = Pergola, .checkpoint = true}, //
	{">pergola"}, //
	{"pergola_animacja_koncowa2", .callback = Pergola2, .audio = {MUSIC, "pergola2_trickstar"}}, //
	{"pergola_animacja_koncowa6", .callback = Pergola}, //
	{"ul_duzy_pusty_mozna_dac_tez_sama_pierwsza_klatke", .audio = {MUSIC, "dwor"}, .callback = Ul, .freezes = {{0, "IMG_0053_maska", .links = {{{0.0, 1.0, 0.0}, .callback = UlLewo}, {{1.0, 0.0, 0.0}, .callback = UlGora}, {{0.0, 0.0, 1.0}, .callback = UlDol}}}}, .checkpoint = true}, //
	{"ul_duzy_animacja_koncowa_samochod", .sounds = {{0, {SOUND, "K ROB FX 03 26 00-001"}}}}, //
	{"pudelko_w_ogrodzie", .audio = {MUSIC, "pienki"}, .freezes = {{22, "pudelko_w_ogrodzie_maska1", .audio = {SOUND, "pac"}}, {56, "pudelko_w_ogrodzie_maska3", .audio = {SOUND, "pac"}}, {93, "pudelko_w_ogrodzie_maska2", .audio = {SOUND, "pac"}}, {119, "pudelko_w_ogrodzie_maska3", .audio = {SOUND, "pac"}}, {157, "pudelko_w_ogrodzie_maska2", .audio = {SOUND, "pac"}}, {183, "pudelko_w_ogrodzie_maska3", .audio = {SOUND, "pac"}}, {195, .footnote = 8}}, .checkpoint = true, .pack = 4}, //
	{"portal_ze_stolika_bialego", .freezes = {{9, "DSCF8382_maska", .audio = {SOUND, "pac"}}, {14, "DSCF8387_maska", .audio = {SOUND, "pac"}}}}, //
	{"siatka_na_drzewie_myszka", .audio = {MUSIC, "myszki"}}, //
	{"drzewko_kolorowe1_maskotki_podwojne_moze_lepsze_TAK"}, //

	{">pudelka", .checkpoint = true}, //
	{"pudelko_wypluwa_szczypczyki_smok_bez_dyn_TAK", .audio = {STOP_MUSIC}, .sounds = {{52, {MUSIC, "smok_lapis"}}, {95, {SOUND, "smok_agentorange", .volume = 0.5}}, {101, {MUSIC, "smok2_lapis", .volume = 0.8}}}, .freezes = {{0, "DSCF5025_maska", .audio = {MUSIC, "JAMMIN K LAP L 18 10 23"}}}, .checkpoint = true, .pack = 7}, //
	{">naparstki"}, //
	{"naparstki_10b_KONCOWKA_chodaki_owce", .audio = {MUSIC, "odwilz_trickstar1"}}, //
	{"01statki_szyszki_tasmy_animacja1", .audio = {ENSURE_MUSIC, "odwilz_trickstar1", 1.0}, .sounds = {{10, {MUSIC, "odwilz_trickstar2"}}, {69, {SOUND, "NOISE LONG SZ ENTER 00 30 96", .volume = 0.5}}}, .checkpoint = true, .pack = 8}, //
	{"02statki_szyszki_tasmy_animacja2", .speed = 0.96}, //
	{"03statki_szyszki_tasmy_animacja3", .callback = Dzwieki, .freezes = {{40, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}}, //
	{"05statki_szyszki_tasmy_animacja4", .freezes = {{69, "DSCF4999_maska", .audio = {SOUND, "tasma_metrograph"}}}, .sounds = {{10, {SOUND, "PIANO SPIILL SZ TRIC L 04 17 48 - 22 44", .volume = 2.0}}, {64, {SOUND, "SWOOSH 4S LIST FX 00 07 10-001 ", .volume = 0.8}}}}, //
	{"06statki_szyszki_tasmy_animacja5"}, //
	{"magnetofon2_bez_myszek", .freezes = {{0, "DSCF9467_maska_magnetofon", .audio = {MUSIC, "odwilz_trickstar3"}}}, .sounds = {{3, {SOUND, "BARREL HIT 2 K STUD 3 FX 05 23 65"}}, {48, {MUSIC, "odwilz_trickstar4"}}}}, //
	{"duch_portalu_animacja2_zlozona_TAK", .callback = DuchPortalu, .speed = 0.92}, //
	{">armata"}, //
	{"podniebny_generator_z_kosmosem", .freezes = {{0, "podniebny_generator_z_kosmosem00_maska", .audio = {SOUND, "BARREL HIT 1K STUD 3 FX 02 00 37 ", .volume = 1.25}}}, .sounds = {{13, {SOUND, "trickstar_koniec", .volume = 0.7}}, {15, {STOP_MUSIC}}}}, //

	{"makieta_w_kosmosie_bez_tla", .audio = {MUSIC, "kosmos_metrograph1"}, .bg = "kosmos", .freezes = {{28, .footnote = 2}}, .checkpoint = true, .pack = 10}, //
	{"makieta_pusta", .audio = {STOP_LOOP, "dwor"}}, //
	{"krzeslo_w_lesie_czesc1", .audio = {LOOP, "dwor", .volume = 0.15, .persist = true}, .freezes = {{12, "krzeslo_w_lesie08_maska", .audio = {MUSIC, "kosmos_metrograph2"}}}}, //
	{"krzeslo_w_lesie_czesc2"}, //
	{"sowka1_wchodzi_na_stol_z_bliska_pojawia_sie_TAK", .audio = {STOP_LOOP, "dwor"}, .sounds = {{1, {SOUND, "pac", .volume = 0.2}}}}, // fade?
	{"sowka2_zaluzje_pojawia_sie2_TAK", .speed = 0.15, .sounds = {{1, {SOUND, "pac", .volume = 0.2}}}}, // fade?
	{"sowki_zamieniaja_sie_krzeslami_po_dwa_i_nie_znikaja_TAK", .freezes = {{85, .footnote = 1}}}, //
	{"sowki_zamieniaja_sie_krzeslami_po_dwa_freeze", .callback = Kosmos, .freezes = {{0, "DSCF0566_maska_obszary", .links = {{{1.0, 0.0, 0.0}, .callback = KosmosSowka, .audio = {MUSIC, "kosmos_metrograph3", .volume = 2.0}}, {{0.0, 1.0, 0.0}, .callback = KosmosRudnik, .audio = {MUSIC, "kosmos_metrograph3", .volume = 2.0}}}}}}, //
	{"drzwi_zamykaja_sie_same", .audio = {MUSIC, "koniec_lapis"}, .bg = "kosmos"}, //
	{"okna_sie_otwieraja_z_sowka2", .bg = "kosmos", .repeats = 1}, //
	{"sowka2_zaluzje_nie_znika_TAK", .repeats = 1}, //
	{"sowka1_zaluzje"}, //
	{"drzwi_z_zewnatrz"}, //
	{"animacja_koncowa", .bg = "kosmos", .sounds = {{18, {SOUND, "pac", .volume = 0.2}}}}, //
	{">myszka"}, //
	{">blank", .audio = {STOP_SOUND, "napisy_metrograph"}}, //
	{"animacje_koncowe_rodzinki", .audio = {SOUND, "napisy_metrograph"}, .callback = Credits, .draw = DrawCredits, .speed = 0.3075}, //
	{">blank", .audio = {STOP_SOUND, "napisy_metrograph"}}, //
	{"donice_13_tasma", .speed = 0.5, .freezes = {{0, "donice_w_ogrodzie_maski", .audio = {SOUND, "pudelko3"}, .links = {{{0.0, 1.0, 0.0}, .ignore = true}}}}}, //
};

void RequestPack(struct Game* game, int sceneid) {
	int last_pack = sceneid;
	if (last_pack < 0) {
		last_pack = 0;
	}
	if ((size_t)last_pack >= sizeof(SCENES) / sizeof(struct SceneDefinition)) {
		last_pack = sizeof(SCENES) / sizeof(struct SceneDefinition) - 1;
	}
	while (SCENES[last_pack].pack == 0 && last_pack > 0) {
		last_pack--;
	}
	game->data->download.requested = SCENES[last_pack].pack;
}

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

	int sceneid = game->data->sceneid;
	do {
		sceneid++;
		if ((size_t)sceneid >= sizeof(SCENES) / sizeof(struct SceneDefinition)) {
			return false;
		}
	} while (!SCENES[sceneid].name);

	RequestPack(game, sceneid);

	if (!game->data->download.started[game->data->download.requested]) {
		Enqueue(game, (struct SceneDefinition){">downloader"});
		return Dispatch(game);
	}

	game->data->sceneid = sceneid;
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

void StartInitialGamestate(struct Game* game, bool show_menu) {
	if (game->data->sceneid < 0) {
		game->data->sceneid = -1;
	}
	if (game->data->sceneid >= (int)((sizeof(SCENES) / sizeof(struct SceneDefinition)) - 1)) {
		game->data->sceneid = sizeof(SCENES) / sizeof(struct SceneDefinition) - 2;
	}
	while (!SCENES[game->data->sceneid + 1].checkpoint) {
		if (game->data->sceneid < 0) {
			break;
		}
		game->data->sceneid--;
	}
	if (show_menu && game->data->sceneid > 0) {
		game->data->menu_requested = true;
	}
	if (SCENES[game->data->sceneid + 1].name[0] == '>') {
		StartGamestate(game, SCENES[game->data->sceneid + 1].name + 1);
		Dispatch(game);
		return;
	}
	if (game->data->sceneid == -1) {
		Enqueue(game, (struct SceneDefinition){"kostki_animacja01_tworzy_sie_kostka", .bg = "ekran_startowy_tlo_przyciete"});
	}
	StartGamestate(game, "anim");
}

void Enqueue(struct Game* game, struct SceneDefinition scene) {
	PrintConsole(game, "Enqueue: %s", scene.name);
	game->data->queue[game->data->queue_pos] = scene;
	game->data->queue_pos++;
}

void DrawSceneToolbox(struct Game* game) {
#ifdef LIBSUPERDERPY_IMGUI
	if (!game->data->toolbox) {
		return;
	}

	igSetNextWindowSize((ImVec2){1024, 700}, ImGuiCond_FirstUseEver);
	igBegin("Scene Selector", NULL, 0);
	igInputText("", game->data->search, 255, ImGuiInputTextFlags_AutoSelectAll, NULL, NULL);
	for (unsigned int i = 0; i < sizeof(SCENES) / sizeof(struct SceneDefinition); i++) {
		if (game->data->search[0]) {
			if (!strstr(SCENES[i].name, game->data->search)) {
				continue;
			}
		}
		char name[255];
		snprintf(name, 255, "%d: %s", i, SCENES[i].name);
		if (igSelectable(name, game->data->sceneid == (int)i, 0, (ImVec2){0, 0})) {
			StopLoops(game);
			StopMusic(game);
			StopSounds(game);
			game->data->sceneid = i;
			game->data->scene = SCENES[i];
			game->data->force_anim_reload = true;
			if (game->data->scene.name[0] != '>') {
				game->data->sceneid--;
				ChangeCurrentGamestate(game, "anim");
			} else {
				ChangeCurrentGamestate(game, game->data->scene.name + 1);
			}
		}
	}
	igEnd();
#endif
}
