#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

struct nodo{
	estado st;
	list<Action> secuencia;
};

struct comparacion{
  public:
	bool operator()(nodo n1,nodo n2,vector<vector<unsigned char>>mapaResultado){
		bool resultado;
		char nodo1 = mapaResultado[n1.st.fila][n1.st.columna];
		char nodo2 = mapaResultado[n2.st.fila][n2.st.columna];
		int total1=0;
		int total2=0;
      if(nodo1 == 'S')
        total1 = 1;
      else if(nodo1 == 'T')
        total1 = 2;
      else if(nodo1 == 'B')
        total1 = 5;
      else if(nodo1 == 'A')
        total1 = 10;

      if(nodo2 == 'S')
        total2 = 1;
      else if(nodo2 == 'T')
        total2 = 2;
      else if(nodo2 == 'B')
        total2 = 5;
      else if(nodo2 == 'A')
        total2 = 10;

		if(total1 < total2)
			resultado = true;
		else
			resultado = false;

	return resultado;
	}
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      hayPlan = false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      hayPlan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}
  


  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino;
    list<Action> plan;


    // Métodos privados de la clase
    Action ultimaAccion;
    bool hayPlan;
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);
    int coste(const nodo n1);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);


};

#endif
