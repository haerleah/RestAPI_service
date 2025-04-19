package ru.s21.lib;

import ru.s21.lib.fsm.GameState;
import ru.s21.lib.processing.Game;

import java.util.Random;

public class RaceFacade {
    private Game game;

    public void initializeGame() {
        game = new Game(new Random());
    }

    public void userInput(Action action, boolean hold) {
        game.processAction(action, hold);
    }

    public State updateCurrentState() {
        return new State(game.getCurrentField(),
                null,
                game.getParameters().getScore(),
                game.getParameters().getHighestScore(),
                game.getParameters().getLevel(),
                game.getParameters().getSpeed(),
                game.getParameters().isPause());
    }

    public GameState getCurrentState() {
        return game.getCurrentState();
    }
}
