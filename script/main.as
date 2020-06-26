#include "level.as"
//#include "editor.as"
#include "Actor.as"
#include "StateMachine.as"
#include "events.as"
#include "globals.as"

Camera@ m_Camera;
PlayerActor m_Player;
Level m_Level;
StateMachine m_StateMachine;
int m_PlayerTeam;
int m_EnemyTeam;

void init(){
    m_Level = Level(Vec2(40,40));
    m_Level.Init();
    //editorInit();
    SetVsync(true);
    @m_Camera = @g_Globals.camera;
    m_Camera.position = Vec2(0,0);
    m_Camera.size = Vec2(1600,900);
    m_Camera.viewport = Rect(0, 0, 1, 1);
    m_Camera.rotation = 0.0f;

    m_PlayerTeam = m_StateMachine.CreateTeam();
    m_StateMachine.AddActorToTeam(m_PlayerTeam, m_Player);

    m_Player.SetGridPos(m_Level.StartPos.x, m_Level.StartPos.y);
    m_Camera.position = m_Level.StartPos * 64 - m_Camera.size  * 0.5f;
    @g_Globals.currentLevel = @m_Level;
}


void command(const string c) {
    if ( c == "reload_level" ) {
        m_Level.reloadLevelGrid();
    }
}

bool reload(){
    m_Level.reloadLevelGrid();
    return true;
}

float fps;
float keyDownTimer = 0;
float keyStepTimer = 0;
Key downedKey;
void update(float dt){
    fps = (1.0f/ dt);
    if(IsConsoleOpen())
        return;

    m_StateMachine.Update(dt);
    //m_Player.Update();
    if(IsKeyPushed(Key::A)){
        m_Camera.position.x -= 64;
    }
    if(IsKeyPushed(Key::D)){
        m_Camera.position.x += 64;
    }
    if(IsKeyPushed(Key::W)){
        m_Camera.position.y -= 64;
    }
    if(IsKeyPushed(Key::S)){
        m_Camera.position.y += 64;
    }
    if(IsKeyPushed(Key::E)){
        m_Camera.size.x += 20 * 16;
        m_Camera.size.y += 20 * 9;
    }
    if(IsKeyPushed(Key::Q)){
        m_Camera.size.x -= 20 * 16;
        m_Camera.size.y -= 20 * 9;
    }

    if(IsKeyReleased(downedKey)){
        keyDownTimer = 0;
    }

    if(IsKeyDown(Key::D)){
        downedKey = Key::D;
        keyDownTimer += dt;
    }
    if(IsKeyDown(Key::A)){
        downedKey = Key::A;
        keyDownTimer += dt;
    }
    if(IsKeyDown(Key::W)){
        downedKey = Key::W;
        keyDownTimer += dt;
    }
    if(IsKeyDown(Key::S)){
        downedKey = Key::S;
        keyDownTimer += dt;
    }
    if(keyDownTimer > 0.5f){
        keyStepTimer += dt;
    }
    if(keyStepTimer > 0.1f){
        keyStepTimer = 0;
        if(downedKey == Key::D){
            m_Camera.position.x += 64;
        }
        if(downedKey == Key::A){
            m_Camera.position.x -= 64;
        }
        if(downedKey == Key::W){
            m_Camera.position.y -= 64;
        }
        if(downedKey == Key::S){
            m_Camera.position.y += 64;
        }
    }
}

void render(){
    ClearWindow(0,0,0);
    AddCamera(m_Camera);
    m_Level.Render(fps);
    m_StateMachine.Render();
    //levelRender();
}