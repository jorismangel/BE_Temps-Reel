#include "includes.h"
#include "global.h"
#include "fonctions.h"

//aminata

/**
 * \fn void initStruct(void)
 * \brief Initialisation des structures de l'application (tâches, mutex, 
 * semaphore, etc.)
 */
void initStruct(void);

/**
 * \fn void startTasks(void)
 * \brief Démarrage des tâches
 */
void startTasks(void);

/**
 * \fn void deleteTasks(void)
 * \brief Arrêt des tâches
 */
void deleteTasks(void);

int main(int argc, char**argv) {
    printf("#################################\n");
    printf("#      DE STIJL PROJECT         #\n");
    printf("#################################\n");

    //signal(SIGTERM, catch_signal);
    //signal(SIGINT, catch_signal);

    /* Avoids memory swapping for this program */
    mlockall(MCL_CURRENT | MCL_FUTURE);
    /* For printing, please use rt_print_auto_init() and rt_printf () in rtdk.h
     * (The Real-Time printing library). rt_printf() is the same as printf()
     * except that it does not leave the primary mode, meaning that it is a
     * cheaper, faster version of printf() that avoids the use of system calls
     * and locks, instead using a local ring buffer per real-time thread along
     * with a process-based non-RT thread that periodically forwards the
     * contents to the output stream. main() must call rt_print_auto_init(1)
     * before any calls to rt_printf(). If you forget this part, you won't see
     * anything printed.
     */
    rt_print_auto_init(1);
    initStruct();
    startTasks();
    pause();
    deleteTasks();

    return 0;
}

void initStruct(void) {

	int err;

	/*
	 * Creation des mutex 
	 */

	// mutexEtat
	if (err = rt_mutex_create(&mutexEtat, NULL)) {
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// mutexImage
	if (err = rt_mutex_create(&mutexImage, NULL)) {
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// mutexMove
	if (err = rt_mutex_create(&mutexMove, NULL)) {
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
	
	// mutexCamera
	if (err = rt_mutex_create(&mutexCamera, NULL)) {
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// mutexServer
	if (err = rt_mutex_create(&mutexServer, NULL)) {
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// mutexMission
	if (err = rt_mutex_create(&mutexMission, NULL)) {
		rt_printf("Error mutex create: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}


	/* 
	 * Creation des semaphores 
	 */
	
	// semConnecterRobot
	if (err = rt_sem_create(&semConnecterRobot, NULL, 0, S_FIFO)) {
		rt_printf("Error semaphore create semConnecterRobot: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
	
	// semStartGetBattery
	if (err = rt_sem_create(&semStartGetBattery, NULL, 0, S_FIFO)) {
		rt_printf("Error semaphore create semStartGetBattery: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
	
	// semStartWatchdog
	if (err = rt_sem_create(&semStartWatchdog, NULL, 0, S_FIFO)) {
		rt_printf("Error semaphore create semStartWatchdog: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}




	/* 
	 * Creation des taches
	 */
	
	// serveur
	if (err = rt_task_create(&tServeur, NULL, 0, PRIORITY_TSERVEUR, 0)) {
		rt_printf("Error task create tServeur: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// connect
	if (err = rt_task_create(&tconnect, NULL, 0, PRIORITY_TCONNECT, 0)) {
		rt_printf("Error task create tconnect: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// move
	if (err = rt_task_create(&tmove, NULL, 0, PRIORITY_TMOVE, 0)) {
		rt_printf("Error task create tmove: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// envoyer
	if (err = rt_task_create(&tenvoyer, NULL, 0, PRIORITY_TENVOYER, 0)) {
		rt_printf("Error task create tenvoyer: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// arena
	if (err = rt_task_create(&tArena, NULL, 0, PRIORITY_TARENA, 0)) {
		rt_printf("Error task createt tArena: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// image
	if (err = rt_task_create(&tImage, NULL, 0, PRIORITY_TIMAGE, 0)) {
		rt_printf("Error task create tImage: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// position
	if (err = rt_task_create(&tPosition, NULL, 0, PRIORITY_TPOSITION, 0)) {
		rt_printf("Error task create tPosition: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// watchdog
	if (err = rt_task_create(&tWatchdog, NULL, 0, PRIORITY_TWATCHDOG, 0)) {
		rt_printf("Error task create tWatchdog: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// mission
	if (err = rt_task_create(&tMission, NULL, 0, PRIORITY_TMISSION, 0)) {
		rt_printf("Error task create tMission: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// battery
	if (err = rt_task_create(&tBattery, NULL, 0, PRIORITY_TBATTERY, 0)) {
		rt_printf("Error task create tBattery: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}



	/* 
	 * Creation des files de messages
	 */

	if (err = rt_queue_create(&queueMsgGUI, "toto", MSG_QUEUE_SIZE*sizeof(DMessage), MSG_QUEUE_SIZE, Q_FIFO)){
	rt_printf("Error msg queue create: %s\n", strerror(-err));
	exit(EXIT_FAILURE);
	}



	/*
	 * Creation des structures globales du projet
	 */

	robot = d_new_robot();
	move = d_new_movement();
	serveur = d_new_server();
    	img = d_new_image();
}

void startTasks() {

	int err;

	// tconnect
	if (err = rt_task_start(&tconnect, &connecter, NULL)) {
		rt_printf("Error task start tconnect: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// tServeur
	if (err = rt_task_start(&tServeur, &communiquer, NULL)) {
		rt_printf("Error task start tServeur: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// tmove
	if (err = rt_task_start(&tmove, &deplacer, NULL)) {
		rt_printf("Error task start tmove: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// tMission
	//if (err = rt_task_start(&tMission, &mission, NULL)) {
 	//	rt_printf("Error task start: %s\n", strerror(-err));
        //	exit(EXIT_FAILURE);
    	//}	

	// tenvoyer
	if (err = rt_task_start(&tenvoyer, &envoyer, NULL)) {
		rt_printf("Error task start tenvoyer: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// tImage
	if (err = rt_task_start(&tImage, &image, NULL)) {
		rt_printf("Error task start tImage: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}

	// tBattery
	if (err = rt_task_start(&tBattery, &battery, NULL)) {
		rt_printf("Error task start tBattery: %s\n", strerror(-err));
		exit(EXIT_FAILURE);
	}
}

void deleteTasks() {
    	rt_task_delete(&tServeur);
    	rt_task_delete(&tconnect);
    	rt_task_delete(&tmove);
    	rt_task_delete(&tImage);
	rt_task_delete(&tMission);
    	rt_task_delete(&tBattery);
}
