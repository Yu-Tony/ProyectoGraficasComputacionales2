#ifndef __gruppmod
#define __gruppmod
#include "Modelo.h"


struct Material {
	char nombre[255];     //material name
	float expEspecular; 
	float refraccion; 
	float factorHalo;
	float transparencia; 
	unsigned int modeloIllum; 
	vector3 ambient;
	vector3 diffuse; 
	vector3 specular;
	std::string map_Ka;
	std::string map_Ks;
	std::string map_Kd;	//diffuseMap
};


class GrupoModelos {
	
	std::vector<Obj> objs;
	std::vector<Material> materials;
	std::vector<Modelo> modelos;
	D3DXMATRIX matrizMundo;

public:

	GrupoModelos(ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, std::string archivoObj, std::string archivoMtl) {
		getParameters(archivoObj, archivoMtl);
		cargaModelos(D3DDevice, D3DContext);
		D3DXMATRIX matrizIdentidad;
		D3DXMatrixIdentity(&matrizIdentidad);
		setMatrizMundo(matrizIdentidad);
		std::cout << std::endl;
	}
	~GrupoModelos() {
		for (Modelo& mod : modelos) {
			mod.UnloadContent();
		}

	}

	void setMatrizMundo(D3DXMATRIX matriz) {
		this->matrizMundo = matriz;
	}

	void setMatrizMundo(D3DXMATRIX matriz, TerrenoRR* terreno) {
		D3DXMATRIX aux;
		this->matrizMundo = matriz;
		D3DXMatrixTranslation(&aux,0.f, terreno->Superficie(this->matrizMundo._41, this->matrizMundo._43)+4.f,0.f);
	
		D3DXMatrixMultiply(&this->matrizMundo, &this->matrizMundo, &aux);

	}

	D3DXMATRIX getMatrizMundo() {
		return this->matrizMundo;
	}
	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, GestorDeLuz* gestor) {
		for (Modelo &m : modelos) {
			m.Draw(vista, proyeccion, matrizMundo, gestor);
	}
	
	}


	
