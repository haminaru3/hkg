#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
//#include <../features/hacks_singleuse.hpp>

class c_userinterface {
private:
	// info tab
	static char username[32];
	static char user_id[64];
	static char build_id[8];

	bool start_animation = true;

	// main menu close/open anim
	float animation_progress;
	float menu_alpha;

public:
	void init();
	void render_ui();

private:
	void bg_color();

	void begin_frame();
	//void render_tabs();
	//void content();
	void wigets_pool();
	void end_frame();
};

static c_userinterface g_ui;