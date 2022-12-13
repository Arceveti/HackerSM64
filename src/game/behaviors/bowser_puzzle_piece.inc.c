
/**
 * Behavior for the sliding Bowser puzzle in Lethal Lava Land.
 */

// The size of an individual piece.
#define LLL_BOWSER_PUZZLE_PIECE_SIZE 512.0f
// How many frames it takes for each piece to move.
#define LLL_BOWSER_PUZZLE_PIECE_MOVE_DURATION 4
// How many frames to shake each piece before moving.
#define LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME 20
// The amount of units the piece shakes before moving.
#define LLL_BOWSER_PUZZLE_PIECE_SHAKE_AMOUNT 6


/*
 * The pieces move in this order:
 *
 *   1, 2, 5, 6, 10, 9, 13, 12, 8, 7, 3, 4
 *
 * Once they reach the end of the routine they follow it backwards until the
 *   puzzle is complete again.
 *
 * Note that pieces 11 and 14 do not move.
 */
struct BowserPuzzleAction {
    s8 index;
    ObjAction8 action;
};

static const struct BowserPuzzleAction sPieceActions[13] = {
    {  0, BOWSER_PUZZLE_PIECE_ACT_IDLE  },
    {  1, BOWSER_PUZZLE_PIECE_ACT_LEFT  },
    {  2, BOWSER_PUZZLE_PIECE_ACT_LEFT  },
    {  5, BOWSER_PUZZLE_PIECE_ACT_UP    },
    {  6, BOWSER_PUZZLE_PIECE_ACT_LEFT  },
    { 10, BOWSER_PUZZLE_PIECE_ACT_UP    },
    {  9, BOWSER_PUZZLE_PIECE_ACT_RIGHT },
    { 13, BOWSER_PUZZLE_PIECE_ACT_UP    },
    { 12, BOWSER_PUZZLE_PIECE_ACT_RIGHT },
    {  8, BOWSER_PUZZLE_PIECE_ACT_DOWN  },
    {  7, BOWSER_PUZZLE_PIECE_ACT_RIGHT },
    {  3, BOWSER_PUZZLE_PIECE_ACT_DOWN  },
    {  4, BOWSER_PUZZLE_PIECE_ACT_LEFT  },
};

/*
 * The puzzle pieces are initially laid out in the following manner:
 *
 *       +---+---+---+
 *       | 1 | 2 | * |
 *   +---+---+---+---+
 *   | 3 | 4 | 5 | 6 |
 *   +---+---+---+---+
 *   | 7 | 8 | 9 |10 |
 *   +---+---+---+---+
 *   |11 |12 |13 |14 |
 *   +---+---+---+---+
 *
 * (* = star platform)
 */
struct BowserPuzzlePiece {
    ModelID16 model;
    s8 xOffset;
    s8 zOffset;
};

static const struct BowserPuzzlePiece sBowserPuzzlePieces[] = {
    { MODEL_LLL_BOWSER_PIECE_1,  -1, -3 },
    { MODEL_LLL_BOWSER_PIECE_2,   1, -3 },
    { MODEL_LLL_BOWSER_PIECE_3,  -3, -1 },
    { MODEL_LLL_BOWSER_PIECE_4,  -1, -1 },
    { MODEL_LLL_BOWSER_PIECE_5,   1, -1 },
    { MODEL_LLL_BOWSER_PIECE_6,   3, -1 },
    { MODEL_LLL_BOWSER_PIECE_7,  -3,  1 },
    { MODEL_LLL_BOWSER_PIECE_8,  -1,  1 },
    { MODEL_LLL_BOWSER_PIECE_9,   1,  1 },
    { MODEL_LLL_BOWSER_PIECE_10,  3,  1 },
    { MODEL_LLL_BOWSER_PIECE_11, -3,  3 },
    { MODEL_LLL_BOWSER_PIECE_12, -1,  3 },
    { MODEL_LLL_BOWSER_PIECE_13,  1,  3 },
    { MODEL_LLL_BOWSER_PIECE_14,  3,  3 },
};

/**
 * Spawn a single puzzle piece.
 */
void bhv_lll_bowser_puzzle_spawn_piece(s32 i, ModelID16 model, const BehaviorScript *behavior,
                                       f32 xOffset, f32 zOffset) {
    struct Object *puzzlePiece = spawn_object(o, model, behavior);
    puzzlePiece->oPosX += xOffset;
    puzzlePiece->oPosY += 50.0f;
    puzzlePiece->oPosZ += zOffset;
    puzzlePiece->oAction = BOWSER_PUZZLE_PIECE_ACT_IDLE;// initialAction; // This action never gets executed.
    puzzlePiece->oBowserPuzzlePieceId = (i + 1);
    puzzlePiece->oBowserPuzzlePieceActionIndex = 0;
    puzzlePiece->oBowserPuzzlePieceIsReversing = FALSE;
}

