package ru.s21.lib.processing;

import lombok.Getter;
import ru.s21.lib.Action;
import ru.s21.lib.datalayer.DataManager;
import ru.s21.lib.datalayer.FileDataManager;
import ru.s21.lib.entities.*;
import ru.s21.lib.fsm.GameState;
import ru.s21.lib.util.EnemyProcessor;
import ru.s21.lib.util.GameStateContainer;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.random.RandomGenerator;

public class Game {
    @Getter
    private final List<EnemyCar> enemies;
    @Getter
    private final GameParameters parameters;
    @Getter
    private EnemyProcessor enemyProcessor;
    private final RandomGenerator random;
    private Thread enemyProcessorThread;
    private PlayerCar player;
    private final GameStateContainer currentState;

    public Game(RandomGenerator random) {
        enemies = new CopyOnWriteArrayList<>();
        parameters = new GameParameters();
        enemyProcessorThread = null;
        currentState = new GameStateContainer();
        this.random = random;

        currentState.setCurrentState(GameState.START);
        try {
            DataManager dataManager = new FileDataManager();
            parameters.setHighestScore(dataManager.getHighestScore());
        } catch (IOException ignored) {
        }
    }

    public void processAction(Action action, boolean hold) {
        switch (currentState.getCurrentState()) {
            case START -> {
                if (action == Action.Start) {
                    enemies.addAll(EnemyCarSpawner.initialSpawn(random));
                    player = PlayerCar.spawn();
                    enemyProcessor = new EnemyProcessor(this, currentState, random);
                    enemyProcessor.setPlayer(player);
                    enemyProcessorThread = new Thread(enemyProcessor);
                    enemyProcessorThread.start();
                    currentState.setCurrentState(GameState.MOVING);
                } else if (action == Action.Terminate) {
                    currentState.setCurrentState(GameState.EXIT);
                }
            }
            case MOVING -> {
                switch (action) {
                    case Left -> {
                        player.switchLine(LineSwitchingDirection.LEFT);
                        if (enemies.stream().anyMatch(enemyCar -> enemyCar.collidesWith(player))) {
                            currentState.setCurrentState(GameState.GAMEOVER);
                            enemyProcessor.stop();
                            player.switchLine(LineSwitchingDirection.RIGHT);
                        }
                    }
                    case Right -> {
                        player.switchLine(LineSwitchingDirection.RIGHT);
                        if (enemies.stream().anyMatch(enemyCar -> enemyCar.collidesWith(player))) {
                            currentState.setCurrentState(GameState.GAMEOVER);
                            enemyProcessor.stop();
                            player.switchLine(LineSwitchingDirection.LEFT);
                        }
                    }
                    case Pause -> {
                        currentState.setCurrentState(GameState.PAUSE);
                        parameters.setPause(true);
                    }
                    case Up -> {
                        if (hold) {
                            enemyProcessor.speedUp();
                        }
                    }
                    case Terminate -> {
                        currentState.setCurrentState(GameState.EXIT);
                        processAction(action, false);
                    }
                }
            }
            case GAMEOVER -> {
                enemies.clear();
                player = null;
                parameters.reset();
                enemyProcessor.stop();
                try {
                    enemyProcessorThread.join();
                } catch (InterruptedException ignored) {
                }
                currentState.setCurrentState(GameState.START);
                processAction(action, false);
            }
            case PAUSE -> {
                if (action == Action.Pause) {
                    parameters.setPause(false);
                }
                if (!parameters.isPause()) {
                    currentState.setCurrentState(GameState.MOVING);
                }
            }
            case EXIT -> {
                enemyProcessor.stop();
                try {
                    enemyProcessorThread.join();
                } catch (InterruptedException ignored) {
                }
            }
        }
    }

    public GameState getCurrentState() {
        return currentState.getCurrentState();
    }

    public int[][] getCurrentField() {
        int[][] field = new int[20][10];

        enemies.forEach(enemyCar -> enemyCar.constructModel().forEach(point -> {
            if (point.x >= 0 && point.x < 10 && point.y >= 0 && point.y < 20) {
                field[point.y][point.x] = 1;
            }
        }));

        if (player != null) {
            player.constructModel().forEach(point -> field[point.y][point.x] = 1);
        }

        return field;
    }

}
