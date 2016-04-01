/*
 * File:   global.h
 * Author: pehladik
 *
 * Created on 21 avril 2011, 12:14
 */

#include "global.h"

//Definition des taches 
RT_TASK tServeur;
RT_TASK tconnect;
RT_TASK tmove;
RT_TASK tenvoyer;
RT_TASK tWatchdog;
RT_TASK tPosition;
RT_TASK tBattery;
RT_TASK tArena;
RT_TASK tImage;
RT_TASK tMission;

//Definition des mutex 
RT_MUTEX mutexEtat;
RT_MUTEX mutexMove;
RT_MUTEX mutexServer;
//Nos mutex
RT_MUTEX mutexCamera;
RT_MUTEX mutexArena;
RT_MUTEX mutexRobot;



//Definition des mutex
RT_SEM semConnecterRobot;

RT_QUEUE queueMsgGUI;

int etatCommMoniteur = 1;
int etatCommRobot = 1;
DRobot *robot;
DMovement *move;
DServer *serveur;
Dcamera *camera;

int MSG_QUEUE_SIZE = 10;
//Valeur du projet initial
/*int PRIORITY_TSERVEUR = 30;
int PRIORITY_TCONNECT = 20;
int PRIORITY_TMOVE = 10;
int PRIORITY_TENVOYER = 25;
int PRIORITY_TCAMERA = 10;*/

//Nos constantes de priorite
int PRIORITY_TCONNECT = 80;
int PRIORITY_TSEND= 70;
int PRIORITY_TRECEIVE = 70;
int PRIORITY_TMOVE = 60;
int PRIORITY_TMISSION = 60;
int PRIORITY_TARENA = 60;
int PRIORITY_TIMAGE = 60;
int PRIORITY_TPOSITION = 60;
int PRIORITY_TBATTERY = 50;


