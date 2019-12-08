#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

static bool Generator(struct Game* game, int frame, int* x, int* y, struct Character* character, void** data) {
	*x = 142;
	*y = 136;
	return false;
}

static bool DuchPortalu(struct Game* game, int frame, int* x, int* y, struct Character* character, void** data) {
	*x = 0;
	*y = -222;
	return false;
}

static bool RegalDmuchawa(struct Game* game, int frame, int* x, int* y, struct Character* character, void** data) {
	*x = 550;
	*y = 60 - cos(frame / 4.0) * 10;
	SetCharacterPosition(game, character, frame * 30 - 800, 68, 0);
	return false;
}

static bool Zakochani(struct Game* game, struct Character* character, void** data) {
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02a_zakochani", .freezes = {{8, "DSCF7440_maska2_z_zakochana_para", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02b_muzykanci"});

	return true;
}

static bool Muzykanci(struct Game* game, struct Character* character, void** data) {
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02b_muzykanci", .freezes = {{26, "DSCF7440_maska2_z_zakochana_para", .links = {{{0.0, 1.0, 0.0}, .ignore = true}}}}});
	Enqueue(game, (struct SceneDefinition){"sowka_i_rzezby_02a_zakochani"});

	return true;
}

static bool Donice(struct Game* game, int frame, int* x, int* y, struct Character* character, void** data) {
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

	return true;
}

static bool DonicaPrawa(struct Game* game, struct Character* character, void** data) {
	unsigned int* left = *data;
	unsigned int* right = left + 1;

	struct SceneDefinition anim = anims_donica_right[*right];
	(*right)++;

	DonicaSetup(game, &anim, data);
	Enqueue(game, anim);

	return true;
}

static bool Dzwieki(struct Game* game, int frame, int* x, int* y, struct Character* character, void** data) {
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

	if (!(*d && *(d + 1) && *(d + 2))) {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek3", .freezes = {{25, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}, .callback_data = *data});
	} else {
		Enqueue(game, (struct SceneDefinition){"04statki_szyszki_tasmy_gra_dzwiek3"});
		free(*data);
		*data = NULL;
	}

	return true;
}

