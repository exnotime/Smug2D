#include "level.as"
#include "editor.as"

Texture@ m_SpriteTex;
Vec2 pos;
Vec2 scale;
Sprite@ m_Sprite;
Font@ m_Font;

void init(){
    print("initializing");
    @m_SpriteTex = LoadTexture("assets/textures/potato.png");
    pos.x = 200;
    pos.y = 500;
    //Create a permanent sprite that does not need call draw every frame
    @m_Sprite = CreateSprite(m_SpriteTex, pos);
    m_Sprite.tint.r = 255;
    m_Sprite.tint.g = 255;
    m_Sprite.tint.b = 255;
    @m_Font = LoadFont("assets/fonts/jackinput.ttf");

    levelInit();
    editorInit();
    SetVsync(true);

    EntityHandle e = CreateEntity();
    CreateTransformComponent(e, Vec2(100,100), Vec2(1.0f, 1.0f), Vec2(0), 0.0f);
    CreateSpriteComponent(e, m_SpriteTex, 3, Rect(), Color(255,255,255));
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
    if(IsKeyDown(Key::Left)){
        m_Sprite.pos.x -= speed * dt;
    }
    if(IsKeyDown(Key::Right)){
        m_Sprite.pos.x += speed * dt;
    }
    if(IsKeyDown(Key::Up)){
        m_Sprite.pos.y -= speed * dt;
    }
    if(IsKeyDown(Key::Down)){
        m_Sprite.pos.y += speed * dt;
    }
    if(IsMouseButtonPushed(MouseButton::Left)){
        Vec2 mousePos = MousePos();
        print("mouse pos " + mousePos.x + ", " + mousePos.y );
        print("sprite pos " + m_Sprite.pos.x + ", " + m_Sprite.pos.y );
        print("sprite size " + m_Sprite.textureSize.x + ", " + m_Sprite.textureSize.y );
        if( mousePos.x >= m_Sprite.pos.x && mousePos.x <= m_Sprite.pos.x + m_Sprite.textureSize.x &&
            mousePos.y >= m_Sprite.pos.y && mousePos.y <= m_Sprite.pos.y + m_Sprite.textureSize.y){

            m_Sprite.tint.r = 255;
            m_Sprite.tint.g = 0;
            m_Sprite.tint.b = 0;
        }else{
            m_Sprite.tint.r = 255;
            m_Sprite.tint.g = 255;
            m_Sprite.tint.b = 255;
        }
    }

    editorUpdate();
}

void render(){
    ClearWindow(0,0,0);
    levelRender();
    DrawText(m_Font, "FPS:" + fps, Vec2(1600 - 300, 10), Color(255,255,255), 30);
}