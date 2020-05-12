#ifndef _modelo
#define _modelo

//#define _XM_NO_INTRINSICS_

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>


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

struct vector2 {
	float u, v;
};

struct Cara {
	float x, y, z, u, v, nx, ny, nz;

};

struct Obj {
	Cara* caras;		//vertex buffer info
	std::string mtl;		//material name
	UINT* indices;		//index buffer info
	int cuenta;			//number of indexes

	Obj(int nIndices) {
		cuenta = nIndices;
		caras = new Cara[nIndices];
		indices = new unsigned int[nIndices];

	}
	Obj() {}


};

class Modelo{

	int cuenta;


	ID3D11VertexShader* VertexShaderVS;
	ID3D11PixelShader* solidColorPS;

	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* colorMap;
	ID3D11ShaderResourceView* normalMap;

	ID3D11SamplerState* colorMapSampler;

	ID3D11Buffer* viewCB;
	ID3D11Buffer* projCB;
	ID3D11Buffer* worldCB;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	UINT* indices;
	


	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;

	Obj obj;



public:
	Modelo(ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, std::string map,std::string normalMap, Obj obj)
	{
		
		d3dContext = D3DContext;
		d3dDevice = D3DDevice;
		this->obj = obj;
		std::wstring w;
		std::copy(map.c_str(), map.c_str() + strlen(map.c_str()), back_inserter(w));
		const WCHAR* text = w.c_str();



		CargaParametros(text, text);

	}

	/*
	~Modelo()
	{
		//libera recursos
		
		UnloadContent();
	}*/
private:

	bool CompileD3DShader(WCHAR* filePath, char* entry, char* shaderModel, ID3DBlob** buffer)
	{
		//forma de compilar el shader
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* errorBuffer = 0;
		HRESULT result;

		result = D3DX11CompileFromFile(filePath, 0, 0, entry, shaderModel, shaderFlags,
			0, 0, buffer, &errorBuffer, 0);
		if (FAILED(result))
		{
			if (errorBuffer != 0)
			{
				OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
				errorBuffer->Release();
			}
			return false;
		}

		if (errorBuffer != 0)
			errorBuffer->Release();

		return true;
	}



	bool CargaParametros(const WCHAR* map,const WCHAR* normalMap)
	{
		HRESULT d3dResult;
		//carga el mapa de alturas

		ID3DBlob* vsBuffer = 0;

		//cargamos el shaders de vertices que esta contenido en el Shader.fx, note
		//que VS_Main es el nombre del vertex shader en el shader, vsBuffer contendra
		//al puntero del mismo
		bool compileResult = CompileD3DShader(L"Modelo.fx", "VS_Main", "vs_4_0", &vsBuffer);
		//en caso de no poder cargarse ahi muere la cosa
		if (compileResult == false)
		{
			return false;
		}

		//aloja al shader en la memoria del gpu o el cpu
		d3dResult = d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(),
			vsBuffer->GetBufferSize(), 0, &VertexShaderVS);
		//en caso de falla sale
		if (FAILED(d3dResult))
		{

			if (vsBuffer)
				vsBuffer->Release();

			return false;
		}

		//lo nuevo, antes creabamos una estructura con la definicion del vertice, ahora creamos un mapa o layout
		//de como queremos construir al vertice, esto es la definicion
		D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

		d3dResult = d3dDevice->CreateInputLayout(solidColorLayout, totalLayoutElements,
			vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);

		vsBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		ID3DBlob* psBuffer = 0;
		// de los vertices pasamos al pixel shader, note que el nombre del shader es el mismo
		//ahora buscamos al pixel shader llamado PS_Main
		compileResult = CompileD3DShader(L"Modelo.fx", "PS_Main", "ps_4_0", &psBuffer);

