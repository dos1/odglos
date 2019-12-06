#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

#ifndef __SWITCH__
#include <sys/mman.h>
#endif
#include <sys/stat.h>

#include <webp/demux.h>

struct AnimationDecoder {
	WebPData data;
	int timestamp, position, duration, swap_duration, old_timestamp;
	WebPAnimDecoder* decoder;
	ALLEGRO_BITMAP *bitmap, *swap;
	int fd;
	uint8_t* buf;
	bool mmaped, shouldload;
	int frame;
	bool done;
	bool initialized;
	int width, height;
	int frames;
	char* name;
	bool repeat;
	struct Game* game;
};

static void Generator(struct Game* game, int frame, int* x, int* y, struct Character* character) {
	*x = 142;
	*y = 136;
}

static void DuchPortalu(struct Game* game, int frame, int* x, int* y, struct Character* character) {
	*x = 0;
	*y = -222;
}

static void RegalDmuchawa(struct Game* game, int frame, int* x, int* y, struct Character* character) {
	*x = 550;
	*y = 60 - cos(frame / 4.0) * 10;
	SetCharacterPosition(game, character, frame * 30 - 800, 68, 0);
}

static struct SceneDefinition SCENES[] = {
	//{"sucha_trawa_aksamitki_waz_stary"},
	{"wedrowka_rodzinki_po_trawce"},
	{"rodzinka_jak_wiewiorki", .freezes = {{20, "DSCF8146_maska"}}},
	{"sowka_wchodzi_na_drzewo"},
	{"sowka_pokazuje_mordke_i_wraca"},
	{"sucha_trawa_aksamitki_samochod_stary"},
	//{"sowka_idzie_w_lesie"},
	//{"kuzyn_w_lesie"},
	{"sowka_konfrontacja_z_rzezba", .freezes = {{0, "DSCF8160_maska"}}},
	{"wrzosy_kuzyn_i_sowka2"},
	{"samochod_kominek"},
	{"samochody_w_lesie", .freezes = {{0, "DSCF2433_maska"}}},
	{"buty_drewniane"},
	{"regal_animacja_sam", .freezes = {{0, ""}}, .repeats = 1, .bg = "regal_dmuchawa_100_9254_tlo_przyciete", .callback = RegalDmuchawa, .character = {"dmuchawa", "dmuchawa_ptaszor_sam"}},
	{"aksamitki_samochod_sowka"},
	{"ciemna_trawa_samochod_sowka"},
	{"wchodzenie_po_schodach_samochod_sowka", .freezes = {{19, "maska_schodek1"}, {23, "maska_schodek2"}, {26, "maska_schodek3"}, {29, "maska_schodek4"}}},
	{"generator_animacja_wstepna", .freezes = {{0, ""}}, .callback = Generator, .bg = "generator_tlo_liscie_przyciete"},
	{"donice_01_samochod_duzy_jedzie_w_prawo"},
	{"donice_10_sowka_srednia_wjezdza_do_donicy_z_prawej"},
	{"donice_08_mala_sowka_z_samochodem_wyjezdza_w_przod"},
	{"swiecznik3_TAK", .freezes = {{0, "DSCF2296_maska_caly"}}},
	//{">swiecznik"},
	{"gawron_i_drewniany_medrzec", .freezes = {{107, "DSCF2982_maska"}, {290, "DSCF3781_maska"}}},
	{"schodzenie_ze_schodow_waz"},
	{"ciemna_trawa_waz_r"},
	{"aksamitki_waz_r"},
	{"donica_w_hortensjach_06_waz"},
	//{">dzwonki"},
	{"stolik_animacja", .freezes = {{0, ""}}, .bg = "stolik_tlo", .fg = "stolik_drzewa_na_przedzie"},
	{"animacja_silacz1", .freezes = {{0, "silacz_maska"}, {22, "silacz_maska"}, {46, "silacz_maska"}}},
	{"donice_02_samochod_duzy_jedzie_w_lewo"},
	{"donice_03_samochod_duzy_wjezdza_do_donicy_z_lewej"},
	{"donice_06_samochod_maly_z_sowka_wyjezdza_w_przod"},
	{"silacz2_maly_samochod_z_sowka_opuszcz_cien_rozny", .freezes = {{0, "silacz_maska"}}},
	{"donice_19_samochod_maly_z_sowka_wjezdza_z_prawej"},
	{"donice_05_samochod_maly_bez_sowki_wyjezdza_w_przod"},
	{"donice_04_samochod_maly_z_sowka_wyjezdza_w_prawo"},
	{"donice_18_samochod_duzy_wyjezdza_w_prawo"},
	{"silacz3_maly_samochod_sam"}, //.freezes = {{0, "silacz_maska"}}},
	{"lira_korbowa", .freezes = {{0, "DSCF8976_maska"}}},
	{"siatka_na_drzewie_myszka"},
	{"rzezby_w_lazience_2_wyciszenie_sznureczka"},
	{"sowka_i_rzezby_2_krotsze", .freezes = {{18, "DSCF7440_maska"}}},
	{"031_donice_dom1"},
	{"turkusowe_cos", .freezes = {{0, "DSCF9030_maska"}}},
	//{"ule_jesienne"},
	{"donice_07_sowka_srednia_wjezdza_do_donicy_z_lewej"},
	{"donice_15_maly_samochodzik_kartonowy_wyjezdza"},
	{"donice_23_sowka_mala_wychodzi_w_przod"},
	{"donice_20_samochod_maly_bez_sowki_wjezdza_z_prawej"},
	{"donice_17_samochod_duzy_wyjezdza_w_lewo"},
	{"donice_24_sowka_mala_wchodzi_z_prawej"},
	{"donice_25_sowka_srednia_wychodzi_z_lewej_donicy"},
	{"donice_26_sowka_srednia_wchodzi_do_prawej_donicy"},
	{"donice_27_sowka_duza_wychodzi_z_lewej_donicy"},
	{"donice_28_sowka_duza_wchodzi_do_lewej_donicy"},
	{"donice_29_sowka_srednia_wychodzi_z_prawej_donicy"},
	{"donice_11_waz_buszuje_w_prawo_w_lewo_i_wchodzi_z_lewej"},
	{"donice_12_waz_idzie_w_prawo_i_wchodzi_do_prawej_donicy"},
	{"donice_21_sowka_rudnik_wychodzi_w_prawo"},
	{"male_dziwne_cos", .freezes = {{0, "DSCF8646_maska"}}},
	{"ul_duzy_pusty_mozna_dac_tez_sama_pierwsza_klatke", .freezes = {{0, "IMG_0053_maska"}}},
	{"ul_duzy_lewo"},
	{"ul_duzy_gora"},
	{"ul_duzy_dol"},
	{"ul_duzy_animacja_koncowa_samochod"},
	{"duza_sowka_na_drewnianym_kole", .freezes = {{13, "IMG_1010_maska"}}},
	{">pergola"},
	{"pergola_animacja_koncowa2"},
	{"pergola_animacja_koncowa6"},
	{"sowka_wchodzi_do_miski_ciemniejsze", .freezes = {{0, "DSCF1595_maska"}}},
	{"donica_w_hortensjach_05_samochod_z_sowka_wjezdza"},
	{"donica_w_hortensjach_03_sowka_srednia_wychodzi"},
	{"donica_w_hortensjach_02_sowka_srednia_wchodzi"},
	{"donica_w_hortensjach_04_samochod_z_sowka_wyjezdza"},
	{"donica_w_hortensjach_06_waz"},
	{">pudelka"},
	{"waz_zmienia_sie_w_kostke", .freezes = {{14, "IMG_0770_maska"}}},
	{">lawka"},
	{"drzewko_kolorowe1_maskotki_podwojne_moze_lepsze_TAK"},
	//{"czerwona_sciana_z_winorosli"},
	{"pudelko_wypluwa_szczypczyki_smok_bez_dyn_TAK", .freezes = {{0, "DSCF5025_maska"}}},
	{">naparstki"},
	{"01statki_szyszki_tasmy_animacja1"},
	{"02statki_szyszki_tasmy_animacja2"},
	{"03statki_szyszki_tasmy_animacja3"},
	{"04statki_szyszki_tasmy_gra_dzwiek1", .freezes = {{0, "DSCF4234_maska"}}},
	{"04statki_szyszki_tasmy_gra_dzwiek2", .freezes = {{0, "DSCF4234_maska"}}},
	{"04statki_szyszki_tasmy_gra_dzwiek3", .freezes = {{0, "DSCF4234_maska"}}},
	{"05statki_szyszki_tasmy_animacja4", .freezes = {{69, "DSCF4999_maska"}}},
	{"06statki_szyszki_tasmy_animacja5"},
	{">armata"},
	{"podniebny_generator_z_kosmosem", .freezes = {{0, "podniebny_generator_z_kosmosem00_maska"}}},
	{"makieta_w_kosmosie_bez_tla", .bg = "kosmos"},
	{"makieta_pusta"},
	{"magnetofon2_bez_myszek", .freezes = {{0, "DSCF9467_maska_magnetofon"}}},
	//{">duch_portalu"},
	//{"duch_portalu_animacja_oczu_osobno_lewe_TAK_DO_GRY", .callback = DuchPortalu},
	//{"duch_portalu_animacja_oczu_osobno_prawe_TAK_DO_GRY", .callback = DuchPortalu},
	{"duch_portalu_animacja2_zlozona_TAK", .callback = DuchPortalu, .freezes = {{0, "duch_portalu_maska_przycieta"}}},
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
	{"pudelko_w_ogrodzie", .freezes = {{22, "pudelko_w_ogrodzie_maska1"}, {56, "pudelko_w_ogrodzie_maska3"}, {93, "pudelko_w_ogrodzie_maska2"}, {119, "pudelko_w_ogrodzie_maska3"}, {157, "pudelko_w_ogrodzie_maska2"}, {183, "pudelko_w_ogrodzie_maska3"}}},
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
	{"portal_ze_stolika_bialego"},
	//{">skrzypce"},
	//{">byk"},

