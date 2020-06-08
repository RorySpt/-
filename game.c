#include "game.h"
#include <stdlib.h>
//**************************************
//游戏：贪吃蛇
//画布：22*14
//像素块：4*4
//**************************************
extern uint rot_timeBase;
#define FeedQuantity 10

uchar greedySnakeBody[14*22][2];//蛇身，最长307
uchar greedySnakeFeedNum[FeedQuantity][2]=//食物
{
	{0xAA,0xAA},{0xAA,0xAA},{0xAA,0xAA},{0xAA,0xAA},{0xAA,0xAA},
	{0xAA,0xAA},{0xAA,0xAA},{0xAA,0xAA},{0xAA,0xAA},{0xAA,0xAA},
};
uchar greedySnakeFeedUseNum=1;
uint greedySnakeBodyLength=2;//蛇身长度，初始2
uchar greedySnakeHead[2];//蛇头,YX
uchar GreedySnakeMovementDirection;//0~3:右下左上

void greedySnake_feed_init()
{
	uchar i;
	for(i=0;i<greedySnakeFeedUseNum;i++)
	{
		greedySnakeFeedNum[i][0]=0xAA;
		greedySnakeFeedNum[i][1]=0xAA;
	}
}
void greedySnake_feed()
{
	uint  i=0;
	uchar j=0;
	uchar y=1;
	uchar n=0;
	for(j=0;j<greedySnakeFeedUseNum;j++,y=1)
	{
		if((greedySnakeFeedNum[j][0]==0xAA)&&(greedySnakeFeedNum[j][1]==0xAA))
			while(y)
			{
				greedySnakeFeedNum[j][0]=rand()%14;
				greedySnakeFeedNum[j][1]=rand()%22;
				y=0;
				for(i=0;i<greedySnakeBodyLength;i++)
				if(greedySnakeFeedNum[j][0]==greedySnakeBody[i][0])if(greedySnakeFeedNum[j][1]==greedySnakeBody[i][1]){y=1;break;}
				if(y==0)if((greedySnakeFeedNum[j][0]==greedySnakeHead[0])&&(greedySnakeFeedNum[j][0]==greedySnakeHead[0]))y=1;
				if(y==0)for(n=0;n<greedySnakeFeedUseNum;n++)
									if(j!=n)
										if(greedySnakeFeedNum[j][0]==greedySnakeFeedNum[n][0])
											if(greedySnakeFeedNum[j][1]==greedySnakeFeedNum[n][1])
											{y=1;break;}
			}
	}
}
uchar greedySnake_move(uchar mode)
{
	uint i;
//	char movx,movy;
//	switch(GreedySnakeMovementDirection)
//	{
//		case 0:{if(greedySnakeHead[1]==21){return 255;}else {movx=1;movy=0;}}//墙
//		case 1:{if(greedySnakeHead[0]==13){return 255;}else {movx=0;movy=1;}}
//		case 2:{if(greedySnakeHead[1]==0){return 255;}else {movx=-1;movy=0;}}
//		case 3:{if(greedySnakeHead[0]==0){return 255;}else {movx=0;movy=-1;}}
//	}
//备用
	uchar HeaderCacheX,HeaderCacheY;
	if(mode==0)
	switch(GreedySnakeMovementDirection)
	{
		case 0:{if(greedySnakeHead[1]==21){return 255;}else {HeaderCacheY=greedySnakeHead[0];HeaderCacheX=greedySnakeHead[1]+1;}break;}//墙
		case 1:{if(greedySnakeHead[0]==13){return 255;}else {HeaderCacheY=greedySnakeHead[0]+1;HeaderCacheX=greedySnakeHead[1];}break;}
		case 2:{if(greedySnakeHead[1]==0 ){return 255;}else {HeaderCacheY=greedySnakeHead[0];HeaderCacheX=greedySnakeHead[1]-1;}break;}
		case 3:{if(greedySnakeHead[0]==0 ){return 255;}else {HeaderCacheY=greedySnakeHead[0]-1;HeaderCacheX=greedySnakeHead[1];}break;}
	}
	if(mode==1)
		switch(GreedySnakeMovementDirection)
	{
		case 0:{if(greedySnakeHead[1]==21){HeaderCacheY=greedySnakeHead[0];HeaderCacheX=0;}else {HeaderCacheY=greedySnakeHead[0];HeaderCacheX=greedySnakeHead[1]+1;}break;}//墙
		case 1:{if(greedySnakeHead[0]==13){HeaderCacheY=0;HeaderCacheX=greedySnakeHead[1];}else {HeaderCacheY=greedySnakeHead[0]+1;HeaderCacheX=greedySnakeHead[1];}break;}
		case 2:{if(greedySnakeHead[1]==0 ){HeaderCacheY=greedySnakeHead[0];HeaderCacheX=21;}else {HeaderCacheY=greedySnakeHead[0];HeaderCacheX=greedySnakeHead[1]-1;}break;}
		case 3:{if(greedySnakeHead[0]==0 ){HeaderCacheY=13;HeaderCacheX=greedySnakeHead[1];}else {HeaderCacheY=greedySnakeHead[0]-1;HeaderCacheX=greedySnakeHead[1];}break;}
	}
	for(i=0;i<greedySnakeBodyLength-1;i++)if(greedySnakeBody[i][0]==(HeaderCacheY))if(greedySnakeBody[i][1]==(HeaderCacheX)){return 254;}//自身
	for(i=0;i<greedySnakeFeedUseNum;i++)if(greedySnakeFeedNum[i][0]==(HeaderCacheY))if(greedySnakeFeedNum[i][1]==(HeaderCacheX))
		{
			greedySnakeBodyLength++;
			greedySnakeFeedNum[i][0]=0xAA;
			greedySnakeFeedNum[i][1]=0xAA;
			for(i=greedySnakeBodyLength-1;i>0;i--){greedySnakeBody[i][0]=greedySnakeBody[i-1][0];greedySnakeBody[i][1]=greedySnakeBody[i-1][1];}
			greedySnakeBody[0][0]=greedySnakeHead[0];greedySnakeBody[0][1]=greedySnakeHead[1];
			greedySnakeHead[0]=HeaderCacheY;	greedySnakeHead[1]=HeaderCacheX;
			return 1;
		}//食物
	for(i=greedySnakeBodyLength-1;i>0;i--){greedySnakeBody[i][0]=greedySnakeBody[i-1][0];greedySnakeBody[i][1]=greedySnakeBody[i-1][1];}
	greedySnakeBody[0][0]=greedySnakeHead[0];greedySnakeBody[0][1]=greedySnakeHead[1];
	greedySnakeHead[0]=HeaderCacheY;	greedySnakeHead[1]=HeaderCacheX;
	return 0;
}