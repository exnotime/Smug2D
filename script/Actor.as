#include "events.as"
//base class for all game actors
//players, enemies
class Actor{
    Actor(){
        m_Entity = CreateEntity();
    }
    ~Actor(){
        DestroyEntity(m_Entity);
    }
    void Init() {}
    bool IsAlive() { return m_Health > 0; }
    void Update(EventStream@ es) {}
    void Render() {}
    void Reset() {}
    EntityHandle GetEntity() { return m_Entity;}

    Vec2 m_GridPosition;
    float m_Health;
    float m_MaxHealth;
    float m_ActionPoints;
    float m_MaxActionPoints;
    EntityHandle m_Entity;
}

class PlayerActor : Actor{
    PlayerActor(){
        Actor();
        CreateTransformComponent(m_Entity, Vec2(4 * 64), Vec2(1), Vec2(0), 0);
        CreateSpriteComponent(m_Entity, LoadTexture("assets/textures/player.png"), 3, Rect(0,0,1,1), Color(255,255,255));
        m_MaxActionPoints = 4;
        m_ActionPoints = 4;
        m_GridPosition = Vec2(4);
    }

    void Update(EventStream@ es){
        
        if(IsKeyPushed(Key::Left)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x - 1, m_GridPosition.y);
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
        } else if(IsKeyPushed(Key::Right)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x + 1, m_GridPosition.y);
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
        } else if(IsKeyPushed(Key::Up)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x, m_GridPosition.y - 1);
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
        } else if(IsKeyPushed(Key::Down)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x, m_GridPosition.y + 1);
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
        }

        
    }

    void Render(){
        TransformComponent@ tc = GetTransformComponent(m_Entity);
        Vec2 pos = tc.position;
        pos.y -= 8;
        pos.x -= 4;
        for(uint ac = 0; ac < uint(m_MaxActionPoints); ++ac){
            pos.x += 12;
            if(float(ac) < m_ActionPoints) {
                DrawCircle(pos, Color(255, 0, 0), 5.0f);
            } else{
                DrawCircle(pos, Color(20, 0, 0), 5.0f);
            }
        }
    }

    void Reset(){
        m_ActionPoints = m_MaxActionPoints;
    }
}