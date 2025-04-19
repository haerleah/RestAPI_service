package ru.s21.lib.datalayer;

import java.io.*;

public class FileDataManager implements DataManager {
    private static final String FILE_NAME = "race_score";
    private final File file;


    public FileDataManager() {
        file = new File(FILE_NAME);
    }

    @Override
    public void saveHighestScore(int score) throws IOException {
        try (FileWriter writer = new FileWriter(file)) {
            writer.write(String.valueOf(score));
        }
    }

    @Override
    public int getHighestScore() throws IOException, NumberFormatException {
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            return Integer.parseInt(reader.readLine());
        }
    }

}
