package ru.s21.server.web.mapper;

import ru.s21.server.domain.model.*;
import ru.s21.server.exception.InvalidActionException;
import ru.s21.server.web.model.GameInfoDTO;
import ru.s21.server.web.model.GameStatusDTO;
import ru.s21.server.web.model.StateDTO;
import ru.s21.server.web.model.UserActionDTO;

public class WebMapper {

    public static GameStatusDTO toDTO(GameStatusModel gameStatusModel){
        return new GameStatusDTO(switch (gameStatusModel) {
            case MOVING -> "Moving";
            case SPAWN -> "Spawn";
            case EXIT -> "Exit";
            case PAUSE -> "Pause";
            case START -> "Start";
            case GAMEOVER -> "Gameover";
            case SHIFTING -> "Shifting";
            case ATTACHING -> "Attaching";
        });
    }

    public static StateDTO toDTO(StateModel stateModel) {
        return StateDTO.builder()
                .field(stateModel.getField())
                .next(stateModel.getNext())
                .level(stateModel.getLevel())
                .pause(stateModel.isPause())
                .score(stateModel.getScore())
                .speed(stateModel.getSpeed())
                .highScore(stateModel.getHighScore())
                .build();
    }

    public static GameInfoDTO toDTO(GameInfo gameInfo) {
        return GameInfoDTO.builder()
                .id(gameInfo.getId())
                .name(gameInfo.getName())
                .build();
    }

    public static UserActionModel toModel(UserActionDTO userActionDTO) {
        boolean hold = userActionDTO.isHold();
        return switch (userActionDTO.getId()) {
            case 1 -> new UserActionModel(ActionModel.Start, hold);
            case 2 -> new UserActionModel(ActionModel.Pause, hold);
            case 3 -> new UserActionModel(ActionModel.Terminate, hold);
            case 4 -> new UserActionModel(ActionModel.Left, hold);
            case 5 -> new UserActionModel(ActionModel.Right, hold);
            case 6 -> new UserActionModel(ActionModel.Up, hold);
            case 7 -> new UserActionModel(ActionModel.Down, hold);
            case 8 -> new UserActionModel(ActionModel.Action, hold);
            default -> throw new InvalidActionException("Invalid action");
        };
    }
}

