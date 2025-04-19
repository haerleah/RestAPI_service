package ru.s21.lib.entities;

import java.awt.Point;
import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.List;

public abstract class CarEntity {
    protected static final int WIDTH = 3;
    protected static final int HEIGHT = 4;
    private static final int[][] MODEL_MASK = {{0, 1, 0},
                                               {1, 1, 1},
                                               {0, 1, 0},
                                               {1, 0, 1}};
    protected Point leftUpPoint;
    protected Point rightDownPoint;


    public CarEntity(Point leftUpPoint, Point rightDownPoint) {
        this.leftUpPoint = leftUpPoint;
        this.rightDownPoint = rightDownPoint;
    }

    public boolean collidesWith(CarEntity carEntity) {
        Rectangle thisCar = new Rectangle(leftUpPoint.x, leftUpPoint.y, WIDTH, HEIGHT);
        Rectangle otherCar = new Rectangle(carEntity.leftUpPoint.x, carEntity.leftUpPoint.y, WIDTH, HEIGHT);
        return thisCar.intersects(otherCar);
    }

    public List<Point> constructModel() {
        List<Point> model = new ArrayList<>();
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (MODEL_MASK[i][j] == 1)
                    model.add(new Point(leftUpPoint.x + j, leftUpPoint.y + i));
            }
        }
        return model;
    }
}
