#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

struct Player {
	struct AnimationDecoder* anim;
	ALLEGRO_BITMAP *bg, *fg;
	double delay;
	int repeats;
	int x, y;
	double scale;
	bool loaded;
	bool finished;
	struct Character* character;
	int freezeno, soundno, freezeplayed;
	ALLEGRO_BITMAP* mask;
	bool frozen, linked;
	char *bgname, *fgname, *maskname;
	void* callback_data;

	struct SceneDefinition scene;
};

struct Player* CreatePlayer(struct Game* game) {
	return calloc(1, sizeof(struct Player));
}

void DestroyPlayer(struct Game* game, struct Player* player) {
	if (player->loaded) {
		DestroyAnimation(player->anim);
		if (player->bg) {
			al_destroy_bitmap(player->bg);
		}
		if (player->fg) {
			al_destroy_bitmap(player->fg);
		}
		if (player->mask) {
			al_destroy_bitmap(player->mask);
		}
		if (player->character) {
			DestroyCharacter(game, player->character);
		}
	}
	free(player);
}

void LoadPlayerAnimation(struct Game* game, struct Player* player, struct SceneDefinition* scene) {
	player->scene = *scene;

	char path[255] = {0};
	snprintf(path, 255, "animations/%s.awebp", player->scene.name);
	HideMouse(game);

	if (player->anim) {
		DestroyAnimation(player->anim);
	}
	if (player->bg && player->scene.bg != player->bgname) {
		al_destroy_bitmap(player->bg);
		player->bg = NULL;
	}
	if (player->fg && player->scene.fg != player->fgname) {
		al_destroy_bitmap(player->fg);
		player->fg = NULL;
	}
	if (player->mask) {
		al_destroy_bitmap(player->mask);
		player->mask = NULL;
	}
	if (player->character) {
		DestroyCharacter(game, player->character);
		player->character = NULL;
	}

	player->anim = CreateAnimation(game, GetDataFilePath(game, path), false);

	if (!player->bg && player->scene.bg) {
		char p[255] = {0};
		snprintf(p, 255, "%s.png", player->scene.bg);
		player->bg = al_load_bitmap(GetDataFilePath(game, p));
		player->bgname = player->scene.bg;
	}
	if (!player->fg && player->scene.fg) {
		char p[255] = {0};
		snprintf(p, 255, "%s.png", player->scene.fg);
		player->fg = al_load_bitmap(GetDataFilePath(game, p));
		player->fgname = player->scene.fg;
	}
	player->repeats = player->scene.repeats;
	if (player->scene.character.name) {
		player->character = CreateCharacter(game, player->scene.character.name);
		char** spritesheet = player->scene.character.spritesheets;
		while (*spritesheet) {
			char p[255] = {};
			snprintf(p, 255, "sprites/%s/%s.awebp", player->scene.character.name, *spritesheet);
			RegisterStreamedSpritesheet(game, player->character, *spritesheet, AnimationStream, DestroyStream, CreateAnimation(game, GetDataFilePath(game, p), player->scene.character.repeat));
			if (player->scene.character.preload) {
				PreloadStreamedSpritesheet(game, player->character, *spritesheet);
			}
			spritesheet++;
		}
		SelectSpritesheet(game, player->character, player->scene.character.spritesheets[0]);
		SetCharacterPosition(game, player->character, 0, 0, 0);

		if (player->scene.character.hidden) {
			HideCharacter(game, player->character);
		}
	}

	player->x = 0;
	player->y = 0;
	player->scale = 1.0;
	player->freezeno = 0;
	player->soundno = 0;
	player->freezeplayed = -1;
	player->linked = false;
	player->frozen = false;
	player->delay = 0;
	player->callback_data = player->scene.callback_data;
	if (player->scene.callback) {
		if (player->scene.callback(game, 0, &player->x, &player->y, &player->scale, player->character, &player->callback_data)) {
			player->finished = true;
		}
	}

	ResetAnimation(player->anim, true);
	HandleAudio(game, player->scene.audio);

	player->loaded = true;
	player->finished = false;
	//PrintConsole(game, "Loaded: %s", path);
}

