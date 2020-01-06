
funcdef void EditorButtonCallback_t();

Texture@ m_EditorMakeTex;
Texture@ m_EditorPlayTex;
array<EditorButton@> m_EditorButtons;
bool m_Playing = false;
bool m_ReloadEditorUI = false;

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
	editorCreateUI();
}

void editorUpdate() {
	if ( m_ReloadEditorUI ) {
		editorReloadUI();
		m_ReloadEditorUI = false;
	}

	if ( IsMouseButtonPushed( MouseButton::Left ) ) {
		for ( uint i = 0; i < m_EditorButtons.length(); ++i ) {
			const Vec2 mousePos = MousePos();
			Sprite@ sprite = @m_EditorButtons[i].m_Sprite;
			if ( mousePos.x >= sprite.pos.x && mousePos.x <= sprite.pos.x + sprite.textureSize.x && mousePos.y >= sprite.pos.y && mousePos.y <= sprite.pos.y + sprite.textureSize.y ) {
				m_EditorButtons[i].m_Callback();
				break;
			}
		}
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
