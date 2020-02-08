#include "level.as"
#include "editor.as"

Texture@ m_SpriteTex;
Texture@ m_AnimalTex;
Vec2 pos;
Vec2 scale;
Sprite@ m_Sprite;
Font@ m_Font;
AnimationHandle m_Animation;
EntityHandle m_AnimatedEntity;
Camera m_Camera;
void init(){
    print("initializing");
    @m_SpriteTex = LoadTexture("assets/textures/potato.png");
    @m_AnimalTex = LoadTexture("assets/textures/animals1.png");
    pos.x = 200;
    pos.y = 500;
    @m_Font = LoadFont("assets/fonts/jackinput.ttf");
    m_Animation = LoadAnimation("assets/sprites/test_frames.sprite");
    levelInit();
    editorInit();
    SetVsync(true);

    m_AnimatedEntity = CreateEntity();
    CreateTransformComponent(m_AnimatedEntity, Vec2(300,100), Vec2(10.0f, 10.0f), Vec2(0, 0), 0.0f);
    CreateSpriteComponent(m_AnimatedEntity);
    SpriteComponent@ sc = GetSpriteComponent(m_AnimatedEntity);
    sc.layer = 3;
    sc.color = Color(255,255,255);
    CreateAnimationComponent(m_AnimatedEntity, m_Animation);

    m_Camera.position = Vec2(0,0);
    m_Camera.size = Vec2(1600,900);
    m_Camera.viewport = Rect(0, 0, 1, 1);
    m_Camera.rotation = 0.0f;
}

void command(const string c){
    if ( c == "reload_level" ) {
        reloadLevelGrid();
    }
}

bool reload(){
    reloadLevelGrid();
    return true;
}

float fps;

void update(float dt){
     fps = (1.0f/ dt);
    if(IsConsoleOpen())
        return;
    const float speed = 1000.0f;
    SpriteComponent@ sprComp = GetSpriteComponent(m_AnimatedEntity);
    TransformComponent@ transform = GetTransformComponent(m_AnimatedEntity); 
    if(IsKeyDown(Key::Left)){
        transform.position.x -= speed * dt;
    }
    if(IsKeyDown(Key::Right)){
        transform.position.x += speed * dt;
    }
    if(IsKeyDown(Key::Up)){
        transform.position.y -= speed * dt;
    }
    if(IsKeyDown(Key::Down)){
        transform.position.y += speed * dt;
    }

    if(IsKeyDown(Key::A)){
        m_Camera.position.x -= speed * dt;
    }
    if(IsKeyDown(Key::D)){
        m_Camera.position.x  += speed * dt;
    }
    if(IsKeyDown(Key::W)){
        m_Camera.position.y  -= speed * dt;
    }
    if(IsKeyDown(Key::S)){
        m_Camera.position.y  += speed * dt;
    }
    if(IsKeyDown(Key::E)){
        m_Camera.rotation  += 100 * dt;
    }
    if(IsKeyDown(Key::Q)){
        m_Camera.rotation  -= 100 * dt;
    }

    editorUpdate();
}

void render(){
    ClearWindow(0,0,0);
    AddCamera(m_Camera);
    levelRender();
    DrawText(m_Font, "FPS:" + fps, Vec2(1600 - 300, 10), Color(255,255,255), 30);
}