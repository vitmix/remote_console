#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <sstream>
#include <Windows.h>

#include "cereal\access.hpp"
#include "cereal\types\string.hpp"
#include "cereal\types\vector.hpp"
#include "cereal\archives\portable_binary.hpp"

template <typename Archive>
void serialize(Archive &ar, COORD &coord)
{
	ar(coord.X, coord.Y);
}

template <typename Archive>
void serialize(Archive &ar, KEY_EVENT_RECORD &ker)
{
	ar(ker.bKeyDown, ker.wRepeatCount, ker.wVirtualKeyCode,
	   ker.wVirtualScanCode, ker.uChar.UnicodeChar,
	   ker.uChar.AsciiChar, ker.dwControlKeyState);
}

template <typename Archive>
void serialize(Archive &ar, MOUSE_EVENT_RECORD &mer)
{
	ar(mer.dwMousePosition, mer.dwButtonState, 
	   mer.dwControlKeyState, mer.dwEventFlags);
}

template <typename Archive>
void serialize(Archive &ar, WINDOW_BUFFER_SIZE_RECORD &wer)
{
	ar(wer.dwSize);
}

template <typename Archive>
void serialize(Archive &ar, MENU_EVENT_RECORD &mer)
{
	ar(mer.dwCommandId);
}

template <typename Archive>
void serialize(Archive &ar, FOCUS_EVENT_RECORD &fer)
{
	ar(fer.bSetFocus);
}

template <typename Archive>
void serialize(Archive &ar, INPUT_RECORD &inrec)
{
	ar( inrec.EventType, inrec.Event.KeyEvent, inrec.Event.MouseEvent,
		inrec.Event.WindowBufferSizeEvent, inrec.Event.MenuEvent,
		inrec.Event.FocusEvent);
}

template <typename Archive>
void serialize(Archive &ar, CHAR_INFO &chinfo)
{
	ar(chinfo.Attributes, chinfo.Char.UnicodeChar, chinfo.Char.AsciiChar);
}

template <typename T>
struct Message
{
public:
	void SetCommand(const std::string &cmdName)
	{
		_command = cmdName;
	}

	void SetData(const std::vector<T> &data)
	{
		_data = data;
	}

	std::string & GetCommand()
	{
		return _command;
	}

	std::vector<T> & GetData()
	{
		return _data;
	}

private:
	friend class cereal::access;

	template <class Archive>
	void serialize(Archive &ar)
	{
        ar(_command, _data);
	}

private:
	std::string _command;
	std::vector<T> _data;
};

template <typename T>
std::string Serialize(Message<T> &m)
{
	std::ostringstream oss;
	{
		cereal::PortableBinaryOutputArchive oArch(oss);
		oArch(m);
	}
	return oss.str();
}

template <typename T>
void Deserialize(Message<T> &m, std::string &str)
{
	std::stringstream iss;
	iss << str;
	{
		cereal::PortableBinaryInputArchive iArch(iss);
		iArch(m);
	}
}
/*
struct MessageSize
{
    void SetMessageSize(size_t sz)
    {
        _size = sz;
    }

    size_t GetMessageSize()
    {
        return _size;
    }

private:
    friend class cereal::access;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(_size);
    }

private:
    size_t _size;
};

std::string Serialize(MessageSize &m)
{
    std::ostringstream oss;
    {
        cereal::PortableBinaryOutputArchive oArch(oss);
        oArch(m);
    }
    return oss.str();
}

void Deserialize(MessageSize &m, std::string &str)
{
    std::stringstream iss;
    iss << str;
    {
        cereal::PortableBinaryInputArchive iArch(iss);
        iArch(m);
    }
}*/

#endif // !MESSAGE_HPP
