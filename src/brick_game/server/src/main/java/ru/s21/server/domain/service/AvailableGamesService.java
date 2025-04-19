package ru.s21.server.domain.service;

import ru.s21.server.domain.model.GameInfo;

import java.util.List;

public interface AvailableGamesService {
    List<GameInfo> getAvailableGames();

    String getGameNameById(int id);
}
