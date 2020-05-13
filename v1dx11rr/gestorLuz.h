#ifndef ___GSLUX
#define ___GSLUX
#include "Modelo.h"

struct FuenteDeLuz {
	vector3 ubicacion;
	vector3 difuso;
	vector3 ambiental;
	vector3 color;

	
};

class GestorDeLuz {
	static GestorDeLuz* instancia;
	FuenteDeLuz datos;
	
	float ma�anaTarde;
	float tardeNoche;
	time_t tiempoActual;
	time_t tiempoPrev;
	bool vuelta;

	GestorDeLuz() {
		ma�anaTarde = 0;
		tardeNoche = 0;
		tiempoPrev = time(0);
		tiempoActual = time(0);
		 vuelta = false;
		datos.ambiental = vector3(0.5f);
		
		
	}

	/*GestorDeLuz(GestorDeLuz *g) {
		ma�anaTarde = 0;
		tardeNoche = 0;
		tiempoPrev = time(0);
		tiempoActual = time(0);
		bool vuelta = false;
	}*/

	~GestorDeLuz() {
		release();
	}
public:

	static GestorDeLuz* getInstancia() {
		if (instancia == nullptr) {
			instancia = new GestorDeLuz();
		}
	
		return instancia;
	}


	void release() {
		if (instancia != nullptr) {
			delete instancia;
		}
		instancia == nullptr;
	}




	FuenteDeLuz getDatos() {
		return datos;
	}

	void Update() {
		float velocidad = 1000.f;
		tiempoActual = time(0);

		if (tiempoPrev + 1 < tiempoActual) {
			tiempoPrev = tiempoActual;
			if (vuelta == false) {
				if (ma�anaTarde < 1.f) {
					ma�anaTarde += 0.01f;
					datos.ambiental.x += 5/velocidad;
					datos.ambiental.y += 5/velocidad;
					datos.ambiental.z += 5/velocidad;

				}
				else {
					if (tardeNoche < 1.f) {
						tardeNoche += 0.01f;
						datos.ambiental.x -= 8/velocidad;
						datos.ambiental.y -= 8/velocidad;
						datos.ambiental.z -= 8/velocidad;
					}
					else {
						vuelta = true;
						ma�anaTarde = 0.0f;

					}

				}
			}
			else {
				if (tardeNoche > 0.f) {
					tardeNoche -= 0.01f;
					datos.ambiental.x += 2/velocidad;
					datos.ambiental.y += 2/velocidad;
					datos.ambiental.z += 2/velocidad;
					
				}
				else {
					vuelta = false;
				}


			}

		}

		
	}





};

GestorDeLuz* GestorDeLuz::instancia = nullptr;

#endif