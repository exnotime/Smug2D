#include "level.as"

funcdef void EditorButtonCallback_t();

Texture@ m_EditorMakeTex;
Texture@ m_EditorPlayTex;
Texture@ m_EditorTileEmptyTex;
Texture@ m_EditorTileFilledTex;
array<EditorButton@> m_EditorButtons;
bool m_Playing = true;
bool m_ReloadEditorUI = false;
bool m_EditorDrawing = false;
int m_TileTypeDrawn = 1;

class EditorButton {
	Sprite@ m_Sprite;
	EditorButtonCallback_t@ m_Callback;

	EditorButton( Sprite@ sprite, EditorButtonCallback_t@ callback ) {
		@m_Sprite = @sprite;
		@m_Callback = @callback;
	}
}

void editorCreateButton( Texture@ texture, const Vec2&in pos, EditorButtonCallback_t@ callback ) {
	m_EditorButtons.insertLast(
		EditorButton(
			CreateSprite(texture, pos),
			callback
		)
	);
}

void editorInit() {
    @m_EditorMakeTex = LoadTexture("assets/textures/dont_ship/editor_make.png");
    @m_EditorPlayTex = LoadTexture("assets/textures/dont_ship/editor_play.png");
    @m_EditorTileEmptyTex = LoadTexture("assets/textures/editor/tile_empty.png");
    @m_EditorTileFilledTex = LoadTexture("assets/textures/editor/tile_filled.png");
	editorCreateUI();
}

void editorUpdate() {
	if ( m_ReloadEditorUI ) {
		editorReloadUI();
		m_ReloadEditorUI = false;
	}

	// Handle clicking of buttons.
	bool clickedButton = false;
	if ( IsMouseButtonPushed( MouseButton::Left ) ) {
		for ( uint i = 0; i < m_EditorButtons.length(); ++i ) {
			const Vec2 mousePos = MousePos();
			Sprite@ sprite = @m_EditorButtons[i].m_Sprite;
			if ( mousePos.x >= sprite.pos.x && mousePos.x <= sprite.pos.x + sprite.textureSize.x && mousePos.y >= sprite.pos.y && mousePos.y <= sprite.pos.y + sprite.textureSize.y ) {
				clickedButton = true;
				m_EditorButtons[i].m_Callback();
				break;
			}
		}
		m_EditorDrawing = !clickedButton;
	}
	
	if ( !m_Playing ) {
		editorUpdateMaking();
	}
}

void editorUpdateMaking() {
	// Draw tiles on the level.
	if ( m_EditorDrawing && m_TileTypeDrawn >= 0 && IsMouseButtonDown( MouseButton::Left ) ) {
		const Vec2 mousePos = MousePos();
		const int tileX = int( mousePos.x / 100.0f );
		const int tileY = int( mousePos.y / 100.0f );
		m_LevelGrid[ tileX, tileY ] = m_TileTypeDrawn;
	}
}

void editorCreateUI() {
	if ( m_Playing ) {
		editorCreatePlayingUI();
	} else {
		editorCreateMakingUI();
	}
}

void editorCreatePlayingUI() {
	editorCreateButton( m_EditorMakeTex, Vec2( 10, 730 ), @OnClickMake );
}

void editorCreateMakingUI() {
	editorCreateButton( m_EditorPlayTex, Vec2( 10, 730 ), @OnClickPlay );
	editorCreateButton( m_EditorTileEmptyTex, Vec2( 900, 10 ), @OnClickTileEmpty );
	editorCreateButton( m_EditorTileFilledTex, Vec2( 960, 10 ), @OnClickTileFilled );
}

void editorDestroyUI() {
	for ( uint i = 0; i < m_EditorButtons.length(); ++i ) {
		DestroySprite( m_EditorButtons[i].m_Sprite );
	}
	m_EditorButtons.resize( 0 );
}

void editorReloadUI() {
	editorDestroyUI();
	editorCreateUI();
}

void OnClickMake() {
	m_Playing = false;
	m_ReloadEditorUI = true;
}

void OnClickPlay() {
	m_Playing = true;
	m_ReloadEditorUI = true;
}

void OnClickTileEmpty() {
	m_TileTypeDrawn = 0;
}

void OnClickTileFilled() {
	m_TileTypeDrawn = 1;
}
