#include "fonctions.h"

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
		nbcommrobot=0;
		
    while (1) {
        rt_printf("tconnect : Attente du sémarphore semConnecterRobot\n");
        rt_sem_p(&semConnecterRobot, TM_INFINITE);
        rt_printf("tconnect : Ouverture de la communication avec le robot\n");
        rt_mutex_acquire(&mutexRobot, TM_INFINITE);
        status = robot->open_device(robot);
        rt_mutex_release(&mutexRobot);
				nbcommrobot++;
        rt_mutex_acquire(&mutexEtat, TM_INFINITE);
        etatCommRobot = status;
        rt_mutex_release(&mutexEtat);

        if (status == STATUS_OK) {
            if (nbcommrobot<2){
            	rt_mutex_acquire(&mutexRobot, TM_INFINITE);
							status = robot->start_insecurely(robot); // sans watchdog
							rt_mutex_release(&mutexRobot);
							//status = robot->start(robot); // quand watchgod ok
						}
            if (status == STATUS_OK){
                rt_printf("tconnect : Robot démarrer\n");
                // Start aquisition de la batterie : envoi de l'évenement
								rt_printf("tconnect : Libération du semaphore semStartGetBattery\n");
								rt_sem_v(&semStartGetBattery);
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
                    
                    action->from_message(action, msg);
                    switch (action->get_order(action)) {
                        case ACTION_CONNECT_ROBOT:
                            rt_printf("tserver : Action connecter robot\n");
                            rt_sem_v(&semConnecterRobot);
                            break;
                        case ACTION_FIND_ARENA:
                        		rt_printf("tserver : Action recherche arene\n");
                        		rt_sem_v(&semStartDetectArena);
                        		break;
                        case ACTION_ARENA_IS_FOUND:
                        		rt_printf("tserver : Action found arene\n");
                        		rt_sem_v(&semStartDetectArena);
                        		break;	
                      	case ACTION_ARENA_FAILED:
		                    		rt_printf("tserver : Action recherche failed\n");
		                    		rt_sem_v(&semStartDetectArena);
		                    		break;
                      	case ACTION_COMPUTE_CONTINUOUSLY_POSITION:
                            rt_printf("tserver : Action calculer position\n");
														rt_sem_v(&semCalcPos);
														rt_sem_p(&semStartImage, TM_INFINITE);
                            break;
												case ACTION_STOP_COMPUTE_POSITION:
                            rt_printf("tserver : Action arrêter de calculer position\n");
                            rt_sem_p(&semCalcPos, TM_INFINITE);
									 					rt_sem_v(&semStartImage);
                            break;
                    }
                    break;
                case MESSAGE_TYPE_MOVEMENT:
                    rt_printf("tserver : Le message reçu %d est un mouvement\n",num_msg);
                    rt_mutex_acquire(&mutexMove, TM_INFINITE);
                    move->from_message(move, msg);
                    move->print(move);
                    rt_mutex_release(&mutexMove);
                    break;
               case MESSAGE_TYPE_MISSION:
                    rt_printf("tserver : Le message reçu %d est une mission\n",num_msg);
                    //instanciation et initialisation de la mission 
                    mission = d_new_mission();
                    //On recupere les informations du message de mission recu
                    d_mission_from_message(mission, msg);
                    rt_sem_p(&semStartMove,TM_INFINITE);
                    rt_sem_v(&semStartMission);
                    break;
            }
        }
    }
}

