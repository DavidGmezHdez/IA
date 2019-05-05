#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <cmath>
#include <queue>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

struct nodo{
  estado st;
	list<Action> secuencia;
  int coste;
	bool operator <(const nodo &n) const{
		if(coste == n.coste)
			return secuencia.size() > n.secuencia.size();
		else
			return coste > n.coste;
	}
};

struct nodo2{
  estado st;
	list<Action> secuencia;
  double heuristica;
  int coste;
  double prioridad;

	bool operator <(const nodo2 &n) const{
		if(prioridad == n.prioridad)
			return secuencia.size() > n.secuencia.size();
		else
			return prioridad > n.prioridad;
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
    bool posicion;

    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Reto(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_buscarPK(const int pos,list<Action> &plan);
    int coste(char c);
    double calculoDistancia(const estado  &actual, const estado &destino);
    double calculoPrioridad(nodo2 nodo);
    


    void PintarMapa(const Sensores &sensores);
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);


};

#endif
