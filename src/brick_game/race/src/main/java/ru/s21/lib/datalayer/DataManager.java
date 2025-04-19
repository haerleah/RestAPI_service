package ru.s21.lib.datalayer;

import java.io.IOException;

public interface DataManager {
    void saveHighestScore(int score) throws IOException;

    int getHighestScore() throws IOException;
}
