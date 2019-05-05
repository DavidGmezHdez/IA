#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

using namespace std;


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	if(sensores.mensajeF != -1 && !posicion){
		fil = sensores.mensajeF;
		col = sensores.mensajeC;
		ultimaAccion = actIDLE;
		posicion = true;
		hayPlan = false;
	}


	if(sensores.destinoF != destino.fila or sensores.destinoC != destino.columna){
		destino.fila = sensores.destinoF;
		destino.columna = sensores.destinoC;
		hayPlan = false;
	}

	if(posicion){
		PintarMapa(sensores);
	}

	if(!hayPlan && posicion){
		actual.fila = fil;
		actual.columna = col;
		actual.orientacion = brujula;
		hayPlan = pathFinding(sensores.nivel,actual,destino,plan);
	}

	Action siguienteAccion;
	if(hayPlan and plan.size()>0){
		if((sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or sensores.terreno[2] =='D' or sensores.superficie[2] == 'a') && plan.front() == actFORWARD){
				hayPlan = false;
				posicion = true;
				siguienteAccion = actIDLE;
		}else{
			siguienteAccion = plan.front();
			plan.erase(plan.begin());
		}
	}
	else{
		plan.clear();
		bool pkencontrado = false;
		int pos;
		for(unsigned int i=0;i<sensores.terreno.size();i++){
				if(sensores.terreno[i] == 'K'){
					pkencontrado = true;
					pos = i;
				}
		}

		if(sensores.terreno[0] == 'K')
			posicion = true;
		else if(pkencontrado){
			pathFinding_buscarPK(pos,plan);
			hayPlan = true;
		}
		else if(sensores.terreno[2] == 'P' or sensores.terreno[2] =='M' or sensores.terreno[2] == 'D' or sensores.terreno[2] == 'a')
			siguienteAccion = actTURN_R;
		else
			siguienteAccion = actFORWARD;		
	}

	ultimaAccion = siguienteAccion;

	switch(ultimaAccion){
		case actTURN_R: brujula = (brujula+1)%4; break;
		case actTURN_L: brujula=(brujula+3)%4; break;
		case actFORWARD:
			switch(brujula){
				case 0: fil--;	break;
				case 1: col++;	break;
				case 2: fil++;	break;
				case 3: col--;	break;
			}
		break;
	}

	
	return siguienteAccion;

}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
			      return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
			      return pathFinding_Anchura(origen,destino,plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
						//return pathFinding_Reto(origen,destino,plan);
						return pathFinding_CostoUniforme(origen,destino,plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
						//return  pathFinding_Reto(origen,destino,plan);
						return pathFinding_Reto(origen,destino,plan);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}




struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

 	nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}




bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan){
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;											// Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.secuencia.empty();

	cola.push(current);

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.front();
		}
	}

 	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}


int ComportamientoJugador::coste(char c){
	int coste;
	if(c == 'S')
		coste = 1;
	if(c == 'T')
		coste = 2;
	if(c =='B')
		coste = 5;
	if(c =='A')
		coste = 10;
	
	return coste;
}




bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan){
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodo> cola;			// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();
	

	cola.push(current);
	

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.coste += 1;
			cola.push(hijoTurnR); 
		}


		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.coste += 1;
			cola.push(hijoTurnL); 
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				hijoForward.coste += coste(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
				cola.push(hijoForward); 
			}	
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.top();
		}
	}

 	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;

}


