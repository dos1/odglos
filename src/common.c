#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

void UnsetSkip(struct Game* game) {
#ifdef __EMSCRIPTEN__
	EM_ASM({ window.ODGLOS && (window.ODGLOS.skip = false); });
#endif
	game->data->skip_requested = false;
}

void PreLogic(struct Game* game, double delta) {
	game->data->hover = false;
	game->data->hover_back = false;

#ifdef __EMSCRIPTEN__
	game->data->skip_requested = EM_ASM_INT({
		window.ODGLOS && (window.ODGLOS.skip_available = $0);
		return window.ODGLOS && window.ODGLOS.skip;
	},
		game->data->skip_available);
#endif

	if (game->data->footnote) {
#ifdef __EMSCRIPTEN__
		game->data->footnote = EM_ASM_INT({
			return window.ODGLOS && (window.ODGLOS.footnote || window.ODGLOS.menu);
		});
#else
		game->data->footnote = false; // not implemented yet
#endif
		if (!game->data->footnote && game->data->audio.paused && !game->data->pause) {
			ResumeAudio(game);
		}
	} else {
		if (game->data->w) {
			game->data->mouseY -= 0.5 * delta;
		}
		if (game->data->a) {
			game->data->mouseX -= 0.33 * delta;
		}
		if (game->data->s) {
			game->data->mouseY += 0.5 * delta;
		}
		if (game->data->d) {
			game->data->mouseX += 0.33 * delta;
		}
		if (game->data->w || game->data->a || game->data->s || game->data->d) {
			game->data->touch = false;
			game->data->mouseX = Clamp(0.0, 1.0, game->data->mouseX);
			game->data->mouseY = Clamp(0.0, 1.0, game->data->mouseY);
		}
	}
}

void PostLogic(struct Game* game, double delta) {
	if (game->data->menu_requested) {
		ShowMenu(game);
		game->data->menu_requested = false;
	}

	DrawSceneToolbox(game);
}

float GetLoadingProgress(struct Game* game) {
	float val = GetDownloadProgress(game);
	val *= 0.7;
	val += (game->data->audio.cached / (float)game->data->audio.cached_no) * 0.1;
	val += game->loading.progress * 0.1;
	if (game->data->dark_loading) {
		val += 0.1;
	}
	return val;
}

#ifdef __EMSCRIPTEN__
void HideHTMLLoading(struct Game* game) {
	EM_ASM({
		window.ODGLOS.hideLoading();
	});
}

void SetHTMLLoadingValue(struct Game* game, float value) {
	EM_ASM({
		window.ODGLOS.setLoadingValue($0);
	},
		value);
}

#endif

void ShowFootnote(struct Game* game, int id) {
	PrintConsole(game, "Footnote %d", id);
	game->data->footnote = true;
	char buf[255];
	snprintf(buf, 255, "footnote%d", id);
	SetConfigOption(game, LIBSUPERDERPY_GAMENAME, buf, "1");
#ifdef __EMSCRIPTEN__
	EM_ASM({
		window.ODGLOS.showFootnote($0);
	},
		id);
#endif
}

void HideFootnote(struct Game* game) {
#ifdef __EMSCRIPTEN__
	EM_ASM({
		if (window.ODGLOS.footnote) {
			window.ODGLOS.hideFootnote();
		} else if (window.ODGLOS.menu) {
			window.ODGLOS.hideMenu();
		}
	});
#else
	game->data->footnote = false;
#endif
}

void ShowMenu(struct Game* game) {
	game->data->footnote = true;
	PauseAudio(game);
#ifdef __EMSCRIPTEN__
	HideHTMLLoading(game);
	EM_ASM({
		window.ODGLOS.showMenu();
	});
	game->data->mouseX = -1.0;
	game->data->mouseY = -1.0;
#endif
}

ALLEGRO_COLOR CheckMaskSized(struct Game* game, ALLEGRO_BITMAP* bitmap, int x, int y, int width, int height) {
	x = Clamp(0.0, 1.0, game->data->mouseX) * (width - 1) - x;
	y = Clamp(0.0, 1.0, game->data->mouseY) * (height - 1) - y;
	if (x < 0 || y < 0 || x >= al_get_bitmap_width(bitmap) || y >= al_get_bitmap_height(bitmap)) {
		game->data->hover = false;
		return al_map_rgba(0, 0, 0, 0);
	}
	ALLEGRO_COLOR color = al_get_pixel(bitmap, x, y);
	game->data->hover = (color.r < 0.5) || (color.g < 0.5) || (color.b < 0.5);
	return color;
}

