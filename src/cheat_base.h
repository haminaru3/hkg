#pragma once
#include <iostream>
#include <map>

#include "protection/prot_defs.h"


///  <summary>
///  Generate a build id that is unique to this build. We do this by generating a random 8 - character string consisting of 8 random characters
///  </summary>
std::string get_build_id() {
	std::srand(std::stoi(u_crypt(__TIMESTAMP__)));

	std::string characters = u_crypt("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	std::string result;

	// Generates a random string of 8 characters.
	for (int i = 0; i < 8; i++) {
		result += characters[std::rand() % characters.length()];
	}

	return result;
}

struct s_cheat_base {
	char username[32] = "";
	int user_id = NULL;

	bool inited = false;
	bool state = false;

	struct {
		HMODULE module = NULL;

		HANDLE main_thread = NULL;
		HANDLE aim_thread = NULL;
		HANDLE hacks_thread = NULL;
		HANDLE threading_helper = NULL; //wtf
	} internal;

	struct {
		bool violation = false;
		std::map<int, const char*> exit_code; //unused
		int exit_method = 0;
	} protection;

	const char* build_id;
	
} h2o2_base;
