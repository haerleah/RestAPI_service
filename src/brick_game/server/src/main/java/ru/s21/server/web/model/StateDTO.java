package ru.s21.server.web.model;

import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class StateDTO {
    // Jackson способен серилиазовать матрицу, поэтому не обязательно маппить ее в строку
    int[][] field;
    int[][] next;
    int score;
    int highScore;
    int level;
    int speed;
    boolean pause;
}
