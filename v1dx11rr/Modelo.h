#ifndef _modelo
#define _modelo

//#define _XM_NO_INTRINSICS_

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>



#include "gestorLuz.h"



struct Agua {
	float animacionDisplacement;
	float animacionMovimiento;
	float relleno0;
	float relleno1;

};

struct vector2 {
	float u, v;
};




struct Cara {
	float x, y, z, u, v, nx, ny, nz,tx, ty, tz, bx, by, bz;

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

class Modelo:public ComunicacionLuces{

	int cuenta;


	ID3D11VertexShader* VertexShaderVS;
	ID3D11PixelShader* solidColorPS;

	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* colorMap;
	ID3D11ShaderResourceView* normalMap;
	ID3D11ShaderResourceView* opacityMap;
	ID3D11ShaderResourceView* displacementMap;

	ID3D11SamplerState* colorMapSampler;

	ID3D11Buffer* viewCB;
	ID3D11Buffer* projCB;
	ID3D11Buffer* worldCB;

	ID3D11Buffer* lagoBuffer;
	Agua lago;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	UINT* indices;
	


	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;

	Obj obj;



public:

	Modelo(Modelo *m) {
		*this = *m;
	}
	Modelo(ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, std::string map,std::string normalMap, Obj obj)
	{
		
		d3dContext = D3DContext;
		d3dDevice = D3DDevice;
		this->obj = obj;
		std::wstring w;
		std::copy(map.c_str(), map.c_str() + strlen(map.c_str()), back_inserter(w));
	
		const WCHAR* text = w.c_str();
		
		std::string normal = map;
		std::string opacity = map;
		int counter = 0;
		for (char& c : normal) {
			if (c != '.') {
				counter++;
			}
			else {
				break;
			}
			
		}
		normal = normal.substr(0, counter);
		opacity = normal;
		normal += "Normal.jpg";
		opacity += "Opacity.jpg";
		
		
		
		std::wstring wNormal;
		std::copy(normal.c_str(), normal.c_str() + strlen(normal.c_str()), back_inserter(wNormal));
		const WCHAR* textNormal = wNormal.c_str();
		
		std::wstring wOpacity;
		std::copy(opacity.c_str(), opacity.c_str() + strlen(opacity.c_str()), back_inserter(wOpacity));
		const WCHAR* textOpacity = wOpacity.c_str();
		getBTN();
		CargaParametros(text, textNormal, textOpacity);

	}

	Modelo(ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, std::string map, std::string normalMap,std::string displacementMap, Obj obj)
	{

		d3dContext = D3DContext;
		d3dDevice = D3DDevice;
		this->obj = obj;
		std::wstring w;
		std::copy(map.c_str(), map.c_str() + strlen(map.c_str()), back_inserter(w));

		const WCHAR* text = w.c_str();

		std::wstring wD;
		std::copy(displacementMap.c_str(), displacementMap.c_str() + strlen(displacementMap.c_str()), back_inserter(wD));

		const WCHAR* textD = wD.c_str();

		std::string normal = map;
		std::string opacity = map;
		int counter = 0;
		for (char& c : normal) {
			if (c != '.') {
				counter++;
			}
			else {
				break;
			}

		}
		normal = normal.substr(0, counter);
		opacity = normal;
		normal += "Normal.jpg";
		opacity += "Opacity.jpg";



		std::wstring wNormal;
		std::copy(normal.c_str(), normal.c_str() + strlen(normal.c_str()), back_inserter(wNormal));
		const WCHAR* textNormal = wNormal.c_str();

		std::wstring wOpacity;
		std::copy(opacity.c_str(), opacity.c_str() + strlen(opacity.c_str()), back_inserter(wOpacity));
		const WCHAR* textOpacity = wOpacity.c_str();
		getBTN();
		CargaParametros(text, textNormal, textOpacity, textD);

	}

	//~Modelo()
	//{
	//	//libera recursos
	//	
	//	
	//}

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

	bool CargaParametros(const WCHAR* map, const WCHAR* normalMap, const WCHAR* opacityMap, const WCHAR* displacementMap)
	{
		HRESULT d3dResult;
		//carga el mapa de alturas

		ID3DBlob* vsBuffer = 0;

		//cargamos el shaders de vertices que esta contenido en el Shader.fx, note
		//que VS_Main es el nombre del vertex shader en el shader, vsBuffer contendra
		//al puntero del mismo
		bool compileResult = CompileD3DShader(L"Shader1.hlsl", "VS_Main", "vs_5_0", &vsBuffer);
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
			{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
		compileResult = CompileD3DShader(L"Shader1.hlsl", "PS_Main", "ps_5_0", &psBuffer);

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

		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, opacityMap, 0, 0, &this->opacityMap, 0);
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, displacementMap, 0, 0, &this->displacementMap, 0);


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


		CreateLuzAmbientalBuffer(&d3dDevice);
		CreateLuzDifusaBuffer(&d3dDevice);


		D3D11_BUFFER_DESC constDescLago;
		ZeroMemory(&constDescLago, sizeof(constDescLago));
		constDescLago.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDescLago.ByteWidth = sizeof(D3DXVECTOR4);
		constDescLago.Usage = D3D11_USAGE_DEFAULT;
		//de vista
		d3dResult = d3dDevice->CreateBuffer(&constDescLago, 0, &lagoBuffer);


		return true;
	}


	bool CargaParametros(const WCHAR* map,const WCHAR* normalMap, const WCHAR* opacityMap)
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
			{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, opacityMap, 0, 0, &this->opacityMap, 0);

