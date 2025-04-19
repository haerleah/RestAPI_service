package ru.s21.server.domain.service;

import org.springframework.stereotype.Service;
import ru.s21.server.domain.model.GameInfo;

import java.util.List;

@Service
public class StaticAvailableGamesService implements AvailableGamesService {
    @Override
    public List<GameInfo> getAvailableGames() {
        return List.of(new GameInfo("Tetris", 1),
                new GameInfo("Snake", 2),
                new GameInfo("Race", 3));
    }

    @Override
    public String getGameNameById(int id) {
        return switch (id) {
            case 1 -> "tetrisGameService";
            case 2 -> "snakeGameService";
            case 3 -> "raceGameService";
            default -> null;
        };
    }
}
