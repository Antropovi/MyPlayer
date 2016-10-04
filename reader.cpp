#include "VideoPlayer.h"


void *reader(void *arg)
{
	struct reader_ctx *ctx = (struct reader_ctx*) arg;
	int index;
	int i;
	int count = 0;

	VideoCapture cap(ctx->filename);
	if(!cap.isOpened()){  // check if we succeeded
		ctx->buffer->eof = true;
		return NULL;
	}

	int amountOfFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	ctx->buffer->height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	ctx->buffer->width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	ctx->buffer->fps = cap.get(CV_CAP_PROP_FPS);

	while (i < amountOfFrames) {
		count = 0;
		while (count < MAX_FRAMES && i < amountOfFrames) {
			pthread_mutex_lock(&ctx->buffer->buffer_mutex);
			index = ctx->buffer->iw;
			count = ctx->buffer->count;
			pthread_mutex_unlock(&ctx->buffer->buffer_mutex);

			if (count == MAX_FRAMES)
				continue;

			cap >> ctx->buffer->frames[index];
			pthread_mutex_lock(&ctx->buffer->buffer_mutex);
			ctx->buffer->iw = (index + 1) % MAX_FRAMES;
			++(ctx->buffer->count);
			pthread_mutex_unlock(&ctx->buffer->buffer_mutex);

			i++;
		}

		usleep(1000*1000 / ctx->buffer->fps);
	}

	pthread_mutex_lock(&ctx->buffer->buffer_mutex);
	ctx->buffer->eof = true;
	pthread_mutex_unlock(&ctx->buffer->buffer_mutex);

	return NULL;
}

