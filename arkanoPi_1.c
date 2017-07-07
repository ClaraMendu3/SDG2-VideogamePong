#include "arkanoPi_1.h"

//Comentar y descomentar para mostrar (o no mostrar) mensajes de depuraci�n por pantalla. Comentar ahorra recursos y da fluidez al refresco de leds
#define __MODO_DEBUG_TERMINAL__

static  tipo_juego juego; //volatile

//Array para tener ordenados los pines de las filas (pues los pines software no van seguidos, son: 0,1, 2, 3, 4, 7,  23)
//para poder utilizar un bucle for y un �ndice a la hora de iluminar...
int matrizPinesFilas[7]={pinFila0, pinFila1, pinFila2,pinFila3, pinFila4, pinFila5, pinFila6};

//Variable global y volatile qe 32 bits que contiene todos los FLAGS del juego (FLAG_PELOTA, FLAG_RAQUETA_DERECHA, FLAG_FINAL_JUEGO...)
//Es volatile porque diferentes recursos pueden acceder a ella (previo bloqueo) y modificar su valor.
volatile int flags = 0;

//Variable global utilizada para el refresco/iluminaci�n de columnas. Empezamos por la columna 0!
int columna=0;

int velocidad = 0;

//Variables globales para almacenar la puntuaci�n de cada jugador en una  partida. Las partidas se finalizar�n cuando uno de los
//jugadores llegue a 2 puntos pues ecribir las matrices con los posibles resultados es tedioso... 
int puntuacionJugadorInferior;
int puntuacionJugadorSuperior;

//Funci�n de retardo.
// espera hasta la pr�xima activaci�n del reloj
void delay_until (unsigned int next) {
	unsigned int now = millis();

	if (next > now) {
		delay (next - now);
    }
}

//------------------------------------------------------
// FUNCIONES DE ACTIVACI�N DE LOS PULSADORES. Activamos los flags de los pulsadores Hay que tener cuidado con los rebotes. 
//------------------------------------------------------
//Activamos los flags de los pulsadores al detectarse en una pulsaci�n a trav�s del pin correspondiente.  (Una interrupci�n consistente en un flanco de bajada!)
//Hay que tener cuidado con los rebotes. De ah� el uso de DEBOUNCE_TIME, debounceTime, delay, millis...
void activaFlagPulsadorIzquierdoInferior(void){
	int debounceTime =0;

	// Pin event (key / button event) debouncing procedure
	// "millis()" returns a number representing the number of milliseconds since our
	// program called one of the wiringPiSetup functions (wiringPiSetupGpio in our case).
	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}

	// Atenci�n a la interrupci�n
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA_INFERIOR;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);


	// Wait for key to be released
	while (digitalRead (pinPulsadorIzquierdoJugadorInferior) == HIGH) {
        delay (1) ;
		}

	debounceTime = millis () + DEBOUNCE_TIME ;
}


void activaFlagPulsadorIzquierdoSuperior(void){
	int debounceTime =0;

	// Pin event (key / button event) debouncing procedure
	// "millis()" returns a number representing the number of milliseconds since our
	// program called one of the wiringPiSetup functions (wiringPiSetupGpio in our case).
	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}

	// Atenci�n a la interrupci�n
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_IZQUIERDA_SUPERIOR;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);



	// Wait for key to be released
	while (digitalRead (pinPulsadorIzquierdoJugadorSuperior) == HIGH) {
	delay (1) ;
	}

	debounceTime = millis () + DEBOUNCE_TIME ;
}

void activaFlagPulsadorDerechoInferior(void){
	// Pin event (key / button event) debouncing procedure
	// "millis()" returns a number representing the number of milliseconds since our
	// program called one of the wiringPiSetup functions (wiringPiSetupGpio in our case).
	int debounceTime =0;

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}

	// Atenci�n a la interrupci�n
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA_INFERIOR;
	flags |= FLAG_TECLA;
        piUnlock (FLAGS_KEY);


       // Wait for key to be released
       while (digitalRead (pinPulsadorDerechoJugadorInferior) == HIGH) {
       delay (1) ;
       }

       debounceTime = millis () + DEBOUNCE_TIME ;
}

void activaFlagPulsadorDerechoSuperior(void){
	// Pin event (key / button event) debouncing procedure
	// "millis()" returns a number representing the number of milliseconds since our
	// program called one of the wiringPiSetup functions (wiringPiSetupGpio in our case).
	int debounceTime =0;

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}

	// Atenci�n a la interrupci�n
	piLock (FLAGS_KEY);
	flags |= FLAG_RAQUETA_DERECHA_SUPERIOR;
	flags |= FLAG_TECLA;
        piUnlock (FLAGS_KEY);



       // Wait for key to be released
       while (digitalRead (pinPulsadorDerechoJugadorSuperior) == HIGH) {
       delay (1) ;
       }

       debounceTime = millis () + DEBOUNCE_TIME ;
}


