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
	int timestamp, position;
	WebPAnimDecoder* decoder;
	ALLEGRO_BITMAP *bitmap, *swap;
	int fd;
	uint8_t* buf;
	bool mmaped, shouldload;
};

struct AnimationDecoder* CreateAnimation(const char* filename) {
	struct AnimationDecoder* anim = calloc(1, sizeof(struct AnimationDecoder));

	WebPDataInit(&anim->data);

	anim->fd = open(filename, O_RDONLY | O_CLOEXEC);

	if (!anim->fd) {
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
			return NULL;
		}
		close(anim->fd);
		anim->mmaped = false;
	} else {
		anim->mmaped = true;
	}
	anim->data.bytes = anim->buf;

	WebPAnimDecoderOptions dec_options;
	WebPAnimDecoderOptionsInit(&dec_options);
	dec_options.color_mode = MODE_RGBA;
	dec_options.use_threads = !IS_EMSCRIPTEN;

	anim->decoder = WebPAnimDecoderNew(&anim->data, &dec_options);

	WebPAnimInfo anim_info;
	WebPAnimDecoderGetInfo(anim->decoder, &anim_info);
	anim->bitmap = al_create_bitmap(anim_info.canvas_width, anim_info.canvas_height);
	anim->swap = al_create_bitmap(anim_info.canvas_width, anim_info.canvas_height);

	ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(anim->bitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
	uint8_t* buf;
	WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp);
	for (int i = 0; i < al_get_bitmap_height(anim->bitmap); i++) {
		memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->bitmap) * al_get_pixel_size(lock->format),
			al_get_bitmap_width(anim->bitmap) * lock->pixel_size);
	}
	al_unlock_bitmap(anim->bitmap);

	lock = al_lock_bitmap(anim->swap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
	WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp);
	for (int i = 0; i < al_get_bitmap_height(anim->swap); i++) {
		memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->swap) * al_get_pixel_size(lock->format),
			al_get_bitmap_width(anim->swap) * lock->pixel_size);
	}
	al_unlock_bitmap(anim->swap);

	return anim;
}

bool UpdateAnimation(struct AnimationDecoder* anim, float timestamp) {
	anim->position += timestamp * 1000 * 1.25;
	ALLEGRO_LOCKED_REGION* lock = NULL;

	if (anim->shouldload) {
		anim->shouldload = false;
		if (!WebPAnimDecoderHasMoreFrames(anim->decoder)) {
			return false;
		}
		lock = al_lock_bitmap(anim->swap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
		uint8_t* buf;
		if (WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp)) {
			for (int i = 0; i < al_get_bitmap_height(anim->swap); i++) {
				memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->swap) * al_get_pixel_size(lock->format),
					al_get_bitmap_width(anim->swap) * lock->pixel_size);
			}
		}
		al_unlock_bitmap(anim->swap);
	}

	if (anim->position > anim->timestamp) {
		ALLEGRO_BITMAP* bmp = anim->bitmap;
		anim->bitmap = anim->swap;
		anim->swap = bmp;
		anim->shouldload = true;
	}

	return true;
}

ALLEGRO_BITMAP* GetAnimationFrame(struct AnimationDecoder* anim) {
	return anim->bitmap;
}

void DestroyAnimation(struct AnimationDecoder* anim) {
#ifndef __SWITCH__
	if (anim->mmaped) {
		munmap(anim->buf, anim->data.size);
		close(anim->fd);
		anim->data.bytes = NULL;
	}
#endif
	WebPDataClear(&anim->data);
	WebPAnimDecoderDelete(anim->decoder);
	al_destroy_bitmap(anim->swap);
	al_destroy_bitmap(anim->bitmap);
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

void CheckMask(struct Game* game, ALLEGRO_BITMAP* bitmap) {
	ALLEGRO_COLOR color = al_get_pixel(bitmap, (int)(game->data->mouseX * game->viewport.width), (int)(game->data->mouseY * game->viewport.height));
	game->data->hover = color.r < 1.0;
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

	if (game->data->cursor) {
		al_draw_scaled_rotated_bitmap(game->data->hover ? game->data->cursorhover : game->data->cursorbmp, 130, 165, game->data->mouseX * game->clip_rect.w + game->clip_rect.x, game->data->mouseY * game->clip_rect.h + game->clip_rect.y, game->clip_rect.w / (double)game->viewport.width * 0.1, game->clip_rect.h / (double)game->viewport.height * 0.1, 0, 0);
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
	al_hold_bitmap_drawing(false);
	SUPPRESS_END
}

bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_M)) {
		ToggleMute(game);
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_F) || (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)) {
		ToggleFullscreen(game);
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

	return false;
}

