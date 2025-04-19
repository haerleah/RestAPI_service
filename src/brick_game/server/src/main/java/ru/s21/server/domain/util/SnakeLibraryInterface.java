package ru.s21.server.domain.util;

import com.sun.jna.Library;
import com.sun.jna.Native;

public interface SnakeLibraryInterface extends Library {
    SnakeLibraryInterface INSTANCE = Native.load("s21_snake", SnakeLibraryInterface.class);

    int getGameStatus();

    GameInfo.ByValue updateScene();

    void processUserAction(int action, boolean hold);

    void initializeGame();

    void freeGameInfo(GameInfo.ByValue gameInfo);
}
