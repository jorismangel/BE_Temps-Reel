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
        void communiquer(void *arg);
        void deplacer(void *arg);
        void envoyer(void *arg);
        //Nos threads supplementaires 
        //void receive(void *arg); equivaut au thread communiquer du projet initial
        void watchdog(void *arg);
        void position(void *arg);
        void arena(void *arg);
        void image(void *arg);
        void battery(void *arg);
#ifdef	__cplusplus
}
#endif

#endif	/* FONCTIONS_H */

