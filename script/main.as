
Texture@ m_SpriteTex;
Vec2 pos;
Vec2 scale;
Sprite@ m_Sprite;
Font@ m_Font;

void init(){
    print("initializing");
    @m_SpriteTex = LoadTexture("assets/potato.png");
    pos.x = 200;
    pos.y = 500;
    //Create a permanent sprite that does not need call draw every frame
    @m_Sprite = CreateSprite(m_SpriteTex, pos);
    m_Sprite.tint.r = 0;
    m_Sprite.tint.g = 0;
    m_Sprite.tint.b = 255;
    @m_Font = LoadFont("assets/jackinput.ttf");
    SetVsync(true);
}

void command(const string c){
    if(c == "nice"){
        SetVsync(false);
    }
}

bool reload(){
    m_Sprite.tint.r = 255;
    m_Sprite.tint.g = 255;
    m_Sprite.tint.b = 255;
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
    DrawText(m_Font, "FPS:" + fps, Vec2(1600 - 300, 10), 30);
}