void activaFlagPulsadorStop(void){
	// Pin event (key / button event) debouncing procedure
	// "millis()" returns a number representing the number of milliseconds since our
	// program called one of the wiringPiSetup functions (wiringPiSetupGpio in our case).
	int debounceTime =0;

	if (millis () < debounceTime) {
	debounceTime = millis () + DEBOUNCE_TIME ;
	return;
	}

	// Atenci�n a la interrupci�n
	piLock (FLAGS_KEY);
	flags |= FLAG_STOP;
	flags |= FLAG_TECLA;
        piUnlock (FLAGS_KEY);

        // Wait for key to be released
        while (digitalRead (pinPulsadorStop) == HIGH) {
        delay (1) ;
        }

        debounceTime = millis () + DEBOUNCE_TIME ;
}

//Peri�dicamente activamos FLAG_PELOTA para desplazar la pelota seg�n la trayectoria correspondiente.
//Activamos FLAG_JOYSTICK por primera vez al crear y consumirse el tiempo indicado por CLK_PELOTA al iniciar el temporizador en el main.
//El temporizador se vuelve a iniciar de nuevo tras activar el flag con tmr_startms(juego.TIMER2, velocidad) para conseguir esta periodicidad y que al consumirse el tiempo
//volvamos a esta funci�n de  nuevo para activar el flag!
void activaFlagPelota(union sigval value){
	piLock (FLAGS_KEY);
	flags |= FLAG_PELOTA;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);

        tmr_startms(juego.TIMER2, velocidad);
	}
	
//------------------------------------------------------
// FUNCI�N DE DESACTIVACI�N SONIDO. Al poner la frecuencia a cero paras el sonido!
//------------------------------------------------------
void desactivaSonido(union sigval value){
	softToneWrite(pinSonido, 0);
	}

//------------------------------------------------------
// FUNCIONES DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// FUNCIONES DE ENTRADA O COMPROBACI�N DE LA MAQUINA DE ESTADOS
//Se comprueba el valor del flag correspondiente y si vale 1, se produce una transici�n en la m�quina de estados
//y se lanza la funci�n de salida asociada.

int compruebaTeclaPulsada (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_TECLA);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaTeclaPelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_PELOTA);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaTeclaRaquetaIzquierdaInferior (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA_INFERIOR);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaTeclaRaquetaIzquierdaSuperior (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_IZQUIERDA_SUPERIOR);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaTeclaRaquetaDerechaInferior (fsm_t* this) {
	int result;


	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA_INFERIOR);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaTeclaRaquetaDerechaSuperior (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_RAQUETA_DERECHA_SUPERIOR);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaTeclaStop (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_STOP);
	piUnlock (FLAGS_KEY);

	return result;
}

int compruebaFinalJuego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);
	result = (flags & FLAG_FINAL_JUEGO);
	piUnlock (FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE ACCION. Debemos desactivar el flag correspondiente!!!
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicializaci�n de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (fsm_t* this) {
	// A completar por el alumno... OKK

	piLock (FLAGS_KEY);
        flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);
		
	//Al iniciar el juego, la puntuaci�n de ambos jugadores es 0!
	puntuacionJugadorInferior= 0;
	puntuacionJugadorSuperior= 0;
		
	piLock (STD_IO_BUFFER_KEY);
	printf("Escriba un valor mayor que 700 para darle la velocidad deseada al movimiento de la pelota. Cuanto mayor es el valor, menor es la velocidad! \n");
	piUnlock (STD_IO_BUFFER_KEY);
	scanf("%d", &velocidad);

		
	InicializaArkanoPi(&(juego.arkanoPi));
		
	//Creamos e inicializamos el temporizador de la pelota, del sonido y del joystick aqu� en vez de en el main pues hasta este punto no es utilizado//No tiene sentido configuarlo antes de tiempo. No ocurre lo mismo con el temporizador encargado de el refresco de la matriz de leds
	//que necesitamos su creaci�n desde el principio.
	  //La funci�n que pones entre par�ntesis de la sentencia juego.TIMERX= tmr_new (Y) es la encargada de actuar al consumirse el CLK_Z correspondiente!!!

	juego.TIMER2= tmr_new (activaFlagPelota);
	tmr_startms(juego.TIMER2, velocidad);
		
	juego.TIMER_SONIDO= tmr_new (desactivaSonido);
}

