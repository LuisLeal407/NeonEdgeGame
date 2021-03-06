#ifndef SOUND_H_
#define SOUND_H_

#include "Resources.h"

class Sound {
private:
	Mix_Chunk* chunk;
	int channel;

public:
	Sound();
	Sound(std::string file);
	~Sound();
	void Open(std::string file);
	void Play(int times);
	void Stop();
	bool IsOpen();
};

#endif /* SOUND_H_ */
