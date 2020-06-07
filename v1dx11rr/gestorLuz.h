#ifndef ___GSLUX
#define ___GSLUX
#include <ctime>
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


struct vector2 {
	float u, v;
};

struct ControlDiaNocheBuffer {
	D3DXVECTOR4 ambiental;
	D3DXVECTOR4 color;
	D3DXVECTOR4 dirLuz;
	D3DXVECTOR4 posCam;

};

struct LuzAmbiental {
	D3DXVECTOR4 luz;
	D3DXVECTOR3 color;
	float atenuador;

};


struct LuzDifusa {
	D3DXVECTOR4 luz;
	D3DXVECTOR4 ubicacionLuz;
	D3DXVECTOR3 ubicacionCamara;
	float atenuador;

};

struct FuenteDeLuz {
	vector3 ubicacion;
	vector3 dirLuz;
	float ambiental;
	vector3 color;
	float difusa;
	float amanecer = 0.5f;
	float atardecer = 0.f;
	float anochecer = 0.f;
	
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
	bool texturasSkydome;


	GestorDeLuz() {
		mañanaTarde = 0;
		tardeNoche = 0;
		tiempoPrev = time(0);
		tiempoActual = time(0);
		 vuelta = false;
		 texturasSkydome = false;
		datos.ambiental = 0.5f;
		datos.difusa = 0.4f;
		datos.color = vector3(204.f, 204.f, 255.f);
		datos.dirLuz = vector3(-300, 0.f, 300.f);
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


	D3DXVECTOR4 getSkydomeParam() {
		D3DXVECTOR4 respuesta;
		
		respuesta.x = datos.amanecer;
		respuesta.y = datos.atardecer;
		respuesta.z = datos.anochecer;

		if (!texturasSkydome)
			respuesta.w = 0.f;
		else
			respuesta.w = 1.f;


		return respuesta;
	}




	void Update(D3DXVECTOR3 posCam) {
		
		tiempoActual = time(0);

		this->posCam = posCam;

		if (tiempoPrev +2< tiempoActual) {
			tiempoPrev = tiempoActual;
			if (vuelta == false) {


				if (mañanaTarde < 1.f) {
					mañanaTarde += 0.01f;
					datos.ambiental += 5/1000.f;
					if(datos.color.x<255.f)
					datos.color.x += 0.51f;
					if(datos.color.y<255.f)
					datos.color.y += 0.51f;

					if(datos.dirLuz.x<0.f)
						datos.dirLuz.x += 3.f;
					if(datos.dirLuz.y<90.f)
						datos.dirLuz.y += 0.9f;
					if(datos.dirLuz.z>0.f)
						datos.dirLuz.z -= 3.f;
					
					datos.difusa += 0.006f;
					datos.amanecer += 0.005f;

				

				}
				else {
					if (tardeNoche < 1.f) 
			{
						texturasSkydome = true;
						tardeNoche += 0.01f;
						datos.ambiental -= 2/1000.f;
					
						if (datos.color.y > 204.f)
						datos.color.y -= 0.51f;
						if (datos.color.z > 204.f)
						datos.color.z -= 0.51f;
						
						if (datos.dirLuz.x <300.f)
							datos.dirLuz.x += 3.f;
						if (datos.dirLuz.y > 0.f)
							datos.dirLuz.y -= 0.9f;
						if (datos.dirLuz.z > -300.f)
							datos.dirLuz.z -= 3.f;
						
						datos.difusa -= 0.009f;
						if (datos.atardecer < 1.f)
						{
							datos.atardecer += 0.02f;
						}
						else
						{
							datos.anochecer += 0.02f;
						}

					}
					else {
						vuelta = true;
						mañanaTarde = 0.f;
						datos.difusa = 0.f;
						texturasSkydome = false;
						datos.amanecer = 0.f;
						datos.atardecer = 0.f;
						datos.anochecer = 0.f;
					}

				}
			}
			else {
				if (tardeNoche > 0.f) {
					tardeNoche -= 0.01f;
					datos.ambiental -= 3/1000.f;
					if (datos.color.x >204.f)
					datos.color.x -= 0.51f;
					if (datos.color.z < 255.f)
					datos.color.z +=0.51;
					datos.amanecer += 0.005f;
					
				}
				else {
					vuelta = false;
					datos.dirLuz.x = -300.f;
					datos.dirLuz.y = 0.f;
					datos.dirLuz.z = 300.f;  
					datos.difusa = 0.004;//VUELVE A SER SOL
					
				}


			}

		}

		
	}





};

GestorDeLuz* GestorDeLuz::instancia = nullptr;



class ComunicacionLuces {
protected:
	 
	 LuzAmbiental luzAmbiental;
	 LuzDifusa luzDifusa;

	ID3D11Buffer* luzAmbientalCB;
	ID3D11Buffer* luzDifusaCB;

	

	bool CreateLuzAmbientalBuffer(ID3D11Device** d3dDevice) {


		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(LuzAmbiental);
		constDesc.Usage = D3D11_USAGE_DEFAULT;

		HRESULT d3dResult = (*d3dDevice)->CreateBuffer(&constDesc, 0, &luzAmbientalCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		


	}

	bool CreateLuzDifusaBuffer(ID3D11Device** d3dDevice) {


		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(LuzDifusa);
		constDesc.Usage = D3D11_USAGE_DEFAULT;

		HRESULT d3dResult = (*d3dDevice)->CreateBuffer(&constDesc, 0, &luzDifusaCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		


	}


	


	
	void UpdateLuzAmbiental(GestorDeLuz* gestor) {
		luzAmbiental.luz.x = gestor->getDatos().ambiental;
		luzAmbiental.luz.y = gestor->getDatos().ambiental;
		luzAmbiental.luz.z = gestor->getDatos().ambiental;
		luzAmbiental.luz.w = 1.f;

		luzAmbiental.color.x = gestor->getDatos().color.x / 255.f;
		luzAmbiental.color.y = gestor->getDatos().color.y / 255.f;
		luzAmbiental.color.z = gestor->getDatos().color.z / 255.f;

		luzAmbiental.atenuador = 0.6f;
		}


	void UpdateLuzDifusa(GestorDeLuz* gestor) {

		luzDifusa.luz.x = gestor->getDatos().difusa;
		luzDifusa.luz.y = gestor->getDatos().difusa;
		luzDifusa.luz.z = gestor->getDatos().difusa;
		luzDifusa.luz.w = 1.f;

		luzDifusa.ubicacionLuz.x = gestor->getDatos().dirLuz.x;
		luzDifusa.ubicacionLuz.y = gestor->getDatos().dirLuz.y;
		luzDifusa.ubicacionLuz.z = gestor->getDatos().dirLuz.z;
		luzDifusa.ubicacionLuz.w = 0.f;

		luzDifusa.ubicacionCamara.x = gestor->getPosCam().x;
		luzDifusa.ubicacionCamara.y = gestor->getPosCam().y;
		luzDifusa.ubicacionCamara.z = gestor->getPosCam().z;
		
		luzDifusa.atenuador = 0.5f;
	}

};
#endif