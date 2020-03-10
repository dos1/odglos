#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

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

	if (game->data->footnote) {
#ifdef __EMSCRIPTEN__
		game->data->footnote = EM_ASM_INT({
			return window.ODGLOS && window.ODGLOS.footnote;
		});
#else
		game->data->footnote = false; // not implemented yet
#endif
	}
}

#ifdef __EMSCRIPTEN__
void HideHTMLLoading(struct Game* game) {
	EM_ASM({
		window.ODGLOS.hideLoading();
	});
}
#endif

void ShowFootnote(struct Game* game, int id) {
	game->data->footnote = true;
#ifdef __EMSCRIPTEN__
	EM_ASM({
		window.ODGLOS.showFootnote($0);
	},
		id);
#endif
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
	if (game->show_console) {
		int x, y, w, h;
		al_get_clipping_rectangle(&x, &y, &w, &h);
		al_hold_bitmap_drawing(true);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), w - 10, h * 0.935, ALLEGRO_ALIGN_RIGHT, "ODGŁOS");
		char revs[255];
		snprintf(revs, 255, "%s-%s", LIBSUPERDERPY_GAME_GIT_REV, LIBSUPERDERPY_GIT_REV);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), w - 10, h * 0.965, ALLEGRO_ALIGN_RIGHT, revs);

		snprintf(revs, 255, "%s (%d)", game->data->scene.name, game->data->debuginfo);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), 10, h * 0.965, ALLEGRO_ALIGN_LEFT, revs);

		al_hold_bitmap_drawing(false);
	}
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

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
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
	data->creditsfont = al_load_font(GetDataFilePath(game, "fonts/DejaVuSansMono.ttf"), 24, 0);
	data->gradient = al_load_bitmap(GetDataFilePath(game, "gradient.png"));
	data->banner = al_load_bitmap(GetDataFilePath(game, "banner.png"));
	data->queue_pos = 0;
	data->queue_handled = 0;
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
	al_destroy_font(game->data->creditsfont);
	al_destroy_bitmap(game->data->banner);

	if (game->data->audio.music) {
		al_destroy_audio_stream(game->data->audio.music);
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].stream) {
			al_destroy_audio_stream(game->data->audio.sounds[i].stream);
			free(game->data->audio.sounds[i].name);
		}
		if (game->data->audio.loops[i].stream) {
			al_destroy_audio_stream(game->data->audio.loops[i].stream);
			free(game->data->audio.loops[i].name);
		}
	}

	free(game->data);
}
