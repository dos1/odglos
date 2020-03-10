#include "common.h"
#include "defines.h"
#include <libsuperderpy.h>

#ifndef __SWITCH__
#include <sys/mman.h>
#endif
#include <sys/stat.h>

#include <webp/demux.h>

struct AnimationDecoder {
	WebPData data;
	int timestamp, position, duration, swap_duration, old_timestamp;
	WebPAnimDecoder* decoder;
	ALLEGRO_BITMAP *bitmap, *swap;
	int fd;
	uint8_t* buf;
	bool mmaped, shouldload;
	int frame;
	bool done;
	bool initialized;
	int width, height;
	int frames;
	char* name;
	bool repeat;
	struct Game* game;
};

struct AnimationDecoder* CreateAnimation(struct Game* game, const char* filename, bool repeat) {
	struct AnimationDecoder* anim = calloc(1, sizeof(struct AnimationDecoder));

	WebPDataInit(&anim->data);

	anim->game = game;

	anim->fd = open(filename, O_RDONLY | O_CLOEXEC);

	if (!anim->fd) {
		free(anim);
		return NULL;
	}

	struct stat s;
	fstat(anim->fd, &s);
	anim->data.size = s.st_size;

	anim->buf = NULL;
#ifndef __SWITCH__
	anim->buf = mmap(0, anim->data.size, PROT_READ, MAP_SHARED, anim->fd, 0);
#endif

	if (!anim->buf) {
		anim->buf = malloc(anim->data.size * sizeof(uint8_t));

		if (read(anim->fd, anim->buf, anim->data.size) != (ssize_t)anim->data.size) {
			free(anim->buf);
			close(anim->fd);
			free(anim);
			return NULL;
		}
		close(anim->fd);
		anim->mmaped = false;
	} else {
		anim->mmaped = true;
	}
	anim->data.bytes = anim->buf;

	// TODO: handle invalid file path

	WebPAnimDecoderOptions dec_options;
	WebPAnimDecoderOptionsInit(&dec_options);
	dec_options.color_mode = MODE_rgbA;
	dec_options.use_threads = !IS_EMSCRIPTEN;

	anim->decoder = WebPAnimDecoderNew(&anim->data, &dec_options);

	WebPAnimInfo anim_info;
	WebPAnimDecoderGetInfo(anim->decoder, &anim_info);

	anim->position = -1;
	anim->frame = -1;
	anim->initialized = false;

	anim->width = anim_info.canvas_width;
	anim->height = anim_info.canvas_height;

	anim->frames = anim_info.frame_count;

	anim->repeat = repeat;

	ALLEGRO_PATH* path = al_create_path(filename);
	anim->name = strdup(al_get_path_basename(path));
	al_destroy_path(path);

	PrintConsole(anim->game, "[AnimationStream] Created: %s", anim->name);

	return anim;
}

void ResetAnimation(struct AnimationDecoder* anim) {
	if (anim->position == 0) {
		return;
	}
	if (!anim->initialized) {
		anim->bitmap = al_create_bitmap(anim->width, anim->height);
		anim->swap = al_create_bitmap(anim->width, anim->height);
		//PrintConsole(anim->game, "[AnimationStream] Initialized: %s", anim->name);
	}
	WebPAnimDecoderReset(anim->decoder);

	ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(anim->bitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
	uint8_t* buf;
	WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp);
	for (int i = 0; i < al_get_bitmap_height(anim->bitmap); i++) {
		memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->bitmap) * al_get_pixel_size(lock->format),
			al_get_bitmap_width(anim->bitmap) * lock->pixel_size);
	}
	al_unlock_bitmap(anim->bitmap);

	anim->duration = anim->timestamp;
	anim->old_timestamp = anim->timestamp;

	anim->shouldload = true;

	anim->position = 0;
	anim->frame = 0;
	anim->done = false;
	anim->initialized = true;
}