/**
 * Spawn the 14 puzzle pieces.
 */
void bhv_lll_bowser_puzzle_spawn_pieces(f32 pieceWidth) {
    s32 i;
    f32 scale = (pieceWidth / 2.0f);

    // Spawn all 14 puzzle pieces.
    for (i = 0; i < ARRAY_COUNT(sBowserPuzzlePieces); i++) {
        bhv_lll_bowser_puzzle_spawn_piece(
            i,
            sBowserPuzzlePieces[i].model,
            bhvLllBowserPuzzlePiece,
            (sBowserPuzzlePieces[i].xOffset * scale),
            (sBowserPuzzlePieces[i].zOffset * scale)
        );
    }

    // The pieces should only be spawned once so go to the next action.
    o->oAction = BOWSER_PUZZLE_ACT_WAIT_FOR_COMPLETE;
}

/*
 * Does the initial spawn of the puzzle pieces and then waits to spawn 5 coins.
 */
void bhv_lll_bowser_puzzle_loop(void) {
    s32 i;

    switch (o->oAction) {
        case BOWSER_PUZZLE_ACT_SPAWN_PIECES:
            bhv_lll_bowser_puzzle_spawn_pieces(LLL_BOWSER_PUZZLE_PIECE_SIZE);
            break;

        case BOWSER_PUZZLE_ACT_WAIT_FOR_COMPLETE:
            // If both completion flags are set and Mario is within 1000 units...
            if ((o->oBowserPuzzleCompletionFlags == (BOWSER_PUZZLE_COMPLETION_FLAG_MARIO_ON_PLATFORM | BOWSER_PUZZLE_COMPLETION_FLAG_PUZZLE_COMPLETE))
             && (o->oDistanceToMario < 1000.0f)) {
                // Spawn 5 coins.
                for (i = 0; i < 5; i++) {
                    spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
                }

                // Reset completion flags (even though they never get checked again).
                o->oBowserPuzzleCompletionFlags = BOWSER_PUZZLE_COMPLETION_FLAGS_NONE;

                // Go to next action so we don't spawn 5 coins ever again.
                o->oAction = BOWSER_PUZZLE_ACT_DONE;
            }
            break;

        case BOWSER_PUZZLE_ACT_DONE:
            break;
    }
}

/*
 * Update the puzzle piece.
 */
void bhv_lll_bowser_puzzle_piece_update(void) {
    // If Mario is standing on this puzzle piece, set a flag in the parent.
    if (gMarioObject->platform == o) {
        o->parentObj->oBowserPuzzleCompletionFlags = BOWSER_PUZZLE_COMPLETION_FLAG_MARIO_ON_PLATFORM;
    }

    // If we should advance to the next action...
    if (!o->oBowserPuzzlePieceContinuePerformingAction) {
        const struct BowserPuzzleAction *actionInfo = &sPieceActions[o->oBowserPuzzlePieceActionIndex];

        // If this piece has an action on this step, use it.
        ObjAction8 action = (o->oBowserPuzzlePieceId == actionInfo->index) ? actionInfo->action : BOWSER_PUZZLE_PIECE_ACT_IDLE;

        // Start doing the next action.
        cur_obj_change_action(action);

        // Advance the pointer to the next action.
        if (o->oBowserPuzzlePieceIsReversing) {
            o->oBowserPuzzlePieceActionIndex--;
        } else {
            o->oBowserPuzzlePieceActionIndex++;
        }

        if (o->oBowserPuzzlePieceActionIndex > ARRAY_COUNT(sPieceActions)) { // If we're past the end of the list...
            o->oBowserPuzzlePieceActionIndex = ARRAY_COUNT(sPieceActions);

            // Start going through the action list backwards.
            o->oBowserPuzzlePieceIsReversing = TRUE;
        } else if (o->oBowserPuzzlePieceActionIndex < 0) { // Or before the beginning of the list...
            o->oBowserPuzzlePieceActionIndex = 0;

            // Set the other completion flag in the parent.
            o->parentObj->oBowserPuzzleCompletionFlags |= BOWSER_PUZZLE_COMPLETION_FLAG_PUZZLE_COMPLETE;

            // Start going through the action list forwards.
            o->oBowserPuzzlePieceIsReversing = FALSE;
        }

        // Keep doing this action until it's complete.
        o->oBowserPuzzlePieceContinuePerformingAction = TRUE;
    }
}

