package ru.s21.lib.entities;

import java.awt.*;

public class EnemyCar extends CarEntity {

    public EnemyCar(Point leftUpPoint, Point rightDownPoint) {
        super(leftUpPoint, rightDownPoint);
    }

    public void move(int offset) {
        super.leftUpPoint.y += offset;
        super.rightDownPoint.y += offset;
    }

    public int getTopY() {
        return super.leftUpPoint.y;
    }
}
