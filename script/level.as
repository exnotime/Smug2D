
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

Vec2 DirectionToVector(Direction d){
    if(d == Direction::East){
        return Vec2(1,0);
    } else if(d == Direction::North){
        return Vec2(0,-1);
    } else if(d == Direction::West){
        return Vec2(-1,0);
    }
    return Vec2(0,1);
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

bool IsRoomColliding(Room r1, Room r2){
    if(r1.Position.x + r1.Size.x >= r2.Position.x && r1.Position.x < r2.Position.x + r2.Size.x
        && r1.Position.y + r1.Size.y >= r2.Position.y && r1.Position.y < r2.Position.y + r2.Size.y){
            return true;
    }
    return false;
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
                m_Grid[x,y].textureIndex = Vec2(22,11);
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
            r.Size.x = Random(5, 10);
            r.Size.y = Random(5, 10);
            m_Rooms.insertLast(r);
            print("Creating start room x:" + r.Position.x + " y:" + r.Position.y);
        }
        
        uint roomCount = 40;
        bool quitMakingRooms = false;
        while(!quitMakingRooms){
            //select random room to extend
            Room@ r = @m_Rooms[Random(0, m_Rooms.length() - 1)];
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
            
            Room newRoom = Room();
            newRoom.Size.x = Random(3, 8);
            newRoom.Size.y = Random(3, 8);

            if(dir == Direction::West){
                newRoom.Position.x = r.Position.x - newRoom.Size.x  - Random(2,6);
                newRoom.Position.y = r.Position.y;
            } else if(dir == Direction::North){
                newRoom.Position.y = r.Position.y - newRoom.Size.y  - Random(2,6);
                newRoom.Position.x = r.Position.x;
            } else if(dir == Direction::East){
                newRoom.Position.x = r.Position.x + r.Size.x + Random(2,6);
                newRoom.Position.y = r.Position.y;
            } else if(dir == Direction::South){
                newRoom.Position.y = r.Position.y + r.Size.y + Random(2,6);
                newRoom.Position.x = r.Position.x;
            }
            bool isColliding = false;
            for(uint k = 0; k < m_Rooms.length(); ++k){
                isColliding = IsRoomColliding(m_Rooms[k], newRoom);
                if(isColliding) break;
            } 
            if(isColliding)
                continue;
            
            r.children.insertLast(m_Rooms.length());
            r.hasDoor[dir] = true;
            print("Creating new room x = " + newRoom.Position.x + " y = " + newRoom.Position.y + " Size w = " + newRoom.Size.x + " h = " + newRoom.Size.y);

            m_Rooms.insertLast(newRoom);
            if(m_Rooms.length() == roomCount){
                quitMakingRooms = true;
            }
        }
        
        //find new bounds
        Vec2 min = Vec2(1000);
        Vec2 max = Vec2(-1000);
        for(uint i = 0; i < m_Rooms.length(); ++i){
            Room r = m_Rooms[i];
            min.x = MinFloat(min.x, r.Position.x);
            min.y = MinFloat(min.y, r.Position.y);

            max.x = MaxFloat(max.x, r.Position.x + r.Size.x);
            max.y = MaxFloat(max.y, r.Position.y + r.Size.y);
        }
        print("Max:x " + max.x + ", y " + max.y);
        print("Min:x " + min.x + ", y " + min.y);

        print("New bounds x = " + int(max.x - min.x) + ", y = " + int(max.y - min.y));
        //normalize rooms positions
        for(uint i = 0; i < m_Rooms.length(); ++i){
            m_Rooms[i].Position -= min;
        }
        m_Grid.resize(int(max.x - min.x), int(max.y - min.y));
        Clear();
        //rasterize the rooms
        for(uint i = 0; i < m_Rooms.length(); ++i){
            for(uint x = 0; x < m_Rooms[i].Size.x; ++x){
                for(uint y = 0; y < m_Rooms[i].Size.y; ++y){
                    m_Grid[m_Rooms[i].Position.x + x, m_Rooms[i].Position.y + y].type = TileType::Floor;
                    m_Grid[m_Rooms[i].Position.x + x, m_Rooms[i].Position.y + y].textureIndex = Vec2(17,13);
                }
            }
        }
        //connect parents and children
        for(uint i = 0; i < m_Rooms.length(); ++i){
            Room parent = m_Rooms[i];
            print("Room " + i + " has " + parent.children.length() + " children");
            for(uint c = 0; c < parent.children.length(); ++c){
                Room child = m_Rooms[parent.children[c]];
                //find direction and start of corridor
                if(child.Position.x < parent.Position.x){ //West
                    Vec2 start = parent.Position;
                    int smallestSide = MinFloat(child.Size.y, parent.Size.y);
                    start.y = start.y + Random(1, smallestSide - 2);
                    int length = start.x - (child.Position.x + child.Size.x);
                    print("Corridor west " + start.x +"," + start.y + " length:" + length + " child: " + parent.children[c]);
                    for(int k = 1; k < length + 1; k++){
                        m_Grid[start.x - k, start.y].type = TileType::Floor;
                        m_Grid[start.x - k, start.y].textureIndex = Vec2(16,1);
                    }
                }else if(child.Position.x > parent.Position.x){ //East
                    Vec2 start = parent.Position;
                    int smallestSide = MinFloat(child.Size.y, parent.Size.y);
                    start.y = start.y + Random(1, smallestSide - 2);
                    start.x = start.x + parent.Size.x - 1;
                    int length = child.Position.x - start.x;
                    print("Corridor east " + start.x +"," + start.y + " length:" + length + " child: " + parent.children[c]);
                    for(int k = 1; k < length; k++){
                        m_Grid[start.x + k, start.y].type = TileType::Floor;
                        m_Grid[start.x + k, start.y].textureIndex = Vec2(17,1);
                    }
                }else if(child.Position.y < parent.Position.y){ //North
                    Vec2 start = parent.Position;
                    int smallestSide = MinFloat(child.Size.x, parent.Size.x);
                    start.x = start.x + Random(1, smallestSide - 2);
                    int length = start.y - (child.Position.y + child.Size.y);
                    print("Corridor north " + start.x +"," + start.y + " length:" + length + " child: " + parent.children[c]);
                    for(int k = 1; k < length + 1; k++){
                        m_Grid[start.x, start.y - k].type = TileType::Floor;
                        m_Grid[start.x, start.y - k].textureIndex = Vec2(18,1);
                    }
                }else if(child.Position.y > parent.Position.y){ //South
                    Vec2 start = parent.Position;
                    int smallestSide = MinFloat(child.Size.x, parent.Size.x);
                    start.x = start.x + Random(1, smallestSide - 2);
                    start.y = start.y + parent.Size.y - 1;
                    int length = child.Position.y - start.y;
                    print("Corridor south " + start.x +"," + start.y + " length:" + length + " child: " + parent.children[c]);
                    for(int k = 1; k < length; k++){
                        m_Grid[start.x, start.y + k].type = TileType::Floor;
                        m_Grid[start.x, start.y + k].textureIndex = Vec2(19,1);
                    }
                }
            }
        }

        @m_BackgroundLayer = CreateRenderTexture("LevelBackground", Vec2(m_Grid.width()  * m_TileSize.x, m_Grid.height() * m_TileSize.y));
        ClearRenderTexture(m_BackgroundLayer, Color(255,255,255));
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

        for(uint i = 0; i < m_Rooms.length(); ++i){
            DrawTextToTexture(m_BackgroundLayer, m_Font, m_Rooms[i].Position * m_TileSize, "Room: " + i, 15, Color(0,0,0));
        }

        FlushRenderTexture(m_BackgroundLayer);

        StartPos = m_Rooms[0].Position;
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
    Vec2 StartPos;
};

Level g_TestLevel;
void TestFunction(int p1){
    
}