		displacementMap = nullptr;

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


		CreateLuzAmbientalBuffer(&d3dDevice);
		CreateLuzDifusaBuffer(&d3dDevice);

		return true;
	}


	void UnloadContent()
	{
		if (colorMapSampler)
			colorMapSampler->Release();
		if (colorMap)
			colorMap->Release();
		
		if (normalMap)
			normalMap->Release();

		if (opacityMap)
			opacityMap->Release();
		
		if (displacementMap)
			displacementMap->Release();

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
		opacityMap = 0;

		VertexShaderVS = 0;
		solidColorPS = 0;
		inputLayout = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		viewCB = 0;
		projCB = 0;
		worldCB = 0;
	}

	public:
	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXMATRIX matrizMundo, GestorDeLuz* gestor)
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
		d3dContext->PSSetShaderResources(2, 1, &opacityMap);
		if (displacementMap != nullptr) {


			d3dContext->VSSetShaderResources(3, 1, &displacementMap);
		}
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

		UpdateLuzAmbiental(gestor);
		UpdateLuzDifusa(gestor);

		d3dContext->UpdateSubresource(luzAmbientalCB, 0, 0, &luzAmbiental, 0, 0);
		d3dContext->UpdateSubresource(luzDifusaCB, 0, 0, &luzDifusa, 0, 0);

		d3dContext->PSSetConstantBuffers(3, 1, &luzAmbientalCB);
		d3dContext->PSSetConstantBuffers(4, 1, &luzDifusaCB);

		if (displacementMap != nullptr) {

			static bool vuelta = false;
			static float movimientoText = 0.f;

			if (vuelta == false) {
				if (movimientoText < 0.03f) {
					movimientoText += 0.00005f;
				}
				else {
					vuelta = true;

				}
			}
			else {
				if (movimientoText > 0.0f) {
					movimientoText -= 0.00005f;
				}
				else {
					vuelta = false;
				}
			}
			lago.animacionDisplacement = movimientoText;
			lago.animacionMovimiento = movimientoText;
			
			(d3dContext)->UpdateSubresource(lagoBuffer, 0, 0, &lago, 0, 0);

			d3dContext->VSSetConstantBuffers(5, 1, &lagoBuffer);
		}


		//cantidad de trabajos
	
		d3dContext->DrawIndexed(cuenta, 0, 0);


	}

	
	void getBTN() {
		for (int i = 0; i < obj.cuenta; i += 3) {
			D3DXVECTOR3 v0(obj.caras[i].x, obj.caras[i].y, obj.caras[i].z);
			D3DXVECTOR3 v1(obj.caras[i + 1].x, obj.caras[i + 1].y, obj.caras[i + 1].z);
			D3DXVECTOR3 v2(obj.caras[i + 2].x, obj.caras[i + 2].y, obj.caras[i + 2].z);

			D3DXVECTOR3 deltaPos1 = v1 - v0;
			D3DXVECTOR3 deltaPos2 = v2 - v0;

			D3DXVECTOR2 uv0(obj.caras[i].u, obj.caras[i].v);
			D3DXVECTOR2 uv1(obj.caras[i + 1].u, obj.caras[i + 1].v);
			D3DXVECTOR2 uv2(obj.caras[i + 2].u, obj.caras[i + 2].v);

			D3DXVECTOR2 deltaUV1 = uv1 - uv0;
			D3DXVECTOR2 deltaUV2 = uv2 - uv0;




			/*D3DXVECTOR3 tangente= (deltaPos1 * deltaUV2.x - deltaPos2 * deltaUV1.x) * r;
			D3DXVECTOR3 binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r; */



			/*		D3DXVECTOR3 binormal; D3DXVec3Cross(&binormal, &normal, &tangente);
					D3DXVec3Normalize(&tangente, &tangente);
					D3DXVec3Normalize(&binormal, &binormal);*/

			D3DXVECTOR3 normal(obj.caras[i].nx, obj.caras[i].ny, obj.caras[i].nz);
			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			D3DXVECTOR3 tangente;

			tangente.x = r * (deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x);
			tangente.y = r * (deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y);
			tangente.z = r * (deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z);

			D3DXVECTOR3 binormal;

			//binormal.x = r * (-deltaUV2.x * deltaPos1.x + deltaUV1.x * deltaPos2.x);
			//binormal.y = r * (-deltaUV2.x * deltaPos1.y + deltaUV1.x * deltaPos2.y);
			//binormal.z = r * (-deltaUV2.x * deltaPos1.z + deltaUV1.x * deltaPos2.z);

			D3DXVec3Cross(&binormal, &normal, &tangente);

			obj.caras[i].tx = tangente.x;
			obj.caras[i].ty = tangente.y;
			obj.caras[i].tz = tangente.z;

			obj.caras[i].bx = binormal.x;
			obj.caras[i].by = binormal.y;
			obj.caras[i].bz = binormal.z;

			obj.caras[i + 1].tx = tangente.x;
			obj.caras[i + 1].ty = tangente.y;
			obj.caras[i + 1].tz = tangente.z;

			obj.caras[i + 1].bx = binormal.x;
			obj.caras[i + 1].by = binormal.y;
			obj.caras[i + 1].bz = binormal.z;

			obj.caras[i + 2].tx = tangente.x;
			obj.caras[i + 2].ty = tangente.y;
			obj.caras[i + 2].tz = tangente.z;

			obj.caras[i + 2].bx = binormal.x;
			obj.caras[i + 2].by = binormal.y;
			obj.caras[i + 2].bz = binormal.z;


		}
	}



	
};





#endif