void ComportamientoJugador::PintarMapa(const Sensores &sensores){
		mapaResultado[fil][col] = sensores.terreno[0];

		if(brujula == 0){
			mapaResultado[fil-1][col-1] = sensores.terreno[1];
			mapaResultado[fil-1][col] = sensores.terreno[2];
			mapaResultado[fil-1][col+1] = sensores.terreno[3];
			mapaResultado[fil-2][col-2] = sensores.terreno[4];
			mapaResultado[fil-2][col-1] = sensores.terreno[5];
			mapaResultado[fil-2][col] = sensores.terreno[6];
			mapaResultado[fil-2][col+1] = sensores.terreno[7];
			mapaResultado[fil-2][col+2] = sensores.terreno[8];
			mapaResultado[fil-3][col-3] = sensores.terreno[9];
			mapaResultado[fil-3][col-2] = sensores.terreno[10];
			mapaResultado[fil-3][col-1] = sensores.terreno[11];
			mapaResultado[fil-3][col] = sensores.terreno[12];
			mapaResultado[fil-3][col+1] = sensores.terreno[13];
			mapaResultado[fil-3][col+2] = sensores.terreno[14];
			mapaResultado[fil-3][col+3] = sensores.terreno[15];
		}
		
		if(brujula == 1){
			mapaResultado[fil-1][col+1] = sensores.terreno[1];
			mapaResultado[fil][col+1] = sensores.terreno[2];
			mapaResultado[fil+1][col+1] = sensores.terreno[3];
			mapaResultado[fil-2][col+2] = sensores.terreno[4];
			mapaResultado[fil-1][col+2] = sensores.terreno[5];
			mapaResultado[fil][col+2] = sensores.terreno[6];
			mapaResultado[fil+1][col+2] = sensores.terreno[7];
			mapaResultado[fil+2][col+2] = sensores.terreno[8];
			mapaResultado[fil-3][col+3] = sensores.terreno[9];
			mapaResultado[fil-2][col+3] = sensores.terreno[10];
			mapaResultado[fil-1][col+3] = sensores.terreno[11];
			mapaResultado[fil][col+3] = sensores.terreno[12];
			mapaResultado[fil+1][col+3] = sensores.terreno[13];
			mapaResultado[fil+2][col+3] = sensores.terreno[14];
			mapaResultado[fil+3][col+3] = sensores.terreno[15];
		}
		
		if(brujula == 2){
			mapaResultado[fil+1][col+1] = sensores.terreno[1];
			mapaResultado[fil+1][col] = sensores.terreno[2];
			mapaResultado[fil+1][col-1] = sensores.terreno[3];
			mapaResultado[fil+2][col+2] = sensores.terreno[4];
			mapaResultado[fil+2][col+1] = sensores.terreno[5];
			mapaResultado[fil+2][col] = sensores.terreno[6];
			mapaResultado[fil+2][col-1] = sensores.terreno[7];
			mapaResultado[fil+2][col-2] = sensores.terreno[8];
			mapaResultado[fil+3][col+3] = sensores.terreno[9];
			mapaResultado[fil+3][col+2] = sensores.terreno[10];
			mapaResultado[fil+3][col+1] = sensores.terreno[11];
			mapaResultado[fil+3][col] = sensores.terreno[12];
			mapaResultado[fil+3][col-1] = sensores.terreno[13];
			mapaResultado[fil+3][col-2] = sensores.terreno[14];
			mapaResultado[fil+3][col-3] = sensores.terreno[15];
		}
		
		if(brujula == 3){
			mapaResultado[fil+1][col-1] = sensores.terreno[1];
			mapaResultado[fil][col-1] = sensores.terreno[2];
			mapaResultado[fil-1][col-1] = sensores.terreno[3];
			mapaResultado[fil+2][col-2] = sensores.terreno[4];
			mapaResultado[fil+1][col-2] = sensores.terreno[5];
			mapaResultado[fil][col-2] = sensores.terreno[6];
			mapaResultado[fil-1][col-2] = sensores.terreno[7];
			mapaResultado[fil-2][col-2] = sensores.terreno[8];
			mapaResultado[fil+3][col-3] = sensores.terreno[9];
			mapaResultado[fil+2][col-3] = sensores.terreno[10];
			mapaResultado[fil+1][col-3] = sensores.terreno[11];
			mapaResultado[fil][col-3] = sensores.terreno[12];
			mapaResultado[fil-1][col-3] = sensores.terreno[13];
			mapaResultado[fil-2][col-3] = sensores.terreno[14];
			mapaResultado[fil-3][col-3] = sensores.terreno[15];
		}
}

double ComportamientoJugador::calculoDistancia(const estado &actual, const estado &destino){
    double resultado = sqrt(pow((destino.fila - actual.fila),2) + pow((destino.columna - actual.columna),2));
    return resultado;
}

double ComportamientoJugador::calculoPrioridad(const nodo2 nodo){
		return nodo.coste + nodo.heuristica;
}

bool ComportamientoJugador::pathFinding_Reto(const estado &origen, const estado &destino, list<Action> &plan){
cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodo2> cola;			// Lista de Abiertos

  nodo2 current;
	current.st = origen;
	current.secuencia.empty();
	current.coste = coste(mapaResultado[current.st.fila][current.st.columna]);
	current.heuristica = calculoDistancia(current.st,destino);
	

	cola.push(current);
	

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo2 hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.coste += 1;
			hijoTurnR.heuristica = calculoDistancia(hijoTurnR.st,destino);
			hijoTurnR.prioridad = calculoPrioridad(hijoTurnR);
			cola.push(hijoTurnR);
		}


		// Generar descendiente de girar a la izquierda
		nodo2 hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.coste += 1;
			hijoTurnL.heuristica = calculoDistancia(hijoTurnL.st,destino);
			hijoTurnL.prioridad = calculoPrioridad(hijoTurnL);
			cola.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo2 hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				hijoForward.coste += coste(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
				hijoForward.heuristica = calculoDistancia(hijoForward.st,destino);
				hijoForward.prioridad = calculoPrioridad(hijoForward);
				cola.push(hijoForward);
			}
		}
		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.top();
		}
	}

 	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
	
}

bool ComportamientoJugador::pathFinding_buscarPK(const int pos,list<Action> &plan){
	switch(pos){
				case 1:
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				break;

				case 2:
				plan.push_back(actFORWARD);
				break;

				case 3:
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_R);
				plan.push_back(actFORWARD);
				break;

				case 4:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;
			
				case 5:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				break;

				case 6:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;

				case 7:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				break;

				case 8:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;


				case 9:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;

				case 10:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;

				case 11:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_L);
				plan.push_back(actFORWARD);
				break;

				case 12:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;

				case 13:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_R);
				plan.push_back(actFORWARD);
				break;

				case 14:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_R);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;

				case 15:
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actTURN_R);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				plan.push_back(actFORWARD);
				break;
				}
}


// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
