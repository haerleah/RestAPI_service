import {GAME_BOARD_WIDTH, GAME_BOARD_HEIGHT, NEXT_BOARD_HEIGHT, NEXT_BOARD_WIDTH} from './config.js';

export class GameBoard {
    constructor($gameBoard) {
        this.element = $gameBoard;
        this.tiles = [];
        for (let i = 0; i < GAME_BOARD_WIDTH; ++i) {
            for (let j = 0; j < GAME_BOARD_HEIGHT; ++j) {
                const $tile = document.createElement('div');
                $tile.classList.add('tile');
                $tile.id = `position-${i}-${j}`;
                this.tiles.push($tile);
                this.element.append($tile);
            }
        }
    }

    getTile(x, y) {
        return this.tiles[y * GAME_BOARD_WIDTH + x];
    }

    enableTile(x, y, color = "black") {
        const tile = this.getTile(x, y);
        tile.classList.add('active');
        tile.style.backgroundColor = color;
    }

    disableTile(x, y) {
        const tile = this.getTile(x, y);
        tile.classList.remove('active');
        tile.style.backgroundColor = '';
    }
}

export class NextBoard {
    constructor($nextBoard) {
        this.element = $nextBoard;
        this.tiles = [];
        for (let i = 0; i < NEXT_BOARD_WIDTH; ++i) {
            for (let j = 0; j < NEXT_BOARD_HEIGHT; ++j) {
                const $tile = document.createElement('div');
                $tile.classList.add('tile');
                $tile.id = `position-${i}-${j}`;
                this.tiles.push($tile);
                this.element.append($tile);
            }
        }
    }


    getTile(x, y) {
        return this.tiles[y * NEXT_BOARD_WIDTH + x];
    }

    enableTile(x, y, color = "black") {
        const tile = this.getTile(x, y);
        tile.classList.add('active');
        tile.style.backgroundColor = color;
    }

    disableTile(x, y) {
        const tile = this.getTile(x, y);
        tile.classList.remove('active');
        tile.style.backgroundColor = '';
    }
}
