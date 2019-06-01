#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar

double contadorFichas(const Environment &E,int jugador){
   int enemigo, bomba, bomba_enemigo;
   int contiguas = 0, contiguas_rival = 0;
   double valor = 0;

      if(jugador==1){
         bomba = 4;
         enemigo = 2;
         bomba_enemigo = 5;
      }
      else{
         bomba = 5;
         enemigo = 1;
         bomba_enemigo = 4;
      }

   // Contamos las fichas por filas
   /*
   - - - - - - -
   - - - - - - -
   - - - - - - -
   - - - - - - -
   - - - - - - -
   - - - - - - -
   - - - - - - - 
   */
   for(int i=0;i<7;i++){
      for(int j=0;j<7;j++){
         if(E.See_Casilla(i,j)==jugador  || E.See_Casilla(i,j) == bomba)
            contiguas++;
            valor-=contiguas;
            contiguas_rival = 0;
         if(E.See_Casilla(i,j)==enemigo || E.See_Casilla(i,j) == bomba_enemigo){
            contiguas_rival++;
            valor+=contiguas_rival;
            contiguas = 0;
         }
      }
   }

   //Contamos las filas por columnas
   /*
   | | | | | | |
   | | | | | | |
   | | | | | | |
   | | | | | | |
   | | | | | | |
   | | | | | | |
   | | | | | | |
   */
   for(int i=0;i<7;i++){
      for(int j=0;j<7;j++){
         if(E.See_Casilla(j,i)==jugador || E.See_Casilla(j,i) == bomba)
            contiguas++;
            valor-=contiguas;
            contiguas_rival = 0;
         if(E.See_Casilla(j,i)==enemigo || E.See_Casilla(j,i) == bomba_enemigo){
            contiguas_rival++;
            valor+=contiguas_rival;
            contiguas = 0;
         }
      }
   }
   
   //Contamos desde derecha abajo hasta izquierda arriba
   /*
   \ \ \ \ \ \ \
   \ \ \ \ \ \ \
   \ \ \ \ \ \ \
   \ \ \ \ \ \ \
   \ \ \ \ \ \ \
   \ \ \ \ \ \ \
   \ \ \ \ \ \ \
   */
   for(int i = 0; i < 4; i++) {
      for(int j = 3; j < 7; j++) {
         for(int k = 0; k < 4; k++) {
            if(i+k==7 || j-k==7) break;
            if(E.See_Casilla(i+k,j-k)==jugador || E.See_Casilla(i+k,j-k) == bomba)
               contiguas++;
               valor-=contiguas;
               contiguas_rival = 0;
            if(E.See_Casilla(i+k,j-k)==enemigo || E.See_Casilla(i+k,j-k) == bomba_enemigo){
               contiguas_rival++;
               valor+=contiguas_rival;
               contiguas = 0;
            }
         }
      }
   }

   //Contamos desde derecha arriba hasta izquierda abajo
   /*
   / / / / / / /
   / / / / / / /
   / / / / / / /
   / / / / / / /
   / / / / / / /
   / / / / / / /
   / / / / / / /
   */
   for(int i = 0; i < 4; i++) {
      for(int j = 3; j < 7; j++) {
         for(int k = 0; k < 4; k++) {
            if(i+k==7 || j+k==7) break;
            if(E.See_Casilla(j+k,i+k)==jugador || E.See_Casilla(j+k,i+k) == bomba)
               contiguas++;
               valor-=contiguas;
               contiguas_rival = 0;
            if(E.See_Casilla(j+k,i+k)==enemigo || E.See_Casilla(j+k,i+k) == bomba_enemigo){
               contiguas_rival++;
               valor+=contiguas_rival;
               contiguas = 0;
            }
         }
      }
   }
   return valor;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador){
   int rival;
   int ganador = estado.RevisarTablero();
   double puntuacion = 0;

   if(jugador == 1)
      rival = 2;
   else
      rival = 1;

   if (ganador==jugador)
      return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
      return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
      return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else{
      contadorFichas(estado,jugador);
    }
}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}



double PodaAlfaBeta(const Environment &E, int jugador, int profundidad, 
 int &accion, double alfa, double beta){
   
   if(E.JuegoTerminado())
      return ValoracionTest(E,jugador);
   else if(profundidad == 0)
      return Valoracion(E,jugador);


   if(profundidad%2==0){
      int ultima_accion = -1;
      int acc;
      double valor;
      Environment nodoHijo = E.GenerateNextMove(ultima_accion);

      while(ultima_accion<8){
         valor = PodaAlfaBeta(nodoHijo,jugador,profundidad-1,acc,alfa,beta);
         if(valor>alfa){
            alfa = valor;
            accion = static_cast<Environment::ActionType>(ultima_accion);
         }
         
         if(beta<=alfa)
            return beta;
         
         nodoHijo = E.GenerateNextMove(ultima_accion);
      }
      return alfa;
   }
   else{
      int ultima_accion = -1;
      int acc;
      double valor;
      Environment nodoHijo;
      while(ultima_accion<8){
         valor = PodaAlfaBeta(nodoHijo,jugador,profundidad-1,acc,alfa,beta);
         if(valor<beta){
            beta = valor;
            accion = static_cast<Environment::ActionType>(ultima_accion);
      }
         
         if(beta<=alfa)
            break;
         
         nodoHijo = E.GenerateNextMove(ultima_accion);
      }
      return beta;
   }   
}




// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;
/*
    //--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui
*/

    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    //  Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    alpha = menosinf;
    beta = masinf;
    int acc = -1;
    valor = PodaAlfaBeta(actual_, jugador_, PROFUNDIDAD_ALFABETA, acc, alpha, beta);
    accion = static_cast< Environment::ActionType > (acc);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}
