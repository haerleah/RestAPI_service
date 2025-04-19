import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import ru.s21.lib.Action;
import ru.s21.lib.RaceFacade;
import ru.s21.lib.State;
import ru.s21.lib.datalayer.DataManager;
import ru.s21.lib.datalayer.FileDataManager;
import ru.s21.lib.entities.EnemyCar;
import ru.s21.lib.fsm.GameState;
import ru.s21.lib.processing.Game;

import java.awt.*;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class RaceTest {
    private Game game;

    @BeforeEach
    public void setUp() {
        File maxScoreFile = new File("race_score");
        maxScoreFile.delete();
        game = new Game(new TestRandom());
    }

    @Test
    public void initializeTest() {
        Assertions.assertEquals(GameState.START, game.getCurrentState());
        Assertions.assertEquals(0, game.getParameters().getScore());
        Assertions.assertEquals(1, game.getParameters().getSpeed());
        Assertions.assertEquals(1, game.getParameters().getLevel());
    }

    @Test
    public void startGameTest() {
        game.processAction(Action.Start, false);
        Assertions.assertEquals(GameState.MOVING, game.getCurrentState());
        Assertions.assertNotEquals(0, game.getCurrentField()[16][4]);
    }

    @Test
    public void endGameTest() {
        game.processAction(Action.Terminate, false);
        Assertions.assertEquals(GameState.EXIT, game.getCurrentState());
    }

    @Test
    public void mismatchInputAtStartTest() {
        game.processAction(Action.Left, false);
        game.processAction(Action.Right, false);
        game.processAction(Action.Up, false);
        game.processAction(Action.Down, true);
        Assertions.assertEquals(GameState.START, game.getCurrentState());
    }

    @Test
    public void leftMoveTest() {
        game.processAction(Action.Start, false);
        for (int i = 0; i < 3; ++i) {
            game.processAction(Action.Left, false);
        }
        Assertions.assertEquals(GameState.MOVING, game.getCurrentState());
        Assertions.assertNotEquals(0, game.getCurrentField()[16][1]);
        Assertions.assertNotEquals(1, game.getCurrentField()[16][5]);
    }

    @Test
    public void rightMoveTest() {
        game.processAction(Action.Start, false);
        for (int i = 0; i < 3; ++i) {
            game.processAction(Action.Right, false);
        }
        Assertions.assertEquals(GameState.MOVING, game.getCurrentState());
        Assertions.assertNotEquals(0, game.getCurrentField()[16][7]);
        Assertions.assertNotEquals(1, game.getCurrentField()[16][5]);
    }

    @Test
    public void pauseTest() {
        game.processAction(Action.Start, false);
        game.processAction(Action.Pause, false);
        Assertions.assertEquals(GameState.PAUSE, game.getCurrentState());
        Assertions.assertTrue(game.getParameters().isPause());
        game.processAction(Action.Pause, false);
        Assertions.assertNotEquals(GameState.PAUSE, game.getCurrentState());
        Assertions.assertFalse(game.getParameters().isPause());
    }

    @Test
    public void mismatchPauseInputTest() {
        game.processAction(Action.Start, false);
        game.processAction(Action.Pause, false);
        Assertions.assertEquals(GameState.PAUSE, game.getCurrentState());
        Assertions.assertTrue(game.getParameters().isPause());
        game.processAction(Action.Left, false);
        game.processAction(Action.Right, false);
        game.processAction(Action.Up, false);
        Assertions.assertEquals(GameState.PAUSE, game.getCurrentState());
        Assertions.assertTrue(game.getParameters().isPause());
    }

    @Test
    public void scoreTest() throws InterruptedException {
        game.processAction(Action.Start, false);
        game.getEnemyProcessor().setSleepTime(1);
        game.processAction(Action.Left, false);
        Thread.sleep(400);
        Assertions.assertTrue(game.getParameters().getScore() > 0);
        Assertions.assertTrue(game.getParameters().getHighestScore() > 0);
    }

    @Test
    public void gameOverTest() throws InterruptedException {
        game.processAction(Action.Start, false);
        game.getEnemyProcessor().setSleepTime(1);
        Thread.sleep(400);
        Assertions.assertEquals(GameState.GAMEOVER, game.getCurrentState());
    }

    @Test
    public void inputAfterGameOverTest() throws InterruptedException {
        game.processAction(Action.Start, false);
        game.getEnemyProcessor().setSleepTime(1);
        Thread.sleep(400);
        Assertions.assertEquals(GameState.GAMEOVER, game.getCurrentState());
        game.processAction(Action.Start, false);
        Assertions.assertEquals(GameState.MOVING, game.getCurrentState());
    }

    @Test
    public void maxScoreTest() throws IOException {
        File maxScoreFile = new File("race_score");
        try (FileWriter writer = new FileWriter(maxScoreFile)) {
            writer.write("1337");
        }
        DataManager dataManager = new FileDataManager();
        Assertions.assertEquals(1337, dataManager.getHighestScore());
    }

    @Test
    public void gameMaxScoreTest() throws IOException {
        File maxScoreFile = new File("race_score");
        try (FileWriter writer = new FileWriter(maxScoreFile)) {
            writer.write("1337");
        }
        Game game = new Game(new TestRandom());
        Assertions.assertEquals(1337, game.getParameters().getHighestScore());
    }

    @Test
    public void leftShiftCollisionTest() {
        EnemyCar leftCar = new EnemyCar(new Point(0, 16), new Point(2, 19));
        game.getEnemies().add(leftCar);
        game.processAction(Action.Start, false);
        game.processAction(Action.Left, false);
        Assertions.assertEquals(GameState.GAMEOVER, game.getCurrentState());
    }

    @Test
    public void rightShiftCollisionTest() {
        EnemyCar rightCar = new EnemyCar(new Point(2 * 3, 16), new Point(2 * 3 + 2, 19));
        game.getEnemies().add(rightCar);
        game.processAction(Action.Start, false);
        game.processAction(Action.Right, false);
        Assertions.assertEquals(GameState.GAMEOVER, game.getCurrentState());
    }

    @Test
    public void speedUpTest() throws InterruptedException {
        EnemyCar rightCar = new EnemyCar(new Point(2 * 3, 19), new Point(2 * 3 + 2, 24));
        game.getEnemies().add(rightCar);
        game.processAction(Action.Start, false);
        game.getEnemyProcessor().setSleepTime(1);
        for (int i = 0; i < 3; ++i) {
            game.processAction(Action.Up, true);
        }
        Thread.sleep(1);
        Assertions.assertTrue(game.getParameters().getScore() > 0);
    }

    @Test
    public void gameTerminationAtPlayingTest() {
        game.processAction(Action.Start, false);
        game.processAction(Action.Left, false);
        game.processAction(Action.Terminate, false);
        Assertions.assertEquals(GameState.EXIT, game.getCurrentState());
    }

    @Test
    public void getCurrentFieldTest() {
        EnemyCar leftCar = new EnemyCar(new Point(0, 16), new Point(2, 19));
        game.getEnemies().add(leftCar);
        int[][] field = game.getCurrentField();
        Assertions.assertEquals(1, field[16][1]);
        Assertions.assertEquals(1, field[17][0]);
        Assertions.assertEquals(1, field[17][1]);
        Assertions.assertEquals(1, field[17][2]);
        Assertions.assertEquals(1, field[18][1]);
        Assertions.assertEquals(1, field[19][0]);
        Assertions.assertEquals(1, field[19][2]);
    }

    @Test
    public void facadeTerminationAtStartTest() {
        RaceFacade facade = new RaceFacade();
        facade.initializeGame();
        facade.userInput(Action.Terminate, false);
        Assertions.assertEquals(GameState.EXIT, facade.getCurrentState());
    }

    @Test
    public void facadeGetCurrentStateTest() {
        RaceFacade facade = new RaceFacade();
        facade.initializeGame();
        State currentState = facade.updateCurrentState();
        Assertions.assertEquals(GameState.START, facade.getCurrentState());
        Assertions.assertArrayEquals(new int[20][10], currentState.field());
        Assertions.assertNull(currentState.next());
        Assertions.assertEquals(0, currentState.highScore());
        Assertions.assertEquals(0, currentState.score());
        Assertions.assertEquals(1, currentState.level());
        Assertions.assertEquals(1, currentState.speed());
        Assertions.assertFalse(currentState.pause());
    }
}