	{"przyciski_na_kominie_przycisk_1_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_1_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_1_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_2_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_2_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_2_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_3_calosc", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_3_samo_wlaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_kominie_przycisk_3_samo_wylaczanie", .bg = "przyciski_na_kominie_tlo", .fg = "przyciski_na_kominie_warstwa_wierzchnia"},
	{"przyciski_na_stacji_przycisk1", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk1_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk1_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk2", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk2_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk2_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk3", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk3_samo_wlaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},
	{"przyciski_na_stacji_przycisk3_samo_wylaczenie", .bg = "przyciski_na_stacji_tlo", .fg = "przyciski_na_stacji_wierzch"},

};

struct AnimationDecoder* CreateAnimation(struct Game* game, const char* filename, bool repeat) {
	struct AnimationDecoder* anim = calloc(1, sizeof(struct AnimationDecoder));

	WebPDataInit(&anim->data);

	anim->game = game;

	anim->fd = open(filename, O_RDONLY | O_CLOEXEC);

	if (!anim->fd) {
		free(anim);
		return NULL;
	}

	struct stat s;
	fstat(anim->fd, &s);
	anim->data.size = s.st_size;

	anim->buf = NULL;
#ifndef __SWITCH__
	anim->buf = mmap(0, anim->data.size, PROT_READ, MAP_SHARED, anim->fd, 0);
#endif