void deplacer(void *arg) {
    int status = 1;
    int gauche;
    int droite;
    int speed=0;
    DMessage *message;

    rt_printf("tmove : Debut de l'éxecution de periodique à 1s\n");
    rt_task_set_periodic(NULL, TM_NOW, 1000000000);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        rt_sem_p(&semStartMove,TM_INFINITE);
        rt_printf("tmove : Activation périodique\n");

        rt_mutex_acquire(&mutexEtat, TM_INFINITE);
        status = etatCommRobot;
        rt_mutex_release(&mutexEtat);
        if (status == STATUS_OK) {
            rt_mutex_acquire(&mutexMove, TM_INFINITE);
           // speed=move->get_speed(move);
            switch (move->get_direction(move)) {
                case DIRECTION_FORWARD:
                   // if (speed<50){
		                  gauche = MOTEUR_ARRIERE_LENT;
		                  droite = MOTEUR_ARRIERE_LENT;
		                /*}else {
		                	gauche = MOTEUR_ARRIERE_RAPIDE;
		                  droite = MOTEUR_ARRIERE_RAPIDE;
		                }*/
                    break;
                case DIRECTION_LEFT:
                  //  if (speed<50){
		                  gauche = MOTEUR_ARRIERE_LENT;
		                  droite = MOTEUR_AVANT_LENT;
		                /* }else {
		                  gauche = MOTEUR_ARRIERE_RAPIDE;
		                  droite = MOTEUR_AVANT_RAPIDE;
		                 }*/
                    break;
                case DIRECTION_RIGHT:
                    //if (speed<50){
		                  gauche = MOTEUR_AVANT_LENT;
		                  droite = MOTEUR_ARRIERE_LENT;
		               /* }else{
		                	gauche = MOTEUR_AVANT_RAPIDE;
		                  droite = MOTEUR_ARRIERE_RAPIDE;
		                }*/
                    break;
                case DIRECTION_STOP:
                    gauche = MOTEUR_STOP;
                    droite = MOTEUR_STOP;
                    break;
                case DIRECTION_STRAIGHT:
		                //if (speed<50){
		                  gauche = MOTEUR_AVANT_LENT;
		                  droite = MOTEUR_AVANT_LENT;
		                /*}else{
		                	gauche = MOTEUR_AVANT_RAPIDE;
		                  droite = MOTEUR_AVANT_RAPIDE;
		                }*/
		                break;
            }
            rt_mutex_release(&mutexMove);
						rt_mutex_acquire(&mutexRobot, TM_INFINITE);
            status = robot->set_motors(robot, gauche, droite);
						rt_mutex_release(&mutexRobot);
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
                rt_sem_v(&semConnecterRobot);
            }
        }
     		rt_sem_v(&semStartMove);
    }
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
	int status=0;
	int err;
	DJpegimage *jpeg;
	DMessage *message;
	
	camera=d_new_camera();
	rt_mutex_acquire(&mutexCamera, TM_INFINITE);
	/*ouverture de la webcam*/
	d_camera_open(camera);
	rt_mutex_release(&mutexCamera);
	
	rt_printf("tImage : Debut de l'éxecution de periodique à 600ms\n");
   rt_task_set_periodic(&tImage, TM_NOW, 600000000);
  
  while(1){
  	rt_sem_p(&semStartImage, TM_INFINITE);
  	message = d_new_message();
  	jpeg=d_new_jpegimage();
  	img=d_new_image();
			
  	rt_printf("tImage : Activation periodique\n");
  	rt_mutex_acquire(&mutexCamera, TM_INFINITE);
  	rt_mutex_acquire(&mutexImage, TM_INFINITE);
		rt_printf("tImage : frame\n");
  	camera->get_frame(camera,img);
  	rt_mutex_release(&mutexCamera);
  	rt_printf("tImage : com\n");
  	d_jpegimage_compress(jpeg,img);
  	rt_mutex_release(&mutexImage);
		rt_printf("tImage : jpeg\n");
  	d_message_put_jpeg_image(message,jpeg);
  	
  	
  	rt_mutex_acquire(&mutexEtat, TM_INFINITE);
    status = etatCommMoniteur;
    rt_mutex_release(&mutexEtat);

   

    rt_printf("tImage : Envoi message\n");
    if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
        message->free(message);
    }
  	rt_sem_v(&semStartImage);
  }

}


