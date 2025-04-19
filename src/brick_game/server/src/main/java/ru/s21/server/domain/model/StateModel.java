package ru.s21.server.domain.model;

import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class StateModel {
    int[][] field;
    int[][] next;
    int score;
    int highScore;
    int level;
    int speed;
    boolean pause;
}
