#include "VideoPlayer.h"
#include <sys/time.h>

void *player(void *arg)
{
	struct player_ctx *ctx = (struct player_ctx*) arg;
	int index[READERS];
	int i;
	Mat finalFrame;
	int count = 0;
	bool eofs = false;
	namedWindow("Video", WINDOW_AUTOSIZE);
	Mat square[READERS];
	int maxDur[READERS];
	int curDur[READERS];
	int duration;
	int finalHeigth;
	int finalWidth;
	int tempHeight;
	int tempWidth;


	// wait start loaded
	for (i = 0; i < READERS; i++){
		do {
			pthread_mutex_lock(&ctx->buffer[i].buffer_mutex);
			count = ctx->buffer[i].count;
			pthread_mutex_unlock(&ctx->buffer[i].buffer_mutex);
			usleep(10000);
		} while (count < MAX_FRAMES);
		tempHeight = ctx->buffer[i].height;
		if (finalHeigth < tempHeight)
			finalHeigth = tempHeight;
		tempWidth = ctx->buffer[i].width;
		if (finalWidth < tempWidth)
			finalWidth = tempWidth;
		maxDur[i] = 1000 / ctx->buffer[i].fps;
		curDur[i] = maxDur[i];
	}

	finalFrame = Mat (finalHeigth, finalWidth, CV_8UC3);

	for (i = 0; i < READERS; i++){
		square[i] = Mat (finalFrame, Rect(0, 0, ctx->buffer[i].width, ctx->buffer[i].height));
	}

	while (!eofs) {
		eofs = true;
		duration = maxDur[0];

		// get indexes
		for (i = 0;  i < READERS; i++)
			do {
				pthread_mutex_lock(&ctx->buffer[i].buffer_mutex);
				eofs &= ctx->buffer[i].eof && ctx->buffer[i].count == 0;
				index[i] = ctx->buffer[i].count > 0 ? ctx->buffer[i].ir : -1 ;
				pthread_mutex_unlock(&ctx->buffer[i].buffer_mutex);
				if (index[i] < 0 && !(ctx->buffer[i].eof))
					usleep(10000);
			} while (index[i] < 0 && !(ctx->buffer[i].eof));

		if (eofs) continue;

		for (i = 0; i < READERS ; i++)
			if (index[i] >= 0) {
				ctx->buffer[i].frames[index[i]].copyTo(square[i]);
				duration = min(curDur[i], duration);
			}


		imshow ("Video", finalFrame);
		waitKey(duration);

		// update indexes
		for (i = 0;  i < READERS; i++) {
			if (duration == curDur[i]){
				pthread_mutex_lock(&ctx->buffer[i].buffer_mutex);
				if (!ctx->buffer[i].eof) {
					--(ctx->buffer[i].count);
					ctx->buffer[i].ir = (index[i] + 1) % MAX_FRAMES;
				}
				pthread_mutex_unlock(&ctx->buffer[i].buffer_mutex);
				curDur[i] = maxDur[i];
			} else {
				curDur[i] -= duration;
			}
		}
	}
	return NULL;
}

