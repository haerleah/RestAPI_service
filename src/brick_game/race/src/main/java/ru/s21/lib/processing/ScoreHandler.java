package ru.s21.lib.processing;

import ru.s21.lib.datalayer.DataManager;
import ru.s21.lib.datalayer.FileDataManager;
import ru.s21.lib.entities.GameParameters;

import java.io.IOException;

public class ScoreHandler {
    private final GameParameters associatedGameParameters;

    public ScoreHandler(GameParameters associatedGameParameters) {
        this.associatedGameParameters = associatedGameParameters;
    }

    public void addScore() {
        int updatedScore = associatedGameParameters.getScore() + 1;
        associatedGameParameters.setScore(updatedScore);

        if (updatedScore > associatedGameParameters.getHighestScore()) {
            associatedGameParameters.setHighestScore(updatedScore);
            saveHighestScore(updatedScore);
        }

        if (updatedScore % 5 == 0 && associatedGameParameters.getLevel() < 10) {
            associatedGameParameters.setLevel(associatedGameParameters.getLevel() + 1);
            if (associatedGameParameters.getLevel() % 2 == 0 && associatedGameParameters.getLevel() > 2) {
                associatedGameParameters.setSpeed(associatedGameParameters.getSpeed() + 1);
            }
        }
    }

    private void saveHighestScore(int score) {
        try {
            DataManager dataManager = new FileDataManager();
            dataManager.saveHighestScore(score);
        } catch (IOException ignored) {
        }
    }
}