void detectArena(void *arg) {


		DMessage *msg = d_new_message();
    DArena *arn=d_new_arena();
    DJpegimage *jpegImage = d_new_jpegimage();


    int status = 0;
    int var = 0;
    int ordre = -1;
    int JaiSem=0;
    
    while(1) {
    		rt_sem_p(&semStartDetectArena, TM_INFINITE);
    		if (!JaiSem){
    			rt_sem_p(&semStartImage, TM_INFINITE);
    			JaiSem=1;
    		}
    		switch(action->get_order(action)){
    		case ACTION_FIND_ARENA:
    						rt_printf("ACTION_FIND_ARENA\n");
    						rt_mutex_acquire(&mutexCamera, TM_INFINITE);
  							rt_mutex_acquire(&mutexImage, TM_INFINITE);
								d_camera_get_frame(camera,img);
								rt_mutex_release(&mutexCamera);
								arn = d_image_compute_arena_position(img);
								d_imageshop_draw_arena(img, arn);
								//on compresse le message 
								d_jpegimage_compress(jpegImage, img);
								rt_mutex_release(&mutexImage);
								msg = d_new_message();
								d_message_put_jpeg_image(msg,jpegImage);
								//on envoie un message
								rt_printf("tAreba : Envoi message\n");
								if (write_in_queue(&queueMsgGUI, msg, sizeof (DMessage)) < 0) {
										msg->free(msg);
								}
								if(status!=-1) {
									status = STATUS_OK;
									rt_printf("message envoyé!\n");
					 		 	}
                break;
            case ACTION_ARENA_IS_FOUND:
            		rt_printf("ACTION_ARENA_IS_FOUND\n");
    						rt_mutex_acquire(&mutexArena, TM_INFINITE);
    						arena=arn;
    						rt_mutex_release(&mutexArena);
                rt_sem_v(&semStartImage);
                JaiSem=0;
                break;
            case ACTION_ARENA_FAILED:
            		rt_printf("ACTION_ARENA_FAILED\n");
                rt_sem_v(&semStartImage);
                JaiSem=0;
            break;
    		
   			}	
		}
}



/*
 * Récupérer la batterie du robot périodiquement : toutes les 250 ms
 * @param arg
 * @author ASHMAWY BOULANGER DIOP MANGEL
 */

void battery(void *arg) {
	rt_printf("tBattery : Début d'acquisition de la batterie\n");

	// Variables
	int battery;
	int err;
	int status;

	// Attente du lancement de l'aquisition de la batterie
	rt_printf("tBattery : Attente du semaphore semStartGetBattery\n");
	err = rt_sem_p(&semStartGetBattery, TM_INFINITE);
	rt_printf("tBattery : Récupération du semaphore semStartGetBattery %d\n", err);

	// Définition du thread tBattery : T=250ms
	rt_task_set_periodic(&tBattery, TM_NOW, 250000000);

	// Création de la variable DBattery
	DBattery *d_battery = d_new_battery();

	// Aquisition périodique
	while(1) {
		// Attente de la période du thread
		rt_task_wait_period(NULL);

	  rt_printf("tBattery : Activation periodique\n");
		
		// Récupérer l'état de la batterie du robot
		rt_mutex_acquire(&mutexRobot, TM_INFINITE); 
		status = robot->get_vbat(robot, &battery);
		rt_mutex_release(&mutexRobot); 


		// Si le status est OK, on transmet au moniteur
		if(status == STATUS_OK) {

			// Mise a jour de la variable etatCommRobot
			rt_mutex_acquire(&mutexEtat, TM_INFINITE); // récupérer le mutex mutexEtat
			etatCommRobot = status; // màj avec le status
			rt_mutex_release(&mutexEtat); // libérer le mutex mutexEtat

			// Création du message contenant l'état de la batterie au moniteur
			DMessage *d_message = d_new_message(); // création du message
			d_battery->set_level(d_battery, battery); // copie de l'état de la batterie dans la bonne variable
			d_message->put_battery_level(d_message, d_battery); // placer l'état de la batterie dans le DMessage
			
			// Envoi de l'état de la batterie au moniteur
      rt_printf("tBattery : Envoi message\n");
        
      if (write_in_queue(&queueMsgGUI, d_message, sizeof (DMessage)) < 0) {
          d_message->free(d_message);
      }
   }
	}
}