static bool CheckColor(ALLEGRO_COLOR color, ALLEGRO_COLOR link) {
	if (color.r < 0) {
		color.r = link.r;
	}
	if (color.g < 0) {
		color.g = link.g;
	}
	if (color.b < 0) {
		color.b = link.b;
	}
	return al_color_distance_ciede2000(color, link) < 0.01;
}

static void CheckHover(struct Game* game, struct Player* player) {
	if (player->mask) {
		ALLEGRO_COLOR color = CheckMask(game, player->mask);
		for (int i = 0; i < 8; i++) {
			if (player->scene.freezes[player->freezeno].links[i].callback || player->scene.freezes[player->freezeno].links[i].name || player->scene.freezes[player->freezeno].links[i].skip || player->scene.freezes[player->freezeno].links[i].ignore) {
				if (CheckColor(player->scene.freezes[player->freezeno].links[i].color, color)) {
					if (!player->scene.freezes[player->freezeno].links[i].ignore) {
						game->data->hover = true;
					} else {
						game->data->hover = false;
					}
				}
			}
		}
	} else {
		ALLEGRO_BITMAP* bitmap = GetAnimationFrame(player->anim);
		int x = Clamp(0.0, 1.0, game->data->mouseX) * (game->viewport.width - 1);
		int y = Clamp(0.0, 1.0, game->data->mouseY) * (game->viewport.height - 1);
		x -= player->x;
		y -= player->y;

		ALLEGRO_COLOR color = al_get_pixel(bitmap, x, y);
		game->data->hover = (color.a > 0.5);
	}
}

void SkipPlayerAnim(struct Game* game, struct Player* player) {
	player->delay = 0.01;
	player->finished = true;
	player->frozen = false;
	player->freezeno++;
	player->repeats = 0;
}

bool UpdatePlayer(struct Game* game, struct Player* player, double delta) {
	float modifier = 1.25 * ((player->scene.speed == 0.0) ? 1.0 : player->scene.speed);

	int frame = GetAnimationFrameNo(player->anim) + GetAnimationFrameCount(player->anim) * (player->scene.repeats - player->repeats);
	game->data->debuginfo = frame;

	if (player->scene.sounds[player->soundno].audio.type && player->scene.sounds[player->soundno].frame == frame) {
		HandleAudio(game, player->scene.sounds[player->soundno].audio);
		player->soundno++;
	}

	if (!player->frozen && player->scene.freezes[player->freezeno].frame == frame) {
		if (player->freezeplayed != player->freezeno) {
			HandleAudio(game, player->scene.freezes[player->freezeno].pre_audio);
			player->freezeplayed = player->freezeno;
		}
		if (player->scene.freezes[player->freezeno].mask) {
			player->frozen = true;
			ShowMouse(game);
			if (!player->mask) {
				if (player->scene.freezes[player->freezeno].mask && player->scene.freezes[player->freezeno].mask[0] != 0) {
					char path[255] = {0};
					snprintf(path, 255, "masks/%s.mask", player->scene.freezes[player->freezeno].mask);
					player->mask = al_load_bitmap(GetDataFilePath(game, path));
				}
			}
			PrintConsole(game, "Freeze: [%d] %s (frame: %d)", player->freezeno, player->scene.freezes[player->freezeno].mask, frame);
		}
		if (player->scene.freezes[player->freezeno].footnote) {
			ShowFootnote(game, player->scene.freezes[player->freezeno].footnote);
			PrintConsole(game, "Footnote: %d (frame: %d)", player->scene.freezes[player->freezeno].footnote, frame);
			player->freezeno++;
		}
	}

	if (player->frozen) {
		CheckHover(game, player);

		if (player->character && player->linked) {
			AnimateCharacter(game, player->character, delta, modifier);
		}
	} else {
		if (player->character) {
			AnimateCharacter(game, player->character, delta, modifier);
		}

		if (player->delay > 0) {
			player->delay -= delta;
			if (player->delay <= 0) {
				if (!player->repeats) {
					player->finished = true;
				} else {
					ResetAnimation(player->anim, true);
					player->repeats--;
				}
			}
		} else {
			if (!UpdateAnimation(player->anim, delta * modifier)) {
				player->delay = GetAnimationFrameDuration(player->anim) / modifier;
				if (player->delay <= 0.0) {
					player->delay = 0.01;
				}
			}
		}
	}

	if (player->scene.callback) {
		if (player->scene.callback(game, GetAnimationFrameNo(player->anim) + GetAnimationFrameCount(player->anim) * (player->scene.repeats - player->repeats), &player->x, &player->y, &player->scale, player->character, &player->callback_data)) {
			SkipPlayerAnim(game, player);
		}
	} else {
		player->x = 0;
		player->y = 0;
		player->scale = 1.0;
	}

	if (player->scene.length && frame > player->scene.length) {
		player->finished = true;
	}

	return player->finished;
}

