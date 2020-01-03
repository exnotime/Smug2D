#include "level.as"

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
    m_Sprite.tint.r = 0;
    m_Sprite.tint.g = 0;
    m_Sprite.tint.b = 255;
    @m_Font = LoadFont("assets/fonts/jackinput.ttf");
    levelInit();
    SetVsync(true);

    file inFile;
    inFile.open("test.txt","r");
    string line = inFile.readLine();
    if(line == "test"){
        m_Sprite.tint.r = 255;
    }

    grid<float> testGrid(2,2);
    testGrid[0, 0] = 1.0f;
    testGrid[1, 0] = 2.0f;
    testGrid[0, 1] = 3.0f;
    testGrid[1, 1] = 4.0f;

    if(testGrid[0, 0] == 1.0f){
        m_Sprite.tint.g = 255;
    }
}

void command(const string c){
    if(c == "nice"){
        SetVsync(false);
    } else if ( c == "reload_level" ) {
        reloadLevelGrid();
    }
}

bool reload(){
    m_Sprite.tint.r = 255;
    m_Sprite.tint.g = 255;
    m_Sprite.tint.b = 255;
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
    if(IsKeyDown(Key::E)){
        m_Sprite.scale.x += 0.5f * dt;
        m_Sprite.scale.y += 0.5f * dt;
    }
    if(IsKeyDown(Key::R)){
        m_Sprite.scale.x -= 0.5f * dt;
        m_Sprite.scale.y -= 0.5f * dt;
    }
}

void render(){
    ClearWindow(0,0,0);
    levelRender();
    DrawText(m_Font, "FPS:" + fps, Vec2(1600 - 300, 10), 30);
}