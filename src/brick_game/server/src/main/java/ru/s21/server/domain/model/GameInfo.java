package ru.s21.server.domain.model;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class GameInfo {
    private String name;
    private int id;
}