//Tener en cuenta que la pala siempre debe tener un cuadradito dentro de la pantalla y su X nos indica el extremo izquierdo!!!!!!!!!!!!!

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobaci�n
// de que la nueva posici�n correspondiente a la raqueta no suponga
// que esta rebase o exceda los l�mites definidos para el �rea de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobaci�n
// de que la nueva posici�n correspondiente a la raqueta no suponga
// que esta rebase o exceda los l�mites definidos para el �rea de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierdaInferior (fsm_t* this) {
	// A completar por el alumno... OKK

	piLock (FLAGS_KEY);
        flags &= ~FLAG_RAQUETA_IZQUIERDA_INFERIOR;
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	if(juego.arkanoPi.raquetaInferior.x == -2 ){
		juego.arkanoPi.raquetaInferior.x =-2;
	}
	else{
		juego.arkanoPi.raquetaInferior.x -=1;

	}

	ActualizaPantalla(&(juego.arkanoPi));
	
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
        PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	#endif
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobaci�n
// de que la nueva posici�n correspondiente a la raqueta no suponga
// que esta rebase o exceda los l�mites definidos para el �rea de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierdaSuperior (fsm_t* this) {
	// A completar por el alumno... OKK

	piLock (FLAGS_KEY);
        flags &= ~FLAG_RAQUETA_IZQUIERDA_SUPERIOR;
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	if(juego.arkanoPi.raquetaSuperior.x == 9 ){
	 juego.arkanoPi.raquetaSuperior.x = 9;
	}
	else{
	 juego.arkanoPi.raquetaSuperior.x +=1;
	}

	ActualizaPantalla(&(juego.arkanoPi));
	
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
        PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	#endif
}

// void MueveRaquetaDerecha (void): funci�n similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerechaInferior (fsm_t* this) {
	// A completar por el alumno... OKK

	piLock (FLAGS_KEY);
	flags &= ~FLAG_RAQUETA_DERECHA_INFERIOR;
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	if(juego.arkanoPi.raquetaInferior.x == 9 ){
	 juego.arkanoPi.raquetaInferior.x =9;
	}

	else{
	 juego.arkanoPi.raquetaInferior.x +=1;
	}

	ActualizaPantalla(&(juego.arkanoPi));
	
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
        PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	#endif
}


// void MueveRaquetaDerecha (void): funci�n similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerechaSuperior (fsm_t* this) {
	// A completar por el alumno... OKK

	piLock (FLAGS_KEY);
        flags &= ~FLAG_RAQUETA_DERECHA_SUPERIOR;
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	if(juego.arkanoPi.raquetaSuperior.x == -2 ){
	 juego.arkanoPi.raquetaSuperior.x = -2;
	}

	else{
	 juego.arkanoPi.raquetaSuperior.x -=1;
	}

	ActualizaPantalla(&(juego.arkanoPi));
	
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
        PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	#endif
}

// void MovimientoPelota (void): funci�n encargada de actualizar la
// posici�n de la pelota conforme a la trayectoria definida para esta.
// Para ello deber�a identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicar�n adicionalmente
// la eliminaci�n del ladrillo). Del mismo modo, deber�a tambi�n
// identificar las situaciones en las que se da por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto esta
// rebase el l�mite inferior del �rea de juego, bien porque se agoten
// los ladrillos visibles en el �rea de juego.
void MovimientoPelota (fsm_t* this) {
	// A completar por el alumno... OKK

	piLock (FLAGS_KEY);
	flags &= ~FLAG_PELOTA;
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);


       //**************PUNTUACI�N**************//        
	//Alguno de los jugaodres no ha conseguido golpear la pelota con la raqueta...
	if(juego.arkanoPi.pelota.y==6 || juego.arkanoPi.pelota.y==0 ){
	 		
	  //Ha fallado el jugador inferior, gana puntos el superior!
	  if(juego.arkanoPi.pelota.y==6){
	  puntuacionJugadorSuperior++;
          }

	  //Ha fallado el jugador superior, gana puntos el inferior!
          else if (juego.arkanoPi.pelota.y==0){
	  puntuacionJugadorInferior++;
	  }		
			
	  //Si alguno de los jugadores ha alcanzado 2 puntos, acaba y gana el juego!
	  if(puntuacionJugadorSuperior==2 || puntuacionJugadorInferior==2) {
				
            //Mostramos la puntuaci�n por pantalla
            #ifdef __MODO_DEBUG_TERMINAL__		
	    piLock (STD_IO_BUFFER_KEY);	
	    printf("\n\n La puntuaci�n es: JugadorSuperior %d , JugadoeInferior %d\n\n", puntuacionJugadorSuperior, puntuacionJugadorInferior);
	    piUnlock (STD_IO_BUFFER_KEY);
            #endif
			
	    //Mostramos la puntuaci�n en la matriz de leds.
	    MuestraPuntuacion(puntuacionJugadorInferior, puntuacionJugadorSuperior);		
         	            			
	    #ifdef __MODO_DEBUG_TERMINAL__ 
	    piLock (STD_IO_BUFFER_KEY);
	    PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
            piUnlock (STD_IO_BUFFER_KEY);
	    #endif
			
	    delay(1500);
	    piLock (FLAGS_KEY);
	    flags|= FLAG_FINAL_JUEGO;
	    piUnlock (FLAGS_KEY);
	   }
			
	   //Ninguno de los jugadores ha alcanzado 2 puntos, pero uno de ellos ha fallado, mostramos la nueva puntuaci�n!
	   else {
				
	    //Mostramos la puntuaci�n por pantalla.
            #ifdef __MODO_DEBUG_TERMINAL__
	    piLock (STD_IO_BUFFER_KEY);	
	    printf("\n\n La puntuaci�n es: JugadorSuperior %d , JugadoeInferior %d\n\n", puntuacionJugadorSuperior, puntuacionJugadorInferior);
	    piUnlock (STD_IO_BUFFER_KEY);
            #endif
			
	    //Mostramos la puntuaci�n en la matriz de leds.
	    MuestraPuntuacion(puntuacionJugadorInferior, puntuacionJugadorSuperior);	
            delay(1500);
			
	    //Reseteamos las raquetas y la pelota para que los jugadores empiecen la siguiente ronda
            InicializaArkanoPi(&(juego.arkanoPi));
	    #ifdef __MODO_DEBUG_TERMINAL__ 
            piLock (STD_IO_BUFFER_KEY);
            PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	    piUnlock (STD_IO_BUFFER_KEY);
	    #endif				
       	   }			
	 }
       //**************PUNTUACI�N**************//  

	 //REBOTES CON RAQUETA INFERIOR
	 if (juego.arkanoPi.pelota.y==5 ){
	  ReboteRaquetaInferior ();
	 }

	 //REBOTES CON RAQUETA SUPERIOR
	 if (juego.arkanoPi.pelota.y==1 ){
	  ReboteRaquetaSuperior ();
	 }

         //REBOTES CON PARED: solo hay que modificar la variaci�n de la X
	 if((juego.arkanoPi.pelota.x==0 && juego.arkanoPi.pelota.xv==-1 ) || (juego.arkanoPi.pelota.x==9 && juego.arkanoPi.pelota.xv==1) ){
          RebotePared();
	 }

	ActualizaPosicionPelota ();

	ActualizaPantalla(&(juego.arkanoPi));
		
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	#endif
}



