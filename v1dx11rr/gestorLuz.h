#ifndef ___GSLUX
#define ___GSLUX


struct FuenteDeLuz {
	vector3 ubicacion;
	vector3 difuso;
	vector3 ambiental;
	vector3 color;

	
};

class GestorDeLuz {
	static GestorDeLuz* instancia;
	FuenteDeLuz datos;
	
	float mañanaTarde;
	float tardeNoche;
	time_t tiempoActual;
	time_t tiempoPrev;
	bool vuelta;

	GestorDeLuz() {
		mañanaTarde = 0;
		tardeNoche = 0;
		tiempoPrev = time(0);
		tiempoActual = time(0);
		bool vuelta = false;
		datos.ambiental = vector3(0.5f);
		
		
	}

	/*GestorDeLuz(GestorDeLuz *g) {
		mañanaTarde = 0;
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

		tiempoActual = time(0);

		if (tiempoPrev + 1 < tiempoActual) {
			tiempoPrev = tiempoActual;
			if (vuelta == false) {
				if (mañanaTarde < 1.f) {
					mañanaTarde += 0.01f;
					datos.ambiental.x += 0.005;
					datos.ambiental.y += 0.005;
					datos.ambiental.z += 0.005;

				}
				else {
					if (tardeNoche < 1.f) {
						tardeNoche += 0.01f;
						datos.ambiental.x -= 0.008;
						datos.ambiental.y -= 0.008;
						datos.ambiental.z -= 0.008;
					}
					else {
						vuelta = true;
						mañanaTarde = 0.0f;

					}

				}
			}
			else {
				if (tardeNoche > 0.f) {
					tardeNoche -= 0.01f;
					datos.ambiental.x += 0.002;
					datos.ambiental.y += 0.002;
					datos.ambiental.z += 0.002;
					
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