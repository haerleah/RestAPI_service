package ru.s21.lib.entities;

import java.awt.*;

public class PlayerCar extends CarEntity {
    private static final int INITIAL_TOP_X = 3;
    private static final int INITIAL_BOTTOM_X = 5;
    private static final int INITIAL_TOP_Y = 16;
    private static final int INITIAL_BOTTOM_Y = 19;

    public static PlayerCar spawn() {
        Point leftUpPoint = new Point(INITIAL_TOP_X, INITIAL_TOP_Y);
        Point rightUpPoint = new Point(INITIAL_BOTTOM_X, INITIAL_BOTTOM_Y);
        return new PlayerCar(leftUpPoint, rightUpPoint);
    }

    private PlayerCar(Point leftUpPoint, Point rightDownPoint) {
        super(leftUpPoint, rightDownPoint);
    }

    public void switchLine(LineSwitchingDirection direction) {
        switch (direction) {
            case LEFT -> {
                if (!validateLeftSwitch())
                    return;
                super.leftUpPoint.x -= 3;
                super.rightDownPoint.x -= 3;
            }

            case RIGHT -> {
                if (!validateRightSwitch())
                    return;
                super.leftUpPoint.x += 3;
                super.rightDownPoint.x += 3;
            }
        }
    }

    private boolean validateRightSwitch() {
        return super.rightDownPoint.x <= 5;
    }

    private boolean validateLeftSwitch() {
        return super.leftUpPoint.x >= 3;
    }
}
