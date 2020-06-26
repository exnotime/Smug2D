//File for global access to the game state
#include "level.as"

class Globals{
    Globals(){
        @this.camera = Camera();
    }
    Level@ currentLevel;
    Camera@ camera;
};

Globals g_Globals;

