#include <Misc\Sound\NoiseMachine.h>

using namespace Logic;

// Initializes all sound system & sounds into memory
void NoiseMachine::init(SoundSettings settings)
{
	// Saves the settings
	m_settings = settings;

	// Creates a empty system
	FMOD_RESULT result;
	ERRCHECK(FMOD::System_Create(&m_system));

	// Get version & gets the hell out if not correct
	unsigned int version;
	result = m_system->getVersion(&version);
	ERRCHECK(result);
	if (version < FMOD_VERSION)
		CRASH_EVERYTHING("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);

	// Initialize the FMOD system
	void* extraDriverData = nullptr;
	ERRCHECK(m_system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, extraDriverData));
	ERRCHECK(result);

	// Specific settings of the system
//	m_system->set3DSettings();

	// Initialize sounds
	initSFX();
	initMusic();
}

// Clears sound system & sounds from memory
void NoiseMachine::clear()
{
	// Release sounds
	for (int i = 0; i < MAX_SFX; i++)
	{
		if (m_sfx[i] != nullptr)
		{
			ERRCHECK(m_sfx[i]->data->release());
			delete m_sfx[i];
			m_sfx[i] = nullptr;
		}
	}

	// Release music
	for (int i = 0; i < MAX_SONGS; i++)
	{
		if (m_music[i] != nullptr)
		{
			ERRCHECK(m_music[i]->data->release());
			delete m_music[i];
			m_music[i] = nullptr;
		}
	}

	// Release system
	ERRCHECK(m_system->close());
	ERRCHECK(m_system->release());
	m_system = nullptr;
}

// Prints out information about the current playing channels (Just for debugging, can be removed, will work without it)
void NoiseMachine::update(ListenerData& listener)
{
	// Sets the new listener "ears"
	ERRCHECK(m_system->set3DListenerAttributes(listener.id, &listener.pos, &listener.vel, &listener.forward, &listener.up));

	// Update the 3D sound engine
	ERRCHECK(m_system->update());

	/* 
		Remove this
	*/
	DEBUG_SOUND(m_sfx[SFX::TEST]);
}

// Play a specific sound effect
void NoiseMachine::playSFX(SFX sfx, bool overdrive)
{
	play(m_sfx[sfx], overdrive);
}

// Play a specific music piece
void NoiseMachine::playMusic(MUSIC music, bool overdrive)
{
	play(m_music[music], overdrive);
}

void NoiseMachine::play(Sound * sound, bool overdrive)
{
	if (overdrive)
		ERRCHECK(m_system->playSound(sound->data, 0, false, &sound->channel));
	else
	{
		bool playing = false;

		// Checking if channel is currently playing something
		if (sound->channel)
			ERRCHECK(sound->channel->isPlaying(&playing));

		// If not playing sound, play sound
		if (!playing)
			ERRCHECK(m_system->playSound(sound->data, 0, false, &sound->channel));
	}
}

// Initialize all sound effects
void NoiseMachine::initSFX()
{
	// Setting full array as nullptr
	for (int i = 0; i < MAX_SFX; i++) m_sfx[i] = nullptr;

	ERRCHECK(createSound(SFX::TEST, CHANNEL_GROUP::CHANNEL_SFX, "test.ogg", FMOD_3D));
}

// Initialize all music pieces
void NoiseMachine::initMusic()
{
	// Setting full array as nullptr
	for (int i = 0; i < MAX_SONGS; i++) m_music[i] = nullptr;

}

// Allocates a specific sound effect into memory
FMOD_RESULT NoiseMachine::createSound(SFX sfx, CHANNEL_GROUP group, std::string path, FMOD_MODE mode)
{
	m_sfx[sfx] = newd Sound();
	FMOD_RESULT result = m_system->createSound(std::string(SOUND_SFX_PATH + path).c_str(), mode, 0, &m_sfx[sfx]->data);
	m_sfx[sfx]->group = group;

	return result;
}

// Allocates a specific music piece into memory
FMOD_RESULT NoiseMachine::createSound(MUSIC music, CHANNEL_GROUP group, std::string path, FMOD_MODE mode)
{
	m_music[music] = newd Sound();
	FMOD_RESULT result = m_system->createSound(std::string(SOUND_MUSIC_PATH + path).c_str(), mode, 0, &m_music[music]->data);
	m_music[music]->group = group;

	return result;
}

void NoiseMachine::DEBUG_SOUND(Sound* sound)
{
	if (sound->channel)
	{
		FMOD_RESULT		result;
		FMOD::Sound*	currentsound = 0;
		bool			playing = 0;
		bool			paused = 0;

		// Setting settings
		sound->setVolume(m_settings.volume[sound->group]);
		sound->setPitch(m_settings.pitch[sound->group]);

		// Checks if channel is playing
		result = sound->channel->isPlaying(&playing);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		// Checks if channel is pausing 
		result = sound->channel->getPaused(&paused);
		if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		{
			ERRCHECK(result);
		}

		printf("FMOD_Info - IsPlaying: %d, IsPausing: %d, Current Sound Playing: %p.\n", playing, paused, sound->data);
	}
}

void NoiseMachine::CRASH_EVERYTHING(const char * format, ...)
{
	printf(format);
	exit(1);
}

void NoiseMachine::ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}
}