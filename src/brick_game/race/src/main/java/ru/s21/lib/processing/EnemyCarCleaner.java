package ru.s21.lib.processing;

import ru.s21.lib.entities.EnemyCar;

import java.util.ArrayList;
import java.util.List;

public class EnemyCarCleaner {
    public static int deleteInvisibleEnemies(List<EnemyCar> enemies) {
        int deletedEnemiesCount = 0;
        List<EnemyCar> enemiesToDelete = new ArrayList<>();
        for (EnemyCar enemyCar : enemies) {
            if (enemyCar.getTopY() > 19) {
                enemiesToDelete.add(enemyCar);
                deletedEnemiesCount++;
            }
        }
        enemies.removeAll(enemiesToDelete);
        return deletedEnemiesCount;
    }
}