private:


	void cargaModelos(ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext) {
		for (Obj& obj : objs) {
			for (Material& mtl : materials) {
				if (strcmp(obj.mtl.c_str(), mtl.nombre) == 0) {

					modelos.push_back(Modelo(D3DDevice, D3DContext, mtl.map_Kd, mtl.map_Kd, obj));

				}

			}
		}
		std::cout << std::endl;
	}





	void getParameters(std::string archivoObj, std::string archivoMtl) {
		objs = loadObj(archivoObj);
		materials = loadMtl(archivoMtl);

	}

	std::vector<Obj> loadObj(std::string archivo) {
		bool primerObj = true;
		std::vector<vector3> verticesS;
		std::vector<vector2> uvs;
		std::vector<vector3> normales;
		std::vector<int> vertexIndices;
		std::vector<int> uvIndices;
		std::vector<int> normalIndices;
		std::string material;


		std::vector<Obj> objs;
		Obj* obj = nullptr;



		errno_t err;
		FILE* file;
		
		err = fopen_s(&file, archivo.c_str(), "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return objs;
		}

		while (1)
		{

			char lineHeader[128];
			// Lee la primera palabra de la línea
			int res = fscanf_s(file, "%s", lineHeader, 128);
			
			if (res == EOF) {
				int cuenta = vertexIndices.size();

			
				int vI = 0, uvI = 0, nI = 0;
				obj = new Obj(cuenta);
				obj->mtl = material;
				for (int i = 0; i < cuenta; i++) {

					obj->caras[i].x = verticesS[vertexIndices[i] - 1].x;
					obj->caras[i].y = verticesS[vertexIndices[i] - 1].y;
					obj->caras[i].z = verticesS[vertexIndices[i] - 1].z;

					obj->caras[i].u = uvs[uvIndices[i] - 1].u;
					obj->caras[i].v = 1-uvs[uvIndices[i] - 1].v;
				
					obj->caras[i].nx = normales[normalIndices[i] - 1].x;
					obj->caras[i].ny = normales[normalIndices[i] - 1].y;
					obj->caras[i].nz = normales[normalIndices[i] - 1].z;
				
					

					obj->indices[i] = i;




				}

				objs.push_back(*obj);

				delete obj;

				obj = nullptr;
				break;
			}

			if (strcmp(lineHeader, "object") == 0) {
				if (primerObj) {
					primerObj = false;
				}
				else {
					int cuenta = vertexIndices.size();


					int vI = 0, uvI = 0, nI = 0;
					obj = new Obj(cuenta);
					obj->mtl = material;
					for (int i = 0; i < cuenta; i++) {

						obj->caras[i].x = verticesS[vertexIndices[i] - 1].x;
						obj->caras[i].y = verticesS[vertexIndices[i] - 1].y;
						obj->caras[i].z = verticesS[vertexIndices[i] - 1].z;

						obj->caras[i].u = uvs[uvIndices[i] - 1].u;

						obj->caras[i].v = 1-uvs[uvIndices[i] - 1].v;

				

						obj->caras[i].nx = normales[normalIndices[i] - 1].x;
						obj->caras[i].ny = normales[normalIndices[i] - 1].y;
						obj->caras[i].nz = normales[normalIndices[i] - 1].z;

						obj->indices[i] = i;



					}
					
					objs.push_back(*obj);
					delete obj;


					obj = nullptr;
				}


			}
			else if (strcmp(lineHeader, "v") == 0) {


				vector3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verticesS.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				vector2 uv;
				fscanf_s(file, "%f %f\n", &uv.u, &uv.v);
				
				uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				vector3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normales.push_back(normal);
			}

			else if (strcmp(lineHeader, "usemtl") == 0) {
				char text[255];
				fscanf_s(file, "%s", text, _countof(text));
				std::string textK(text);
				textK.resize(strlen(textK.c_str()));
				material = textK;
			}

			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return objs;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}



		}







		return objs;
	}
	std::vector<Material> loadMtl(std::string archivo) {

		std::vector<Material> materiales;

		Material* mtl = nullptr;
		bool primerMaterial = true;
		errno_t err;
		FILE* file;
		err = fopen_s(&file, archivo.c_str(), "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");


			return materiales;
		}

		while (1)
		{
			char primeraPalabra[128];
			// Lee la primera palabra de la línea
			int res = fscanf_s(file, "%s", primeraPalabra, 128);
			if (res == EOF) {
				if (!primerMaterial) {
					materiales.push_back(*mtl);
					delete mtl;
					mtl = nullptr;
				}

				break;
			}

			if (strcmp(primeraPalabra, "newmtl") == 0) {
				if (primerMaterial) {
					primerMaterial = false;
				}
				else {
					materiales.push_back(*mtl);
					delete mtl;
					mtl = nullptr;
				}


				mtl = new Material;

				char nombre[255]; char* p = nombre;
				fscanf_s(file, "%s", nombre, _countof(nombre));

				strcpy_s(mtl->nombre, 255, nombre);
			}
			else if (strcmp(primeraPalabra, "Ns") == 0) {
				float Ns = 0.f;
				fscanf_s(file, "%f ", &Ns);
				mtl->expEspecular = Ns;
			}
			else if (strcmp(primeraPalabra, "Ni") == 0) {
				float Ni = 0.f;
				fscanf_s(file, "%f", &Ni);
				mtl->refraccion = Ni;
			}
			else if (strcmp(primeraPalabra, "d") == 0) {
				float d = 0.f;
				fscanf_s(file, "%f", &d);
				mtl->factorHalo = d;

			}
			else if (strcmp(primeraPalabra, "Tr") == 0) {
				float Tr = 0.f;
				fscanf_s(file, "%f", &Tr);
				mtl->transparencia = Tr;
			}
			else if (strcmp(primeraPalabra, "illum") == 0) {
				int illum = 0;
				fscanf_s(file, "%d", &illum);
				mtl->modeloIllum = illum;
			}
			else if (strcmp(primeraPalabra, "Ka") == 0) {
				vector3* vec = new vector3();
				fscanf_s(file, "%f %f %f\n", &vec->x, &vec->y, &vec->z);
				mtl->ambient = *vec;
				delete vec;
			}
			else if (strcmp(primeraPalabra, "Kd") == 0) {
				vector3* vec = new vector3();
				fscanf_s(file, "%f %f %f\n", &vec->x, &vec->y, &vec->z);
				mtl->diffuse = *vec;
				delete vec;
			}
			else if (strcmp(primeraPalabra, "Ks") == 0) {
				vector3* vec = new vector3();
				fscanf_s(file, "%f %f %f\n", &vec->x, &vec->y, &vec->z);
				mtl->specular = *vec;
				delete vec;
			}
			else if (strcmp(primeraPalabra, "map_Ka") == 0) {
				char map[255];
				fscanf_s(file, "%s", map, _countof(map));
				std::string mapK(map);
				mapK.resize(strlen(mapK.c_str()));
				mtl->map_Ka=mapK;

			}
			else if (strcmp(primeraPalabra, "map_Kd") == 0) {
				char map[255];
				fscanf_s(file, "%s", map, _countof(map));
				std::string mapK(map);
				mapK.resize(strlen(mapK.c_str()));
				mtl->map_Kd = mapK;

			}
			else if (strcmp(primeraPalabra, "map_Ks") == 0) {
				char map[255];
				fscanf_s(file, "%s", map, _countof(map));
				std::string mapK(map);
				mapK.resize(strlen(mapK.c_str()));
				mtl->map_Ks = mapK;

			}


		}

		return materiales;
	}

};


#endif