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
extern RT_TASK tMission;
extern RT_TASK tImage;
extern RT_TASK tWatchdog;
extern RT_TASK tPosition;
extern RT_TASK tBattery;
extern RT_TASK tArena;
extern RT_TASK tImage;
extern RT_TASK tMission;



/* @descripteurs des mutex */
extern RT_MUTEX mutexEtat;
extern RT_MUTEX mutexMove;
extern RT_MUTEX mutexCamera;
extern RT_MUTEX mutexServer;
extern RT_MUTEX mutexMission;


/* @descripteurs des sempahore */
extern RT_SEM semConnecterRobot;

/* @descripteurs des files de messages */
extern RT_QUEUE queueMsgGUI;

/* @variables partagées */
extern int etatCommMoniteur;
extern int etatCommRobot;
extern DServer *serveur;
extern DRobot *robot;
extern DMovement *move;
extern DCamera *camera;
extern DMission *mission;
extern DImage *img;
extern DArena *arena;

/* @constantes */
extern int MSG_QUEUE_SIZE;
extern int PRIORITY_TSERVEUR;
extern int PRIORITY_TCONNECT;
extern int PRIORITY_TMOVE;
extern int PRIORITY_TENVOYER;
//Nos priorites
extern int PRIORITY_TWATCHDOG;
extern int PRIORITY_TCONNECT;
extern int PRIORITY_TSEND;
extern int PRIORITY_TRECEIVE;
extern int PRIORITY_TMOVE;
extern int PRIORITY_TMISSION;
extern int PRIORITY_TARENA;
extern int PRIORITY_TIMAGE;
extern int PRIORITY_TPOSITION;
extern int PRIORITY_TBATTERY;


#endif	/* GLOBAL_H */

