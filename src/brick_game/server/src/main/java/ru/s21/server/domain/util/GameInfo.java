package ru.s21.server.domain.util;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import lombok.Data;
import lombok.EqualsAndHashCode;


@EqualsAndHashCode(callSuper = true)
@Structure.FieldOrder({ "field", "next", "score", "high_score", "level", "speed", "pause" })
@Data
public class GameInfo extends Structure {
    public Pointer field;
    public Pointer next;
    public int score;
    public int high_score;
    public int level;
    public int speed;
    public int pause;

    public GameInfo() {
        super();
    }

    public static class ByValue extends GameInfo implements Structure.ByValue {
        public ByValue() {
            super();
        }
    }
}
