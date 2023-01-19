




#include <TimerOne.h>	 // pour l'interruption temps http://www.arduino.cc/playground/Code/Timer1
#include <stdio.h> 			// pour le traitement de la trame contenant le changement de consigne

// definition des macros pour piloter les sorties
#define  allume(index)  (digitalWrite(canal[index].sortie, HIGH))
#define  eteint(index)  (digitalWrite(canal[index].sortie, LOW))

#define MAX_CANAL 8 			// definir ici le nombre de canaux à gérer (8 par défaut)

int ForMaxCanal = MAX_CANAL-1; 	// variable permettant de definir le nombre de passage dans les différentes boucles FOR

//valeur de timeout pour la reception de la trame
int tps_max_lecture = 200;	//lecture d'un code, compteur max entre tous les caractères d'un code
int tps_max_carte = 1000;	//compteur max entre reception d'un caractère

int timer;

long retard[]= {
  1469 , // 98 % 1 225,3V retard / zéro = 1469 ms
  287 , // 96 % 2 222,7V retard / zéro = 1867 ms
  234 , // 94 % 3 220,6V retard / zéro = 2154 ms
  201 , // 92 % 4 218,2V retard / zéro = 2388 ms
  180 , // 90 % 5 215,4V retard / zéro = 2589 ms
  164 , // 88 % 6 213,3V retard / zéro = 2769 ms
  152 , // 86 % 7 210,8V retard / zéro = 2933 ms
  143 , // 84 % 8 208V retard / zéro = 3085 ms
  135 , // 82 % 9 205,7V retard / zéro = 3228 ms
  129 , // 80 % 10 202,8V retard / zéro = 3363 ms
  124 , // 78 % 11 200,5V retard / zéro = 3492 ms
  120 , // 76 % 12 197,6V retard / zéro = 3616 ms
  116 , // 74 % 13 195,2V retard / zéro = 3736 ms
  112 , // 72 % 14 192,4V retard / zéro = 3852 ms
  110 , // 70 % 15 189,6V retard / zéro = 3964 ms
  108 , // 68 % 16 186,8V retard / zéro = 4074 ms
  106 , // 66 % 17 184V retard / zéro = 4182 ms
  105 , // 64 % 18 180,9V retard / zéro = 4288 ms
  103 , // 62 % 19 178,1V retard / zéro = 4393 ms
  102 , // 60 % 20 175,1V retard / zéro = 4496 ms
  101 , // 58 % 21 172,1V retard / zéro = 4598 ms
  101 , // 56 % 22 168,9V retard / zéro = 4699 ms
  100 , // 54 % 23 166,2V retard / zéro = 4800 ms
  100 , // 52 % 24 162,6V retard / zéro = 4900 ms
  100 , // 50 % 25 159,3V retard / zéro = 5000 ms
  101 , // 48 % 26 155,8V retard / zéro = 5100 ms
  100 , // 46 % 27 152,6V retard / zéro = 5201 ms
  101 , // 44 % 28 149,1V retard / zéro = 5301 ms
  102 , // 42 % 29 145,3V retard / zéro = 5402 ms
  103 , // 40 % 30 141,8V retard / zéro = 5504 ms
  105 , // 38 % 31 138V retard / zéro = 5607 ms
  106 , // 36 % 32 133,8V retard / zéro = 5712 ms
  108 , // 34 % 33 130V retard / zéro = 5818 ms
  110 , // 32 % 34 126V retard / zéro = 5926 ms
  112 , // 30 % 35 121,7V retard / zéro = 6036 ms
  116 , // 28 % 36 117,1V retard / zéro = 6148 ms
  120 , // 26 % 37 112,6V retard / zéro = 6264 ms
  124 , // 24 % 38 107,7V retard / zéro = 6384 ms
  129 , // 22 % 39 102,4V retard / zéro = 6508 ms
  135 , // 20 % 40 97,2V retard / zéro = 6637 ms
  143 , // 18 % 41 92V retard / zéro = 6772 ms
  152 , // 16 % 42 85,7V retard / zéro = 6915 ms
  164 , // 14 % 43 79,4V retard / zéro = 7067 ms
  180 , // 12 % 44 72,8V retard / zéro = 7231 ms
  201 , // 10 % 45 64,8V retard / zéro = 7411 ms
  234 , // 8 % 46 56,4V retard / zéro = 7612 ms
  286 , // 6 % 47 46V retard / zéro = 7846 ms
  399 , // 4 % 48 32,4V retard / zéro = 8132 ms
  500//1469 , // 2 % 49 0V retard / zéro = 8531 ms
};