ALLEGRO_COLOR CheckMask(struct Game* game, ALLEGRO_BITMAP* bitmap) {
	return CheckMaskSized(game, bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap));
}

void DrawTexturedRectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color) {
	ALLEGRO_VERTEX vtx[4];
	int ii = 0;

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
			if (rand() % 2) {
				randx = -randx;
			}
			if (rand() % 2) {
				randy = -randy;
			}

			float color = 1.0 + (rand() / (double)RAND_MAX) * 0.01 - 0.005;

			al_draw_tinted_scaled_rotated_bitmap(GetGamestateFramebuffer(game, tmp), al_map_rgba_f(color, color, color, color), 0, 0,
				game->clip_rect.x + randx, game->clip_rect.y + randy, game->clip_rect.w / (double)al_get_bitmap_width(GetGamestateFramebuffer(game, tmp)), game->clip_rect.h / (double)al_get_bitmap_height(GetGamestateFramebuffer(game, tmp)), 0.0, 0);
			drawn = true;
		}
		tmp = GetNextGamestate(game, tmp);
	}
	if (!drawn) {
		DrawTexturedRectangle(game->clip_rect.x, game->clip_rect.y, game->clip_rect.w, game->clip_rect.h, al_map_rgb(0, 0, 0));
	}
	al_use_shader(NULL);

	if (game->data->pause) {
		al_draw_tinted_scaled_bitmap(game->data->gradient, al_map_rgba_f(0.5, 0.5, 0.5, 0.5),
			0, 0, al_get_bitmap_width(game->data->gradient), al_get_bitmap_height(game->data->gradient),
			game->clip_rect.x, game->clip_rect.y, game->clip_rect.w, game->clip_rect.h, 0);

		al_draw_text(game->data->font, al_map_rgb(255, 255, 255), game->clip_rect.x + game->clip_rect.w / 2.0, game->clip_rect.y + game->clip_rect.h / 2.0, ALLEGRO_ALIGN_CENTER, "PAUSED");
	}

	bool hover = game->data->hover;

	if (game->config.mute) {
		al_draw_tinted_scaled_rotated_bitmap(game->data->mute, al_map_rgba(0, 0, 0, 255), 48, 48, game->clip_rect.x + game->clip_rect.w * 40.0 / 1280.0, game->clip_rect.y + game->clip_rect.h * 682.0 / 720.0, 0.5 * game->clip_rect.w / (double)game->viewport.width / LIBSUPERDERPY_IMAGE_SCALE, 0.5 * game->clip_rect.h / (double)game->viewport.height / LIBSUPERDERPY_IMAGE_SCALE, 0.0, 0);
		al_draw_scaled_rotated_bitmap(game->data->mute, 48, 48, game->clip_rect.x + game->clip_rect.w * 38.0 / 1280.0, game->clip_rect.y + game->clip_rect.h * 680.0 / 720.0, 0.5 * game->clip_rect.w / (double)game->viewport.width / LIBSUPERDERPY_IMAGE_SCALE, 0.5 * game->clip_rect.h / (double)game->viewport.height / LIBSUPERDERPY_IMAGE_SCALE, 0.0, 0);
		if (game->data->mouseX < 0.05 && game->data->mouseY > 0.90 && game->data->mouseX > 0.01 && game->data->mouseY < 0.98) {
			hover = true;
		}
	}

	if (game->data->cursor && !game->data->footnote) {
#ifdef __EMSCRIPTEN__
		ALLEGRO_BITMAP* menu = game->data->menu;
		if (game->data->mouseX > 0.94 && game->data->mouseY > 0.90 && game->data->mouseX < 0.98 && game->data->mouseY < 0.98) {
			hover = true;
			menu = game->data->menu2;
		}

		al_draw_scaled_rotated_bitmap(menu, 25, 28, game->clip_rect.x + game->clip_rect.w * 1227.0 / 1280.0, game->clip_rect.y + game->clip_rect.h * 669.0 / 720.0, game->clip_rect.w / (double)game->viewport.width / LIBSUPERDERPY_IMAGE_SCALE, game->clip_rect.h / (double)game->viewport.height / LIBSUPERDERPY_IMAGE_SCALE, 0.0, 0);
#endif
	}

	if (game->data->pause || (game->data->cursor && !game->data->footnote)) {
		if (!game->data->touch) {
			al_draw_scaled_rotated_bitmap(hover ? (game->data->hover_back ? game->data->cursorback : game->data->cursorhover) : game->data->cursorbmp, game->data->hover_back ? 50 : 9, game->data->hover_back ? 30 : 4, game->data->mouseX * game->clip_rect.w + game->clip_rect.x, game->data->mouseY * game->clip_rect.h + game->clip_rect.y, game->clip_rect.w / (double)game->viewport.width * 0.69 / LIBSUPERDERPY_IMAGE_SCALE, game->clip_rect.h / (double)game->viewport.height * 0.69 / LIBSUPERDERPY_IMAGE_SCALE, 0, 0);
		}
	}
}

