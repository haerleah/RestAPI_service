package ru.s21.server.domain.mapper;

import ru.s21.lib.Action;
import ru.s21.lib.State;
import ru.s21.lib.fsm.GameState;
import ru.s21.server.domain.model.ActionModel;
import ru.s21.server.domain.model.GameStatusModel;
import ru.s21.server.domain.model.StateModel;

public class RaceMapper {
    public static ActionModel toModel(Action action) {
        return switch (action) {
            case Up -> ActionModel.Up;
            case Down -> ActionModel.Down;
            case Action -> ActionModel.Action;
            case Left -> ActionModel.Left;
            case Right -> ActionModel.Right;
            case Pause -> ActionModel.Pause;
            case Start -> ActionModel.Start;
            case Terminate -> ActionModel.Terminate;
        };
    }

    public static GameStatusModel toModel(GameState gameState) {
        return switch (gameState) {
            case PAUSE -> GameStatusModel.PAUSE;
            case START -> GameStatusModel.START;
            case GAMEOVER -> GameStatusModel.GAMEOVER;
            case EXIT -> GameStatusModel.EXIT;
            case MOVING -> GameStatusModel.MOVING;
        };
    }

    public static StateModel toModel(State gameState) {
        return StateModel.builder()
                .field(gameState.field())
                .next(gameState.next())
                .speed(gameState.speed())
                .level(gameState.level())
                .pause(gameState.pause())
                .score(gameState.score())
                .highScore(gameState.highScore())
                .build();
    }

    public static Action toLibModel(ActionModel actionModel) {
        return switch (actionModel) {
            case Right -> Action.Right;
            case Left -> Action.Left;
            case Up -> Action.Up;
            case Down -> Action.Down;
            case Pause -> Action.Pause;
            case Start -> Action.Start;
            case Terminate -> Action.Terminate;
            case Action -> Action.Action;
        };
    }
}
