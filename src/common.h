#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

#ifdef __EMSCRIPTEN__
#define IS_EMSCRIPTEN 1
#else
#define IS_EMSCRIPTEN 0
#endif

#define DOWNLOAD_PARTS 11

enum AUDIO_TYPE {
	NO_AUDIO = 0,
	SOUND,
	MUSIC,
	LOOP,
	STOP_LOOP,
	STOP_MUSIC,
	STOP_SOUND,
	ENSURE_MUSIC
};

struct Audio {
	enum AUDIO_TYPE type;
	char* name;
	bool persist;
	float volume;
	bool stop_music;
};

struct FreezeLink {
	ALLEGRO_COLOR color;
	char* name;
	bool (*callback)(struct Game*, struct Character*, void**);
	bool ignore;
	bool skip;
	struct Audio audio;
};

struct FreezeFrame {
	int frame;
	char* mask;
	int footnote;
	bool skip;
	struct FreezeLink links[8];
	bool (*callback)(struct Game*, struct Character*, void**);
	struct Audio audio, pre_audio;
};

struct AudioFrame {
	int frame;
	struct Audio audio;
};

struct SceneDefinition {
	char* name;
	char* bg;
	char* fg;
	float speed;
	int repeats;
	int pack;
	int length;
	ALLEGRO_COLOR color;
	bool checkpoint;
	bool (*callback)(struct Game*, int, int*, int*, double*, struct Character*, void**);
	void (*draw)(struct Game*, int, void**);
	void* callback_data;

	struct {
		char* name;
		char* spritesheets[8];
		bool preload;
		bool hidden;
		bool repeat;
	} character;

	struct FreezeFrame freezes[64];
	struct AudioFrame sounds[64];

	struct Audio audio;
};

struct CommonResources {
	// Fill in with common data accessible from all gamestates.
	ALLEGRO_SHADER* grain;
	double mouseX, mouseY;
	bool cursor;
	bool hover;
	double start_time;
	ALLEGRO_BITMAP *cursorbmp, *cursorhover, *gradient;
	bool pause;
	int debuginfo;

	int sceneid;
	struct SceneDefinition scene;
	struct SceneDefinition queue[64];
	int queue_pos;
	int queue_handled;

	ALLEGRO_FONT *font, *creditsfont;
	ALLEGRO_BITMAP *banner, *menu, *menu2, *mute;

	bool footnote;
	bool menu_requested;
	bool skip_requested;
	bool skip_available;

	bool touch;
	bool toolbox;
	bool force_anim_reload;

	bool w, a, s, d;

	bool dark_loading;

	char search[255];

	struct {
		float progress[DOWNLOAD_PARTS];
		bool loaded[DOWNLOAD_PARTS];
		bool started[DOWNLOAD_PARTS];
		int requested;
		bool additional;
	} download;

	struct {
		ALLEGRO_SAMPLE_INSTANCE* music;
		ALLEGRO_SAMPLE* music_sample;
		double music_pos;
		char* music_name;
		struct {
			ALLEGRO_SAMPLE_INSTANCE* sample_instance;
			ALLEGRO_SAMPLE* sample;
			double pos;
			char* name;
			bool persist;
		} loops[32];
		struct {
			ALLEGRO_SAMPLE_INSTANCE* sample_instance;
			ALLEGRO_SAMPLE* sample;
			double pos;
			char* name;
		} sounds[32];
		bool paused;
	} audio;
};

void StartDownloadPacks(struct Game* game);
void RequestPack(struct Game* game, int sceneid);
bool Dispatch(struct Game* game);
void Enqueue(struct Game* game, struct SceneDefinition scene);
void StartInitialGamestate(struct Game* game, bool show_menu);
void DrawSceneToolbox(struct Game* game);

struct AnimationDecoder* CreateAnimation(struct Game* game, const char* filename, bool repeat);
bool UpdateAnimation(struct AnimationDecoder* anim, float timestamp);
void DestroyAnimation(struct AnimationDecoder* anim);
ALLEGRO_BITMAP* GetAnimationFrame(struct AnimationDecoder* anim);
float GetAnimationFrameDuration(struct AnimationDecoder* anim);
int GetAnimationFrameNo(struct AnimationDecoder* anim);
const char* GetAnimationName(struct AnimationDecoder* anim);
void ResetAnimation(struct AnimationDecoder* anim, bool reset_bitmap);
bool IsAnimationComplete(struct AnimationDecoder* anim);
int GetAnimationFrameCount(struct AnimationDecoder* anim);

void UnsetSkip(struct Game* game);
void DrawBuildInfo(struct Game* game);
void PreLogic(struct Game* game, double delta);
void PostLogic(struct Game* game, double delta);
ALLEGRO_COLOR CheckMask(struct Game* game, ALLEGRO_BITMAP* bitmap);
void DrawTexturedRectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color);
struct CommonResources* CreateGameData(struct Game* game);
void DestroyGameData(struct Game* game);
bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev);
void Compositor(struct Game* game);
void ShowMouse(struct Game* game);
void HideMouse(struct Game* game);
#ifdef __EMSCRIPTEN__
void HideHTMLLoading(struct Game* game);
void SetHTMLLoadingValue(struct Game* game, float value);
#endif
void ShowFootnote(struct Game* game, int id);
void HideFootnote(struct Game* game);
void ShowMenu(struct Game* game);

SPRITESHEET_STREAM_DESCTRUCTOR(DestroyStream);
SPRITESHEET_STREAM(AnimationStream);

void PlayMusic(struct Game* game, char* name, float volume);
void StopMusic(struct Game* game);
void EnsureMusic(struct Game* game, char* name, float volume);
void PlaySound(struct Game* game, char* name, float volume);
void StopSound(struct Game* game, char* name);
void StopSounds(struct Game* game);
void PlayLoop(struct Game* game, char* name, float volume, bool persist);
void StopLoop(struct Game* game, char* name);
void StopLoops(struct Game* game);
void HandleAudio(struct Game* game, struct Audio audio);
void PauseAudio(struct Game* game);
void ResumeAudio(struct Game* game);

void StartDownloading(struct Game* game);

struct Player* CreatePlayer(struct Game* game);
void DestroyPlayer(struct Game* game, struct Player* player);
void LoadPlayerAnimation(struct Game* game, struct Player* player, struct SceneDefinition* scene);
bool UpdatePlayer(struct Game* game, struct Player* player, double delta);
void DrawPlayer(struct Game* game, struct Player* player);
void ProcessPlayerEvent(struct Game* game, struct Player* player, ALLEGRO_EVENT* ev);
bool PlayerIsFinished(struct Game* game, struct Player* player);
void SkipPlayerAnim(struct Game* game, struct Player* player);
