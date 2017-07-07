#include "arkanoPiLib.h"

int ladrillos_basico[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
};

tipo_pantalla bienvenida={
     	       {{0,0,0,0,0,0,0},
		{0,1,1,1,1,1,0},
		{0,0,0,1,0,0,0},
		{0,0,0,1,0,0,0},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,1,0,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,1,1,1,0,1,0},
		{0,0,0,0,0,0,0}}
};

//MEJORA: Estado Parada. (Pantalla STOP)
tipo_pantalla stop={
     	       {{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,1,1,1,1,1,0},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,1,1,1,1,1,0},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0}}
};

//MEJORA: Estado Parada (Pantalla PLAY/FLECHA)
tipo_pantalla go={
     	        {{0,0,1,1,1,0,0},
		{0,0,1,1,1,0,0},
		{0,0,1,1,1,0,0},
		{0,0,1,1,1,0,0},
		{0,0,1,1,1,0,0},
		{0,0,1,1,1,0,0},
		{1,1,1,1,1,1,1},
		{0,1,1,1,1,1,0},
		{0,0,1,1,1,0,0},
		{0,0,0,1,0,0,0}}
};




tipo_pantalla cero_uno = {
     	       {{0,1,1,1,1,1,0},
		{1,0,0,0,0,0,1},
		{1,0,0,0,0,0,1},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,0,1,0,0,0,1},
		{0,1,0,0,0,0,1},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1},
		{0,0,0,0,0,0,1}}
};

tipo_pantalla uno_cero = {
     	       {{0,0,1,0,0,0,1},
		{0,1,0,0,0,0,1},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,1,1,1,1,1,0},
		{1,0,0,0,0,0,1},
		{1,0,0,0,0,0,1},
		{0,1,1,1,1,1,0},}
};

tipo_pantalla uno_uno = {
     	       {{0,0,1,0,0,0,1},
		{0,1,0,0,0,0,1},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,1,0,0,0,1},
		{0,1,0,0,0,0,1},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1}}
};

tipo_pantalla uno_dos = {
     	       {{0,0,1,0,0,0,1},
		{0,1,0,0,0,0,1},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0},
		{0,1,0,0,0,0,1},
		{1,0,0,0,0,1,1},
		{1,0,0,0,1,0,1},
		{1,0,0,1,0,0,1},
		{0,1,1,0,0,0,1}}
};


tipo_pantalla dos_uno = {
     	       {{0,1,0,0,0,0,1},
		{1,0,0,0,0,1,1},
		{1,0,0,0,1,0,1},
		{1,0,0,1,0,0,1},
		{0,1,1,0,0,0,1},
		{0,0,1,0,0,0,1},
		{0,1,0,0,0,0,1},
		{1,1,1,1,1,1,1},
		{0,0,0,0,0,0,1},
		{0,0,0,0,0,0,1}}
};

tipo_pantalla dos_cero = {
     	       {{0,1,0,0,0,0,1},
		{1,0,0,0,0,1,1},
		{1,0,0,0,1,0,1},
		{1,0,0,1,0,0,1},
		{0,1,1,0,0,0,1},
		{0,0,0,0,0,0,0},
		{0,1,1,1,1,1,0},
		{1,0,0,0,0,0,1},
		{1,0,0,0,0,0,1},
		{0,1,1,1,1,1,0}}
};


tipo_pantalla cero_dos = {
     	       {{0,1,1,1,1,1,0},
		{1,0,0,0,0,0,1},
		{1,0,0,0,0,0,1},
		{0,1,1,1,1,1,0},
		{0,0,0,0,0,0,0},
		{0,1,0,0,0,0,1},
		{1,0,0,0,0,1,1},
		{1,0,0,0,1,0,1},
		{1,0,0,1,0,0,1},
		{0,1,1,0,0,0,1}}
};

//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------
void ReseteaMatriz(tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}

