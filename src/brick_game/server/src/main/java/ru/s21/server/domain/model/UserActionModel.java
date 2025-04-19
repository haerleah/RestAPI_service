package ru.s21.server.domain.model;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class UserActionModel {
    ActionModel action;
    boolean hold;
}