// void FinalJuego (void): funci�n encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (fsm_t* this) {
	// A completar por el alumno... OKK

        piLock (FLAGS_KEY);
	flags &= ~FLAG_FINAL_JUEGO;
	piUnlock (FLAGS_KEY);
		


	piLock (STD_IO_BUFFER_KEY);
	printf("\n\n Ha finalizado el juego!!!\n\n");

	if(puntuacionJugadorInferior==2){
	 printf("\n\n Ha perdido el jugador superior!!!\n\n");
	}

	else if (puntuacionJugadorSuperior==2){
	 printf("\n\n Ha perdido el jugador inferior!!!\n\n");
	}
	piUnlock (STD_IO_BUFFER_KEY);

	//Activamos FLAGS_KEY para que la m��quina de estados interprete que tenemos una tecla pulsada y directamente pase a WAIT_START y lance ReseteaJuego.
	//Si queremos  que s�lo se produzca la transici��n al pulsar una tecla, deber�amos borrar estas l�neas, pero...
	piLock (FLAGS_KEY);
	flags|= FLAG_TECLA;
	piUnlock (FLAGS_KEY);

        //Paramos los temporizadors de la pelota, de sonido y del joystick pues el juego ha acabado y ya no son necesarios.
	tmr_stop(juego.TIMER2);	
	tmr_stop(juego.TIMER_SONIDO);
}

//void ReseteaJuego (void): funci�n encargada de llevar a cabo la
// reinicializaci�n de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (fsm_t* this) {
	// A completar por el alumno... OKK

	//Pintamos un mensaje de bienvenida, un Hi!
	PintaMensajeInicialPantalla(&(juego.arkanoPi.pantalla), &bienvenida);

	//Desactivamos FLAGS_KEY pues ya hemos atendido el evento de tecla pulsada para resetear el juego
	piLock (FLAGS_KEY);
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	//Escribimos unas instrucciones por pantalla
	piLock (STD_IO_BUFFER_KEY);
	printf("\n");
	printf("***************************\n");
	printf("**********ARKANOPI PONG*********\n");
	printf("***************************\n");

	PintaPantallaPorTerminal ((tipo_pantalla *)(& (juego.arkanoPi.pantalla)));
	printf("\nPULSE PARA COMENZAR EL JUEGO...\n");
	printf("i-Mueve la  raqueta inferior  a la  izquierda\n");
	printf("o-Mueve la  raqueta inferior  a la  derecha\n");
	printf("k-Mueve la  raqueta superior  a la  izquierda\n");
	printf("l-Mueve la  raqueta superior  a la  derecha\n");
	printf("s-Para el juego\n");
	printf("s-Reanuda el juego\n");
	piUnlock (STD_IO_BUFFER_KEY);

}

//------------------------------------------------------
// FUNCIONES AUXILIARES
//------------------------------------------------------