void DrawBuildInfo(struct Game* game) {
	SUPPRESS_WARNING("-Wdeprecated-declarations")
	if (game->show_console) {
		int x = 0, y = 0, w = 0, h = 0;
		al_get_clipping_rectangle(&x, &y, &w, &h);
		al_hold_bitmap_drawing(true);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), w - 10, h * 0.935, ALLEGRO_ALIGN_RIGHT, "ODGŁOS");
		char revs[255];
		snprintf(revs, 255, "%s-%s", LIBSUPERDERPY_GAME_GIT_REV, LIBSUPERDERPY_GIT_REV);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), w - 10, h * 0.965, ALLEGRO_ALIGN_RIGHT, revs);

		snprintf(revs, 255, "[%d] %s (%d)", game->data->sceneid, game->data->scene.name, game->data->debuginfo);
		DrawTextWithShadow(game->_priv.font_console, al_map_rgb(255, 255, 255), 10, h * 0.965, ALLEGRO_ALIGN_LEFT, revs);

		al_hold_bitmap_drawing(false);
	}
	SUPPRESS_END

	if (game->data->pause) {
		DrawSceneToolbox(game);
	}
}

static void HandlePointerEmulation(struct Game* game, ALLEGRO_EVENT* ev) {
	struct CommonResources* data = game->data;

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_W)) {
		data->w = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_W)) {
		data->w = false;
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_A)) {
		data->a = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_A)) {
		data->a = false;
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_S)) {
		data->s = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_S)) {
		data->s = false;
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_D)) {
		data->d = true;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_D)) {
		data->d = false;
	}

#ifdef __SWITCH__
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 13)) {
		data->w = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 13)) {
		data->w = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 12)) {
		data->a = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 12)) {
		data->a = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 15)) {
		data->s = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 15)) {
		data->s = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 14)) {
		data->d = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 14)) {
		data->d = false;
	}
#endif

#ifdef ALLEGRO_MACOSX
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 11)) {
		data->w = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 11)) {
		data->w = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 13)) {
		data->a = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 13)) {
		data->a = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 12)) {
		data->s = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 12)) {
		data->s = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 14)) {
		data->d = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 14)) {
		data->d = false;
	}
#endif

#ifdef ALLEGRO_WINDOWS
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 13)) {
		data->w = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 13)) {
		data->w = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 11)) {
		data->a = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 11)) {
		data->a = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 12)) {
		data->s = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 12)) {
		data->s = false;
	}

	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 10)) {
		data->d = true;
	}
	if ((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) && (ev->joystick.button == 10)) {
		data->d = false;
	}
#endif

#ifdef ALLEGRO_WITH_XWINDOWS
	if (ev->type == ALLEGRO_EVENT_JOYSTICK_AXIS && (ev->joystick.stick == 0 || ev->joystick.stick == 3)) {
#else
	if (ev->type == ALLEGRO_EVENT_JOYSTICK_AXIS && ev->joystick.stick == 0) {
#endif
		if (ev->joystick.axis == 1) {
			if (ev->joystick.pos < -0.25) {
				data->w = true;
			} else {
				data->w = false;
			}
			if (ev->joystick.pos > 0.25) {
				data->s = true;
			} else {
				data->s = false;
			}
		}
		if (ev->joystick.axis == 0) {
			if (ev->joystick.pos < -0.25) {
				data->a = true;
			} else {
				data->a = false;
			}
			if (ev->joystick.pos > 0.25) {
				data->d = true;
			} else {
				data->d = false;
			}
		}
	}

	if (((ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.button == 0)) ||
		((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ENTER))) {
		ev->type = ALLEGRO_EVENT_MOUSE_BUTTON_DOWN;
		ev->mouse.button = 1;
	}
}

bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_M)) {
		ToggleMute(game);
	}

	if (game->data->footnote) {
		if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
			HideFootnote(game);
		}
		return false;
	}

#ifndef __EMSCRIPTEN__
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_F)) {
		ToggleFullscreen(game);
	}
