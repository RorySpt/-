#ifndef _GAME_H_
#define _GAME_H_

#include "habit.h"
extern uchar greedySnakeBody[14*22][2];//�����307
extern uchar greedySnakeFeedNum[10][2];//ʳ��
extern uchar greedySnakeFeedUseNum;
extern uint greedySnakeBodyLength;//�����ȣ���ʼ2
extern uchar greedySnakeHead[2];//��ͷ,YX
extern uchar GreedySnakeMovementDirection;//0~3:��������
extern uchar greedySnake_move(uchar mode);
extern void greedySnake_feed();
extern void greedySnake_feed_init();
#endif