bool UpdateAnimation(struct AnimationDecoder* anim, float timestamp) {
	if (!anim->initialized) {
		ResetAnimation(anim);
	}

	anim->position += timestamp * 1000;
	ALLEGRO_LOCKED_REGION* lock = NULL;

	if (anim->shouldload) {
		anim->shouldload = false;
		if (!WebPAnimDecoderHasMoreFrames(anim->decoder)) {
			if (anim->repeat) {
				ResetAnimation(anim);
				return true;
			}
			anim->done = true;
			return false;
		}
		uint8_t* buf;
		if (WebPAnimDecoderGetNext(anim->decoder, &buf, &anim->timestamp)) {
			lock = al_lock_bitmap(anim->swap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY);
			for (int i = 0; i < al_get_bitmap_height(anim->swap); i++) {
				memcpy(lock->data + i * lock->pitch, buf + i * al_get_bitmap_width(anim->swap) * al_get_pixel_size(lock->format),
					al_get_bitmap_width(anim->swap) * lock->pixel_size);
			}
			al_unlock_bitmap(anim->swap);
			anim->swap_duration = anim->timestamp - anim->old_timestamp;
		}
	}

	if (anim->position >= anim->old_timestamp) {
		if (!anim->done) {
			ALLEGRO_BITMAP* bmp = anim->bitmap;
			anim->bitmap = anim->swap;
			anim->swap = bmp;
			anim->shouldload = true;
			anim->old_timestamp = anim->timestamp;
			anim->frame++;
			anim->duration = anim->swap_duration;
		}

		if (!anim->repeat && !WebPAnimDecoderHasMoreFrames(anim->decoder)) {
			return false;
		}
	}

	return true;
}

ALLEGRO_BITMAP* GetAnimationFrame(struct AnimationDecoder* anim) {
	if (!anim->initialized) {
		ResetAnimation(anim);
	}
	return anim->bitmap;
}

int GetAnimationFrameNo(struct AnimationDecoder* anim) {
	return anim->frame;
}

const char* GetAnimationName(struct AnimationDecoder* anim) {
	return anim->name;
}

int GetAnimationFrameCount(struct AnimationDecoder* anim) {
	return anim->frames;
}

float GetAnimationFrameDuration(struct AnimationDecoder* anim) {
	if (!anim->initialized) {
		ResetAnimation(anim);
	}
	return (float)anim->duration / 1000.0;
}

bool IsAnimationComplete(struct AnimationDecoder* anim) {
	return anim->done;
}

void DestroyAnimation(struct AnimationDecoder* anim) {
	//PrintConsole(anim->game, "[AnimationStream] Destroying: %s", anim->name);
#ifndef __SWITCH__
	if (anim->mmaped) {
		munmap(anim->buf, anim->data.size);
		close(anim->fd);
		anim->data.bytes = NULL;
	}
#endif
	WebPDataClear(&anim->data);
	WebPAnimDecoderDelete(anim->decoder);
	if (anim->initialized) {
		al_destroy_bitmap(anim->swap);
		al_destroy_bitmap(anim->bitmap);
	}
	free(anim->name);
	free(anim);
}

SPRITESHEET_STREAM_DESCTRUCTOR(DestroyStream) {
	DestroyAnimation(data);
}

SPRITESHEET_STREAM(AnimationStream) {
	bool complete = !UpdateAnimation(data, delta);
	ALLEGRO_BITMAP* bitmap = al_clone_bitmap(GetAnimationFrame(data));
	double duration = GetAnimationFrameDuration(data);
	int frame = GetAnimationFrameNo(data);
	//PrintConsole(game, "STREAM: frame %d duration %f delta %f", frame, GetAnimationFrameDuration(data), delta);
	if (complete) {
		PrintConsole(game, "[AnimationStream] %s: complete", GetAnimationName(data));
		ResetAnimation(data);
	}
	return (struct SpritesheetFrame){
		.bitmap = bitmap,
		.duration = duration * 1000,
		.tint = al_map_rgb(255, 255, 255),
		.row = 1,
		.col = 1,
		.x = 0,
		.y = 0,
		.sx = 0,
		.sy = 0,
		.sw = al_get_bitmap_width(bitmap),
		.sh = al_get_bitmap_height(bitmap),
		.flipX = false,
		.flipY = false,
		.start = frame == 0,
		.end = complete,
		.shared = false,
	};
}
