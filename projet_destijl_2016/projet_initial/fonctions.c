#include "fonctions.h"
#include <math.h>

int write_in_queue(RT_QUEUE *msgQueue, void * data, int size);

void envoyer(void * arg) {
    DMessage *msg;
    int err;

    while (1) {
        rt_printf("tenvoyer : Attente d'un message\n");
        if ((err = rt_queue_read(&queueMsgGUI, &msg, sizeof (DMessage), TM_INFINITE)) >= 0) {
            rt_printf("tenvoyer : envoi d'un message au moniteur\n");
            serveur->send(serveur, msg);
            msg->free(msg);
        } else {
            rt_printf("Error msg queue write: %s\n", strerror(-err));
        }
    }
}


void connecter(void * arg) {
    int status;
    DMessage *message;

    rt_printf("tconnect : Debut de l'exécution de tconnect\n");

    while (1) {
	// Attente de l'acquisition du semaphore semConnecterRobot
        rt_printf("tconnect : Attente du sémarphore semConnecterRobot\n");
        rt_sem_p(&semConnecterRobot, TM_INFINITE);

	// Ouverture de la communication avec le robot
        rt_printf("tconnect : Ouverture de la communication avec le robot\n");
        status = robot->open_device(robot);

	// Mise a jour de la variable etatCommRobot
	rt_mutex_acquire(&mutexEtat, TM_INFINITE); // récupérer le mutex mutexEtat
	etatCommRobot = status; // màj avec le status
	rt_mutex_release(&mutexEtat); // libérer le mutex mutexEtat

        // Si le status est OK, on démarre le robot
	if (status == STATUS_OK) {
            	
		status = robot->start_insecurely(robot); // sans watchdog
		//status = robot->start(robot); // quand watchgod ok
           	
		// Si le status est OK, on démarre le watchdog et l'aquisition de la battery
		if (status == STATUS_OK){

		        rt_printf("tconnect : Robot démarré\n");

			// Start aquisition de la batterie : envoi de l'évenement
			rt_printf("tconnect : Libération du semaphore semStartGetBattery\n");
			rt_sem_v(&semStartGetBattery);

			// Start watchdof : envoi de l'évenement
			rt_printf("tconnect : Libération du semaphore semStartWatchdog\n");
			rt_sem_v(&semStartWatchdog);
            	}
        }

        message = d_new_message();
        message->put_state(message, status);

        rt_printf("tconnecter : Envoi message\n");
        message->print(message, 100);

        if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
            message->free(message);
        }
    }
}

void communiquer(void *arg) {
    DMessage *msg = d_new_message();
    int var1 = 1;
    int num_msg = 0;

    rt_printf("tserver : Début de l'exécution de serveur\n");
    serveur->open(serveur, "8000");
    rt_printf("tserver : Connexion\n");

    rt_mutex_acquire(&mutexEtat, TM_INFINITE);
    etatCommMoniteur = 0;
    rt_mutex_release(&mutexEtat);

    while (var1 > 0) {
        rt_printf("tserver : Attente d'un message\n");
        var1 = serveur->receive(serveur, msg);
        num_msg++;
        if (var1 > 0) {
            switch (msg->get_type(msg)) {
                case MESSAGE_TYPE_ACTION:
                    rt_printf("tserver : Le message %d reçu est une action\n",
                            num_msg);
                    DAction *action = d_new_action();
                    action->from_message(action, msg);
                    switch (action->get_order(action)) {
                        case ACTION_CONNECT_ROBOT:
                            rt_printf("tserver : Action connecter robot\n");
                            rt_sem_v(&semConnecterRobot);
                            break;
                    }
                    break;
                case MESSAGE_TYPE_MOVEMENT:
                    rt_printf("tserver : Le message reçu %d est un mouvement\n",
                            num_msg);
                    rt_mutex_acquire(&mutexMove, TM_INFINITE);
                    move->from_message(move, msg);
                    move->print(move);
                    rt_mutex_release(&mutexMove);
                    break;
					 case MESSAGE_TYPE_MISSION:
                    rt_printf("tserver : Le message reçu %d est une mission\n",
                            num_msg);
    					  //instanciation et initialisation de la mission 
	 					  mission = d_new_mission();
						  //On recupere les informations du message de mission recu
						  d_mission_from_message(mission, msg);
						  rt_mutex_release(&mutexMission);
                    break;
            }
        }
    }
}