// void ActualizaPosicionPelota (void): para actualizar la posicion de la pelota
//seg�n la trayectoria.
void ActualizaPosicionPelota (void) {

	//Hacia abajo y derecha (diagonal)
	if((juego.arkanoPi.pelota.xv == 1 ) && (juego.arkanoPi.pelota.yv==1)){
	 juego.arkanoPi.pelota.x+=1;
	 juego.arkanoPi.pelota.y+=1;
	}

	//Hacia abajo (recto)
	else if((juego.arkanoPi.pelota.xv == 0 ) && (juego.arkanoPi.pelota.yv==1)){
	 juego.arkanoPi.pelota.y+=1;
	}

	//Hacia abajo e izquierda (diagonal)
	else if((juego.arkanoPi.pelota.xv == -1 ) && (juego.arkanoPi.pelota.yv==1)){
	 juego.arkanoPi.pelota.x-=1;
	 juego.arkanoPi.pelota.y+=1;
	}

	//Hacia arriba y izquierda (diagonal)
	else if((juego.arkanoPi.pelota.xv == -1 ) && (juego.arkanoPi.pelota.yv==-1)){
	 juego.arkanoPi.pelota.x-=1;
	 juego.arkanoPi.pelota.y-=1;
	}

	//Hacia arriba (recto)
	else if((juego.arkanoPi.pelota.xv ==0 ) && (juego.arkanoPi.pelota.yv==-1)){
	 juego.arkanoPi.pelota.y-=1;
	}

	//Hacia arriba y derecha (diagonal)
	else if((juego.arkanoPi.pelota.xv == 1 ) && (juego.arkanoPi.pelota.yv==-1)){
	 juego.arkanoPi.pelota.x+=1;
	 juego.arkanoPi.pelota.y-=1;
	}
}

//void ReboteRaquetaInferior (void)
void ReboteRaquetaInferior (void) {
	
        //Hacemos sonar un sonido de una cierta frecuencia/ciclo de trabajo mediante PMW y en cuanto pase el tiempo indicado por CLK_SONIDO
        //nos vamos a la funci�n desactivaSonido que se encarga de parar el sonido pues contiene la sentencia softToneWrite(pinSonido, 0);
        //que lo que hace es poner la frecuencia a 0 y por tanto parar el sonido como hemos visto en la documentaci�n de la funci�n.
        //[This updates the tone frequency value on the given pin. The tone will be played until you set the frequency to 0.]
        softToneWrite(pinSonido, 500);
        tmr_startms(juego.TIMER_SONIDO,CLK_SONIDO);

        #ifdef __MODO_DEBUG_TERMINAL__ 
        printf("\n\n Vas a rebotar con raqueta inferior! \n\n");
        #endif

	//La pelota llega en diagonal hacia la derecha y descendente
	if(juego.arkanoPi.pelota.xv==1 && juego.arkanoPi.pelota.yv==1){

	 if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=-1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x+1){
	  juego.arkanoPi.pelota.xv=1;
	  juego.arkanoPi.pelota.yv=-1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x+2){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=-1;
	 }
	}

	//La pelota llega recto descendente
	else if(juego.arkanoPi.pelota.xv==0 && juego.arkanoPi.pelota.yv==1){

	 if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x){
	  juego.arkanoPi.pelota.xv=-1;
	  juego.arkanoPi.pelota.yv=-1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x+1){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=-1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x+2){
	  juego.arkanoPi.pelota.xv=1;
	  juego.arkanoPi.pelota.yv=-1;
	 }
	}

	//La pelota llega en diagonal hacia la izquierda y descendente
	else if(juego.arkanoPi.pelota.xv==-1 && juego.arkanoPi.pelota.yv==1){

	 if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=-1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x+1){
	  juego.arkanoPi.pelota.xv=-1;
	  juego.arkanoPi.pelota.yv=-1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaInferior.x+2){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=-1;
	 }

        }
}

