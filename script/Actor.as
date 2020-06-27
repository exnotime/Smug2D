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
        AnimationHandle playerAnimation = LoadAnimation("assets/sprites/player.sprite");
        CreateAnimationComponent(m_Entity, playerAnimation);
        m_MaxActionPoints = 4;
        m_ActionPoints = 4;
        m_GridPosition = Vec2(4);

        walkingRightAnimation = GetAnimationIndex(playerAnimation, "WalkingRight");
        walkingUpAnimation = GetAnimationIndex(playerAnimation, "WalkingUp");
        walkingDownAnimation = GetAnimationIndex(playerAnimation, "WalkingDown");
    }

    void Update(EventStream@ es){
        
        //dont push more events while we are still waiting for an event to finsish
        if(es.EventCount() > 0){
            return;
        }

        if(IsKeyDown(Key::Left)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x - 1, m_GridPosition.y);
            @me.actor = @this;
            me.time = 0.5f; //Todo: Get from animation
            es.PushNewEvent(me);
            m_ActionPoints--;
            SpriteComponent@ sc = GetSpriteComponent(m_Entity);
            sc.flipX = true;
            AnimationComponent@ ac = GetAnimationComponent(m_Entity);
            PlayAnimation(ac, walkingRightAnimation, false, alternate);
        } else if(IsKeyDown(Key::Right)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x + 1, m_GridPosition.y);
            me.time = 0.5f;
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
            SpriteComponent@ sc = GetSpriteComponent(m_Entity);
            sc.flipX = false;
            AnimationComponent@ ac = GetAnimationComponent(m_Entity);
            PlayAnimation(ac, walkingRightAnimation, false, alternate);
        } else if(IsKeyDown(Key::Up)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x, m_GridPosition.y - 1);
            me.time = 0.5f;
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
            AnimationComponent@ ac = GetAnimationComponent(m_Entity);
            PlayAnimation(ac, walkingUpAnimation, false, alternate);
        } else if(IsKeyDown(Key::Down)){
            MovementEvent me;
            me.from = m_GridPosition;
            me.to = Vec2(m_GridPosition.x, m_GridPosition.y + 1);
            me.time = 0.5f;
            @me.actor = @this;
            es.PushNewEvent(me);
            m_ActionPoints--;
            AnimationComponent@ ac = GetAnimationComponent(m_Entity);
            PlayAnimation(ac, walkingDownAnimation, false, alternate);
        }
        alternate = !alternate;
    }

    void SetGridPos(uint x, uint y){
        TransformComponent@ tc = GetTransformComponent(m_Entity);
        tc.position = Vec2(x,y) * 64;
        m_GridPosition = Vec2(x,y);
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

    private int walkingRightAnimation;
    private int walkingDownAnimation;
    private int walkingUpAnimation;
    private bool alternate = false;
}