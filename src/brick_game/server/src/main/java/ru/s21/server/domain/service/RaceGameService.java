package ru.s21.server.domain.service;

import org.springframework.stereotype.Service;
import ru.s21.lib.RaceFacade;
import ru.s21.server.domain.mapper.RaceMapper;
import ru.s21.server.domain.model.GameStatusModel;
import ru.s21.server.domain.model.StateModel;
import ru.s21.server.domain.model.UserActionModel;

@Service
public class RaceGameService implements GameService {
    private final RaceFacade raceFacade;

    public RaceGameService() {
        System.out.println(System.getProperty("java.library.path"));
        raceFacade = new RaceFacade();
    }

    @Override
    public synchronized void initializeGame() {
        raceFacade.initializeGame();
    }

    @Override
    public synchronized void processUserAction(UserActionModel userActionModel) {
        raceFacade.userInput(RaceMapper.toLibModel(userActionModel.getAction()), userActionModel.isHold());
    }

    @Override
    public synchronized GameStatusModel getGameStatus() {
        return RaceMapper.toModel(raceFacade.getCurrentState());
    }

    @Override
    public synchronized StateModel updateCurrentState() {
        return RaceMapper.toModel(raceFacade.updateCurrentState());
    }
}
