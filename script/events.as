#include "globals.as"
#include "Actor.as"
enum EventType{
    MOVEMENT = 0,
    ATTACK = 1,
    ITEM = 2
};

class Event{
    EventType eventType;
    bool Perform(float dt){
        print("base perform");
        return true;
    }
};

class MovementEvent : Event {
    MovementEvent() { eventType = MOVEMENT; }
    Vec2 from;
    Vec2 to;
    Actor@ actor;
    float time = 0;
    float timer = 0;
    bool Perform(float dt){
        //print("moving from (" + from.x + "," + from.y+ ") to (" + to.x + "," + to.y + ")");
        if(!g_Globals.currentLevel.IsWalkable(to)){
            return true;
        }
        Vec2 delta = (to * 64) - (from * 64);
        delta *= (timer / time);
        TransformComponent@ tc = GetTransformComponent(actor.m_Entity);
        tc.position = (from * 64) + delta;
        if(timer >= time){
            tc.position = to * 64;
            g_Globals.camera.position += (to - from) * 64;
            actor.m_GridPosition = to;
            AnimationComponent@ ac = GetAnimationComponent(actor.m_Entity);
            PauseAnimation(ac);
            return true;
        }
        timer += dt;
        return false;
    }
}

class EventStream{
    void PushNewEvent(Event@ e){
        m_Events.insertLast(e);
    }

    bool GetNextEvent(Event@ &out e){
        if(consumed == m_Events.length()){
            return false;
        }
        @e = m_Events[consumed];
        return true;
    }

    void Consume(){
        consumed++;
    }

    int EventCount(){
        return m_Events.length() - consumed;
    }

    private uint consumed = 0;
    private array<Event@> m_Events;
}