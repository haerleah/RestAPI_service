package ru.s21.server.domain.mapper;

import com.sun.jna.Pointer;
import ru.s21.server.domain.model.GameStatusModel;
import ru.s21.server.domain.model.StateModel;
import ru.s21.server.domain.util.GameInfo;

import java.util.Arrays;

public class JNAMapper {
    public static GameStatusModel toModel(int value) throws IllegalArgumentException {
        return switch (value) {
            case 0 -> GameStatusModel.START;
            case 1 -> GameStatusModel.SPAWN;
            case 2 -> GameStatusModel.MOVING;
            case 3 -> GameStatusModel.SHIFTING;
            case 4 -> GameStatusModel.ATTACHING;
            case 5 -> GameStatusModel.GAMEOVER;
            case 6 -> GameStatusModel.EXIT;
            case 7 -> GameStatusModel.PAUSE;
            default -> throw new IllegalStateException("Unexpected value: " + value);
        };
    }

    public static StateModel toModel(GameInfo gameInfo) throws IllegalArgumentException {
        int[][] field = new int[20][10];
        Pointer[] fieldPointerArray = gameInfo.getField().getPointerArray(0, 20);

        for (int i = 0; i < 20; i++) {
            int[] row = fieldPointerArray[i].getIntArray(0, 10);
            field[i] = Arrays.copyOf(row, row.length);
        }

        int[][] next;
        try {
            next = new int[4][4];
            Pointer[] nextPointerArray = gameInfo.getNext().getPointerArray(0, 4);
            for (int i = 0; i < 4; i++) {
                int[] row = nextPointerArray[i].getIntArray(0, 4);
                next[i] = Arrays.copyOf(row, row.length);
            }
        } catch (Exception e) {
            next = null;
        }

        return StateModel.builder()
                .field(field)
                .next(next)
                .level(gameInfo.getLevel())
                .pause(gameInfo.getPause() == 1)
                .speed(gameInfo.getSpeed())
                .highScore(gameInfo.getHigh_score())
                .score(gameInfo.getScore())
                .build();

    }
}
