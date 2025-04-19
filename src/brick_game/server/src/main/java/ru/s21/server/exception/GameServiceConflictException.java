package ru.s21.server.exception;

import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.ResponseStatus;

@ResponseStatus(value = HttpStatus.CONFLICT)
public class GameServiceConflictException extends RuntimeException {
    public GameServiceConflictException(String message) {super(message);}
}
