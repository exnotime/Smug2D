#include "globals.as"
#include "Actor.as"
enum EventType{
    MOVEMENT = 0,
    ATTACK = 1,
    ITEM = 2
};

class Event{
    EventType eventType;
    void Perform(){
        print("base perform");
    }
};

class MovementEvent : Event {
    MovementEvent() { eventType = MOVEMENT; }
    Vec2 from;
    Vec2 to;
    Actor@ actor;
    
    void Perform(){
        print("moving from (" + from.x + "," + from.y+ ") to (" + to.x + "," + to.y + ")");
        if(g_Globals.currentLevel.IsWalkable(to)){
            actor.m_GridPosition = to;
            TransformComponent@ tc = GetTransformComponent(actor.m_Entity);
            tc.position = to * 64;

            g_Globals.camera.position += (to - from) * 64;
        }
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
        consumed++;
        return true;
    }

    private uint consumed = 0;
    private array<Event@> m_Events;
}