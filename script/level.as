
enum TileType{
    Empty,
    Floor,
    Wall,
    Door,
    Trap,
    SecretWall
}

class Tile{
    Vec2 textureIndex;
    TileType type;
}

enum Direction{
    East,
    North,
    West,
    South
}

class Room{
    Room(){
        hasDoor.resize(4);
    }

    Vec2 Position;
    Vec2 Size;
    array<bool> hasDoor;
    array<int> children;
}

class Level {
    Level(){

    }
    Level(Vec2 size){
        m_Size = size;
        m_Grid.resize( int(size.x), int(size.y));
        m_TileSize = Vec2(64, 64);
        Clear(); 
    }

    void Clear() {
        for ( uint y = 0; y < m_Grid.height(); ++y ) {
            for ( uint x = 0; x < m_Grid.width(); ++x ) {
                m_Grid[x,y].textureIndex = 1;
                m_Grid[x,y].type = TileType::Empty;
            }
        }
    }

    void reloadLevelGrid() {
        Clear();
    }

    void Init() {
        m_Textures.insertLast(LoadTexture("assets/textures/roguelikeDungeon_transparent.png"));
        @m_Font = LoadFont("assets/fonts/jackinput.ttf");
       
        //create source room
        {
            Room r = Room();
            r.Position.x = 32 + Random(-16, 16);
            r.Position.y = 32 + Random(-16, 16);
            r.Size.x = Random(4, 10);
            r.Size.y = Random(4, 10);
            m_Rooms.insertLast(r);
        }
        print("Creating start room");
        uint roomCount = 10;
        bool quitMakingRooms = false;
        while(!quitMakingRooms){
            //select random room to extend
            Room r = m_Rooms[Random(0, m_Rooms.length() - 1)];
            if(r.children.length() == 4){
                continue;
            }
            //find direction we have not extended in yet
            Direction dir;
            while(true){
                int d = Random(0, 4);
                if(!r.hasDoor[d]){
                    dir = Direction(d);
                    break;
                }
            }
            //create new room
            r.children.insertLast(m_Rooms.length());
            Room newRoom = Room();
            newRoom.Size.x = Random(4, 10);
            newRoom.Size.y = Random(4, 10);

            if(dir == Direction::West){
                newRoom.Position.x = r.Position.x - newRoom.Size.x  - 2;
                newRoom.Position.y = r.Position.y;
            } else if(dir == Direction::North){
                newRoom.Position.y = r.Position.y - newRoom.Size.y  - 2;
                newRoom.Position.x = r.Position.x;
            } else if(dir == Direction::East){
                newRoom.Position.x = r.Position.x + r.Size.x + 2;
                newRoom.Position.y = r.Position.y;
            } else if(dir == Direction::South){
                newRoom.Position.y = r.Position.y + r.Size.y + 2;
                newRoom.Position.x = r.Position.x;
            }
            
            print("Creating new room x = " + newRoom.Position.x + " , y = " + newRoom.Position.y);

            m_Rooms.insertLast(newRoom);
            if(m_Rooms.length() == roomCount){
                quitMakingRooms = true;
            }
        }

        //find new bounds
        Vec2 min, max;
        for(uint i = 0; i < m_Rooms.length(); ++i){
            Room r = m_Rooms[i];
            min.x = MinFloat(min.x, r.Position.x);
            min.y = MinFloat(min.y, r.Position.y);

            max.x = MaxFloat(max.x, r.Position.x + r.Size.x);
            max.x = MaxFloat(max.x, r.Position.y + r.Size.y);
        }
        print("New bounds x = " + int(max.x - min.x) + ", y = " + int(max.y - min.y));
        //normalize rooms positions
        for(uint i = 0; i < m_Rooms.length(); ++i){
            Room r = m_Rooms[i];
            r.Position -= min;
        }
        m_Grid.resize(int(max.x - min.x), int(max.y - min.y));

        @m_BackgroundLayer = CreateRenderTexture("LevelBackground", m_Size * m_TileSize);
        ClearRenderTexture(m_BackgroundLayer, Color(0,0,0));
        for (uint y = 0; y < m_Grid.height(); ++y ) {
            for ( uint x = 0; x < m_Grid.width(); ++x ) {
                DrawSpriteToTexture(m_BackgroundLayer, m_Textures[0], Vec2(x,y) * m_TileSize, Vec2(4), m_Grid[x,y].textureIndex * Vec2(16) + m_Grid[x,y].textureIndex, Vec2(16));
            }
        }
        for (uint y = 0; y < m_Grid.height(); ++y ) {
            DrawRectToTexture(m_BackgroundLayer, Vec2(0, y * m_TileSize.y), Vec2(m_Grid.width() * m_TileSize.x, 1),Color(255,0,0));
        }
        for (uint x = 0; x < m_Grid.width(); ++x ) {
            DrawRectToTexture(m_BackgroundLayer, Vec2(x * m_TileSize.x, 0), Vec2(1, m_Grid.height() * m_TileSize.y),Color(255,0,0));
        }
        FlushRenderTexture(m_BackgroundLayer);
    }

    void Render(float fps){
        DrawText(m_Font, "Fps: " + fps, Vec2(0), Color(255,255,255));
        DrawText(m_Font, "Width: " + m_Grid.width() + " Height: " + m_Grid.height(), Vec2(0, 30), Color(255,255,255));
        DrawSprite(GetTextureFromRenderTexture(m_BackgroundLayer), Vec2(0), Vec2(1), 1);
    }

    bool IsWalkable(Vec2 coord){
        return m_Grid[uint(coord.x), uint(coord.y)].type == TileType::Floor;
    }

    Font@ m_Font;
    RenderTexture@ m_BackgroundLayer;
    Vec2 m_TileSize;
    Vec2 m_Size;
    array<Texture@> m_Textures;
    grid<Tile> m_Grid;
    array<Room> m_Rooms;
};

Level g_TestLevel;
void TestFunction(int p1){
    
}