#ifndef __RE_SYMBOLS_H__
#define __RE_SYMBOLS_H__

#include "RE_Internal Header.hpp"

namespace RE {

#define CHAR_TEXTURE_COUNT 257
#define CHAR_TEXTURE_SIZE 8
	extern const uint8_t aau8CharacterTextures[CHAR_TEXTURE_COUNT][CHAR_TEXTURE_SIZE * CHAR_TEXTURE_SIZE];

#define WINDOW_BUTTON_TEXTURE_COUNT 3
#define WINDOW_BUTTON_TEXTURE_SIZE 16
	extern const uint8_t aau8WindowButtonTextures[WINDOW_BUTTON_TEXTURE_COUNT][WINDOW_BUTTON_TEXTURE_SIZE * WINDOW_BUTTON_TEXTURE_SIZE];

}

#endif /* __RE_SYMBOLS_H__ */