//void ReboteRaquetaSuperior (void)
void ReboteRaquetaSuperior (void) {

       softToneWrite(pinSonido, 500);
       tmr_startms(juego.TIMER_SONIDO,CLK_SONIDO);

       #ifdef __MODO_DEBUG_TERMINAL__ 
       printf("\n\n Vas a rebotar con raqueta superior! \n\n");
       #endif

       //La pelota llega en diagonal hacia la derecha y ascendente
       if(juego.arkanoPi.pelota.xv==1 && juego.arkanoPi.pelota.yv==-1){

	 if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x+1){
	  juego.arkanoPi.pelota.xv=1;
	  juego.arkanoPi.pelota.yv=1;
	 }

	 else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x+2){
	  juego.arkanoPi.pelota.xv=0;
	  juego.arkanoPi.pelota.yv=1;
	 }
	}

	//La pelota llega recto ascendente
	else if(juego.arkanoPi.pelota.xv==0 && juego.arkanoPi.pelota.yv==-1){

	  if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x){
	   juego.arkanoPi.pelota.xv=-1;
	   juego.arkanoPi.pelota.yv=1;
	  }

	  else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x+1){
	   juego.arkanoPi.pelota.xv=0;
	   juego.arkanoPi.pelota.yv=1;
	  }

	  else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x+2){
	   juego.arkanoPi.pelota.xv=1;
	   juego.arkanoPi.pelota.yv=1;
	  }
	}

	//La pelota llega en diagonal hacia la izquierda y ascendente
	else if(juego.arkanoPi.pelota.xv==-1 && juego.arkanoPi.pelota.yv==-1){

	  if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x){
	   juego.arkanoPi.pelota.xv=0;
	   juego.arkanoPi.pelota.yv=1;
	  }

	  else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x+1){
	   juego.arkanoPi.pelota.xv=-1;
	   juego.arkanoPi.pelota.yv=1;
	  }

	  else if(juego.arkanoPi.pelota.x==juego.arkanoPi.raquetaSuperior.x+2){
	   juego.arkanoPi.pelota.xv=0;
	   juego.arkanoPi.pelota.yv=1;
	  }
        }
}

//void RebotePared (void)
void RebotePared () {

	softToneWrite(pinSonido, 300);
	tmr_startms(juego.TIMER_SONIDO,CLK_SONIDO);
	
        #ifdef __MODO_DEBUG_TERMINAL__ 
	printf("\n\n Vas a rebotar con pared! \n\n");
	#endif
	
	juego.arkanoPi.pelota.xv=-(juego.arkanoPi.pelota.xv);
}

// MEJORA: void MuestraPuntuacion(...):
void MuestraPuntuacion(int puntuacionJugador1, int puntuacionJugador2) {

	if(puntuacionJugador1==0 && puntuacionJugador2==1){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &cero_uno);
	}

	else if(puntuacionJugador1==1 && puntuacionJugador2==0){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &uno_cero);
	}

	else if(puntuacionJugador1==1 && puntuacionJugador2==1){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &uno_uno);
	}

	else if(puntuacionJugador1==2 && puntuacionJugador2==1){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &dos_uno);
	}

	else if(puntuacionJugador1==1 && puntuacionJugador2==2){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &uno_dos);
	}

	else if(puntuacionJugador1==2 && puntuacionJugador2==0){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &dos_cero);
	}

	else if(puntuacionJugador1==0 && puntuacionJugador2==2){
	 PintaMensajeInicialPantalla (&(juego.arkanoPi.pantalla), &cero_dos);
	}
}



//------------------------------------------------------
// FUNCIONES DE MEJORA. ESTADO PARADA
//------------------------------------------------------

//MEJORA: Estado Parada
// void PararJuego (void): funci�n que para el juego y muestra
// en la pantalla y en la matriz de leds un STOP
void PararJuego (fsm_t* this) {

	piLock (FLAGS_KEY);
	flags &= ~FLAG_STOP;
        flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);

	PintaMensajeInicialPantalla(&(juego.arkanoPi.pantalla), &stop);
	
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
        PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
	piUnlock (STD_IO_BUFFER_KEY);
	#endif
}

