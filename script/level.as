
grid<int> m_LevelGrid(17,9,-1);
Texture@ m_TileErrorTex;
Texture@ m_TileEmptyTex;
Texture@ m_TileFilledTex;

void levelInit() {
    @m_TileErrorTex = LoadTexture("assets/textures/tile_error.png");
    @m_TileEmptyTex = LoadTexture("assets/textures/tile_empty.png");
    @m_TileFilledTex = LoadTexture("assets/textures/tile_filled.png");
    
    // Create level grid from file.
    file levelFile;
    if ( levelFile.open("assets/levels/level_0.txt","r") >= 0 ) {
        uint rowIndex = 0;
        while ( !levelFile.isEndOfFile() ) {
            string rowString = levelFile.readLine();
            for ( uint i = 0; i < rowString.length() && i < m_LevelGrid.width(); ++i ) {
                if ( rowString[i] == 78 ) { // 'N'
                    m_LevelGrid[i, rowIndex] = 1;
                } else if ( rowString[i] == 46 ) { // '.'
                    m_LevelGrid[i, rowIndex] = 0;
                } else {
                    break;
                }
            }
            ++rowIndex;
        }
        levelFile.close();
    }
}


void levelRender() {
    // Draw level-grid
    Vec2 tileScale( 1.0f, 1.0f );
    for ( uint y = 0; y < m_LevelGrid.height(); ++y ) {
        for ( uint x = 0; x < m_LevelGrid.width(); ++x ) {
            Vec2 tilePos( x * 100.0f, y * 100.0f );
            if ( m_LevelGrid[x,y] < 0 ) {
                DrawSprite( m_TileErrorTex, tilePos, tileScale );
            } else if ( m_LevelGrid[x,y] == 0 ) {
                DrawSprite( m_TileEmptyTex, tilePos, tileScale );
            } else if ( m_LevelGrid[x,y] == 1 ) {
                DrawSprite( m_TileFilledTex, tilePos, tileScale );
            }
        }
    }
}
