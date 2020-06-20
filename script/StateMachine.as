#include "Actor.as"
#include "events.as"

class StateMachine {
    int CurrentTeam; 
    int Turn;
    int CurrentActorIndex;
    array<array<Actor@>> TeamActors;
    EventStream Events;
    float GameSpeed; //time between each AI action
    float timer;

    int CreateTeam(){
        int team = TeamActors.length();
        TeamActors.insertLast(array<Actor@>());
        return team;
    }

    void AddActorToTeam(int team, Actor@ a){
        TeamActors[team].insertLast(a);
    }

    void Update(float deltaTime){
        timer += deltaTime;
        
        Event@ event;
        if(Events.GetNextEvent(event)){
            if(event.eventType == MOVEMENT){
                MovementEvent@ me = cast<MovementEvent>(event);
                me.Perform();
            }
        }

        Actor@ actor = TeamActors[CurrentTeam][CurrentActorIndex];
        actor.Update(Events); 
        //handle turns
        if(actor.m_ActionPoints == 0){
            CurrentActorIndex++;
            if(CurrentActorIndex >= int(TeamActors[CurrentTeam].length())){
                CurrentTeam++;
                CurrentActorIndex = 0;
                if(CurrentTeam >= int(TeamActors.length())){
                    CurrentTeam = 0;
                }

                for(uint a = 0; a < TeamActors[CurrentTeam].length(); ++a){
                    TeamActors[CurrentTeam][a].Reset();
                }
            }
        }
    }

    void Render(){
        //go through current teams actors and call their render function
        for(uint a = 0; a < TeamActors[CurrentTeam].length(); ++a){
            Actor@ actor = TeamActors[CurrentTeam][a];
            actor.Render();
        }
    }
};