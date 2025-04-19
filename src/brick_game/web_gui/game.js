import {applyRootStyles} from './src/utils.js';
import {GameBoard, NextBoard} from './src/game-board.js';
import {rootStyles, keyCodes} from './src/config.js';

let shouldPolling = false;

applyRootStyles(rootStyles);
const gameBoard = new GameBoard(document.querySelector('#game-board'));

const nextBoard = new NextBoard(document.querySelector('#next-board'));

const $sidePanel = document.querySelector('#side-panel');

document.addEventListener("DOMContentLoaded", () => {
    const buttonsList = document.getElementById("buttonsList");
    fetch("/api/games")
        .then(response => response.json())
        .then(data => {
            data.forEach(gameInfo => {
                const button = document.createElement("button");
                const inputId = document.createElement("input");
                inputId.hidden = true;
                button.textContent = gameInfo.name;
                inputId.value = gameInfo.id;
                button.appendChild(inputId);
                buttonsList.appendChild(button);
            });
            buttonsList.querySelectorAll("button").forEach(button =>
                button.addEventListener("click", function () {
                    chooseGame(button.textContent, button.querySelector("input").value);
                })
            );
        });
});

document.addEventListener('keydown', function (event) {
    if (keyCodes.up.includes(event.code)) {
        void makeAction(6, event.repeat);
    }
    if (keyCodes.right.includes(event.code)) {
        void makeAction(5, event.repeat);
    }
    if (keyCodes.down.includes(event.code)) {
        void makeAction(7, event.repeat);
    }
    if (keyCodes.left.includes(event.code)) {
        void makeAction(4, event.repeat);
    }
    if (keyCodes.enter.includes(event.code)) {
        shouldPolling = true;
        void pollGameState();
        void makeAction(1, false);
    }
    if (keyCodes.action.includes(event.code)) {
        void makeAction(8, false);
    }
});

document.getElementById("pauseButton").addEventListener("click", () => {
    void makeAction(2, false);
});

document.getElementById("exitGame").addEventListener("click", () => {
    shouldPolling = false;
    makeAction(3, false).then(() => document.location.href = "/");
});

function chooseGame(name, id) {
    fetch(`/api/games/${id}`, {method: "POST"})
        .then(response => {
            if (!response.ok) {
                response.json().then(data => alert("Error " + response.status + "!\n" + data.message));
            } else {
                document.getElementById("initialMenu").style.display = "none";
                document.getElementById("game").style.display = "flex";
                document.getElementById("title").textContent = name;
                shouldPolling = true;
                void pollGameStatus();
                alert("Game was successfully started");
            }
        })
}

function makeAction(id, hold) {
    const userAction = {
        id: id,
        hold: hold
    };
    return fetch("/api/actions", {
        method: "POST",
        headers: {
            'Accept': 'application/json, text/plain, */*',
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(userAction)
    }).then(response => {
        if (!response.ok) {
            response.json().then(data => alert("Error " + response.status + "!\n" + data.message));
        }
    });
}

function renderTips(tipsData) {
    const tipsBlock = document.getElementById("topmostDiv");
    const tipsLabel = document.getElementById("topmostLabel");
    tipsBlock.style.display = "none";
    document.getElementById("exitGame").disabled = false;
    switch (tipsData.status) {
        case "Start":
            tipsBlock.style.display = "flex";
            tipsLabel.textContent = "Нажмите Enter, чтобы начать игру";
            break;
        case "Gameover":
            tipsBlock.style.display = "flex";
            tipsLabel.textContent = "Вы проиграли! Нажмите Enter, чтобы начать заново";
            break;
        case "Pause" :
            tipsBlock.style.display = "flex";
            tipsLabel.textContent = "Пауза...";
            document.getElementById("exitGame").disabled = true;
            break;
        default:
            break;
    }
}

function renderGame(gameData) {
    document.getElementById("currentScore").textContent = gameData.score;
    document.getElementById("highestScore").textContent = gameData.highScore;
    document.getElementById("level").textContent = gameData.level;
    document.getElementById("speed").textContent = gameData.speed;

    const field = gameData.field;

    for (let i = 0; i < field.length; ++i) {
        for (let j = 0; j < field[i].length; ++j) {
            switch (field[i][j]) {
                case 0:
                    gameBoard.disableTile(j, i);
                    break;
                case 1:
                    gameBoard.enableTile(j, i, "crimson");
                    break;
                case 2:
                    gameBoard.enableTile(j, i, "darkturquoise");
                    break;
                case 3:
                    gameBoard.enableTile(j, i, "seagreen");
                    break;
                case 4:
                    gameBoard.enableTile(j, i, "dodgerblue");
                    break;
                case 5:
                    gameBoard.enableTile(j, i, "gold");
                    break;
                case 6:
                    gameBoard.enableTile(j, i, "hotpink");
                    break;
                case 7:
                    gameBoard.enableTile(j, i, "indigo");
                    break;
                case 26:
                    gameBoard.enableTile(j, i, "crimson");
                    break;
                default:
                    gameBoard.enableTile(j, i, "dodgerblue");
                    break;
            }
        }
    }

    if (gameData.next !== null) {
        const next = gameData.next;

        for (let i = 0; i < next.length; ++i) {
            for (let j = 0; j < next[i].length; ++j) {
                switch (next[i][j]) {
                    case 1:
                        nextBoard.enableTile(j, i, "crimson");
                        break;
                    case 2:
                        nextBoard.enableTile(j, i, "darkturquoise");
                        break;
                    case 3:
                        nextBoard.enableTile(j, i, "seagreen");
                        break;
                    case 4:
                        nextBoard.enableTile(j, i, "dodgerblue");
                        break;
                    case 5:
                        nextBoard.enableTile(j, i, "gold");
                        break;
                    case 6:
                        nextBoard.enableTile(j, i, "hotpink");
                        break;
                    case 7:
                        nextBoard.enableTile(j, i, "indigo");
                        break;
                    case 8:
                        nextBoard.enableTile(j, i);
                        break;
                    default:
                        nextBoard.disableTile(j, i);
                }
            }
        }
    }
}

async function pollGameState() {
    while (shouldPolling) {
        try {
            const gameResponse = await fetch("api/state");
            const gameData = await gameResponse.json();
            const statusResponse = await fetch("api/status");
            const statusData = await statusResponse.json();

            renderTips(statusData);
            renderGame(gameData);
        } catch (e) {
            console.error(e);
            break;
        }
        await new Promise(resolve => setTimeout(resolve, 17));
    }
}

async function pollGameStatus() {
    while (shouldPolling) {
        try {
            const statusResponse = await fetch("api/status");
            const statusData = await statusResponse.json();

            renderTips(statusData);
        } catch (e) {
            console.error(e);
            break;
        }
        await new Promise(resolve => setTimeout(resolve, 10000));
    }
}