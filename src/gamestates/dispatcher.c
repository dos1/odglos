/*! \file example.c
 *  \brief Example gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	bool _unused;
};

int Gamestate_ProgressCount = 1;

static char* ANIMATIONS[] = {
	"001_sowka_na_kocyku/sowka_na_kocyku",
	"002_wedrowka_rodzinki_po_trawce/wedrowka_rodzinki_po_trawce",
	"003_rodzinka_jak_wiewiorki/rodzinka_jak_wiewiorki",
	"004_sowka_wchodzi_na_drzewo/sowka_wchodzi_na_drzewo",
	"005_sowka_pokazuje_mordke_i_wraca/sowka_pokazuje_mordke_i_wraca",
	"006_dzwonki_w_lesie/anim",
	"008_kuzyn_w_lesie/kuzyn_w_lesie",
	"009_sowka_idzie_w_lesie/sowka_idzie_w_lesie",
	"010_sowka_na_trawie/sowka_na_trawie",
	"011_buty_drewniane/buty_drewniane",
	"012_sowka_konfrontacja_z_rzezba/sowka_konfrontacja_z_rzezba",
	"013_wrzosy_kuzyn_i_sowka/wrzosy_kuzyn_i_sowka2",
	"014_samochody_w_lesie/samochody_w_lesie",
	"015_ciemna_trawa_samochod_sowka/ciemna_trawa_samochod_sowka",
	NULL, /* 016_pudelka_od_cioci */
	"017_ciemna_trawa_waz/ciemna_trawa_waz",
	"018_wiklinowe_kolo/wiklinowe_kolo",
	"019_aksamitki/aksamitki",
	"020_wchodzenie_po_schodach/wchodzenie_po_schodach",
	"021_donice_w_ogrodzie_stare/donice_w_ogrodzie_stare_samochod_waz",
	"022_samochod_waz_kominek/samochod_waz_kominek",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/024_donice_w_ogrodzie1",
	"025_silacz1/animacja_silacz1",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/026_donice_w_ogrodzie2",
	"027_silacz2_maly_samochod_z_sowka_opuszcz_cien_rozny/anim",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/028_donice_w_ogrodzie3",
	"029_silacz3_maly_samochod_sam/anim",
	"030a_rzezby_w_lazience/rzezby_w_lazience",
	"030b_sowka_i_rzezby_animacja/sowka_i_rzezby",
	"031_058_donice_dom/031_donice_dom1",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/032_donice_w_ogrodzie4",
	"033_gawron_i_drewniany_medrzec/gawron_i_drewniany_medrzec",
	"034_plansza_do_sudoku_pionki/plansza_do_sudoku_pionki",
	"035_038_donice_na_tarasie/035_donice_na_tarasie1",
	"036_039_048_donica_w_hortensjach/036_donica_w_hortensjach1",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/037_donice_w_ogrodzie5",
	"035_038_donice_na_tarasie/038_donice_na_tarasie2",
	"036_039_048_donica_w_hortensjach/039_donica_w_hortensjach2",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/040_donice_w_ogrodzie6",
	"041_049_duza_donica/041_duza_donica1",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/042_donice_w_ogrodzie7",
	"043_sowka_transformuje_w_wiklinie/anim",
	"044_sowka_wchodzi_do_miski_ciemniejsze/anim",
	NULL, /* 045_pergola */
	"045_pergola/pergola_animacja_koncowa/animacja_koncowa2",
	"045_pergola/pergola_animacja_koncowa/animacja_koncowa6",
	"046_pudelko_w_ogrodzie/pudelko_w_ogrodzie",
	"047_portal_ze_stolika_bialego/portal_ze_stolika_bialego",
	"036_039_048_donica_w_hortensjach/048_donica_w_hortensjach3",
	"041_049_duza_donica/049_duza_donica2",
	"050_sowki_waz_w_lisciach_przy_domu/sowki_waz_w_lisciach_przy_domu",
	"050a_kuzyn_zaglada_z_duzej_donicy/kuzyn_zaglada_z_duzej_donicy",
	"051_sowka_i_male_fortepiany/sowka_mini_lego_fortepiany",
	"052_turkusowe_cos/turkusowe_cos",
	"053_male_dziwne_cos/male_dziwne_cos",
	"054_lira_korbowa/lira_korbowa",
	"055_skrzypce2/skrzypce2",
	"056_swiecznik_test/anim",
	"057_szczypczyki_testowe/anim",
	"031_058_donice_dom/058_donice_dom2",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/059_donice_w_ogrodzie8",
	"060_magnetofon/magnetofon2_bez_myszek",
};

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	do {
		game->data->animationid++;
		if (game->data->animationid == sizeof(ANIMATIONS) / sizeof(char*)) {
			game->data->animationid = 0;
		}
	} while (!ANIMATIONS[game->data->animationid]);
	game->data->animation = ANIMATIONS[game->data->animationid];
	UnloadGamestate(game, "anim");
	StopCurrentGamestate(game);
	StartGamestate(game, "anim");
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}
