package ru.s21.lib.util;

import ru.s21.lib.fsm.GameState;

public class GameStateContainer {
    private GameState currentState;

    public synchronized GameState getCurrentState() {
        return currentState;
    }

    public synchronized void setCurrentState(GameState currentState) {
        this.currentState = currentState;
    }
}