	if (!anim->buf) {
		anim->buf = malloc(anim->data.size * sizeof(uint8_t));

		if (read(anim->fd, anim->buf, anim->data.size) != (ssize_t)anim->data.size) {
			free(anim->buf);
			close(anim->fd);
			free(anim);
			return NULL;
		}
		close(anim->fd);
		anim->mmaped = false;
	} else {
		anim->mmaped = true;
	}
	anim->data.bytes = anim->buf;

	// TODO: handle invalid file path

	WebPAnimDecoderOptions dec_options;
	WebPAnimDecoderOptionsInit(&dec_options);
	dec_options.color_mode = MODE_rgbA;
	dec_options.use_threads = !IS_EMSCRIPTEN;

	anim->decoder = WebPAnimDecoderNew(&anim->data, &dec_options);

	WebPAnimInfo anim_info;
	WebPAnimDecoderGetInfo(anim->decoder, &anim_info);

	anim->position = -1;
	anim->frame = -1;
	anim->initialized = false;

	anim->width = anim_info.canvas_width;
	anim->height = anim_info.canvas_height;

	anim->frames = anim_info.frame_count;

	anim->repeat = repeat;

	ALLEGRO_PATH* path = al_create_path(filename);
	anim->name = strdup(al_get_path_basename(path));
	al_destroy_path(path);

