#pragma once
#include "imports.h"
#include <shlobj.h>

//#define ENABLE_FILE_LOGS
//	#define ENABLE_CONSOLE_LOGS

std::string get_folder_loc(int csidl) {
	char buffer[_MAX_PATH];
	LPITEMIDLIST pidl = 0;
	HRESULT result = SHGetSpecialFolderLocation(NULL, csidl, &pidl);
	*buffer = 0;

	if (result == 0) {
		SHGetPathFromIDList(pidl, buffer);
		CoTaskMemFree(pidl);
		return string(buffer);
	}
	return string(buffer);
}

namespace logs {
	string logFile = "";


	///  <summary>
	///  Get current date / time. This is used to generate log messages when we're running in debug mode
	///  </summary>
	///  <param name="s">" now " or " date "</param>
	///  <returns>String with current date / time or empty string if not set</returns>
	inline string get_cur_datetime(string s) {
		time_t now = time(0);
		struct tm  tstruct;
		char  buf[80];
		tstruct = *localtime(&now);
		if (s == "now")
			strftime(buf, sizeof(buf), "[%Y / %m-%d / %X]:", &tstruct);
		else if (s == "date")
			strftime(buf, sizeof(buf), "[%Y / %m-%d]:", &tstruct);
		return string(buf);
	};

	inline bool exists() {
		string appdata = get_folder_loc(CSIDL_LOCAL_APPDATA);
		logFile = appdata + "/Cortex/log.txt";
		//cout << "[!] appdata " << appdata << endl;

		if (CreateDirectory((appdata + "/Cortex/").c_str(), NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError()) {
			return true;
		}
		return false;
	}


	inline void clear() {
		if (exists()) {
			std::ofstream ofs;
			ofs.open(logFile.c_str(), std::ofstream::out | std::ofstream::trunc);
			ofs.close();
		}
	}


	///  <summary>
	///  Adds a message to the log file. This is a convenience function for logging to the console and file if it exists.
	///  </summary>
	///  <param name="logMsg">The message to log ( without newline )</param>
	inline void addLog(string logMsg) {
#if defined(ENABLE_FILE_LOGS)
		// Write the current time to the log file.
		if (exists()) {
			string now = get_cur_datetime("now");
			ofstream ofs(logFile.c_str(), std::ios_base::out | std::ios_base::app);
			ofs << now << ' ' << logMsg << '\n';
			ofs.close();
		}
#endif
#if defined (ENABLE_CONSOLE_LOGS)
		cout << logMsg << endl;
#endif
	}

	inline void add(string logMsg) {
		//addLog(logMsg);
	}
}

#define add_log(str) //logs::add(enc(str))
