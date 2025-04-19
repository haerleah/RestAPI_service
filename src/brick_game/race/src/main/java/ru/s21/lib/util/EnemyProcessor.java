package ru.s21.lib.util;

import lombok.Setter;
import ru.s21.lib.entities.EnemyCar;
import ru.s21.lib.entities.GameParameters;
import ru.s21.lib.entities.PlayerCar;
import ru.s21.lib.fsm.GameState;
import ru.s21.lib.processing.EnemyCarCleaner;
import ru.s21.lib.processing.EnemyCarSpawner;
import ru.s21.lib.processing.Game;
import ru.s21.lib.processing.ScoreHandler;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.LockSupport;
import java.util.random.RandomGenerator;

public class EnemyProcessor implements Runnable {
    private static final int BASE_TICK_DURATION = 50;
    private static final int FRAME_DURATION = 1500;
    private final AtomicBoolean shouldContinue;
    private final GameParameters associatedGameParameters;
    private final List<EnemyCar> enemies;
    private final ScoreHandler scoreHandler;
    private final GameStateContainer stateContainer;
    private final RandomGenerator random;
    @Setter
    private PlayerCar player;
    private final AtomicInteger counter;
    private int deltaAmplifier;
    @Setter
    private long sleepTime;
    private int previousSpeed;

    public EnemyProcessor(Game associatedGame, GameStateContainer stateContainer, RandomGenerator random) {
        shouldContinue = new AtomicBoolean(true);
        this.associatedGameParameters = associatedGame.getParameters();
        this.enemies = associatedGame.getEnemies();
        this.stateContainer = stateContainer;
        this.random = random;
        scoreHandler = new ScoreHandler(associatedGameParameters);
        counter = new AtomicInteger(0);
        previousSpeed = associatedGameParameters.getSpeed();
        deltaAmplifier = 0;
        sleepTime = (long) 10e6;
    }

    @Override
    public void run() {
        while (shouldContinue.get()) {
            if (associatedGameParameters.getSpeed() != previousSpeed) {
                deltaAmplifier = amplifyDelta();
                previousSpeed = associatedGameParameters.getSpeed();
            }

            counter.set(counter.addAndGet(BASE_TICK_DURATION + deltaAmplifier));

            if (counter.get() >= FRAME_DURATION && stateContainer.getCurrentState() != GameState.PAUSE) {
                processEnemyMoving(random);
            }

            LockSupport.parkNanos(sleepTime);
        }
    }

    private void processEnemyMoving(RandomGenerator random) {
        enemies.addAll(EnemyCarSpawner.spawn(random));

        enemies.forEach(enemyCar -> enemyCar.move(1));
        resetTimer();

        if (enemies.stream().anyMatch(enemyCar -> enemyCar.collidesWith(player))) {
            enemies.forEach(enemyCar -> enemyCar.move(-1));
            stateContainer.setCurrentState(GameState.GAMEOVER);
            shouldContinue.set(false);
        }

        int deletedEnemiesCount = EnemyCarCleaner.deleteInvisibleEnemies(enemies);
        if (deletedEnemiesCount > 0) {
            scoreHandler.addScore();
        }
    }

    public void speedUp() {
        counter.set(counter.addAndGet(500));
    }

    public void stop() {
        shouldContinue.set(false);
    }

    public void resetTimer() {
        counter.set(0);
    }

    private int amplifyDelta() {
        return (int) (((double) BASE_TICK_DURATION / 2) * Math.log(associatedGameParameters.getSpeed()));
    }
}