	PrintConsole(anim->game, "[AnimationStream] Created: %s", anim->name);

	return anim;
}

void ResetAnimation(struct AnimationDecoder* anim) {
	if (anim->position == 0) {
		return;
	}
	if (!anim->initialized) {
		anim->bitmap = al_create_bitmap(anim->width, anim->height);
		anim->swap = al_create_bitmap(anim->width, anim->height);
		//PrintConsole(anim->game, "[AnimationStream] Initialized: %s", anim->name);
	}
	WebPAnimDecoderReset(anim->decoder);

	ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(anim->bitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
	uint8_t* buf;
	WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp);
	for (int i = 0; i < al_get_bitmap_height(anim->bitmap); i++) {
		memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->bitmap) * al_get_pixel_size(lock->format),
			al_get_bitmap_width(anim->bitmap) * lock->pixel_size);
	}
	al_unlock_bitmap(anim->bitmap);

	anim->duration = anim->timestamp;
	anim->old_timestamp = anim->timestamp;

	anim->shouldload = true;

	anim->position = 0;
	anim->frame = 0;
	anim->done = false;
	anim->initialized = true;
}

bool UpdateAnimation(struct AnimationDecoder* anim, float timestamp) {
	if (!anim->initialized) {
		ResetAnimation(anim);
	}

	anim->position += timestamp * 1000;
	ALLEGRO_LOCKED_REGION* lock = NULL;

	if (anim->shouldload) {
		anim->shouldload = false;
		if (!WebPAnimDecoderHasMoreFrames(anim->decoder)) {
			if (anim->repeat) {
				ResetAnimation(anim);
				return true;
			}
			anim->done = true;
			return false;
		}
		uint8_t* buf;
		if (WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp)) {
			lock = al_lock_bitmap(anim->swap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
			for (int i = 0; i < al_get_bitmap_height(anim->swap); i++) {
				memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->swap) * al_get_pixel_size(lock->format),
					al_get_bitmap_width(anim->swap) * lock->pixel_size);
			}
			al_unlock_bitmap(anim->swap);
			anim->swap_duration = anim->timestamp - anim->old_timestamp;
		}
	}

	if (anim->position >= anim->old_timestamp) {
		if (!anim->done) {
			ALLEGRO_BITMAP* bmp = anim->bitmap;
			anim->bitmap = anim->swap;
			anim->swap = bmp;
			anim->shouldload = true;
			anim->old_timestamp = anim->timestamp;
			anim->frame++;
			anim->duration = anim->swap_duration;
		}

		if (!anim->repeat && !WebPAnimDecoderHasMoreFrames(anim->decoder)) {
			return false;
		}
	}

	return true;
}

ALLEGRO_BITMAP* GetAnimationFrame(struct AnimationDecoder* anim) {
	if (!anim->initialized) {
		ResetAnimation(anim);
	}
	return anim->bitmap;
}

