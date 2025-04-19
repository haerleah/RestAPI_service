package ru.s21.lib;

public record State(int[][] field, int[][] next, int score, int highScore, int level, int speed, boolean pause) {
}