#ifndef _JQ8900_H_
#define _JQ8900_H_

#include "habit.h"

extern void Tell_theTime();
extern void Tell_theData();
extern void JpAlarmMusic(uchar dat);	//����
extern void JqPlaySpecifiedTrack(uint dat);//����ָ����Ŀ0~65535
extern void JqSpecify_volume(uchar dat);//ָ������
//***************************************
//�������ܣ�����ѭ��ģʽ
//ָ��		��00��ȫ��ѭ����01������ѭ����02������ֹͣ
//					03��ȫ�������04��Ŀ¼ѭ����05��Ŀ¼���
//					06��Ŀ¼˳��07��ȫ��˳��
//***************************************
extern void JqSetLoopMode(uchar dat);//
extern void JqPlayCmd(uchar dat);//02:���ţ�03����ͣ��04��ֹͣ��05����һ����06����һ��
extern void JqPlayCmd2(uchar dat);//02:���ţ�03����ͣ��04��ֹͣ��05����һ����06����һ��
extern void JqQueryCmd(uchar dat);

extern uchar RiPlaybackStatus;//����״̬
extern uchar RiTotalTrackTlaybackTime[3];//��Ŀ��ʱ�䣬ʱ����
extern uchar RiCurrentPlayingTime[3];//��Ŀ�Ѳ���ʱ��
extern uint RiTotalTrack;//����Ŀ
extern uint RiCurrentTrack;//��ǰ��Ŀ

//extern void JqPlaybackStatus();
#endif