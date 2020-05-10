#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

static ALLEGRO_SAMPLE* GetCachedSound(struct Game* game, const char* path) {
	for (int i = 0; i < game->data->audio.cached; i++) {
		if (strcmp(game->data->audio.cache[i].name, path) == 0) {
			return game->data->audio.cache[i].sample;
		}
	}
	FatalError(game, true, "Could not find sound %s in cache", path);
	return NULL;
}

static void CollectSounds(struct Game* game) {
	if (game->data->audio.paused) {
		return;
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].sample_instance) {
			if (!al_get_sample_instance_playing(game->data->audio.sounds[i].sample_instance)) {
				al_destroy_sample_instance(game->data->audio.sounds[i].sample_instance);
				free(game->data->audio.sounds[i].name);
				game->data->audio.sounds[i].sample_instance = NULL;
				game->data->audio.sounds[i].name = NULL;
			}
		}
	}
}

void PlayMusic(struct Game* game, char* name, float volume) {
	ALLEGRO_SAMPLE_INSTANCE* old_sample_instance = game->data->audio.music;
	char* old_name = game->data->audio.music_name;
	char path[255] = {};
	snprintf(path, 255, "sounds/%s.flac", name);
	PrintConsole(game, "Starting music: %s", name);
	game->data->audio.music = al_create_sample_instance(GetCachedSound(game, GetDataFilePath(game, path)));
	game->data->audio.music_name = strdup(name);
	al_attach_sample_instance_to_mixer(game->data->audio.music, game->audio.music);
	al_set_sample_instance_playmode(game->data->audio.music, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(game->data->audio.music, volume);
	al_set_sample_instance_speed(game->data->audio.music, GetGameSpeed(game));
	al_play_sample_instance(game->data->audio.music);
	if (old_sample_instance) {
		al_destroy_sample_instance(old_sample_instance);
		free(old_name);
	}
}

void StopMusic(struct Game* game) {
	if (game->data->audio.music) {
		al_destroy_sample_instance(game->data->audio.music);
		free(game->data->audio.music_name);
		game->data->audio.music = NULL;
		game->data->audio.music_name = NULL;
	}
	PrintConsole(game, "Music stopped.");
}

void EnsureMusic(struct Game* game, char* name, float volume) {
	if (!game->data->audio.music_name || strcmp(name, game->data->audio.music_name) != 0) {
		PlayMusic(game, name, volume);
	}
}

void PlaySound(struct Game* game, char* name, float volume) {
	int i = 0;
	CollectSounds(game);
	for (i = 0; i <= 32; i++) {
		if (i == 32) {
			PrintConsole(game, "ERROR: All sound slots already taken!");
			return;
		}
		if (!game->data->audio.sounds[i].sample_instance) {
			break;
		}
	}
	char path[255] = {};
	game->data->audio.sounds[i].name = strdup(name);
	snprintf(path, 255, "sounds/%s.flac", name);
	PrintConsole(game, "Starting sound: %s", name);
	game->data->audio.sounds[i].sample_instance = al_create_sample_instance(GetCachedSound(game, GetDataFilePath(game, path)));
	al_attach_sample_instance_to_mixer(game->data->audio.sounds[i].sample_instance, game->audio.fx);
	al_set_sample_instance_playmode(game->data->audio.sounds[i].sample_instance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(game->data->audio.sounds[i].sample_instance, volume);
	al_set_sample_instance_speed(game->data->audio.sounds[i].sample_instance, GetGameSpeed(game));
	al_play_sample_instance(game->data->audio.sounds[i].sample_instance);
}

void StopSound(struct Game* game, char* name) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].name && strcmp(name, game->data->audio.sounds[i].name) == 0) {
			al_destroy_sample_instance(game->data->audio.sounds[i].sample_instance);
			free(game->data->audio.sounds[i].name);
			game->data->audio.sounds[i].sample_instance = NULL;
			game->data->audio.sounds[i].name = NULL;
			return;
		}
	}
}