void deplacer(void *arg) {
    int status = 1;
    int gauche;
    int droite;
    int speed =0;
    
    DMessage *message;

    rt_printf("tmove : Debut de l'éxecution de periodique à 500ms\n");
    rt_task_set_periodic(NULL, TM_NOW, 500000000);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        rt_printf("tmove : Activation périodique\n");

        rt_mutex_acquire(&mutexEtat, TM_INFINITE);
        status = etatCommRobot;
        rt_mutex_release(&mutexEtat);

        if (status == STATUS_OK) {
            rt_mutex_acquire(&mutexMove, TM_INFINITE);
            speed=move->get_speed(move);
            switch (move->get_direction(move)) {
                case DIRECTION_FORWARD:
                		if (speed<50){
		                  gauche = MOTEUR_ARRIERE_LENT;
		                  droite = MOTEUR_ARRIERE_LENT;
		                }else {
		                	gauche = MOTEUR_ARRIERE_RAPIDE;
		                  droite = MOTEUR_ARRIERE_RAPIDE;
		                }
                    break;
                case DIRECTION_LEFT:
                		if (speed<50){
		                  gauche = MOTEUR_ARRIERE_LENT;
		                  droite = MOTEUR_AVANT_LENT;
		                 }else {
		                  gauche = MOTEUR_ARRIERE_RAPIDE;
		                  droite = MOTEUR_AVANT_RAPIDE;
		                 }
                    break;
                case DIRECTION_RIGHT:
                		if (speed<50){
		                  gauche = MOTEUR_AVANT_LENT;
		                  droite = MOTEUR_ARRIERE_LENT;
		                }else{
		                	gauche = MOTEUR_AVANT_RAPIDE;
		                  droite = MOTEUR_ARRIERE_RAPIDE;
		                }
                    break;
                case DIRECTION_STOP:
                    gauche = MOTEUR_STOP;
                    droite = MOTEUR_STOP;
                    break;
                case DIRECTION_STRAIGHT:
                	if (speed<50){
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_AVANT_LENT;
                  }else{
                  	gauche = MOTEUR_AVANT_RAPIDE;
                    droite = MOTEUR_AVANT_RAPIDE;
                  }
                    break;
            }
            rt_mutex_release(&mutexMove);

            status = robot->set_motors(robot, gauche, droite);

            if (status != STATUS_OK) {
                rt_mutex_acquire(&mutexEtat, TM_INFINITE);
                etatCommRobot = status;
                rt_mutex_release(&mutexEtat);

                message = d_new_message();
                message->put_state(message, status);

                rt_printf("tmove : Envoi message\n");
                if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
                    message->free(message);
                }
            }
        }
    }
}
//TODO

void detectArena(void *arg) {



}

int write_in_queue(RT_QUEUE *msgQueue, void * data, int size) {
    void *msg;
    int err;

    msg = rt_queue_alloc(msgQueue, size);
    memcpy(msg, &data, size);

    if ((err = rt_queue_send(msgQueue, msg, sizeof (DMessage), Q_NORMAL)) < 0) {
        rt_printf("Error msg queue send: %s\n", strerror(-err));
    }
    rt_queue_free(&queueMsgGUI, msg);

    return err;
}


void image(void *arg) {

	camera = d_new_camera();
	int err;
	DJpegimage *jpeg=d_new_jpegimage();
	DMessage *message = d_new_message();
	
	/*initialisation de la camera*/
	rt_mutex_acquire(&mutexCamera, TM_INFINITE);
	camera = d_new_camera();
	
	/*ouverture de la webcam*/
	d_camera_open(camera);
	rt_mutex_release(&mutexCamera);
	
	rt_printf("tImage : Debut de l'éxecution de periodique à 600ms\n");
   rt_task_set_periodic(NULL, TM_NOW, 600000000);
  
  while(1){
  	rt_printf("tImage : Activation periodique\n");
  	rt_mutex_acquire(&mutexCamera, TM_INFINITE);
  	d_camera_get_frame(camera,img);
  	rt_mutex_release(&mutexCamera);
  	
  	rt_mutex_acquire(&mutexImage, TM_INFINITE);
  	d_jpegimage_compress(jpeg,img);
  	rt_mutex_release(&mutexImage);
  	d_message_put_jpeg_image(message,jpeg);
  	
  	rt_mutex_acquire(&mutexServer, TM_INFINITE);
  	if(d_server_send(serveur,message)==-1){
  		rt_printf("echec envoie image au serveur\n");
  	}
  	rt_mutex_release(&mutexServer);
  }

}




