#pragma once

#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <unistd.h>


using namespace cv;
using namespace std;

// see http://stackoverflow.com/questions/13675132/pthread-cond-wait-for-2-threads

/* Video ring buffer length */
#define MAX_FRAMES 10

/* Readers number */
/* todo change name */
#define READERS 2

/* Video ring buffer */
struct video_frames {
	int fps;
	int height;
	int width;
	int ir;
	int iw;
	int count;
	bool eof;
	Mat frames[MAX_FRAMES];
	pthread_mutex_t buffer_mutex;
};

struct reader_ctx {
	string filename;
	struct video_frames *buffer;
};

struct player_ctx {
	struct video_frames *buffer;
};

void *reader(void *arg);
void *player(void *arg);