void ShowMouse(struct Game* game) {
	game->data->cursor = true;
}

void HideMouse(struct Game* game) {
	game->data->cursor = false;
}

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
	"016_pudelka_od_cioci_tmp/anim",
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
	"051_z_regal_08_gora_bok/regal_anim1",
	"052_turkusowe_cos/turkusowe_cos",
	"053_male_dziwne_cos/male_dziwne_cos",
	"054_lira_korbowa/lira_korbowa",
	"055_skrzypce2/skrzypce2",
	"056_swiecznik_test/anim",
	"057_szczypczyki_testowe/anim",
	"031_058_donice_dom/058_donice_dom2",
	"024_026_028_032_037_040_042_059_donice_w_ogrodzie/059_donice_w_ogrodzie8",
	"060_magnetofon/magnetofon2_bez_myszek",

	//"061_finale/001_sowka_wchodzi_do_studia/sowka_wchodzi_do_studia1",
	"061_finale/001_sowka_wchodzi_do_studia/sowka_wchodzi_do_studia2_TAK",
	"061_finale/002_sowki_zamieniaja_sie_krzeslami/sowki_zamieniaja_sie_krzeslami_po_dwa_i_nie_znikaja_TAK",
	//"061_finale/002_sowki_zamieniaja_sie_krzeslami/sowki_zamieniaja_sie_krzeslami_po_dwa_i_znikaja",
	"061_finale/003_sowka1_wlacza_konsole_z_daleka2/sowka1_wlacza_konsole_z_daleka2",
	"061_finale/004_okna_sie_otwieraja_z_sowka2/okna_sie_otwieraja_z_sowka2",
	"061_finale/005_sowka1_wlacza_konsole_z_bliska1/sowka1_wlacza_konsole_z_bliska1",
	"061_finale/006_drzwi_zamykaja_sie_same/drzwi_zamykaja_sie_same",
	"061_finale/007_sowka1_wchodzi_na_stol_z_bliska/sowka1_wchodzi_na_stol_z_bliska_nie_znika_TAK",
	//"061_finale/007_sowka1_wchodzi_na_stol_z_bliska/sowka1_wchodzi_na_stol_z_bliska_znika",
	"061_finale/008_sowka2_zaluzje/sowka2_zaluzje_nie_znika_TAK",
	//"061_finale/008_sowka2_zaluzje/sowka2_zaluzje_pojawia_sie",
	"061_finale/009_sowka2_klika_konsole/sowka2_klika_konsole_nie_znika_TAK",
	//"061_finale/009_sowka2_klika_konsole/sowka2_klika_konsole_znika",
	"061_finale/010_sowka1_zaluzje/sowka1_zaluzje",
	"061_finale/011_animacja_koncowa/animacja_koncowa",

};

void Dispatch(struct Game* game) {
	do {
		game->data->animationid++;
		if (game->data->animationid == sizeof(ANIMATIONS) / sizeof(char*)) {
			game->data->animationid = 0;
		}
	} while (!ANIMATIONS[game->data->animationid]);
	game->data->animation = ANIMATIONS[game->data->animationid];
}

struct CommonResources* CreateGameData(struct Game* game) {
	struct CommonResources* data = calloc(1, sizeof(struct CommonResources));
	data->grain = CreateShader(game, GetDataFilePath(game, "shaders/vertex.glsl"), GetDataFilePath(game, "shaders/grain.glsl"));
	data->mouseX = -1;
	data->mouseY = -1;
	data->cursor = false;
	data->cursorbmp = al_load_bitmap(GetDataFilePath(game, "kursor_normal.webp"));
	data->cursorhover = al_load_bitmap(GetDataFilePath(game, "kursor_hover.webp"));
	data->start_time = al_get_time();
	data->lowmem = false;
	data->animationid = -1;
	return data;
}

void DestroyGameData(struct Game* game) {
	if (game->data->next) {
		free(game->data->next);
	}
	DestroyShader(game, game->data->grain);
	al_destroy_bitmap(game->data->cursorbmp);
	al_destroy_bitmap(game->data->cursorhover);
	free(game->data);
}