//MEJORA: Estado Parada
// void ReanudarJuego(void): funci�n que reanuda el juego y muestra
// en la pantalla y en la matriz de leds todos los elementos (ladrillos, raqueta y pelota) de nuevo. Previamente muestra en la matriz de leds una flecha.
void ReanudarJuego (fsm_t* this) {

	piLock (FLAGS_KEY);
	flags &= ~FLAG_STOP;
	flags &= ~FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	
	PintaMensajeInicialPantalla(&(juego.arkanoPi.pantalla), &go);
        delay (1500);

	ActualizaPantalla(&(juego.arkanoPi));
	
	#ifdef __MODO_DEBUG_TERMINAL__ 
	piLock (STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal((&(juego.arkanoPi.pantalla)));
        piUnlock (STD_IO_BUFFER_KEY);
	#endif


}

//------------------------------------------------------
// FUNCIONES DE TEMPORIZACION/REFRESCO MATRIZ LEDS
//------------------------------------------------------

//Se encarga de "hacer la tabla de verdad" del codificador pues hay que tener en cuenta que s�lo tenemos 4 pines para columnas
//y tenemos 10 columnas, por eso utilizamos un deco! 

void excitarColumna (void) {

	if(columna==0){
         digitalWrite (pinColumna0, 0);
         digitalWrite (pinColumna1, 0);
         digitalWrite (pinColumna2, 0);
         digitalWrite (pinColumna3, 0);
	}

	if(columna==1){
	 digitalWrite (pinColumna0, 1);
	 digitalWrite (pinColumna1, 0);
	 digitalWrite (pinColumna2, 0);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==2){
         digitalWrite (pinColumna0, 0);
	 digitalWrite (pinColumna1, 1);
         digitalWrite (pinColumna2, 0);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==3){
	 digitalWrite (pinColumna0, 1);
	 digitalWrite (pinColumna1, 1);
	 digitalWrite (pinColumna2, 0);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==4){
	 digitalWrite (pinColumna0, 0);
	 digitalWrite (pinColumna1, 0);
	 digitalWrite (pinColumna2, 1);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==5){
	 digitalWrite (pinColumna0, 1);
	 digitalWrite (pinColumna1, 0);
	 digitalWrite (pinColumna2, 1);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==6){
	 digitalWrite (pinColumna0, 0);
	 digitalWrite (pinColumna1, 1);
	 digitalWrite (pinColumna2, 1);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==7){
	 digitalWrite (pinColumna0, 1);
	 digitalWrite (pinColumna1, 1);
	 digitalWrite (pinColumna2, 1);
	 digitalWrite (pinColumna3, 0);
	}

	if(columna==8){
	 digitalWrite (pinColumna0, 0);
         digitalWrite (pinColumna1, 0);
         digitalWrite (pinColumna2, 0);
	 digitalWrite (pinColumna3, 1);
	}

	if(columna==9){
         digitalWrite (pinColumna0, 1);
	 digitalWrite (pinColumna1, 0);
         digitalWrite (pinColumna2, 0);
	 digitalWrite (pinColumna3, 1);
	}
}

//Se encarga del refresco de la matriz de leds.
void timer_refresco_display_isr(union sigval value) {

	int fila;
        
	//En primer lugar, elegimos la columna a iluminar.
	excitarColumna();
		
	for(fila=0;fila<7;fila++) {
            
	 //Fijamos la columna y vamos recorriendo las 7 filas.
	 if(juego.arkanoPi.pantalla.matriz[columna][fila]==1){
	  //Hay que tener en cuenta que las filas, a diferencia de las columnas son activas a nivel bajo!
	  digitalWrite (matrizPinesFilas[fila], 0);
         }

	 else {
	  digitalWrite (matrizPinesFilas[fila], 1);
         }
        }

	//Actualizar la columna a iluminar
	if(columna==9){
	 columna=0;
	}
	else {
	 columna++;
	}

	//Empieza la cuenta de nuevo.
	tmr_startms(juego.TIMER1, CLK_DISPLAY);
}

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

// int systemSetup (void): procedimiento de configuracion del sistema.
// Realizar�, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librer�as (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones peri�dicas y sus correspondientes temporizadores,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
int system_setup (void) {
	int x = 0;

	piLock (STD_IO_BUFFER_KEY);

	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
        }


        #ifdef __MODO_DEBUG_TERMINAL__ 
	// Lanzamos thread para exploracion del teclado convencional del PC
	x = piThreadCreate (thread);

	if (x != 0) {
		printf ("it didn't start!!!\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
        }
        #endif
 
	piUnlock (STD_IO_BUFFER_KEY);

	return 1;
}

void fsm_setup(fsm_t* arkanoPi_fsm) {
	piLock (FLAGS_KEY);
	flags = 0;
	piUnlock (FLAGS_KEY);

	ReseteaJuego(arkanoPi_fsm);

}

//------------------------------------------------------
// PI_THREAD (thread_explora_teclado): Thread function for keystrokes detection and interpretation
//------------------------------------------------------
PI_THREAD (thread) {
	int teclaPulsada;

	while(1) {
		delay(2000); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock (STD_IO_BUFFER_KEY);

		if(kbhit()) {
			teclaPulsada = kbread();
			//printf("Tecla %c\n", teclaPulsada);

			switch(teclaPulsada) {
			 case 'i':
			  piLock (FLAGS_KEY);
			  flags |= FLAG_RAQUETA_IZQUIERDA_INFERIOR;
			  flags |= FLAG_TECLA;
		          piUnlock (FLAGS_KEY);
			 break;

			 case 'o':
			  piLock (FLAGS_KEY);
			  flags |= FLAG_RAQUETA_DERECHA_INFERIOR;
			  flags |= FLAG_TECLA;
		          piUnlock (FLAGS_KEY);
			break;

			case 'k':
                         piLock (FLAGS_KEY);
			 flags |= FLAG_RAQUETA_IZQUIERDA_SUPERIOR;
			 flags |= FLAG_TECLA;
			 piUnlock (FLAGS_KEY);
			break;

			case 'l':
			 piLock (FLAGS_KEY);
			 flags |= FLAG_RAQUETA_DERECHA_SUPERIOR;
			 flags |= FLAG_TECLA;
			 piUnlock (FLAGS_KEY);
			break;

			case 'p':
			 piLock (FLAGS_KEY);
                         flags |= FLAG_PELOTA;
                         flags |= FLAG_TECLA;
		         piUnlock (FLAGS_KEY);
			break;

			case 's':
			 piLock (FLAGS_KEY);
			 flags |= FLAG_STOP;
                         flags |= FLAG_TECLA;
			 piUnlock (FLAGS_KEY);
			break;

                        default:
			 printf("INVALID KEY!!!\n");
			break;
			}
		}

		piUnlock (STD_IO_BUFFER_KEY);
	}
}

int main () {
	unsigned int next;

	//Creamos el temporizador del refresco de la matriz de leds
	juego.TIMER1= tmr_new (timer_refresco_display_isr);
	tmr_startms(juego.TIMER1, CLK_DISPLAY);	

	wiringPiSetupGpio();
	//Ponemos los pines de las columnas y filas a OUTPUT
	pinMode (pinColumna0, OUTPUT);
	pinMode (pinColumna1, OUTPUT);
	pinMode (pinColumna2, OUTPUT);
	pinMode (pinColumna3, OUTPUT);
	pinMode (pinFila0, OUTPUT);
	pinMode (pinFila1, OUTPUT);
	pinMode (pinFila2, OUTPUT);
	pinMode (pinFila3, OUTPUT);
	pinMode (pinFila4, OUTPUT);
	pinMode (pinFila5, OUTPUT);
	pinMode (pinFila6, OUTPUT);

	//Ponemos los pines de los pulsadores a INPUT
	pinMode (pinPulsadorIzquierdoJugadorInferior, INPUT);
	pinMode (pinPulsadorIzquierdoJugadorSuperior, INPUT);
	pinMode (pinPulsadorDerechoJugadorInferior, INPUT);
	pinMode (pinPulsadorDerechoJugadorSuperior, INPUT);
	pinMode (pinPulsadorStop, INPUT);
		
	//Interrupciones externas. Si detecta un flanco de bajada en los pines de los pulsadores, lanza la funci�n indicada
	//para activar los flags correspondientes. (Los pulsadores son activos a nivel bajo!)
	wiringPiISR (pinPulsadorDerechoJugadorInferior, INT_EDGE_FALLING, activaFlagPulsadorDerechoInferior);
	wiringPiISR (pinPulsadorDerechoJugadorSuperior, INT_EDGE_FALLING, activaFlagPulsadorDerechoSuperior);
	wiringPiISR (pinPulsadorIzquierdoJugadorInferior, INT_EDGE_FALLING, activaFlagPulsadorIzquierdoInferior);
	wiringPiISR (pinPulsadorIzquierdoJugadorSuperior, INT_EDGE_FALLING, activaFlagPulsadorIzquierdoSuperior);
	wiringPiISR (pinPulsadorStop, INT_EDGE_FALLING, activaFlagPulsadorStop);
		
	//Ponemos el pin  de sonido a OUTPUT
	pinMode (pinSonido, OUTPUT);

        //Indicamos que pin vamos a utlizar para el sonido con PWM
	softToneCreate(pinSonido);

	// Maquina de estados: lista de transiciones
	// {EstadoOrigen,Funci�nDeEntrada,EstadoDestino,Funci�nDeSalida}
	fsm_trans_t tabla[] = {
	 { WAIT_START, compruebaTeclaPulsada,  WAIT_PUSH, InicializaJuego },
	 { WAIT_PUSH, compruebaTeclaPelota, WAIT_PUSH,  MovimientoPelota },
	 { WAIT_PUSH, compruebaTeclaRaquetaDerechaSuperior, WAIT_PUSH, MueveRaquetaDerechaSuperior },
	 { WAIT_PUSH, compruebaTeclaRaquetaIzquierdaSuperior, WAIT_PUSH, MueveRaquetaIzquierdaSuperior },
	 { WAIT_PUSH, compruebaTeclaRaquetaDerechaInferior, WAIT_PUSH, MueveRaquetaDerechaInferior },
	 { WAIT_PUSH, compruebaTeclaRaquetaIzquierdaInferior, WAIT_PUSH, MueveRaquetaIzquierdaInferior },
	 { WAIT_PUSH, compruebaFinalJuego, WAIT_END, FinalJuego },
	 { WAIT_END, compruebaTeclaPulsada, WAIT_START, ReseteaJuego },
	 { WAIT_PUSH, compruebaTeclaStop, WAIT_STOP, PararJuego },
     	 { WAIT_STOP, compruebaTeclaStop, WAIT_PUSH, ReanudarJuego },
         { -1, NULL, -1, NULL },
	};

	fsm_t* arkanoPi_fsm = fsm_new (WAIT_START, tabla, NULL);

	// Configuracion e inicializaci�n del sistema
	system_setup();
	fsm_setup (arkanoPi_fsm);
	tmr_startms(juego.TIMER1, CLK_DISPLAY);

	next = millis();
	while (1) {
	 fsm_fire (arkanoPi_fsm);
	 next += CLK_MS;
	 delay_until (next);
	}

	fsm_destroy (arkanoPi_fsm);
	tmr_destroy (juego.TIMER1);
	tmr_destroy (juego.TIMER2);
	tmr_destroy (juego.TIMER_SONIDO);

}

