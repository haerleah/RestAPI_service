package ru.s21.lib.entities;

import lombok.Data;

@Data
public class GameParameters {
    private int score;
    private int highestScore;
    private int level = 1;
    private boolean pause = false;
    private volatile int speed = 1;

    public void reset() {
        score = 0;
        level = 1;
        speed = 1;
    }
}
