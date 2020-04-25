/*! \file example.c
 *  \brief Example gamestate.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	struct AnimationDecoder* animation;
	struct Character* button;
	ALLEGRO_BITMAP *bmps[17], *mask, *btn;
	int current;

	bool playing;
	bool buffered;
	int loaded_frame;
	int play;
	double delay;
	double counter;

	double timeout;
	int sequence[4];
	bool success;
	int position;
	bool pressed;
	bool footnoted;

	int user;
};

int Gamestate_ProgressCount = 4;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	if (game->data->footnote) { return; }

	if (delta > GetAnimationFrameDuration(data->animation)) {
		delta = GetAnimationFrameDuration(data->animation);
	}

	UpdateAnimation(data->animation, delta);

	if (GetAnimationFrameNo(data->animation) != data->current) {
		data->current = GetAnimationFrameNo(data->animation);
		//PrintConsole(game, "%d", data->current);
		if (data->loaded_frame < data->current) {
			data->bmps[data->current] = al_clone_bitmap(GetAnimationFrame(data->animation));
			data->loaded_frame = data->current;
		}
		if (data->current < 16) {
			char path[255] = {};
			snprintf(path, 255, "lawka/%d", data->current + 1);
			StopSound(game, path);
			PlaySound(game, path, 1.0);
		}
	}

	if (IsAnimationComplete(data->animation) && !data->buffered) {
		data->buffered = true;
		data->delay = 1.0;
		data->playing = true;
	}

	if (data->playing) {
		data->delay -= delta;
		if (data->delay < 0.0) {
			if (data->success) {
				data->delay = 0.1 + ((rand() % 50) / 1000.0);
				data->play = rand() % 17;
			} else if (data->pressed) {
				data->delay = data->user ? 8.0 : 4.0;
				data->pressed = false;
				data->play = 16;
				data->position = 0;
			} else if (data->position < 3) {
				HideMouse(game);
				data->delay = 0.4;
				data->play = data->sequence[data->position++];
			} else if (data->position == 3) {
				data->delay = 8.0;
				data->position = 0;
				data->play = 16;
				ShowMouse(game);
			}

			if (data->play < 16) {
				char path[255] = {};
				snprintf(path, 255, "lawka/%d", data->play + 1);
				StopSound(game, path);
				PlaySound(game, path, 1.0);
			}
		}
		if (data->success) {
			data->counter += delta;
		}
	}
	if (data->counter > 4.0) {
		if (!data->footnoted) {
			data->footnoted = true;
			ShowFootnote(game, 5);
		} else {
			ChangeCurrentGamestate(game, "anim");
		}
	}

	CheckMask(game, data->mask);

	if (game->data->skip_requested) {
		UnsetSkip(game);
		data->playing = true;
		data->success = true;
		data->counter = true;
		data->delay = 0;
		game->data->skip_available = false;
		HideMouse(game);
	}

	if (!data->success && data->buffered) {
		game->data->skip_available = true;
	}

	if (game->data->cursor && IsOnCharacter(game, data->button, game->viewport.width * game->data->mouseX, game->viewport.height * game->data->mouseY, true)) {
		game->data->hover = true;
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	ALLEGRO_BITMAP* bitmap = NULL;
	if (data->playing) {
		bitmap = data->bmps[data->play];
	} else {
		bitmap = GetAnimationFrame(data->animation);
	}
	al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	DrawCharacter(game, data->button);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (game->data->footnote) { return; }

	ALLEGRO_COLOR color = CheckMask(game, data->mask);

	if (game->data->cursor && IsOnCharacter(game, data->button, game->viewport.width * game->data->mouseX, game->viewport.height * game->data->mouseY, true)) {
		if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) ||
			(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
			(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
			data->delay = 0;
			data->position = 0;
			data->play = 16;
		}
		return;
	}

	if (game->data->cursor && game->data->hover && data->play == 16 && !data->success) {
		if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) ||
			(ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) ||
			(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
			int pos = round(color.r * 255 / 10.0);
			PrintConsole(game, "%d", pos);
			if (pos > 15) {
				return;
			}
			data->play = 15 - pos;
			data->delay = 0.2;
			data->pressed = true;
			data->playing = true;
			char path[255] = {};
			snprintf(path, 255, "lawka/%d", data->play + 1);
			StopSound(game, path);
			PlaySound(game, path, 1.0);

			if (data->sequence[data->user] == data->play) {
				data->user++;
			} else {
				data->user = 0;
				if (data->sequence[data->user] == data->play) {
					data->user++;
				}
			}
			if (data->user == 3) {
				data->success = true;
				game->data->skip_available = false;
				HideMouse(game);
			}
		}
	}

	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP))) {
		ChangeCurrentGamestate(game, "anim");
	}
	if (game->show_console && ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_COMMA))) {
		game->data->sceneid--;
		ChangeCurrentGamestate(game, "anim");
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));

	data->animation = CreateAnimation(game, GetDataFilePath(game, "lawka_w_parku/lawka.awebp"), false);
	progress(game);
	data->bmps[0] = al_clone_bitmap(GetAnimationFrame(data->animation));
	progress(game);

	data->btn = al_load_bitmap(GetDataFilePath(game, "lawka_w_parku/button.png"));
	data->button = CreateCharacter(game, "button");
	RegisterSpritesheetFromBitmap(game, data->button, "button", data->btn);
	SelectSpritesheet(game, data->button, "button");
	LoadSpritesheets(game, data->button, progress);

	data->mask = al_load_bitmap(GetDataFilePath(game, "masks/lawka_w_parku_maski.mask"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar

	data->loaded_frame = 0;
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	DestroyAnimation(data->animation);
	for (int i = 0; i < 17; i++) {
		al_destroy_bitmap(data->bmps[i]);
	}
	al_destroy_bitmap(data->mask);
	al_destroy_bitmap(data->btn);
	DestroyCharacter(game, data->button);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	HideMouse(game);
	PlayMusic(game, "dwor", 0.25);
	data->current = 0;
	data->playing = false;
	data->play = 16;
	data->counter = 0;
	data->footnoted = false;
	data->user = 0;
	data->buffered = false;
	data->delay = 0;
	data->timeout = 0;
	data->success = false;
	data->position = 0;
	data->pressed = false;
	for (int i = 0; i < 4; i++) {
		data->sequence[i] = rand() % 16;
		if (i > 0) {
			while (data->sequence[i - 1] == data->sequence[i]) {
				data->sequence[i] = rand() % 16;
			}
		}
	}
	SetCharacterPosition(game, data->button, 946 + 62 / 2.0, 486 + 41 / 2.0, 0);
	ResetAnimation(data->animation, true);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	game->data->skip_available = false;
}