void PlayLoop(struct Game* game, char* name, float volume, bool persist) {
	int i = 32;
	for (int j = 31; j >= 0; j--) {
		if (game->data->audio.loops[j].name && strcmp(name, game->data->audio.loops[j].name) == 0) {
			// already playing
			return;
		}
		if (!game->data->audio.loops[j].sample_instance) {
			i = j;
		}
	}
	if (i == 32) {
		PrintConsole(game, "ERROR: All loop slots already taken!");
		return;
	}
	char path[255] = {};
	game->data->audio.loops[i].name = strdup(name);
	snprintf(path, 255, "sounds/%s.flac", name);
	PrintConsole(game, "Starting loop: %s", name);
	game->data->audio.loops[i].sample_instance = al_create_sample_instance(GetCachedSound(game, GetDataFilePath(game, path)));
	game->data->audio.loops[i].persist = persist;
	al_attach_sample_instance_to_mixer(game->data->audio.loops[i].sample_instance, game->audio.fx);
	al_set_sample_instance_playmode(game->data->audio.loops[i].sample_instance, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(game->data->audio.loops[i].sample_instance, volume);
	al_set_sample_instance_speed(game->data->audio.loops[i].sample_instance, GetGameSpeed(game));
	al_play_sample_instance(game->data->audio.loops[i].sample_instance);
}

void StopLoop(struct Game* game, char* name) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.loops[i].name && strcmp(name, game->data->audio.loops[i].name) == 0) {
			al_destroy_sample_instance(game->data->audio.loops[i].sample_instance);
			free(game->data->audio.loops[i].name);
			game->data->audio.loops[i].sample_instance = NULL;
			game->data->audio.loops[i].name = NULL;
			return;
		}
	}
}

void StopLoops(struct Game* game) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.loops[i].sample_instance) {
			if (!game->data->audio.loops[i].persist) {
				al_destroy_sample_instance(game->data->audio.loops[i].sample_instance);
				free(game->data->audio.loops[i].name);
				game->data->audio.loops[i].sample_instance = NULL;
				game->data->audio.loops[i].name = NULL;
			}
		}
	}
}

void StopSounds(struct Game* game) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].sample_instance) {
			al_destroy_sample_instance(game->data->audio.sounds[i].sample_instance);
			free(game->data->audio.sounds[i].name);
			game->data->audio.sounds[i].sample_instance = NULL;
			game->data->audio.sounds[i].name = NULL;
		}
	}
}

void HandleAudio(struct Game* game, struct Audio audio) {
	float volume = audio.volume;
	if (volume == 0.0) {
		// default to full volume when not specified in struct initializer
		volume = 1.0;
	}
	if (audio.stop_music) {
		StopMusic(game);
	}
	switch (audio.type) {
		case NO_AUDIO:
			return;
		case SOUND:
			PlaySound(game, audio.name, volume);
			return;
		case MUSIC:
			PlayMusic(game, audio.name, volume);
			return;
		case LOOP:
			PlayLoop(game, audio.name, volume, audio.persist);
			return;
		case STOP_LOOP:
			StopLoop(game, audio.name);
			return;
		case STOP_MUSIC:
			StopMusic(game);
			return;
		case STOP_SOUND:
			StopSound(game, audio.name);
			return;
		case ENSURE_MUSIC:
			EnsureMusic(game, audio.name, volume);
			return;
	}
}

void PauseAudio(struct Game* game) {
	if (game->data->audio.paused) {
		return;
	}
	CollectSounds(game);
	if (game->data->audio.music) {
		game->data->audio.music_pos = al_get_sample_instance_position(game->data->audio.music);
		al_set_sample_instance_playing(game->data->audio.music, false);
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].sample_instance) {
			game->data->audio.sounds[i].pos = al_get_sample_instance_position(game->data->audio.sounds[i].sample_instance);
			al_set_sample_instance_playing(game->data->audio.sounds[i].sample_instance, false);
		}
		if (game->data->audio.loops[i].sample_instance) {
			game->data->audio.loops[i].pos = al_get_sample_instance_position(game->data->audio.loops[i].sample_instance);
			al_set_sample_instance_playing(game->data->audio.loops[i].sample_instance, false);
		}
	}
	game->data->audio.paused = true;
}

void ResumeAudio(struct Game* game) {
	if (!game->data->audio.paused) {
		return;
	}
	if (game->data->audio.music) {
		al_set_sample_instance_position(game->data->audio.music, game->data->audio.music_pos);
		al_set_sample_instance_playing(game->data->audio.music, true);
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].sample_instance) {
			al_set_sample_instance_position(game->data->audio.sounds[i].sample_instance, game->data->audio.sounds[i].pos);
			al_set_sample_instance_playing(game->data->audio.sounds[i].sample_instance, true);
		}
		if (game->data->audio.loops[i].sample_instance) {
			al_set_sample_instance_position(game->data->audio.loops[i].sample_instance, game->data->audio.loops[i].pos);
			al_set_sample_instance_playing(game->data->audio.loops[i].sample_instance, true);
		}
	}
	game->data->audio.paused = false;
}
