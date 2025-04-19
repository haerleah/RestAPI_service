package ru.s21.server.web.model;

import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class GameInfoDTO {
    int id;
    String name;
}
