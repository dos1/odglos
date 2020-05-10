#include "common.h"
#include "defines.h"
#include <getopt.h>
#include <libsuperderpy.h>
#include <signal.h>
#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <allegro5/allegro_physfs.h>
#include <physfs.h>
#endif

static _Noreturn void derp(int sig) {
	ssize_t __attribute__((unused)) n = write(STDERR_FILENO, "Segmentation fault\nI just don't know what went wrong!\n", 54);
	abort();
}

int main(int argc, char** argv) {
	signal(SIGSEGV, derp);

	srand(time(NULL));

	al_set_org_name(LIBSUPERDERPY_VENDOR);
	al_set_app_name(LIBSUPERDERPY_GAMENAME_PRETTY);

	struct Game* game = libsuperderpy_init(argc, argv, LIBSUPERDERPY_GAMENAME,
		(struct Params){
			1280,
			720,
			.show_loading_on_launch = true,
			.fixed_size = IS_EMSCRIPTEN,
			.handlers = (struct Handlers){
				.event = GlobalEventHandler,
				.destroy = DestroyGameData,
				.prelogic = PreLogic,
				.postlogic = PostLogic,
				.compositor = Compositor,
				.postdraw = DrawBuildInfo,
			},
		});

	if (!game) { return 1; }

#ifdef __EMSCRIPTEN__
	PHYSFS_init(argv[0]);
	PHYSFS_mount(".", NULL, true);
	al_set_physfs_file_interface();
#endif

	game->data = CreateGameData(game);

	LoadGamestate(game, "logo");
	LoadGamestate(game, "anim");
	LoadGamestate(game, "myszka");
	LoadGamestate(game, "end");
	LoadGamestate(game, "blank");
	LoadGamestate(game, "headphones");
#ifndef __EMSCRIPTEN__
	LoadGamestate(game, "byk");
	LoadGamestate(game, "naparstki");
	LoadGamestate(game, "lawka");
	LoadGamestate(game, "pudelka");
	LoadGamestate(game, "pergola");
	LoadGamestate(game, "armata");
#endif

	RequestPack(game, game->data->sceneid + 1);
	StartDownloading(game);

#ifdef __EMSCRIPTEN__
	StartGamestate(game, "downloader");
#else
	StartInitialGamestate(game, false);
#endif

	SetupSoundCache(game);
#ifdef __EMSCRIPTEN__
	PrintConsole(game, "Caching sounds...");
	int i = 0;
	while (CacheNextSound(game)) {
		i++;
		if (i == 10) {
			emscripten_sleep(0);
			i = 0;
		}
	}
	PrintConsole(game, "Sounds cache ready.");
#endif

	al_hide_mouse_cursor(game->display);

	return libsuperderpy_run(game);
}
