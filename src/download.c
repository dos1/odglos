#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>
#ifdef __EMSCRIPTEN__
#include <physfs.h>
#endif

struct LoadingData {
	struct Game* game;
	char* name;
	int number;
};

#ifdef __EMSCRIPTEN__
static void DownloadNextPack(struct Game* game);

static void DownloadOnLoad(unsigned handle, void* d, void* buf, unsigned size) {
	struct LoadingData* data = d;

	PrintConsole(data->game, "[%d] Mounting %s... (%d)", handle, data->name, data->number);
	PHYSFS_mountMemory(buf, size, free, data->name, "/data", false);
	data->game->data->download.loaded[data->number] = true;
	DownloadNextPack(data->game);
	free(data->name);
	free(data);
}

static void DownloadOnError(unsigned handle, void* d, int error, const char* err) {
	struct LoadingData* data = d;
	DownloadNextPack(data->game);
}

static void DownloadOnProgress(unsigned handle, void* d, int loaded, int total) {
	struct LoadingData* data = d;
	data->game->data->download.progress[data->number] = loaded / (double)total;
}
#endif

static void DownloadNextPack(struct Game* game) {
	int current = 0;
	while (game->data->download.loaded[current]) {
		current++;
		if (current >= DOWNLOAD_PARTS) {
			return;
		}
	}

#ifdef __EMSCRIPTEN__
	struct LoadingData* d = malloc(sizeof(struct LoadingData));
	d->game = game;
	d->name = strdup(PunchNumber(game, "dataXX.zip", 'X', current));
	d->number = current;
	int handle = emscripten_async_wget2_data(d->name, "GET", "", d, false, DownloadOnLoad, DownloadOnError, DownloadOnProgress);
	PrintConsole(game, "Started downloading %s: %d", d->name, handle);
#else
	game->data->download.loaded[current] = true;
	game->data->download.started[current] = true;
	game->data->download.progress[current] = 1.0;
	DownloadNextPack(game);
#endif
}

void StartDownloading(struct Game* game) {
	DownloadNextPack(game);
}
