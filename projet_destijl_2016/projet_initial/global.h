/* 
 * File:   global.h
 * Author: pehladik
 *
 * Created on 12 janvier 2012, 10:11
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include "includes.h"

/* @descripteurs des tâches */
extern RT_TASK tServeur;
extern RT_TASK tconnect;
extern RT_TASK tmove;
extern RT_TASK tenvoyer;
extern RT_TASK tImage;
extern RT_TASK tArena;
extern RT_TASK tBattery;
extern RT_TASK tPosition;
extern RT_TASK tMission;

/* @descripteurs des mutex */
extern RT_MUTEX mutexEtat;
extern RT_MUTEX mutexMove;
extern RT_MUTEX mutexCamera;
extern RT_MUTEX mutexImage;
extern RT_MUTEX mutexArena;
extern RT_MUTEX mutexRobot;


/* @descripteurs des sempahore */
extern RT_SEM semConnecterRobot;
extern RT_SEM semStartImage;
extern RT_SEM semStartDetectArena;
extern RT_SEM semStartGetBattery;
extern RT_SEM semCalcPos;
extern RT_SEM semStartMission;
extern RT_SEM semStartMove;

/* @descripteurs des files de messages */
extern RT_QUEUE queueMsgGUI;

/* @variables partagées */
extern int etatCommMoniteur;
extern int etatCommRobot;
extern int nbcommrobot;
extern DServer *serveur;
extern DRobot *robot;
extern DMovement *move;
extern DCamera *camera;
extern DImage * img;
extern DArena *arena ;
extern DAction *action ;
extern DMission *mission;

/* @constantes */
extern int MSG_QUEUE_SIZE;
extern int PRIORITY_TSERVEUR;
extern int PRIORITY_TCONNECT;
extern int PRIORITY_TMOVE;
extern int PRIORITY_TENVOYER;
extern int PRIORITY_TIMAGE;
extern int PRIORITY_TARENA;
extern int PRIORITY_TBATTERY;
extern int PRIORITY_TPOSITION;
extern int PRIORITY_TMISSION;

#endif	/* GLOBAL_H */

