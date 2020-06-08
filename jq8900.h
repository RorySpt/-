#ifndef _JQ8900_H_
#define _JQ8900_H_

#include "habit.h"

extern void Tell_theTime();
extern void Tell_theData();
extern void JpAlarmMusic(uchar dat);	//闹铃
extern void JqPlaySpecifiedTrack(uint dat);//播放指定曲目0~65535
extern void JqSpecify_volume(uchar dat);//指定音量
//***************************************
//函数功能：设置循环模式
//指令		：00：全盘循环，01：单曲循环，02：单曲停止
//					03：全盘随机，04：目录循环，05：目录随机
//					06：目录顺序，07：全盘顺序
//***************************************
extern void JqSetLoopMode(uchar dat);//
extern void JqPlayCmd(uchar dat);//02:播放，03：暂停，04：停止，05：上一曲，06：下一曲
extern void JqPlayCmd2(uchar dat);//02:播放，03：暂停，04：停止，05：上一曲，06：下一曲
extern void JqQueryCmd(uchar dat);

extern uchar RiPlaybackStatus;//播放状态
extern uchar RiTotalTrackTlaybackTime[3];//曲目总时间，时分秒
extern uchar RiCurrentPlayingTime[3];//曲目已播放时间
extern uint RiTotalTrack;//总曲目
extern uint RiCurrentTrack;//当前曲目

//extern void JqPlaybackStatus();
#endif