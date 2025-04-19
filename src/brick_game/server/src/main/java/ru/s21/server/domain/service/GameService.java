package ru.s21.server.domain.service;

import ru.s21.server.domain.model.GameStatusModel;
import ru.s21.server.domain.model.StateModel;
import ru.s21.server.domain.model.UserActionModel;

public interface GameService {
    void initializeGame();

    void processUserAction(UserActionModel userActionModel);

    GameStatusModel getGameStatus();

    StateModel updateCurrentState();

}
