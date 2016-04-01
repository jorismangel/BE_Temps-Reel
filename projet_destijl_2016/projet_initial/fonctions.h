/* 
 * File:   fonctions.h
 * Author: pehladik
 *
 * Created on 12 janvier 2012, 10:19
 */

#ifndef FONCTIONS_H
#define	FONCTIONS_H

#include "global.h"
#include "includes.h"

#ifdef	__cplusplus
extern "C" {
#endif
        void connecter(void * arg);
        void communiquer(void *arg); 	// recevoir des ordres du moniteur
        void deplacer(void *arg);	// deplacer le robot
        void envoyer(void *arg);	// envoyer des messages au moniteur
        void image(void *arg);		// recuperer les images de la camera


#ifdef	__cplusplus
}
#endif

#endif	/* FONCTIONS_H */

