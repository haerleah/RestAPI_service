export const GAME_BOARD_WIDTH = 10;
export const GAME_BOARD_HEIGHT = 20;
export const NEXT_BOARD_WIDTH = 4;
export const NEXT_BOARD_HEIGHT = 4;

export const rootStyles = {
    '--tile-size': '20px',
    '--tile-color': '#eee',
    '--tile-active-color': '#222',
    '--game-board-width': GAME_BOARD_WIDTH,
    '--game-board-height': GAME_BOARD_HEIGHT,
    '--next-board-width': NEXT_BOARD_WIDTH,
    '--next-board-height': NEXT_BOARD_HEIGHT,
    '--game-board-gap': '2px',
    '--game-board-background': '#333',
};

export const keyCodes = {
    up: ['ArrowUp', 'KeyW', 'KeyI'],
    right: ['ArrowRight', 'KeyD', 'KeyL'],
    down: ['ArrowDown', 'KeyS', 'KeyK'],
    left: ['ArrowLeft', 'KeyA', 'KeyJ'],
    enter: ['Enter'],
    action: ['KeyR', 'Space'],
};