void bhv_lll_bowser_puzzle_piece_move(f32 xOffset, f32 zOffset) {
    // Reset the Y offset in case it has drifted.
    o->oBowserPuzzlePieceOffsetY = 0.0f;

    // For the first LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME frames, shake the puzzle piece up and down.
    if (o->oTimer < LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME) {
        if ((o->oTimer % 2) != 0) {
            o->oBowserPuzzlePieceOffsetY = -LLL_BOWSER_PUZZLE_PIECE_SHAKE_AMOUNT;
        }
    } else {
        // On frame LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME, play the shifting sound.
        if (o->oTimer == LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME) {
            cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE);
        }

        // For the number of frames specified by duration, move the piece.
        if (o->oTimer < (LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME + LLL_BOWSER_PUZZLE_PIECE_MOVE_DURATION)) {
            // If reversing through the action list, flip the direction.
            if (o->oBowserPuzzlePieceIsReversing) {
                o->oBowserPuzzlePieceOffsetX -= xOffset;
                o->oBowserPuzzlePieceOffsetZ -= zOffset;
            } else {
                o->oBowserPuzzlePieceOffsetX += xOffset;
                o->oBowserPuzzlePieceOffsetZ += zOffset;
            }
        } else {
            // This doesn't actually accomplish anything since
            // bhv_lll_bowser_puzzle_piece_update will call cur_obj_change_action
            // at the beginnnig of the next frame before the next action is performed anyway.
            o->oAction = BOWSER_PUZZLE_PIECE_ACT_IDLE;

            // Advance to the next action.
            o->oBowserPuzzlePieceContinuePerformingAction = FALSE;
        }
    }
}

void bhv_lll_bowser_puzzle_piece_idle(void) {
    // For the the number of frames specified by duration, do nothing.
    // Then advance to the next action.
    if (o->oTimer >= (LLL_BOWSER_PUZZLE_PIECE_SHAKE_TIME + LLL_BOWSER_PUZZLE_PIECE_MOVE_DURATION)) {
        o->oBowserPuzzlePieceContinuePerformingAction = FALSE;
    }
}

// The amount a piece moves per frame.
#define LLL_BOWSER_PUZZLE_PIECE_SPEED (LLL_BOWSER_PUZZLE_PIECE_SIZE / LLL_BOWSER_PUZZLE_PIECE_MOVE_DURATION)

void bhv_lll_bowser_puzzle_piece_move_left(void) {
    bhv_lll_bowser_puzzle_piece_move(-LLL_BOWSER_PUZZLE_PIECE_SPEED, 0.0f);
}

void bhv_lll_bowser_puzzle_piece_move_right(void) {
    bhv_lll_bowser_puzzle_piece_move( LLL_BOWSER_PUZZLE_PIECE_SPEED, 0.0f);
}

void bhv_lll_bowser_puzzle_piece_move_up(void) {
    bhv_lll_bowser_puzzle_piece_move(0.0f, -LLL_BOWSER_PUZZLE_PIECE_SPEED);
}

void bhv_lll_bowser_puzzle_piece_move_down(void) {
    bhv_lll_bowser_puzzle_piece_move(0.0f,  LLL_BOWSER_PUZZLE_PIECE_SPEED);
}

ObjActionFunc sBowserPuzzlePieceActions[] = {
    [BOWSER_PUZZLE_PIECE_ACT_IDLE ] = bhv_lll_bowser_puzzle_piece_idle,
    [BOWSER_PUZZLE_PIECE_ACT_LEFT ] = bhv_lll_bowser_puzzle_piece_move_left,
    [BOWSER_PUZZLE_PIECE_ACT_RIGHT] = bhv_lll_bowser_puzzle_piece_move_right,
    [BOWSER_PUZZLE_PIECE_ACT_UP   ] = bhv_lll_bowser_puzzle_piece_move_up,
    [BOWSER_PUZZLE_PIECE_ACT_DOWN ] = bhv_lll_bowser_puzzle_piece_move_down,
};

void bhv_lll_bowser_puzzle_piece_loop(void) {
    bhv_lll_bowser_puzzle_piece_update();

    cur_obj_call_action_function(sBowserPuzzlePieceActions);
    vec3f_sum(&o->oPosVec, &o->oBowserPuzzlePieceOffsetVec, &o->oHomeVec);
}
