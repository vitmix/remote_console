#ifndef CMD_PROCESS_HPP
#define CMD_PROCESS_HPP

#include <QCoreApplication>
#include <windows.h>
#include <stdexcept>

class CmdProcess
{
public:
	CmdProcess()
	{
        ZeroMemory(&_si, sizeof(STARTUPINFO));
        _si.cb = sizeof(STARTUPINFO);
        _si.dwFlags = STARTF_USESHOWWINDOW;
        _si.wShowWindow = SW_HIDE;
        ZeroMemory(&_pi, sizeof(PROCESS_INFORMATION));

        QString qsExePath = "C:\\Windows\\System32\\cmd.exe";
        QString qsCommandLine = "";
		if (!CreateProcess(
            NULL,
            (TCHAR*)(qsExePath.utf16()),
			NULL, 
			NULL, 
            TRUE,
            0,
			NULL, 
			NULL, 
			&_si, 
			&_pi))
			throw std::runtime_error{ "Error : Cannot create process" };
	}

	void AttachCmdConsole()
	{
		if (!AttachConsole(_pi.dwProcessId))
			throw std::runtime_error{ "Error : Cannot attach cmd console" };
	}

	~CmdProcess()
	{
		CloseHandle(_pi.hThread);
		CloseHandle(_pi.hProcess);
	}

private:
	STARTUPINFO _si;
	PROCESS_INFORMATION _pi;
};

#endif // CMD_PROCESS_HPP
