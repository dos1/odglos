#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

static void CollectStreams(struct Game* game) {
	if (game->data->audio.paused) {
		return;
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].stream) {
			if (!al_get_audio_stream_playing(game->data->audio.sounds[i].stream)) {
				al_destroy_audio_stream(game->data->audio.sounds[i].stream);
				free(game->data->audio.sounds[i].name);
				game->data->audio.sounds[i].stream = NULL;
				game->data->audio.sounds[i].name = NULL;
			}
		}
	}
}

void PlayMusic(struct Game* game, char* name, float volume) {
	StopMusic(game);
	char path[255] = {};
	snprintf(path, 255, "sounds/%s.flac.opus", name);
	PrintConsole(game, "Starting music: %s", name);
	game->data->audio.music = al_load_audio_stream(GetDataFilePath(game, path), 4, 2048);
	al_attach_audio_stream_to_mixer(game->data->audio.music, game->audio.music);
	al_set_audio_stream_playmode(game->data->audio.music, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(game->data->audio.music, volume);
	al_set_audio_stream_playing(game->data->audio.music, true);
}

void StopMusic(struct Game* game) {
	if (game->data->audio.music) {
		al_destroy_audio_stream(game->data->audio.music);
		game->data->audio.music = NULL;
	}
	PrintConsole(game, "Music stopped.");
}

void EnsureMusic(struct Game* game, char* name, float volume) {
	if (!game->data->audio.music) {
		PlayMusic(game, name, volume);
	}
}

void PlaySound(struct Game* game, char* name, float volume) {
	int i;
	CollectStreams(game);
	for (i = 0; i <= 32; i++) {
		if (i == 32) {
			PrintConsole(game, "ERROR: All sound slots already taken!");
			return;
		}
		if (!game->data->audio.sounds[i].stream) {
			break;
		}
	}
	char path[255] = {};
	game->data->audio.sounds[i].name = strdup(name);
	snprintf(path, 255, "sounds/%s.flac.opus", name);
	PrintConsole(game, "Starting sound: %s", name);
	game->data->audio.sounds[i].stream = al_load_audio_stream(GetDataFilePath(game, path), 4, 2048);
	al_attach_audio_stream_to_mixer(game->data->audio.sounds[i].stream, game->audio.fx);
	al_set_audio_stream_playmode(game->data->audio.sounds[i].stream, ALLEGRO_PLAYMODE_ONCE);
	al_set_audio_stream_gain(game->data->audio.sounds[i].stream, volume);
	al_set_audio_stream_playing(game->data->audio.sounds[i].stream, true);
}

void StopSound(struct Game* game, char* name) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].name && strcmp(name, game->data->audio.sounds[i].name) == 0) {
			al_destroy_audio_stream(game->data->audio.sounds[i].stream);
			free(game->data->audio.sounds[i].name);
			game->data->audio.sounds[i].stream = NULL;
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
		if (!game->data->audio.loops[j].stream) {
			i = j;
		}
	}
	if (i == 32) {
		PrintConsole(game, "ERROR: All loop slots already taken!");
		return;
	}
	char path[255] = {};
	game->data->audio.loops[i].name = strdup(name);
	snprintf(path, 255, "sounds/%s.flac.opus", name);
	PrintConsole(game, "Starting loop: %s", name);
	game->data->audio.loops[i].stream = al_load_audio_stream(GetDataFilePath(game, path), 4, 2048);
	game->data->audio.loops[i].persist = persist;
	al_attach_audio_stream_to_mixer(game->data->audio.loops[i].stream, game->audio.fx);
	al_set_audio_stream_playmode(game->data->audio.loops[i].stream, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(game->data->audio.loops[i].stream, volume);
	al_set_audio_stream_playing(game->data->audio.loops[i].stream, true);
}

void StopLoop(struct Game* game, char* name) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.loops[i].name && strcmp(name, game->data->audio.loops[i].name) == 0) {
			al_destroy_audio_stream(game->data->audio.loops[i].stream);
			free(game->data->audio.loops[i].name);
			game->data->audio.loops[i].stream = NULL;
			game->data->audio.loops[i].name = NULL;
			return;
		}
	}
}

void StopLoops(struct Game* game) {
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.loops[i].stream) {
			if (!game->data->audio.loops[i].persist) {
				al_destroy_audio_stream(game->data->audio.loops[i].stream);
				free(game->data->audio.loops[i].name);
				game->data->audio.loops[i].stream = NULL;
				game->data->audio.loops[i].name = NULL;
			}
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
	CollectStreams(game);
	if (game->data->audio.music) {
		al_set_audio_stream_playing(game->data->audio.music, false);
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].stream) {
			al_set_audio_stream_playing(game->data->audio.sounds[i].stream, false);
		}
		if (game->data->audio.loops[i].stream) {
			al_set_audio_stream_playing(game->data->audio.loops[i].stream, false);
		}
	}
	game->data->audio.paused = true;
}

void ResumeAudio(struct Game* game) {
	if (!game->data->audio.paused) {
		return;
	}
	if (game->data->audio.music) {
		al_set_audio_stream_playing(game->data->audio.music, true);
	}
	for (int i = 0; i < 32; i++) {
		if (game->data->audio.sounds[i].stream) {
			al_set_audio_stream_playing(game->data->audio.sounds[i].stream, true);
		}
		if (game->data->audio.loops[i].stream) {
			al_set_audio_stream_playing(game->data->audio.loops[i].stream, true);
		}
	}
	game->data->audio.paused = false;
}
