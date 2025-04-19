package ru.s21.server.domain.service;

import org.springframework.stereotype.Service;
import ru.s21.server.domain.mapper.JNAMapper;
import ru.s21.server.domain.model.GameStatusModel;
import ru.s21.server.domain.model.StateModel;
import ru.s21.server.domain.model.UserActionModel;
import ru.s21.server.domain.util.GameInfo;
import ru.s21.server.domain.util.SnakeLibraryInterface;

@Service
public class SnakeGameService implements GameService {
    private final SnakeLibraryInterface library = SnakeLibraryInterface.INSTANCE;
    @Override
    public synchronized void initializeGame() {
        library.initializeGame();
    }

    @Override
    public synchronized void processUserAction(UserActionModel userActionModel) {
        library.processUserAction(userActionModel.getAction().ordinal(), userActionModel.isHold());
    }

    @Override
    public synchronized GameStatusModel getGameStatus() {
        return JNAMapper.toModel(library.getGameStatus());
    }

    @Override
    public synchronized StateModel updateCurrentState() {
        GameInfo.ByValue state = library.updateScene();
        state.setAutoRead(false);
        state.setAutoWrite(false);
        state.read();
        StateModel stateModel = JNAMapper.toModel(state);
        library.freeGameInfo(state);
        return stateModel;
    }
}
