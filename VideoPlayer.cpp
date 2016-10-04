// VideoPlayer.cpp : Defines the entry point for the console application.
//

#include "VideoPlayer.h"

int main()
{
	int rc;
	video_frames frames[READERS];

	reader_ctx rd_ctx[READERS];
	player_ctx pl_ctx = {
			.buffer = frames
		};

	pthread_t thr_read[2];
	pthread_t thr_player;

	frames[0].width = 0;
	frames[0].height = 0;
	frames[0].fps = 0;
	frames[0].ir = 0;
	frames[0].iw = 0;
	frames[0].count = 0;
	frames[0].eof = false;
	pthread_mutex_init(&frames[0].buffer_mutex, NULL);

	frames[1].width = 0;
	frames[1].height = 0;
	frames[1].fps = 0;
	frames[1].ir = 0;
	frames[1].iw = 0;
	frames[1].count = 0;
	frames[1].eof = false;
	pthread_mutex_init(&frames[1].buffer_mutex, NULL);

	rd_ctx[0].buffer = &frames[0];
	rd_ctx[0].filename = "3.mp4";


	rd_ctx[1].buffer = &frames[1];
	rd_ctx[1].filename = "2.mp4";

	rc = pthread_create(&thr_read[0], NULL, reader, &rd_ctx[0]);
	if (rc != 0) {
		perror("Error create thread");
		return rc;
	}

	rc = pthread_create(&thr_read[1], NULL, reader, &rd_ctx[1]);
	if (rc != 0) {
		perror("Error create thread");
		return rc;
	}

	rc = pthread_create(&thr_player, NULL, player, &pl_ctx);
	if (rc != 0) {
		perror("Error create thread");
		return rc;
	}

	pthread_join(thr_read[0], NULL);
	pthread_join(thr_read[1], NULL);
	pthread_join(thr_player, NULL);

	return 0;
}

