package ru.s21.server.web.controller.advice;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import ru.s21.server.exception.*;
import ru.s21.server.web.model.ErrorMessageDTO;

@RestControllerAdvice(annotations = RestExceptionHandler.class)
public class GameRestControllerAdvice {
    @ExceptionHandler(GameNotStartedException.class)
    public ResponseEntity<ErrorMessageDTO> handleBadRequestException(GameNotStartedException e) {
        ErrorMessageDTO errorDTO = new ErrorMessageDTO(e.getMessage());
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(errorDTO);
    }

    @ExceptionHandler(GameNotFoundException.class)
    public ResponseEntity<ErrorMessageDTO> handleGameNotFoundException(GameNotFoundException e) {
        ErrorMessageDTO errorDTO = new ErrorMessageDTO(e.getMessage());
        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(errorDTO);
    }

    @ExceptionHandler(GameServiceConflictException.class)
    public ResponseEntity<ErrorMessageDTO> handleGameServiceConflictException(GameServiceConflictException e) {
        ErrorMessageDTO errorDTO = new ErrorMessageDTO(e.getMessage());
        return ResponseEntity.status(HttpStatus.CONFLICT).body(errorDTO);
    }

    @ExceptionHandler(InvalidActionException.class)
    public ResponseEntity<ErrorMessageDTO> handleInvalidActionException(InvalidActionException e) {
        ErrorMessageDTO errorDTO = new ErrorMessageDTO(e.getMessage());
        return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(errorDTO);
    }

    @ExceptionHandler(Exception.class)
    public ResponseEntity<ErrorMessageDTO> handleOtherException(Exception e) {
        ErrorMessageDTO errorDTO = new ErrorMessageDTO("Internal Server Error");
        return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(errorDTO);
    }
}
