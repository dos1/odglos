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

static char* PACK_POSTLOAD[DOWNLOAD_PARTS][3] = {
	{"byk"},
	{"lawka"},
	{},
	{"pergola"},
	{"pudelka"},
	{},
	{},
	{"naparstki"},
	{"armata"},
	{},
	{},
};

void MountDataPacks(struct Game* game) {
	for (int i = 0; i < DOWNLOAD_PARTS; i++) {
		if (game->data->download.pack[i].mounted || !game->data->download.pack[i].loaded) {
			continue;
		}
#ifdef __EMSCRIPTEN__
		char* filename = PunchNumber(game, "dataXX.zip", 'X', i);
		PrintConsole(game, "[%d] Mounting %s...", i, filename);
		PHYSFS_mountMemory(game->data->download.pack[i].data, game->data->download.pack[i].size, free, filename, "/data", false);
#endif
		int j = 0;
		while (PACK_POSTLOAD[i][j]) {
			LoadGamestate(game, PACK_POSTLOAD[i][j]);
			j++;
		}
		game->data->download.pack[i].mounted = true;
	}
}

float GetDownloadProgress(struct Game* game) {
	int n = 0;
	float val = 0.0;
	for (int i = 0; i <= game->data->download.requested; i++) {
		val += game->data->download.pack[i].progress;
		n += 1;
	}
	val /= (float)n;
	return val;
}

#ifdef __EMSCRIPTEN__
static void DownloadNextPack(struct Game* game);

static void DownloadOnLoad(unsigned handle, void* d, void* buf, unsigned int size) {
	struct LoadingData* data = d;

	PrintConsole(data->game, "[%d] Pack %s downloaded (%d)", handle, data->name, data->number);
	data->game->data->download.pack[data->number].data = buf;
	data->game->data->download.pack[data->number].size = size;
	data->game->data->download.pack[data->number].loaded = true;
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
	data->game->data->download.pack[data->number].progress = loaded / (double)total;
	SetHTMLLoadingValue(data->game, GetDownloadProgress(data->game));
}
#endif

static void DownloadNextPack(struct Game* game) {
	int current = 0;
	while (game->data->download.pack[current].loaded) {
		current++;
		if (current >= DOWNLOAD_PARTS) {
			return;
		}
	}

	game->data->download.pack[current].requested = true;
#ifdef __EMSCRIPTEN__
	struct LoadingData* d = malloc(sizeof(struct LoadingData));
	d->game = game;
	d->name = strdup(PunchNumber(game, "dataXX.zip", 'X', current));
	d->number = current;
	int handle = emscripten_async_wget2_data(d->name, "GET", "", d, false, DownloadOnLoad, DownloadOnError, DownloadOnProgress);
	PrintConsole(game, "Started downloading %s: %d", d->name, handle);
#else
	game->data->download.pack[current].loaded = true;
	game->data->download.pack[current].mounted = true;
	game->data->download.pack[current].progress = 1.0;
	DownloadNextPack(game);
#endif
}

void StartDownloading(struct Game* game) {
	if (!game->data->download.pack[0].requested) {
		DownloadNextPack(game);
	}
}