void ReseteaLadrillos(tipo_pantalla *p_ladrillos) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2 - 1;

	p_pelota->y = MATRIZ_ALTO/2 - 1;

	// Trayectoria inicial
	p_pelota->yv = 1;
	//p_pelota->xv = 0;
	srand(time(NULL));
	p_pelota->xv = rand()%3 -1; //Trayectoria inicial aleatoria!!!!!
	printf("Trayectoria inicial (xv, xy): %d %d \n", p_pelota->xv, p_pelota->yv);
}

void ReseteaRaquetaInferior(tipo_raqueta *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

void ReseteaRaquetaSuperior(tipo_raqueta *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2;
	p_raqueta->y = 0;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

// void PintaMensajeInicialPantalla (...): metodo encargado de aprovechar
// el display para presentar un mensaje de bienvenida al usuario
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	// A completar por el alumno... OKK
	int filas, columnas = 0;


	for(columnas=0;columnas<MATRIZ_ANCHO;columnas++) {
		for(filas=0;filas<MATRIZ_ALTO;filas++) {
			p_pantalla->matriz[columnas][filas] = p_pantalla_inicial ->matriz[columnas][filas];
		}
	}
}

// void PintaPantallaPorTerminal (...): metodo encargado de mostrar
// el contenido o la ocupaci�n de la matriz de leds en la ventana de
// terminal o consola. Este m�todo sera fundamental para facilitar
// la labor de depuraci�n de errores (por ejemplo, en la programaci�n
// de los diferentes movimientos tanto de la raqueta como de la pelota).
void PintaPantallaPorTerminal  (tipo_pantalla *p_pantalla) {
	// A completar por el alumno... OKK

	int filas,columnas=0;
	for(filas=0;filas<MATRIZ_ALTO;filas++) {
		for(columnas=0;columnas<MATRIZ_ANCHO;columnas++) {
			int elemento = p_pantalla-> matriz[columnas][filas];

			printf("%d",  elemento);
		}
		printf("\n");
	}	
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n\n -------------------------------------------\n\n");
	printf("\n");
	printf("\n");
	printf("\n");
}

// void PintaLadrillos(...): funcion encargada de pintar los ladrillos
// en sus correspondientes posiciones dentro del �rea de juego
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

// void PintaRaqueta(...): funcion encargada de pintar la raqueta
// en su posicion correspondiente dentro del �rea de juego
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<RAQUETA_ANCHO;i++) {
		for(j=0;j<RAQUETA_ALTO;j++) {
			if (( (p_raqueta->x+i >= 0) && (p_raqueta->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta->y+j >= 0) && (p_raqueta->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta->x+i][p_raqueta->y+j] = 1;
		}
	}
}

// void PintaPelota(...): funcion encargada de pintar la pelota
// en su posicion correspondiente dentro del �rea de juego
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

// void ActualizaPantalla(...): metodo cuya ejecucion estara ligada a
// cualquiera de los movimientos de la raqueta o de la pelota y que
// sera el encargado de actualizar convenientemente la estructura de datos
// en memoria que representa el �rea de juego y su correspondiente estado.
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
    // Borro toda la pantalla
    ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

    // A completar por el alumno... OKK

    PintaLadrillos(&(p_arkanoPi -> ladrillos),&(p_arkanoPi->pantalla));
    PintaPelota(&(p_arkanoPi->pelota), &(p_arkanoPi->pantalla));
    PintaRaqueta(&(p_arkanoPi->raquetaInferior), &(p_arkanoPi->pantalla));
    PintaRaqueta(&(p_arkanoPi->raquetaSuperior), &(p_arkanoPi->pantalla));
}

// void InicializaArkanoPi(...): metodo encargado de la inicializaci�n
// de toda variable o estructura de datos especificamente ligada al
// desarrollo del juego y su visualizacion.
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
    // A completar por el alumno... OKK

    ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

    ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
    ReseteaRaquetaInferior((tipo_raqueta*)(&(p_arkanoPi->raquetaInferior)));
    ReseteaRaquetaSuperior((tipo_raqueta*)(&(p_arkanoPi->raquetaSuperior)));

    ActualizaPantalla(p_arkanoPi);
}