int GetAnimationFrameNo(struct AnimationDecoder* anim) {
	return anim->frame;
}

const char* GetAnimationName(struct AnimationDecoder* anim) {
	return anim->name;
}

int GetAnimationFrameCount(struct AnimationDecoder* anim) {
	return anim->frames;
}

float GetAnimationFrameDuration(struct AnimationDecoder* anim) {
	if (!anim->initialized) {
		ResetAnimation(anim);
	}
	return (float)anim->duration / 1000.0;
}

bool IsAnimationComplete(struct AnimationDecoder* anim) {
	return anim->done;
}

void DestroyAnimation(struct AnimationDecoder* anim) {
	//PrintConsole(anim->game, "[AnimationStream] Destroying: %s", anim->name);
#ifndef __SWITCH__
	if (anim->mmaped) {
		munmap(anim->buf, anim->data.size);
		close(anim->fd);
		anim->data.bytes = NULL;
	}
#endif
	WebPDataClear(&anim->data);
	WebPAnimDecoderDelete(anim->decoder);
	if (anim->initialized) {
		al_destroy_bitmap(anim->swap);
		al_destroy_bitmap(anim->bitmap);
	}
	free(anim->name);
	free(anim);
}

void SwitchScene(struct Game* game, char* name) {
	PrintConsole(game, "switching to %s", name);
	if (game->data->next) {
		free(game->data->next);
	}
	game->data->next = strdup(name);
	LoadGamestate(game, name);
	StopCurrentGamestate(game);
	if (game->data->lowmem) {
		UnloadCurrentGamestate(game);
	}
	StartGamestate(game, name);
	//StartGamestate(game, "myszka");
}

void PreLogic(struct Game* game, double delta) {
	game->data->hover = false;
}

ALLEGRO_COLOR CheckMask(struct Game* game, ALLEGRO_BITMAP* bitmap) {
	// TODO: apply distortion coming from compositor
	ALLEGRO_COLOR color = al_get_pixel(bitmap, (int)(game->data->mouseX * al_get_bitmap_width(bitmap)), (int)(game->data->mouseY * al_get_bitmap_height(bitmap)));
	game->data->hover = (color.r < 0.5) || (color.g < 0.5) || (color.b < 0.5);
	return color;
}

void DrawTexturedRectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color) {
	ALLEGRO_VERTEX vtx[4];
	int ii;

	vtx[0].x = x1;
	vtx[0].y = y1;
	vtx[0].u = 0.0;
	vtx[0].v = 0.0;
	vtx[1].x = x1;
	vtx[1].y = y2;
	vtx[1].u = 0.0;
	vtx[1].v = 1.0;
	vtx[2].x = x2;
	vtx[2].y = y2;
	vtx[2].u = 1.0;
	vtx[2].v = 1.0;
	vtx[3].x = x2;
	vtx[3].y = y1;
	vtx[3].u = 1.0;
	vtx[3].v = 0.0;

	for (ii = 0; ii < 4; ii++) {
		vtx[ii].color = color;
		vtx[ii].z = 0;
	}

	al_draw_prim(vtx, 0, 0, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
}

void Compositor(struct Game* game) {
	struct Gamestate* tmp = GetNextGamestate(game, NULL);
	ClearToColor(game, al_map_rgb(0, 0, 0));

	al_use_shader(game->data->grain);
	al_set_shader_float("time", al_get_time() - game->data->start_time);
	al_set_shader_bool("desaturate", game->data->pause);

	if (game->loading.shown) {
		al_draw_bitmap(GetGamestateFramebuffer(game, GetGamestate(game, NULL)), game->clip_rect.x, game->clip_rect.y, 0);
		al_use_shader(NULL);
		return;
	}

	bool drawn = false;
	while (tmp) {
		if (IsGamestateVisible(game, tmp)) {
			float randx = (rand() / (double)RAND_MAX) * 3.0 * game->clip_rect.w / 3200.0;
			float randy = (rand() / (double)RAND_MAX) * 3.0 * game->clip_rect.h / 1800.0;
			if (rand() % 200) {
				randx = 0;
				randy = 0;
			}

			float color = 1.0 + (rand() / (double)RAND_MAX) * 0.01 - 0.005;

			al_draw_tinted_scaled_rotated_bitmap(GetGamestateFramebuffer(game, tmp), al_map_rgba_f(color, color, color, color), 0, 0,
				game->clip_rect.x + randx, game->clip_rect.y + randy, game->clip_rect.w / (double)al_get_bitmap_width(GetGamestateFramebuffer(game, tmp)) * 1.01, game->clip_rect.h / (double)al_get_bitmap_height(GetGamestateFramebuffer(game, tmp)) * 1.01, 0.0, 0);
			drawn = true;
		}
		tmp = GetNextGamestate(game, tmp);
	}
	if (!drawn) {
		al_draw_filled_rectangle(game->clip_rect.x, game->clip_rect.y, game->clip_rect.w, game->clip_rect.h, al_map_rgb(0, 0, 0));
	}
	al_use_shader(NULL);

	if (game->data->pause) {
		al_draw_tinted_scaled_bitmap(game->data->gradient, al_map_rgba_f(0.5, 0.5, 0.5, 0.5),
			0, 0, al_get_bitmap_width(game->data->gradient), al_get_bitmap_height(game->data->gradient),
			game->clip_rect.x, game->clip_rect.y, game->clip_rect.w, game->clip_rect.h, 0);

		al_draw_text(game->data->font, al_map_rgb(255, 255, 255), game->clip_rect.x + game->clip_rect.w / 2.0, game->clip_rect.y + game->clip_rect.h / 2.0, ALLEGRO_ALIGN_CENTER, "PAUSED");
	}

	if (game->data->cursor) {
		al_draw_scaled_rotated_bitmap(game->data->hover ? game->data->cursorhover : game->data->cursorbmp, 9, 4, game->data->mouseX * game->clip_rect.w + game->clip_rect.x, game->data->mouseY * game->clip_rect.h + game->clip_rect.y, game->clip_rect.w / (double)game->viewport.width * 0.69 / LIBSUPERDERPY_IMAGE_SCALE, game->clip_rect.h / (double)game->viewport.height * 0.69 / LIBSUPERDERPY_IMAGE_SCALE, 0, 0);
	}
}

void DrawBuildInfo(struct Game* game) {
	SUPPRESS_WARNING("-Wdeprecated-declarations")
	int x, y, w, h;
	al_get_clipping_rectangle(&x, &y, &w, &h);
	al_hold_bitmap_drawing(true);
	DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), w - 10, h * 0.935, ALLEGRO_ALIGN_RIGHT, "ODGŁOS PREALPHA");
	char revs[255];
	snprintf(revs, 255, "%s-%s", LIBSUPERDERPY_GAME_GIT_REV, LIBSUPERDERPY_GIT_REV);
	DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), w - 10, h * 0.965, ALLEGRO_ALIGN_RIGHT, revs);

	if (game->_priv.showconsole && game->data->sceneid >= 0 && (size_t)game->data->sceneid < sizeof(SCENES) / sizeof(struct SceneDefinition)) {
		snprintf(revs, 255, "%s (%d)", SCENES[game->data->sceneid].name, game->data->debuginfo);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), 10, h * 0.965, ALLEGRO_ALIGN_LEFT, revs);
	}

	al_hold_bitmap_drawing(false);
	SUPPRESS_END
}

bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_M)) {
		ToggleMute(game);
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_F)) { // || (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)) {
		ToggleFullscreen(game);
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_P)) {
		if (!game->data->pause) {
			PauseAllGamestates(game);
		} else {
			ResumeAllGamestates(game);
		}
		game->data->pause = !game->data->pause;
	}

	if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
		game->data->mouseX = Clamp(0, 1, (ev->mouse.x - game->clip_rect.x) / (double)game->clip_rect.w);
		game->data->mouseY = Clamp(0, 1, (ev->mouse.y - game->clip_rect.y) / (double)game->clip_rect.h);
	}

	if (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN || ev->type == ALLEGRO_EVENT_TOUCH_MOVE) {
		game->data->mouseX = Clamp(0, 1, (ev->touch.x - game->clip_rect.x) / (double)game->clip_rect.w);
		game->data->mouseY = Clamp(0, 1, (ev->touch.y - game->clip_rect.y) / (double)game->clip_rect.h);
	}

#ifndef __EMSCRIPTEN__
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadAllGamestates(game);
	}
#endif

	if (game->_priv.showconsole && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		if (game->data->sceneid < -1) {
			game->data->sceneid = -1;
		}
	}

	return false;
}

void ShowMouse(struct Game* game) {
	game->data->cursor = true;
}

void HideMouse(struct Game* game) {
	game->data->cursor = false;
}

bool Dispatch(struct Game* game) {
	do {
		game->data->sceneid++;
		if ((size_t)game->data->sceneid >= sizeof(SCENES) / sizeof(struct SceneDefinition)) {
			return false;
		}
	} while (!SCENES[game->data->sceneid].name);
	game->data->scene = &SCENES[game->data->sceneid];
	PrintConsole(game, "Dispatch: %s", game->data->scene->name);
	return true;
}

struct CommonResources* CreateGameData(struct Game* game) {
	struct CommonResources* data = calloc(1, sizeof(struct CommonResources));
	data->grain = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/grain.glsl"));
	data->mouseX = -1;
	data->mouseY = -1;
	data->cursor = false;
	data->cursorbmp = al_load_bitmap(GetDataFilePath(game, "kursor_standby.png"));
	data->cursorhover = al_load_bitmap(GetDataFilePath(game, "kursor_hover.png"));
	data->start_time = al_get_time();
	data->lowmem = false;
	data->sceneid = -1;
	data->pause = false;
	data->font = al_load_font(GetDataFilePath(game, "fonts/DejaVuSansMono.ttf"), 42, 0);
	data->gradient = al_load_bitmap(GetDataFilePath(game, "gradient.webp"));
	return data;
}

void DestroyGameData(struct Game* game) {
	if (game->data->next) {
		free(game->data->next);
	}
	DestroyShader(game, game->data->grain);
	al_destroy_bitmap(game->data->cursorbmp);
	al_destroy_bitmap(game->data->cursorhover);
	al_destroy_bitmap(game->data->gradient);
	al_destroy_font(game->data->font);
	free(game->data);
}

SPRITESHEET_STREAM_DESCTRUCTOR(DestroyStream) {
	DestroyAnimation(data);
}

SPRITESHEET_STREAM(AnimationStream) {
	bool complete = !UpdateAnimation(data, delta);
	ALLEGRO_BITMAP* bitmap = al_clone_bitmap(GetAnimationFrame(data));
	double duration = GetAnimationFrameDuration(data);
	int frame = GetAnimationFrameNo(data);
	//PrintConsole(game, "STREAM: frame %d duration %f delta %f", frame, GetAnimationFrameDuration(data), delta);
	if (complete) {
		PrintConsole(game, "[AnimationStream] %s: complete", GetAnimationName(data));
		ResetAnimation(data);
	}
	return (struct SpritesheetFrame){
		.bitmap = bitmap,
		.duration = duration * 1000,
		.tint = al_map_rgb(255, 255, 255),
		.row = 1,
		.col = 1,
		.x = 0,
		.y = 0,
		.sx = 0,
		.sy = 0,
		.sw = al_get_bitmap_width(bitmap),
		.sh = al_get_bitmap_height(bitmap),
		.flipX = false,
		.flipY = false,
		.start = frame == 0,
		.end = complete,
		.shared = false,
	};
}
