package ru.s21.lib.processing;

import ru.s21.lib.entities.EnemyCar;

import java.awt.*;
import java.util.*;
import java.util.List;
import java.util.random.RandomGenerator;

public class EnemyCarSpawner {
    private static int spawnTick = 0;

    public static List<EnemyCar> spawn(RandomGenerator random) {
        if (spawnTick == 9) {
            int enemiesCount = random.nextInt(2) + 1;
            List<EnemyCar> enemies = new ArrayList<>();

            List<Integer> spawnPositions = Arrays.asList(0, 1, 2);
            Collections.shuffle(spawnPositions, random);
            spawnPositions = spawnPositions.subList(0, enemiesCount);

            for (int i = 0; i < enemiesCount; ++i) {
                Point leftUpPoint = new Point(spawnPositions.get(i) * 3, -5);
                Point rightDownPoint = new Point(spawnPositions.get(i) * 3 + 2, -1);
                enemies.add(new EnemyCar(leftUpPoint, rightDownPoint));
            }
            spawnTick = 0;
            return enemies;
        }
        spawnTick++;
        return Collections.emptyList();
    }

    public static List<EnemyCar> initialSpawn(RandomGenerator random) {
        spawnTick = 9;
        return spawn(random);
    }
}