		if (compileResult == false)
		{
			return false;
		}
		//ya compilado y sin error lo ponemos en la memoria
		d3dResult = d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(),
			psBuffer->GetBufferSize(), 0, &solidColorPS);

		psBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

	




	
		 cuenta = obj.cuenta;
	

		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_DEFAULT;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(INT) * cuenta;
		indexDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA resourceData1;
		ZeroMemory(&resourceData1, sizeof(resourceData1));
		resourceData1.pSysMem = obj.indices;

		d3dResult = d3dDevice->CreateBuffer(&indexDesc, &resourceData1, &indexBuffer);

		if (FAILED(d3dResult))
		{
			return false;
		}
		
		

		//proceso de guardar el buffer de vertices para su uso en el render
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(Cara) * cuenta;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = obj.caras;

		d3dResult = d3dDevice->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);

		if (FAILED(d3dResult))
		{
			MessageBox(0, L"Error", L"Error al crear vertex buffer", MB_OK);
			return false;
		}
		//ya una vez pasados los vertices al buffer borramos el arreglo donde los habiamos creado inicialmente
	

		//creamos los indices para hacer el terreno
	
		//crea los accesos de las texturas para los shaders 
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, map, 0, 0, &this->colorMap, 0);
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, normalMap, 0, 0, &this->normalMap, 0);
		

		if (FAILED(d3dResult))
		{
			return false;
		}
		//aqui creamos el sampler
		D3D11_SAMPLER_DESC colorMapDesc;
		ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
		colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
		//con la estructura de descripion creamos el sampler
		d3dResult = d3dDevice->CreateSamplerState(&colorMapDesc, &colorMapSampler);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//creamos los buffers para el shader para poder pasarle las matrices
		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(D3DXMATRIX);
		constDesc.Usage = D3D11_USAGE_DEFAULT;
		//de vista
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &viewCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//de proyeccion
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &projCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//de mundo
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &worldCB);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//posicion de la camara
		D3DXVECTOR3 eye = D3DXVECTOR3(0.0f, 100.0f, 200.0f);
		//a donde ve
		D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//crea la matriz de vista
		D3DXMatrixLookAtLH(&viewMatrix, &eye, &target, &up);
		//la de proyeccion
		D3DXMatrixPerspectiveFovLH(&projMatrix, D3DX_PI / 4.0, 1.0, 0.01f, 1000.0f);
		//las transpone para acelerar la multiplicacion
		D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
		D3DXMatrixTranspose(&projMatrix, &projMatrix);

		return true;
	}
	public:

	void UnloadContent()
	{
		if (colorMapSampler)
			colorMapSampler->Release();
		if (colorMap)
			colorMap->Release();
		
		if (normalMap)
			normalMap->Release();
		


		if (VertexShaderVS)
			VertexShaderVS->Release();
		if (solidColorPS)
			solidColorPS->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (viewCB)
			viewCB->Release();
		if (projCB)
			projCB->Release();
		if (worldCB)
			worldCB->Release();
		

		colorMapSampler = 0;
		colorMap = 0;
		normalMap = 0;


		VertexShaderVS = 0;
		solidColorPS = 0;
		inputLayout = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		viewCB = 0;
		projCB = 0;
		worldCB = 0;
	}


	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXMATRIX matrizMundo)
	{
		static float rotation = 0.0f;
		rotation += 0.01;

		//paso de datos, es decir cuanto es el ancho de la estructura
		unsigned int stride = sizeof(Cara);
		unsigned int offset = 0;

		//define la estructura del vertice a traves de layout
		d3dContext->IASetInputLayout(inputLayout);

		//define con que buffer trabajara
		d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		//define con buffer de indices trabajara
		d3dContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//define la forma de conexion de los vertices
		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Establece el vertex y pixel shader que utilizara
		d3dContext->VSSetShader(VertexShaderVS, 0, 0);
		d3dContext->PSSetShader(solidColorPS, 0, 0);
		//pasa lo sbuffers al shader
		d3dContext->PSSetShaderResources(0, 1, &colorMap);
		d3dContext->PSSetShaderResources(1, 1, &normalMap);
	
		d3dContext->PSSetSamplers(0, 1, &colorMapSampler);

		//mueve la camara
		
		D3DXMATRIX rotationY;
		D3DXMatrixRotationY(&rotationY, 0.01);
		viewMatrix *= rotationY;

		D3DXMATRIX worldMat = matrizMundo;
		D3DXMatrixTranspose(&worldMat, &worldMat);
		//actualiza los buffers del shader
		d3dContext->UpdateSubresource(worldCB, 0, 0, &worldMat, 0, 0);
		d3dContext->UpdateSubresource(viewCB, 0, 0, &vista, 0, 0);
		d3dContext->UpdateSubresource(projCB, 0, 0, &proyeccion, 0, 0);
		//le pasa al shader los buffers
		d3dContext->VSSetConstantBuffers(0, 1, &worldCB);
		d3dContext->VSSetConstantBuffers(1, 1, &viewCB);
		d3dContext->VSSetConstantBuffers(2, 1, &projCB);
		//cantidad de trabajos
	
		d3dContext->DrawIndexed(cuenta, 0, 0);


	}

	void cargaModelo() {


		std::vector<vector3> verticesS;
		std::vector<vector2> uvs;
		std::vector<vector3> normales;
		std::vector<int> vertexIndices;
		std::vector<int> uvIndices;
		std::vector<int> normalIndices;

		errno_t err;
		FILE* file;
		err = fopen_s(&file, "tetera.obj", "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return;
		}

		while (1)
		{
			char lineHeader[128];
			// Lee la primera palabra de la línea
			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break; // EOF = End Of File, es decir, el final del archivo. Se finaliza el ciclo.

			if (strcmp(lineHeader, "v") == 0) {
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
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
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

	


		struct Cara {
			float x, y, z, u, v, nx, ny, nz;
		};
	cuenta = vertexIndices.size();
		Cara* cara = new Cara[cuenta];
		int counter = 0;
		int vI = 0, uvI = 0, nI = 0;
		for (int i = 0; i < vertexIndices.size(); i++) {

			cara[i].x = verticesS[vertexIndices[i] - 1].x;
			cara[i].y = verticesS[vertexIndices[i] - 1].y;
			cara[i].z = verticesS[vertexIndices[i] - 1].z;

			cara[i].u = uvs[uvIndices[i] - 1].u;
			cara[i].v = uvs[uvIndices[i] - 1].v;

			cara[i].nx = normales[normalIndices[i] - 1].x;
			cara[i].ny = normales[normalIndices[i] - 1].y;
			cara[i].nz = normales[normalIndices[i] - 1].z;

		}


		int u = 0;
	
	
	}
};
#endif