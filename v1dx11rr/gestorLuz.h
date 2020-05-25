#ifndef ___GSLUX
#define ___GSLUX
#include <ctime>
#include <conio.h>
#include <vector>
#include <iostream>
struct vector3 {
	float x, y, z;

	vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;

	}

	vector3(float u) {
		this->x = u;
		this->y = u;
		this->z = u;
	}
	vector3() {}
};


struct ControlDiaNocheBuffer {
	D3DXVECTOR4 ambiental;
	D3DXVECTOR4 color;
	D3DXVECTOR4 dirLuz;
	D3DXVECTOR4 posCam;

};


struct FuenteDeLuz {
	vector3 ubicacion;
	vector3 dirLuz;
	float ambiental;
	vector3 color;
	
	
};

class GestorDeLuz {
	static GestorDeLuz* instancia;
	FuenteDeLuz datos;
	D3DXVECTOR3 posCam;
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
		 vuelta = false;
		datos.ambiental = 0.5f;
		datos.color = vector3(191.25f, 191.25f, 255.f);
		datos.dirLuz = vector3(-238.f, 5.18f, 243.88f);
	}

	

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

	D3DXVECTOR3 getPosCam() {
		return posCam;
	}


	FuenteDeLuz getDatos() {
		return datos;
	}

	void Update(D3DXVECTOR3 posCam) {
		float velocidad = 1000.f;
		tiempoActual = time(0);

		this->posCam = posCam;

		if (tiempoPrev  < tiempoActual) {
			tiempoPrev = tiempoActual;
			if (vuelta == false) {
				if (mañanaTarde < 1.f) {
					mañanaTarde += 0.01f;
					datos.ambiental += 5/velocidad;
					if(datos.color.x<255.f)
					datos.color.x += 0.6375f;
					if(datos.color.y<255.f)
					datos.color.y += 0.6375f;

					if(datos.dirLuz.x<0.f)
						datos.dirLuz.x += 2.38f;
					if(datos.dirLuz.y<90.f)
						datos.dirLuz.y += 0.9f;
					if(datos.dirLuz.z>0.f)
						datos.dirLuz.z -= 2.4388f;
					
					
					


				}
				else {
					if (tardeNoche < 1.f) {
						tardeNoche += 0.01f;
						datos.ambiental -= 2/velocidad;
					
						if (datos.color.y > 127.5f)
						datos.color.y -= 1.275f;
						if (datos.color.z > 127.5f)
						datos.color.z -= 1.275f;
						
						if (datos.dirLuz.x < 237.27f)
							datos.dirLuz.x += 2.3727f;
						if (datos.dirLuz.y > 5.11f)
							datos.dirLuz.y -= 0.85f;
						if (datos.dirLuz.z > -240.22f)
							datos.dirLuz.z -= 2.4022f;
						
				
					}
					else {
						vuelta = true;
						mañanaTarde = 0.f;

					}

				}
			}
			else {
				if (tardeNoche > 0.f) {
					tardeNoche -= 0.01f;
					datos.ambiental -= 3/velocidad;
					if (datos.color.x > 191.25f)
					datos.color.x -= 0.6375f;
					if (datos.color.y < 191.25f)
					datos.color.y += 0.6375;
					if (datos.color.z < 255.f)
					datos.color.z += 1.275;
					
					
				}
				else {
					vuelta = false;
					datos.dirLuz = 0.f;
				}


			}

		}

		
	}





};

GestorDeLuz* GestorDeLuz::instancia = nullptr;



class ComunicacionLuces {
protected:
	 std::unique_ptr<ControlDiaNocheBuffer> control;

	ID3D11Buffer* controlBufferCB;

	bool CreateLucesBuffer(ID3D11Device** d3dDevice) {


		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(ControlDiaNocheBuffer);
		constDesc.Usage = D3D11_USAGE_DEFAULT;

		HRESULT d3dResult = (*d3dDevice)->CreateBuffer(&constDesc, 0, &controlBufferCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		control.reset(new ControlDiaNocheBuffer());


	}


	
	void UpdateLuces(GestorDeLuz* gestor) {
		control->ambiental.x = gestor->getDatos().ambiental;
		control->ambiental.y = gestor->getDatos().ambiental;
		control->ambiental.z = gestor->getDatos().ambiental;
		control->ambiental.w = 1.f;

		control->color.x = gestor->getDatos().color.x / 255.f;
		control->color.y = gestor->getDatos().color.y / 255.f;
		control->color.z = gestor->getDatos().color.z / 255.f;



		control->color.w = 1.f;

		control->dirLuz.x = gestor->getDatos().dirLuz.x;
		control->dirLuz.y = gestor->getDatos().dirLuz.y;
		control->dirLuz.z = gestor->getDatos().dirLuz.z;
		control->dirLuz.w = 0.f;

		control->posCam.x = gestor->getPosCam().x;
		control->posCam.y = gestor->getPosCam().y;
		control->posCam.z = gestor->getPosCam().z;
		control->posCam.w = 0.f;
		
	}


};
#endif