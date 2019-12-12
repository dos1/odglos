#include "common.h"
#include "defines.h"
#include <getopt.h>
#include <libsuperderpy.h>
#include <signal.h>
#include <stdio.h>

static _Noreturn void derp(int sig) {
	ssize_t __attribute__((unused)) n = write(STDERR_FILENO, "Segmentation fault\nI just don't know what went wrong!\n", 54);
	abort();
}

int main(int argc, char** argv) {
	signal(SIGSEGV, derp);

	srand(time(NULL));

	al_set_org_name("Holy Pangolin");
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
				.compositor = Compositor,
				.postdraw = DrawBuildInfo,
			},
		});

	if (!game) { return 1; }

	game->data = CreateGameData(game);

#ifndef __SWITCH__
	int ram = al_get_ram_size();
	game->data->lowmem = ram <= 2048;
	if (ram < 0 || IS_EMSCRIPTEN) {
		game->data->lowmem = false;
	}
	if (game->data->lowmem) {
		PrintConsole(game, "Detected low memory (%d)", ram);
	}
#endif
	game->data->lowmem = strtol(GetConfigOptionDefault(game, LIBSUPERDERPY_GAMENAME, "lowmem", game->data->lowmem ? "1" : "0"), NULL, 10);

	static struct option long_options[] =
		{
			{"lowmem", no_argument, 0, 'l'},
			{"nolowmem", no_argument, 0, 'n'},
			{0, 0, 0, 0}};
	int c;
	do {
		c = getopt_long_only(argc, argv, "ln", long_options, NULL);
		switch (c) {
			case 'l':
				game->data->lowmem = true;
				break;
			case 'n':
				game->data->lowmem = false;
				break;
		}
	} while (c != -1);

	if (game->data->lowmem) {
		PrintConsole(game, "Low memory mode enabled");
	} else {
		LoadGamestate(game, "logo");
		LoadGamestate(game, "anim");
		LoadGamestate(game, "myszka");
		LoadGamestate(game, "naparstki");
		LoadGamestate(game, "naparstki2");
		LoadGamestate(game, "end");
		LoadGamestate(game, "lawka");
		LoadGamestate(game, "byk");
		LoadGamestate(game, "blank");
		LoadGamestate(game, "pudelka");
		LoadGamestate(game, "pergola");
		LoadGamestate(game, "armata");
	}

	StartGamestate(game, "anim");

	al_hide_mouse_cursor(game->display);

	return libsuperderpy_run(game);
}
