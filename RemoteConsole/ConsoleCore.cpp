#include "ConsoleCore.h"

void ConsoleCore::UpdateHandles()
{
    _hIN = GetStdHandle(STD_INPUT_HANDLE);
    _hOUT = GetStdHandle(STD_OUTPUT_HANDLE);
}

void ConsoleCore::UpdateReadingInfo()
{
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
	GetConsoleScreenBufferInfo(_hOUT, &buffInfo);
	
	_buffSize = { buffInfo.dwSize.X, buffInfo.dwSize.Y };
	_buffCoord = { 0, 0 };
	_readSpace = { 0, 0, buffInfo.dwSize.X, buffInfo.dwSize.Y };
}

std::vector<CHAR_INFO> ConsoleCore::ReadOutputFromConsole()
{
	std::vector<CHAR_INFO> outBuff(_buffSize.X * _buffSize.Y);

	if (!ReadConsoleOutput( _hOUT,
							outBuff.data(),
							_buffSize,
							_buffCoord,
							&_readSpace))
	{
		throw std::runtime_error{ "Error : ReadConsoleOutput" };
	}

	return outBuff;
}

std::vector<INPUT_RECORD> ConsoleCore::ReadInputFromConsole()
{
	DWORD numOfInputEvents{ 0 };
	DWORD numOfReadEvents{ 0 };

	if (!GetNumberOfConsoleInputEvents(_hIN, &numOfInputEvents)) {
		throw std::runtime_error{ "Error : GetNumberOfConsoleInputEvents" };
	}

    std::vector<INPUT_RECORD> tmpBuff(numOfInputEvents, INPUT_RECORD{ 0 });

	if (numOfInputEvents != 0) {
		if (!ReadConsoleInput(  _hIN,
								tmpBuff.data(),
								numOfInputEvents,
								&numOfReadEvents))
		{
			throw std::runtime_error{ "Error : ReadConsoleInput" };
		}
	}

	return tmpBuff;
}

void ConsoleCore::WriteOutputToConsole(std::vector<CHAR_INFO> &outBuff)
{
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
	GetConsoleScreenBufferInfo(_hOUT, &buffInfo);

	if (_buffSize.X > buffInfo.dwSize.X ||
		_buffSize.Y > buffInfo.dwSize.Y)
		return;

	if (!WriteConsoleOutput(_hOUT,
							outBuff.data(),
							_buffSize,
							_buffCoord,
							&_readSpace))
	{
		throw std::runtime_error{ "Error : WriteConsoleOutput" };
	}
}

void ConsoleCore::WriteInputToConsole(std::vector<INPUT_RECORD>& inBuff)
{
	DWORD  numOfEventsWritten{ 0 };

	if (!WriteConsoleInput( _hIN,
							inBuff.data(),
							static_cast<DWORD>(inBuff.size()),
							&numOfEventsWritten))
	{
		throw std::runtime_error{ "Error : WriteConsoleInput" };
	}
}

void ConsoleCore::SetConsoleSize(COORD sz)
{
	if (!SetConsoleScreenBufferSize(_hOUT, sz)) {
		throw std::runtime_error{ "Error : SetConsoleScreenBufferSize" };
	}
}

void ConsoleCore::SetCursorPos(COORD pos)
{
	if (!SetConsoleCursorPosition(_hOUT, pos)) {
		throw std::runtime_error{ "Error : SetConsoleCursorPosition" };
	}
}

void ConsoleCore::SetWindow(COORD upLeft, COORD lwRight)
{
	SMALL_RECT wndw = { upLeft.X, upLeft.Y, lwRight.Y, lwRight.X };
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
	GetConsoleScreenBufferInfo(_hOUT, &buffInfo);

	if (wndw.Right == buffInfo.srWindow.Right &&
		wndw.Bottom == buffInfo.srWindow.Bottom)
		return;

	if (wndw.Right > buffInfo.dwMaximumWindowSize.X &&
		wndw.Bottom > buffInfo.dwMaximumWindowSize.Y)
		return;

	if (!SetConsoleWindowInfo(_hOUT, TRUE, &wndw)) {
		throw std::runtime_error{ "Error : SetConsoleWindowInfo" };
	}
}

COORD ConsoleCore::GetBufferSize()
{
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
	GetConsoleScreenBufferInfo(_hOUT, &buffInfo);
	return buffInfo.dwSize;
}

COORD ConsoleCore::GetCursorPosition()
{
	CONSOLE_SCREEN_BUFFER_INFO buffInfo;
	GetConsoleScreenBufferInfo(_hOUT, &buffInfo);
	return buffInfo.dwCursorPosition;
}