void DrawPlayer(struct Game* game, struct Player* player) {
	if (!player->loaded) {
		return;
	}

	al_clear_to_color(player->scene.color);

	ALLEGRO_BITMAP* bitmap;
	if (player->bg) {
		bitmap = player->bg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
	bitmap = GetAnimationFrame(player->anim);
	al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), player->x, player->y, al_get_bitmap_width(bitmap) * player->scale, al_get_bitmap_height(bitmap) * player->scale, 0);

	if (player->scene.draw) {
		player->scene.draw(game, GetAnimationFrameNo(player->anim) + GetAnimationFrameCount(player->anim) * (player->scene.repeats - player->repeats), &player->callback_data);
	}

	if (player->character) {
		DrawCharacter(game, player->character);
	}

	if (player->fg) {
		bitmap = player->fg;
		al_draw_scaled_bitmap(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 0, 0, game->viewport.width, game->viewport.height, 0);
	}
}

void ProcessPlayerEvent(struct Game* game, struct Player* player, ALLEGRO_EVENT* ev) {
	if (player->frozen) {
		CheckHover(game, player);
	}

	if (game->data->hover && (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) || (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN))) {
		if (player->frozen && !player->linked) {
			HandleAudio(game, player->scene.freezes[player->freezeno].audio);

			for (int i = 0; i < 8; i++) {
				if (player->scene.freezes[player->freezeno].links[i].callback || player->scene.freezes[player->freezeno].links[i].name || player->scene.freezes[player->freezeno].links[i].skip) {
					if (CheckColor(player->scene.freezes[player->freezeno].links[i].color, CheckMask(game, player->mask))) {
						PrintConsole(game, "Linked!");
						if (player->character && player->scene.freezes[player->freezeno].links[i].name) {
							SelectSpritesheet(game, player->character, player->scene.freezes[player->freezeno].links[i].name);
							ShowCharacter(game, player->character);
							HideMouse(game);
							player->linked = true;
							return;
						}
						if (player->scene.freezes[player->freezeno].links[i].callback) {
							if (!player->scene.freezes[player->freezeno].links[i].callback(game, player->character, &player->callback_data)) {
								player->linked = true;
								return;
							}
						}
						HandleAudio(game, player->scene.freezes[player->freezeno].links[i].audio);
						if (player->scene.freezes[player->freezeno].links[i].skip) {
							SkipPlayerAnim(game, player);
							return;
						}
					}
				}
			}
			if (player->scene.freezes[player->freezeno].callback) {
				player->scene.freezes[player->freezeno].callback(game, player->character, &player->callback_data);
			}
			if (player->scene.freezes[player->freezeno].skip) {
				SkipPlayerAnim(game, player);
				return;
			}
			player->freezeno++;
			al_destroy_bitmap(player->mask);
			player->mask = NULL;
			player->frozen = false;
			HideMouse(game);
			PrintConsole(game, "Unfreeze!");
		}
	}
}

bool PlayerIsFinished(struct Game* game, struct Player* player) {
	return player->finished;
}
