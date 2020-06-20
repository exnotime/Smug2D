#include "level.as"
//#include "editor.as"
#include "Actor.as"
#include "StateMachine.as"
#include "events.as"
#include "globals.as"

Camera m_Camera;
PlayerActor m_Player;
PlayerActor m_Player2;
Level m_Level;
StateMachine m_StateMachine;
int m_PlayerTeam;
int m_EnemyTeam;

void init(){
    m_Level = Level(Vec2(40,40));
    m_Level.Init();
    //editorInit();
    SetVsync(true);

    m_Camera.position = Vec2(0,0);
    m_Camera.size = Vec2(1600,900);
    m_Camera.viewport = Rect(0, 0, 1, 1);
    m_Camera.rotation = 0.0f;

    m_PlayerTeam = m_StateMachine.CreateTeam();
    m_StateMachine.AddActorToTeam(m_PlayerTeam, m_Player);

    m_Player2.m_GridPosition = Vec2(5,5);

    m_EnemyTeam = m_StateMachine.CreateTeam();
    m_StateMachine.AddActorToTeam(m_EnemyTeam, m_Player2);

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
}

void render(){
    ClearWindow(0,0,0);
    AddCamera(m_Camera);
    m_Level.Render(fps);
    m_StateMachine.Render();
    //levelRender();
}