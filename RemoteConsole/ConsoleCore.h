#ifndef CONSOLE_CORE_HPP
#define CONSOLE_CORE_HPP

#include <windows.h>
#include <vector>
#include <stdexcept>

class ConsoleCore
{
public:
	ConsoleCore() 
		: _hIN{ GetStdHandle(STD_INPUT_HANDLE) }
		, _hOUT{ GetStdHandle(STD_OUTPUT_HANDLE) }
	{
		DWORD conMode{ 0 };

		if (!GetConsoleMode(_hIN, &conMode)) {
			throw std::runtime_error{ "Error : GetConsoleMode" };
		}
		conMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(_hIN, conMode)) {
			throw std::runtime_error{ "Error : SetConsoleMode" };
		}
	}

    void UpdateHandles();

	void UpdateReadingInfo();

	std::vector<CHAR_INFO> ReadOutputFromConsole();

	std::vector<INPUT_RECORD> ReadInputFromConsole();

	void WriteOutputToConsole(std::vector<CHAR_INFO> &outBuff);

	void WriteInputToConsole(std::vector<INPUT_RECORD> &inBuff);

	void SetConsoleSize(COORD sz);

	void SetCursorPos(COORD pos);

	void SetWindow(COORD upLeft, COORD lwRight);

	COORD GetBufferSize();

	COORD GetCursorPosition();

private:
	CONSOLE_SCREEN_BUFFER_INFO _buffInfo;
	COORD _buffSize, _buffCoord;
	SMALL_RECT _readSpace;

	HANDLE _hIN, _hOUT;
};

#endif // CONSOLE_CORE_HPP

