#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

void PlayMusic(struct Game* game, char* name, bool loop, bool layer) {
	if (game->data->music && !layer) {
		al_destroy_audio_stream(game->data->music);
		game->data->music = false;
		// TODO: leak
	}
	if (name[0]) {
		char path[255] = {};
		snprintf(path, 255, "sounds/%s.flac.opus", name);
		PrintConsole(game, "Starting music: %s", name);
		game->data->music = al_load_audio_stream(GetDataFilePath(game, path), 4, 2048);
		al_attach_audio_stream_to_mixer(game->data->music, game->audio.music);
		al_set_audio_stream_playing(game->data->music, true);
		al_set_audio_stream_playmode(game->data->music, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
	} else {
		PrintConsole(game, "Music stopped.");
	}
}
