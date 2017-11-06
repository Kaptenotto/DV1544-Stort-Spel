#ifndef STATEGAME_H
#define STATEGAME_H

// C++ Includes
#include <stdio.h>
#include <thread>

// State Include
#include <State.h>

namespace Logic
{
    class StateGame : public State
    {
    public:
        StateGame();
        ~StateGame();
        void reset();

        void update(float deltaTime);
        void render() const;

        void switchState(StateType menuState);

    private:
        StateType m_currentStateType;
        State* m_currentState;
    };
}

#endif // !STATEGAME_H