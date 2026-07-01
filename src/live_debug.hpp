#pragma once
#include "imports.h"
#include <sstream>
#include <vector>


RGBA debug_color = RGBA(255, 0, 0, 255);
namespace dbg {

    class DbgObject {
    public:
        string starttext{};
        std::string arg;

        DbgObject(std::string starttext) {
            this->starttext = starttext;
            this->arg = "";
        }

        

        ///  <summary>
        ///  Updates the argument to a new value. This is used to convert an argument to a string before passing it to the command and to set the value of the command's argument.
        ///  </summary>
        ///  <param name="argInput">The value to set the command's argument</param>
        template <typename T>
        void updateArg(T argInput) {
            this->arg = arg_to_string(argInput);
        }


    };

    DbgObject vector_aimbot_target{ "Vector | Target: " };
    DbgObject silent_aimbot_target{ "Silent | Target: " };
    DbgObject since_last_tick{ " Since Last Tick: " };
	DbgObject current_tick{ " Current Tick: " };

    ///  <summary>
    ///  Render a single debug object to the screen. This is called by DbgObject :: debug () to render a single line of debug output
    ///  </summary>
    ///  <param name="debuggable">The object to render.</param>
    ///  <param name="linenum">The line number of the object to render</param>
    void render_single(DbgObject debuggable, int linenum) {
        std::string output = debuggable.starttext;

        s_esptext_render_params dbgtext_render{ 2,2,0,1 };

        output += debuggable.arg;
        renderer.RenderText(output, { 30, 30 + (28.f * linenum) }, debug_color, dbgtext_render);

        //std::cout << output << " rendered " << std::endl;
    }

    void render_all() {
        return;
        //vector_aimbot_target.updateArg("NONE");
    }

}
