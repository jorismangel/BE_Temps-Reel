/*
 * File:   global.h
 * Author: pehladik
 *
 * Created on 21 avril 2011, 12:14
 */

#include "global.h"

RT_TASK tServeur;
RT_TASK tconnect;
RT_TASK tmove;
RT_TASK tenvoyer;
RT_TASK tImage;
RT_TASK tArena;
RT_TASK tBattery;
RT_TASK tPosition;
RT_TASK tMission;

RT_MUTEX mutexEtat;
RT_MUTEX mutexMove;
RT_MUTEX mutexCamera;
RT_MUTEX mutexImage;
RT_MUTEX mutexArena;
RT_MUTEX mutexRobot;

RT_SEM semConnecterRobot;
RT_SEM semStartImage;
RT_SEM semStartDetectArena;
RT_SEM semStartGetBattery;
RT_SEM semCalcPos;
RT_SEM semStartMission;
RT_SEM semStartMove;

RT_QUEUE queueMsgGUI;

int etatCommMoniteur = 1;
int etatCommRobot = 1;
int nbcommrobot;
DRobot *robot;
DMovement *move;
DServer *serveur;
DCamera *camera;
DImage * img;
DArena *arena ;
DAction *action ;
DMission *mission;


int MSG_QUEUE_SIZE = 10;

int PRIORITY_TSERVEUR = 30;
int PRIORITY_TCONNECT = 20;
int PRIORITY_TMOVE = 10;
int PRIORITY_TENVOYER = 25;
int PRIORITY_TIMAGE = 15;
int PRIORITY_TARENA = 16;
int PRIORITY_TBATTERY= 19;
int PRIORITY_TPOSITION= 17;
int PRIORITY_TMISSION= 12;
