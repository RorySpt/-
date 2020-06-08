#ifndef _GAME_H_
#define _GAME_H_

#include "habit.h"
extern uchar greedySnakeBody[14*22][2];//蛇身，最长307
extern uchar greedySnakeFeedNum[10][2];//食物
extern uchar greedySnakeFeedUseNum;
extern uint greedySnakeBodyLength;//蛇身长度，初始2
extern uchar greedySnakeHead[2];//蛇头,YX
extern uchar GreedySnakeMovementDirection;//0~3:右下左上
extern uchar greedySnake_move(uchar mode);
extern void greedySnake_feed();
extern void greedySnake_feed_init();
#endif