void calcul_position(void *arg) {

	DImage *image;
	DPosition *position;
	DJpegimage *newImage;
	DMessage* messagePosition;
	DMessage* messageImage;

	// Définition du thread tPosition : T=600ms
	rt_task_set_periodic(&tPosition, TM_NOW, 600000000);


	while(1) {
		// Attente de la période du thread
		rt_task_wait_period(NULL);
		//On prend le semaphore
		rt_sem_p(&semCalcPos, TM_INFINITE);
		rt_printf("tPosition : Activation periodique\n");
		
		image = d_new_image();
		position = d_new_position();
		newImage = d_new_jpegimage();
		messagePosition = d_new_message();
		messageImage = d_new_message();

		//On fait une capture de l'image actuelle
		rt_mutex_acquire(&mutexCamera,TM_INFINITE);
		d_camera_get_frame(camera, image);
		rt_mutex_release(&mutexCamera);
																									
		//On calcule la position du robot à l'image capturée
		position = d_image_compute_robot_position(image,NULL);
		if (position != NULL)
		{
			//On dessine la position sur l'image capturée
			d_imageshop_draw_position(image, position);
			//On fabrique le message contenant la position
			d_message_put_position(messagePosition, position);
			//On envoie la position
			if (write_in_queue(&queueMsgGUI, messagePosition, sizeof (DMessage)) < 0) {
          messagePosition->free(messagePosition);
      }
		}

			//On compresse la nouvelle image
			d_jpegimage_compress(newImage, image);

			//On fabrique le message contenant l'image
			d_message_put_jpeg_image(messageImage, newImage);
			//Maintenant on envoie l'image		
			if (write_in_queue(&queueMsgGUI, messageImage, sizeof (DMessage)) < 0) {
          messageImage->free(messageImage);
      }

		//On libère le semaphore
		rt_sem_v(&semCalcPos);
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
	 DImage *image;
	 DMessage* message = d_new_message();
	 
	 while(1){
	 rt_sem_p(&semStartMission, TM_INFINITE);
	 positionActuelle=d_new_position();
	 positionVoulue=d_new_position();
	 image=d_new_image();
	 rt_printf("tserver : Début de l'exécution du thread tMission\n");
	 
	 //Je recupere le numero de la mission
	 id = d_mission_get_id(mission); 

	 //Je recupere la position a atteindre
	 d_mission_get_position(mission, positionVoulue);
	 //Je recupere la position actuelle
	 	
																									
		//On calcule la position du robot à l'image capturée
		do
		{
			rt_mutex_acquire(&mutexCamera,TM_INFINITE);
			d_camera_get_frame(camera, image);
			rt_mutex_release(&mutexCamera);
			positionActuelle = d_image_compute_robot_position(image,NULL);
			
		}while (positionActuelle == NULL);
	 
   /*************************************/
	 
	
	 //Je calcule les écarts de position pour le futur deplacement
	 angle = positionVoulue->orientation - positionActuelle->orientation;
	 range = sqrt(pow(positionVoulue->x - positionActuelle->x, 2) + pow(positionVoulue->y - positionActuelle->y, 2));
	 
	 //Maintenant on rotate pour s'aligner avec la destination
	 rt_mutex_acquire(&mutexRobot,TM_INFINITE);
	 status = d_robot_turn(robot,angle,direction);
	 rt_mutex_release(&mutexRobot);
	 rt_mutex_acquire(&mutexEtat, TM_INFINITE);
   etatCommRobot = status;
   rt_mutex_release(&mutexEtat);

	 if(status ==STATUS_OK)
	 {
		printf("Turn ok\n");
	 }
	 else
	 {
		printf("Error Turn\n");
	 }
	 //Puis on avance pour arriver à destination
	 rt_mutex_acquire(&mutexRobot,TM_INFINITE);
	 status = d_robot_move(robot,range);
	 rt_mutex_release(&mutexRobot);
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
	 sleep(5);
	 d_message_mission_terminate(message,id);
	 
	 if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) {
          message->free(message);
   }
	 rt_sem_v(&semStartMove);
}
}