typedef struct Canal Canal;
struct Canal
{
  int consigne; 						// consigne actuelle
  int consigne_cible;					// consigne niveau de canal (0=100%, 50=0%)
  int compteur_transition;			// temps de transition lors d'un changement niveau de canal (par pas de 10 ms) ex: 2 x 50 cycles de 10ms= 1s de transition
  int compteur_transition_courant;	// compte le nombre de transition
  int sortie;							// affecte une pin pour chaque canal.
  char mnemo[16];						// mnemonique de la variable
};

//creation des instances
Canal canal[MAX_CANAL]; // L'ordinateur comprend qu'il s'agit de "struct Coordonnees" grâce au typedef

volatile int c1=0;	// index c1 pour lecture des données de chaque canal (n° pin, consigne)
volatile int c2=0;	// index c2 nombre de passage dans la boucle de controle retard de phase (49 passages)


void initialiserCanal() //on initialise les paramètres de chaque canal.
{
  // configuration du canal 0
  canal[0].consigne=50;
  canal[0].consigne_cible=50;
  canal[0].compteur_transition=1;
  canal[0].compteur_transition_courant=0;
  canal[0].sortie=3;
  canal[0].mnemo[16]='salon';

  // configuration du canal 1
  canal[1].consigne=50;
  canal[1].consigne_cible=50;
  canal[1].compteur_transition=1;
  canal[1].compteur_transition_courant=0;
  canal[1].sortie=4;
  canal[1].mnemo[16]='N/A';

  // configuration du canal 2
  canal[2].consigne=50;
  canal[2].consigne_cible=50;
  canal[2].compteur_transition=1;
  canal[2].compteur_transition_courant=0;
  canal[2].sortie=5;
  canal[2].mnemo[16]='N/A';

  // configuration du canal 3
  canal[3].consigne=50;
  canal[3].consigne_cible=50;
  canal[3].compteur_transition=1;
  canal[3].compteur_transition_courant=0;
  canal[3].sortie=6;
  canal[3].mnemo[16]='N/A';

  // configuration du canal 4
  canal[4].consigne=50;
  canal[4].consigne_cible=50;
  canal[4].compteur_transition=1;
  canal[4].compteur_transition_courant=0;
  canal[4].sortie=7;
  canal[4].mnemo[16]='N/A';

  // configuration du canal 5
  canal[5].consigne=50;
  canal[5].consigne_cible=50;
  canal[5].compteur_transition=1;
  canal[5].compteur_transition_courant=0;
  canal[5].sortie=8;
  canal[5].mnemo[16]='N/A';

  // configuration du canal 6
  canal[6].consigne=50;
  canal[6].consigne_cible=50;
  canal[6].compteur_transition=1;
  canal[6].compteur_transition_courant=0;
  canal[6].sortie=9;
  canal[6].mnemo[16]='N/A';

  // configuration du canal 7
  canal[7].consigne=50;
  canal[7].consigne_cible=50;
  canal[7].compteur_transition=1;
  canal[7].compteur_transition_courant=0;
  canal[7].sortie=10;
  canal[7].mnemo[16]='N/A';

}






//

void setup() {   
  
 // Serial.begin(9600);

  initialiserCanal();

  // initialise les sorties des canaux (triacs)
  for (c1=0; c1 <= ForMaxCanal; c1++){ 				// on parcourt les 8 canaux pour les configurer
    pinMode(canal[c1].sortie, OUTPUT); 	// on associe chaque canal a une pin, qu'on configure en sortie digitale
    eteint(canal[c1].sortie);				// et on éteint la sortie
    canal[c1].compteur_transition_courant=canal[c1].compteur_transition;

  }




  // initialise l'interruption de temps Timer1
  Timer1.initialize();			    // Initialize TimerOne library for the freq we need


  // Attache l'interruption 0 à la pin 2 pour la detection du passage a zero (Zero Cross Detection)
  attachInterrupt(2, detection_zero, FALLING);   // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection

}								   // Fin du setup


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

}                                 // Fin de detection_zero

void controle_canaux() {  // ici on verifie si le triac doit etre amorce

  c2=c2++;

  attachInterrupt(2, detection_zero, FALLING);  	// on attache une interruption sur la pin 2 (interruption 0)
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





void loop() {     

  lecture();
  //Serial.println(analogRead(5));

}

int lecture(){		// lecture d'une trame type: " D/aaa/bbb/F"
  // ou "D" caractere de debut de trame
  // ou "aaaa" n° de sortie dont la consigne est a modifier
  // ou "bbbb" nouvelle consigne de la sortie (entre 0 et 100%)




  int timeout=0;



  timeout++;

  if(timeout>tps_max_lecture)
  {
    
    
    return -1;
  }
  if(timeout> tps_max_lecture)
  {

    return -2;
  }


  canal[2].consigne=20;
  canal[2].consigne_cible=20;

//circle_shadow();  





}//END void loop()
