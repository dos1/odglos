#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

#ifdef __EMSCRIPTEN__
#define IS_EMSCRIPTEN 1
#else
#define IS_EMSCRIPTEN 0
#endif

struct CommonResources {
	// Fill in with common data accessible from all gamestates.
	ALLEGRO_SHADER* grain;
	double mouseX, mouseY;
	char* next;
	bool cursor;
	bool hover;
	double start_time;
	ALLEGRO_BITMAP *cursorbmp, *cursorhover, *gradient;
	bool lowmem;
	bool pause;

	int animationid;
	char* animation;

	ALLEGRO_FONT* font;
};

bool Dispatch(struct Game* game);

struct AnimationDecoder* CreateAnimation(const char* filename);
bool UpdateAnimation(struct AnimationDecoder* anim, float timestamp);
void DestroyAnimation(struct AnimationDecoder* anim);
ALLEGRO_BITMAP* GetAnimationFrame(struct AnimationDecoder* anim);
float GetAnimationFrameDuration(struct AnimationDecoder* anim);
int GetAnimationFrameNo(struct AnimationDecoder* anim);
const char* GetAnimationName(struct AnimationDecoder* anim);
void ResetAnimation(struct AnimationDecoder* anim);
bool IsAnimationComplete(struct AnimationDecoder* anim);

void DrawBuildInfo(struct Game* game);
void SwitchScene(struct Game* game, char* name);
void PreLogic(struct Game* game, double delta);
ALLEGRO_COLOR CheckMask(struct Game* game, ALLEGRO_BITMAP* bitmap);
void DrawTexturedRectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color);
struct CommonResources* CreateGameData(struct Game* game);
void DestroyGameData(struct Game* game);
bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev);
void Compositor(struct Game* game);
void ShowMouse(struct Game* game);
void HideMouse(struct Game* game);

SPRITESHEET_STREAM_DESCTRUCTOR(DestroyStream);
SPRITESHEET_STREAM(AnimationStream);
