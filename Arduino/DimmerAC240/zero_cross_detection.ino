void detection_zero() {        	// Fonction associee à l'interruption 0                          

  detachInterrupt(2); 			// on desactive l'interruption zero crossing

  c2=0;

  for (c1=0; c1 <= ForMaxCanal; c1++){ 	// on parcourt les 8 sorties pour verifier leur consigne
    if  (canal[c1].consigne>=49){ // si consigne 0%
      eteint(c1);			// alors on éteint
    }

    if  (canal[c1].consigne<=0){ 	// si consigne 100%
      allume(c1);			// alors on allume
    }
    // gestion transition automatique
    if (canal[c1].consigne<canal[c1].consigne_cible){ 									// si la consigne actuelle est inferieur a la consigne demandee
      canal[c1].compteur_transition_courant=canal[c1].compteur_transition_courant-1; 	//on decremente le compteur de transition
      if (canal[c1].compteur_transition_courant<=0){ 									// et si celui ci est nul
        canal[c1].consigne=canal[c1].consigne+1; 									// on incremente la consigne actuelle pour tendre a se rapprocher de la consigne demandee
        canal[c1].compteur_transition_courant=canal[c1].compteur_transition; 		// et on re-initialise le compteur de transition
      }
    }

    if (canal[c1].consigne>canal[c1].consigne_cible){									// si la consigne actuelle est superieur a la consigne demandee
      canal[c1].compteur_transition_courant=canal[c1].compteur_transition_courant-1;	// on decremente le compteur de transition
      if (canal[c1].compteur_transition_courant<=0){									// et si celui ci est nul
        canal[c1].consigne=canal[c1].consigne-1;									// on decremente la consigne actuelle pour tendre a se rapprocher de la consigne demandee
        canal[c1].compteur_transition_courant=canal[c1].compteur_transition;		// et on re-initialise le compteur de transition
      }
    }

  }

  Timer1.attachInterrupt(controle_canaux, retard[c2]);      // on attache l'interruption temporelle
  Serial.println("Zero");

}                                 // Fin de detection_zero

void controle_canaux() {  // ici on verifie si le triac doit etre amorce

  c2=c2++;

  attachInterrupt(0, detection_zero, FALLING);  	// on attache une interruption sur la pin 2 (interruption 0)
  Timer1.detachInterrupt();						// on detache l'interruption temporelle

  if (c2>=49){ // si dernier cycle alors

    for (c1=0; c1 <= ForMaxCanal; c1++){ 	// on parcourt les 8 sorties
      eteint(c1);				// et on eteint le canal en vue du prochain cycle
    }


  }
  else { // sinon

    Timer1.attachInterrupt(controle_canaux, retard[c2]);      // on attache une interruption temporelle

    for (c1=0; c1 <= ForMaxCanal; c1++){ 	// on parcourt les 8 sorties pour verifier leur consigne
      if  (canal[c1].consigne==c2)	// si consigne est egale a la valeur traitee (n° de passage dans la boucle)
      { 
        allume(c1);
      }			// alors on allume le canal
    }

  }                                    // End controle_canaux function

}

int time_out(){
    int timeout=0;
  timeout++;

  if(timeout>tps_max_lecture)
  {
    Serial.println("T1");
    return -1;
  }
  if(timeout> tps_max_lecture)
  {
    Serial.println("T2");
    return -2;
  }
  
}
