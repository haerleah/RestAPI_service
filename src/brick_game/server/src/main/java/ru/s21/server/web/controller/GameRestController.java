package ru.s21.server.web.controller;

import org.springframework.web.bind.annotation.*;
import ru.s21.server.domain.model.ActionModel;
import ru.s21.server.domain.service.AvailableGamesService;
import ru.s21.server.domain.service.GameService;
import ru.s21.server.exception.*;
import ru.s21.server.web.mapper.WebMapper;
import ru.s21.server.web.model.GameInfoDTO;
import ru.s21.server.web.model.GameStatusDTO;
import ru.s21.server.web.model.StateDTO;
import ru.s21.server.web.model.UserActionDTO;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@RestController
@RestExceptionHandler
@RequestMapping("/api")
public class GameRestController {
    private final Map<String, GameService> serviceMap;
    private final AvailableGamesService availableGamesService;
    private GameService currentGameService;

    public GameRestController(Map<String, GameService> serviceMap, AvailableGamesService availableGamesService) {
        this.serviceMap = serviceMap;
        this.availableGamesService = availableGamesService;
    }

    @GetMapping("/games")
    List<GameInfoDTO> getGames() {
        return availableGamesService.getAvailableGames().stream()
                .map(WebMapper::toDTO).collect(Collectors.toList());
    }

    @PostMapping("/games/{gameId}")
    void chooseGame(@PathVariable int gameId) {
        if (currentGameService != null) {
            throw new GameServiceConflictException("Game already exists");
        }

        if(availableGamesService.getGameNameById(gameId) == null) {
            throw new GameNotFoundException("Game not found");
        }

        currentGameService = serviceMap.get(availableGamesService.getGameNameById(gameId));
        currentGameService.initializeGame();
    }

    @PostMapping("/actions")
    void submitAction(@RequestBody UserActionDTO userActionDTO) {
        if(currentGameService == null) {
            throw new GameNotStartedException("Game not started");
        }
        currentGameService.processUserAction(WebMapper.toModel(userActionDTO));

        if(WebMapper.toModel(userActionDTO).getAction() == ActionModel.Terminate) {
            currentGameService = null;
        }
    }

    @GetMapping("/state")
    StateDTO getState() {
        if(currentGameService == null) {
            throw new GameNotStartedException("Game not started");
        }
        return WebMapper.toDTO(currentGameService.updateCurrentState());
    }

    @GetMapping("/status")
    GameStatusDTO getStatus() {
        if(currentGameService == null) {
            throw new GameNotStartedException("Game not started");
        }
        return WebMapper.toDTO(currentGameService.getGameStatus());
    }
}