#endif

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_P)) {
		if (!game->data->pause) {
			PauseAudio(game);
			PauseAllGamestates(game);
		} else {
			ResumeAudio(game);
			ResumeAllGamestates(game);
		}
		game->data->pause = !game->data->pause;
	}

	if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
		game->data->mouseX = Clamp(0, 1, (ev->mouse.x - game->clip_rect.x) / (double)game->clip_rect.w);
		game->data->mouseY = Clamp(0, 1, (ev->mouse.y - game->clip_rect.y) / (double)game->clip_rect.h);
		game->data->touch = false;
	}

	if (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN || ev->type == ALLEGRO_EVENT_TOUCH_MOVE) {
		game->data->mouseX = Clamp(0, 1, (ev->touch.x - game->clip_rect.x) / (double)game->clip_rect.w);
		game->data->mouseY = Clamp(0, 1, (ev->touch.y - game->clip_rect.y) / (double)game->clip_rect.h);
		game->data->touch = true;
	}

#ifdef __EMSCRIPTEN__
	if (game->data->cursor && game->data->mouseX > 0.94 && game->data->mouseY > 0.90 && game->data->mouseX < 0.98 && game->data->mouseY < 0.98) {
		if ((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
			ShowMenu(game);
			return true;
		}
	}
#endif

	if (game->config.mute && game->data->mouseX < 0.05 && game->data->mouseY > 0.90 && game->data->mouseX > 0.01 && game->data->mouseY < 0.98) {
		if ((game->data->cursor && ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (game->data->cursor && ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
			ToggleMute(game);
			return true;
		}
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
#ifndef __EMSCRIPTEN__
		UnloadAllGamestates(game);
#else
		ShowMenu(game);
#endif
		return true;
	}

	if (game->show_console) {
		if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA)) {
			game->data->sceneid--;
			if (game->data->sceneid < -1) {
				game->data->sceneid = -1;
			}
		}

		if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SLASH) && game->data->skip_available) {
			PrintConsole(game, "Skip requested!");
			game->data->skip_requested = true;
		}
	}

#ifdef LIBSUPERDERPY_IMGUI
	if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev->mouse.button == 2) {
		if (game->show_console || game->data->toolbox) {
			PrintConsole(game, "Debug interface toggled.");
			game->data->toolbox = !game->data->toolbox;
			if (game->data->toolbox) {
				al_show_mouse_cursor(game->display);
			} else {
				al_hide_mouse_cursor(game->display);
			}
		}
		return true;
	}
#endif

	HandlePointerEmulation(game, ev);

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
	data->cursorback = al_load_bitmap(GetDataFilePath(game, "kursor_hover3.png"));
	data->menu = al_load_bitmap(GetDataFilePath(game, "menu_przycisk1_standby.png"));
	data->menu2 = al_load_bitmap(GetDataFilePath(game, "menu_przycisk2_hover.png"));
	data->start_time = al_get_time();
#ifdef __EMSCRIPTEN__
	data->sceneid = strtol(GetConfigOptionDefault(game, LIBSUPERDERPY_GAMENAME, "scene", "0"), NULL, 10) - 1;
#else
	data->sceneid = -1;
#endif
	data->pause = false;
	data->font = al_load_font(GetDataFilePath(game, "fonts/DejaVuSansMono.ttf"), 42, 0);
	data->creditsfont = al_load_font(GetDataFilePath(game, "fonts/DejaVuSansMono.ttf"), 48, 0);
	data->gradient = al_load_bitmap(GetDataFilePath(game, "gradient.png"));
	data->banner = al_load_bitmap(GetDataFilePath(game, "banner.png"));
	data->mute = al_load_bitmap(GetDataFilePath(game, "mute.png"));
	data->logo = al_load_bitmap(GetDataFilePath(game, "holypangolin.png"));
	data->queue_pos = 0;
	data->queue_handled = 0;
	return data;
}

void DestroyGameData(struct Game* game) {
	DestroyShader(game, game->data->grain);
	al_destroy_bitmap(game->data->cursorbmp);
	al_destroy_bitmap(game->data->cursorhover);
	al_destroy_bitmap(game->data->cursorback);
	al_destroy_bitmap(game->data->gradient);
	al_destroy_bitmap(game->data->menu);
	al_destroy_bitmap(game->data->menu2);
	al_destroy_font(game->data->font);
	al_destroy_font(game->data->creditsfont);
	al_destroy_bitmap(game->data->banner);
	al_destroy_bitmap(game->data->mute);
	al_destroy_bitmap(game->data->logo);

	StopMusic(game);
	StopLoops(game);
	StopSounds(game);
	DestroySoundCache(game);

	free(game->data);
}