void mission_fct(void * arg) {

	 int id =0;
	 DPosition *positionActuelle;
	 DPosition *positionVoulue;
	 int angle = 0;
	 int direction = 0; //Sens horaire
	 int range = 0; 
	 int status = 0; 
	 DMessage* message = d_new_message();
	 
	 while(1){
	 rt_mutex_acquire(&mutexMission, TM_INFINITE);

	 rt_printf("tserver : Début de l'exécution du thread tMission\n");
	 
	 //Je recupere le numero de la mission
	 id = d_mission_get_id(mission); 

	 //Je recupere la position a atteindre
	 d_mission_get_position(mission, positionVoulue);
	 //Je recupere la position actuelle
	 positionActuelle = d_image_compute_robot_position(img,arena);
	
	 //Je calcule les écarts de position pour le futur deplacement
	 angle = positionVoulue->orientation - positionActuelle->orientation;
	 range = sqrt(pow(positionVoulue->x - positionActuelle->x, 2) + pow(positionVoulue->y - positionActuelle->y, 2));
	 
	 //Maintenant on rotate pour s'aligner avec la destination
	 status = d_robot_turn(robot,angle,direction);
	 if(status ==STATUS_OK)
	 {
		printf("Turn ok\n");
	 }
	 else
	 {
		printf("Error Turn\n");
	 }
	 //Puis on avance pour arriver à destination
	 status = d_robot_move(robot,range);
	 if(status ==STATUS_OK)
	 {
		printf("Move ok\n");
	 }
	 else
	 {
		printf("Error Move\n");
	 }

	 //Arrive a destination, envoie message de mission terminee
	 rt_printf("Nous sommes arrives a destination\n");
	 //On informe de la fin de la mission en envoyant un message 
	 d_message_mission_terminate(message,id);
	 status = d_server_send(serveur,message);

	 //On garde le mutex bloquant, il sera debloque que lors de la reception d'une nouvelle demande de mission
}
}









/*
 * Récupérer la batterie du robot périodiquement : toutes les 250 ms
 * @param arg
 * @author MANGEL
 */
void battery(void *arg) {
	rt_printf("tBattery : Début d'acquisition de la batterie\n");

	// Variables
	int battery;
	int err;
	int status;
	
	// Attente du lancement de l'aquisition de la batterie
	rt_printf("tBattery : Attente du semaphore semStartGetBattery\n");
	err = rt_sem_p(&tBattery, TM_INFINITE);
	rt_printf("tBattery : Récupération du semaphore semStartGetBattery\n");

	// Définition du thread tBattery : T=250ms
	rt_task_set_periodic(&tBattery, TM_NOW, 250000000);

	// Création de la variable DBattery
	DBattery *d_battery = d_new_battery();

	// Aquisition périodique
	while(1) {
		// Attente de la période du thread
		rt_task_wait_period(NULL);
		
		// Récupérer l'état de la batterie du robot
		status = robot->get_vbat(robot, &battery);

		// Mise a jour de la variable etatCommRobot
		rt_mutex_acquire(&mutexEtat, TM_INFINITE); // récupérer le mutex mutexEtat
		etatCommRobot = status; // màj avec le status
		rt_mutex_release(&mutexEtat); // libérer le mutex mutexEtat

		// Tester l'état de la connexion du robot
		test_com_robot("battery");

		// Si le status est OK, on transmet au moniteur
		if(status = STATUS_OK) {
			// Envoi de l'état de la batterie au moniteur
			DMessage *d_message_battery = d_new_message(); // création du message
			d_battery->set_level(d_battery, battery); // copie de l'état de la batterie dans la bonne variable
			d_message_battery->put_battery_level(d_message_battery, d_battery); // placer l'état de la batterie dans le DMessage
			err = serveur->send(serveur, d_message_battery); 
			
			// Test de la communication avec le moniteur
			if(err < 0) {
				// Mise a jour de la variable etatCommMoniteur
				rt_mutex_acquire(&mutexEtat, TM_INFINITE); // récupérer le mutex mutexEtat
				etatCommMoniteur = 1; // màj avec 1
				rt_mutex_release(&mutexEtat); // libérer le mutex mutexEtat
			}
		}
	}
}

/*
 * Tester l'état de la communication avec le robot
 * Test du compteur d'échec : s'il dépasse 3, il faut executer le restart
 * @param fct nom de la fonction yaant appelé test_com_robot
 * @ author MANGEL
 */
void test_com_robot(const char* fct) {
	rt_printf("test_com_robot : la fonction appelante est %s\n", fct);
}