static bool Ul(struct Game* game, int frame, int* x, int* y, struct Character* character, void** data) {
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

static struct SceneDefinition SCENES[] = {
	//{"sucha_trawa_aksamitki_waz_stary"},
	{"wedrowka_rodzinki_po_trawce"},
	{"rodzinka_jak_wiewiorki", .freezes = {{20, "DSCF8146_maska"}}},
	{"sowka_wchodzi_na_drzewo"},
	{"sowka_pokazuje_mordke_i_wraca"},
	{"buty_drewniane"},
	{"regal_animacja_sam", .freezes = {{0, ""}}, .repeats = 1, .bg = "regal_dmuchawa_100_9254_tlo_przyciete", .callback = RegalDmuchawa, .character = {"dmuchawa", {"dmuchawa_ptaszor_sam"}, .repeat = true}},
	{"generator_animacja_wstepna", .freezes = {{0, ""}}, .callback = Generator, .bg = "generator_tlo_liscie_przyciete"},
	{"sucha_trawa_aksamitki_samochod_stary"},
	//{"sowka_idzie_w_lesie"},
	//{"kuzyn_w_lesie"},
	{"sowka_konfrontacja_z_rzezba", .freezes = {{0, "DSCF8160_maska"}}},
	{"wrzosy_kuzyn_i_sowka2"},
	{"samochod_kominek"},
	//{">byk"},
	{"swiecznik3_TAK", .freezes = {{0, "DSCF2296_maska_caly"}}},
	//{">swiecznik"},
	{"samochody_w_lesie", .freezes = {{0, "DSCF2433_maska"}}},
	{"aksamitki_samochod_sowka", .speed = 1.25},

	{"donice_02_samochod_duzy_jedzie_w_lewo", .speed = 0.75},
	{"donice_10_sowka_srednia_wjezdza_do_donicy_z_prawej", .freezes = {{15, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}},
	{"donice_01_samochod_duzy_jedzie_w_prawo"},
	{"donice_08_mala_sowka_z_samochodem_wyjezdza_w_przod"},

	{"przyciski_na_stacji_przycisk1", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk1_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk1_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk2", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk2_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk2_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk3", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk3_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk3_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_kominie_przycisk_1_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_1_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_1_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_2_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_2_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_2_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_3_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_3_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_3_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"gawron_i_drewniany_medrzec", .freezes = {{107, "DSCF2982_maska"}, {290, "DSCF3781_maska"}}},
	//{">skrzypce"},
	{"ciemna_trawa_samochod_sowka", .speed = 1.25},
	{"ciemna_trawa_waz_r", .speed = 1.25},
	{"wchodzenie_po_schodach_samochod_sowka", .freezes = {{19, "maska_schodek1"}, {23, "maska_schodek2"}, {26, "maska_schodek3"}, {29, "maska_schodek4"}}},
	{"schodzenie_ze_schodow_waz"},
	{"lira_korbowa", .freezes = {{0, "DSCF8976_maska"}}}, {"male_dziwne_cos", .freezes = {{0, "DSCF8646_maska"}}, .repeats = 1}, {"turkusowe_cos", .freezes = {{0, "DSCF9030_maska"}}},
	//{">dzwonki"},
	{"animacja_silacz1", .freezes = {{0, "silacz_maska"}, {22, "silacz_maska"}, {46, "silacz_maska"}}},

	//{"donice_02_samochod_duzy_jedzie_w_lewo", .speed = 0.75},

	{"donice_14_samochod_nadjezdza_z_prawej_i_wjezdza_do_donicy_z_lewej", .callback = Donice, .freezes = {{16, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .callback = DonicaLewa}, {{0.0, 1.0, 0.0}, .callback = DonicaPrawa}}}}},
	{"donice_12_waz_idzie_w_prawo_i_wchodzi_do_prawej_donicy"},

	{"silacz2_maly_samochod_z_sowka_opuszcz_cien_rozny", .freezes = {{0, "silacz_maska"}}},
	{"silacz3_maly_samochod_sam"}, //.freezes = {{0, "silacz_maska"}}},
	{">lawka"},
	{"rzezby_w_lazience_2_wyciszenie_sznureczka"},
	{"sowka_i_rzezby_01_sowka_przejezdza", .freezes = {{18, "DSCF7440_maska2_z_zakochana_para", .links = {{{1.0, 0.0, 0.0}, .callback = Zakochani}, {{0.0, 1.0, 0.0}, .callback = Muzykanci}}}}},
	{"031_donice_dom1"},
	{"donice_16_samochod_kartonowy_duzy_wjezdza_z_prawej"},
	{"donice_22_sowka_srednia_whodzi_do_duzej_donicy_z_lewej", .freezes = {{7, "donice_w_ogrodzie_maski", .links = {{{1.0, 0.0, 0.0}, .ignore = true}}}}},
	//{"donice_07_sowka_srednia_wjezdza_do_donicy_z_lewej"},
	{"donice_15_maly_samochodzik_kartonowy_wyjezdza"},
	{"donice_23_sowka_mala_wychodzi_w_przod"},
	//{"donice_20_samochod_maly_bez_sowki_wjezdza_z_prawej"},
	{"donice_24_sowka_mala_wchodzi_z_prawej"},
	{"donice_25_sowka_srednia_wychodzi_z_lewej_donicy"},
	{"donice_26_sowka_srednia_wchodzi_do_prawej_donicy"},
	{"donice_27_sowka_duza_wychodzi_z_lewej_donicy"},
	//{"donice_28_sowka_duza_wchodzi_do_lewej_donicy"},
	//{"donice_29_sowka_srednia_wychodzi_z_prawej_donicy"},
	{"donice_11_waz_buszuje_w_prawo_w_lewo_i_wchodzi_z_lewej"},

	{"aksamitki_waz_r", .speed = 1.25},
	//{"donica_w_hortensjach_01_donica_sie_pojawia", .freezes = {{10, "donica_w_hortensjach_maska"}}},
	{"donica_w_hortensjach_06_waz", .freezes = {{10, "donica_w_hortensjach_maska"}}},
	{"waz_zmienia_sie_w_kostke", .freezes = {{14, "IMG_0770_maska"}}},
	{">pudelka"},
	{"portal_ze_stolika_bialego", .freezes = {{9, "DSCF8382_maska"}, {14, "DSCF8387_maska"}}},
	{"ul_duzy_pusty_mozna_dac_tez_sama_pierwsza_klatke", .callback = Ul, .freezes = {{0, "IMG_0053_maska", .links = {{{0.0, 1.0, 0.0}, .callback = UlLewo}, {{1.0, 0.0, 0.0}, .callback = UlGora}, {{0.0, 0.0, 1.0}, .callback = UlDol}}}}},
	{"ul_duzy_animacja_koncowa_samochod"},
	{"pudelko_w_ogrodzie", .freezes = {{22, "pudelko_w_ogrodzie_maska1"}, {56, "pudelko_w_ogrodzie_maska3"}, {93, "pudelko_w_ogrodzie_maska2"}, {119, "pudelko_w_ogrodzie_maska3"}, {157, "pudelko_w_ogrodzie_maska2"}, {183, "pudelko_w_ogrodzie_maska3"}}},
	{"siatka_na_drzewie_myszka"},
	{"drzewko_kolorowe1_maskotki_podwojne_moze_lepsze_TAK"},
	{"sowka_wchodzi_do_miski_ciemniejsze", .freezes = {{0, "DSCF1595_maska"}}},
	{"duza_sowka_na_drewnianym_kole", .freezes = {{13, "IMG_1010_maska"}}},
	{">pergola"},
	{"pergola_animacja_koncowa2"},
	{"pergola_animacja_koncowa6"},
	//{"stolik_animacja", .freezes = {{0, ""}}, .bg = "stolik_tlo", .fg = "stolik_drzewa_na_przedzie"},
	{"pudelko_wypluwa_szczypczyki_smok_bez_dyn_TAK", .freezes = {{0, "DSCF5025_maska"}}},
	{">naparstki"},
	{"01statki_szyszki_tasmy_animacja1"},
	{"02statki_szyszki_tasmy_animacja2"},
	{"03statki_szyszki_tasmy_animacja3", .callback = Dzwieki, .freezes = {{40, "DSCF4234_maska", .links = {{{0.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek1}, {{10.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek2}, {{20.0 / 255.0, 0.0, 0.0}, .callback = Dzwiek3}}}}},
	{"05statki_szyszki_tasmy_animacja4", .freezes = {{69, "DSCF4999_maska"}}},
	{"06statki_szyszki_tasmy_animacja5"},
	{"magnetofon2_bez_myszek", .freezes = {{0, "DSCF9467_maska_magnetofon"}}},
	//{">duch_portalu"},
	//{"duch_portalu_animacja_oczu_osobno_lewe_TAK_DO_GRY", .callback = DuchPortalu},
	//{"duch_portalu_animacja_oczu_osobno_prawe_TAK_DO_GRY", .callback = DuchPortalu},
	{"duch_portalu_animacja2_zlozona_TAK", .callback = DuchPortalu, .freezes = {{0, "duch_portalu_maska_przycieta"}}},
	{">armata"},
	{"podniebny_generator_z_kosmosem", .freezes = {{0, "podniebny_generator_z_kosmosem00_maska"}}},
	{"makieta_w_kosmosie_bez_tla", .bg = "kosmos"},
	{"makieta_pusta"},
	{"krzeslo_w_lesie_czesc1", .freezes = {{12, "krzeslo_w_lesie08_maska"}}},
	{"krzeslo_w_lesie_czesc2"},
	{"sowka1_wchodzi_na_stol_z_bliska_pojawia_sie_TAK"}, // fade?
	{"sowka2_zaluzje_pojawia_sie2_TAK", .speed = 0.15}, // fade?
	{"sowki_zamieniaja_sie_krzeslami_po_dwa_i_nie_znikaja_TAK", .freezes = {{85, "DSCF0566_maska_obszary"}}},
	{"sowka1_wlacza_konsole_z_daleka2", .bg = "kosmos"},
	{"sowka1_wlacza_konsole_z_bliska_lewa_konsola", .freezes = {{0, "DSCF0067_maska_ze_stolem"}}},
	{"sowka1_wlacza_konsole_z_bliska_srodkowa_konsola", .freezes = {{0, "DSCF0067_maska_ze_stolem"}}},
	{"sowka1_zaluzje"},
	{"sowka1_wchodzi_na_stol_z_bliska_nie_znika_TAK"},
	{"drzwi_zamykaja_sie_same", .bg = "kosmos"},
	{"sowka2_klika_konsole_prawa", .freezes = {{0, "DSCF0286_maska"}}},
	{"sowka2_klika_konsole_lewa", .freezes = {{0, "DSCF0286_maska"}}},
	{"sowka2_zaluzje_nie_znika_TAK"},
	{"okna_sie_otwieraja_z_sowka2", .bg = "kosmos"},
	{"buzia_01_bez_niczego"},
	{"buzia_02_sowa"},
	{"buzia_03_kuzyn"},
	{"buzia_04_myszka"},
	{"zamiana_myszki_w_bramie"},
	{"altanka_samochod"},
	{"wiklinowy_cyrk_po_dwa_bez_myszki"},
	{"wiklinowy_cyrk_sama_myszka"},
	//{"myszkowanie_w_wiklinie"},
	{"wiklinowe_kolo1_samochod"},
	{"wiklinowe_kolo2_pilka"},
	{"wiklinowe_kolo3_myszka"},
	{"animacja_koncowa", .bg = "kosmos"},
	{"animacje_koncowe_rodzinki"},
	{"donice_13_tasma"},
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
	return true;
}

void Enqueue(struct Game* game, struct SceneDefinition scene) {
	PrintConsole(game, "Enqueue: %s", scene.name);
	game->data->queue[game->data->queue_pos] = scene;
	game->data->queue_pos++;
}
