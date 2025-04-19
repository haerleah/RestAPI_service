import java.util.random.RandomGenerator;

public class TestRandom implements RandomGenerator {
    @Override
    public int nextInt(int bound) {
        // Provides spawn at middle
        return 0;
    }

    @Override
    public long nextLong() {
        return 0;
    }

}
