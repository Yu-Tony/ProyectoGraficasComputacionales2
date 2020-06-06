#ifndef _dxrr
#define _dxrr


#define ESFINGEROTX	270.f
#define ESFINGETRY -53.f

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include "TerrenoRR.h"
#include "Camara.h"
#include "SkyDome.h"
#include "Billboard.h"
#include <string>
#include "Modelo.h"
#include "GrupoModelos.h"

#include "gestorLuz.h"




class DXRR{	

private:
	int ancho;
	int alto;
	float anchoF;
		float altoF;
public:	
	HINSTANCE hInstance;
	HWND hWnd;

	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferTarget;

	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilDisabledState;

	ID3D11BlendState *alphaBlendState, *commonBlendState;

	
	std::unique_ptr<TerrenoRR> terreno;
	
	std::unique_ptr<SkyDome> skydome;
	std::unique_ptr<BillboardRR> piramidesMP;
	std::unique_ptr<BillboardRR> piramidesMP2;
	std::unique_ptr<BillboardRR> arenaMP;
	std::unique_ptr<BillboardRR> oasis;
	std::unique_ptr<BillboardRR> palmera;


	std::unique_ptr<Camara> camara;
	
	GestorDeLuz* gestorDeLuz;
	
	
	std::unique_ptr<GrupoModelos> elefante;
	std::unique_ptr<GrupoModelos> obelisco;
	std::unique_ptr<GrupoModelos> esfinge;
	std::unique_ptr<GrupoModelos> box;
	std::unique_ptr<GrupoModelos> box1;
	std::unique_ptr<GrupoModelos> arbol;
	std::unique_ptr<GrupoModelos> arbusto;
	std::unique_ptr<GrupoModelos> arbusto1;
	std::unique_ptr<GrupoModelos> monstera;
	std::unique_ptr<GrupoModelos> puerta;
	std::unique_ptr<GrupoModelos> gato;

	std::unique_ptr<GrupoModelos> lago;


	float izqder;
	float arriaba;
	float vel;


	bool wPressed;
	bool aPressed;
	bool sPressed;
	bool dPressed;
	
	float mouseX;
	float mouseY;

	bool gamePad = false;

	float nn;
    DXRR(HWND hWnd, int Ancho, int Alto)
	{
		nn = 0;
		ancho = Ancho;
		alto = Alto;
		anchoF = (float)ancho;
		altoF = (float)alto;
		mouseX = ancho / 2;
		mouseY = alto / 2;
		driverType = D3D_DRIVER_TYPE_NULL;
		featureLevel = D3D_FEATURE_LEVEL_11_0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
		IniciaD3D(hWnd);
		izqder = 0;
		arriaba = 0;
		gestorDeLuz = GestorDeLuz::getInstancia();
		camara = std::make_unique<Camara>(D3DXVECTOR3(-30.52f,80.f,-124.f), D3DXVECTOR3(0,80,0), D3DXVECTOR3(0,1,0), Ancho, Alto);
		TexturaTerreno* map0, *map1,  *map2;
		map0 = new TexturaTerreno(L"piedra.jpg", L"piedraNormal.jpg");    //AZUL
		map1 = new TexturaTerreno(L"tierra.jpg", L"tierraNormal.jpg");  //ROJO
		map2 = new TexturaTerreno(L"sand1.jpg", L"sandNormal.jpg");   //VERDE
		terreno = std::make_unique<TerrenoRR>(500, 500, d3dDevice, d3dContext, map0, map1, map2, L"Heighmap211.jpg", L"blend12.jpg");
		delete map0; delete map1; delete map2;
		map0 = new TexturaTerreno(L"agua.jpg", L"aguaNormal.jpg");

		delete map0;
		
		skydome = std::make_unique<SkyDome>(32, 32, 450.0f, &d3dDevice, &d3dContext, L"Sky1.jpg", L"skynight.jpg", L"Sky3.png");
		
		
		elefante= std::make_unique <GrupoModelos>(d3dDevice, d3dContext, "modelos/elephant/elephant.obj", "modelos/elephant/elephant.mtl");
		obelisco= std::make_unique <GrupoModelos>(d3dDevice, d3dContext, "modelos/obelisco/obe.obj", "modelos/obelisco/obe.mtl");
		box = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/casa/casa.obj", "modelos/casa/casa.mtl");
		box1 = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/casa1/casa1.obj", "modelos/casa1/casa1.mtl");
		arbol=std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/arbol/arbol.obj", "modelos/arbol/arbol.mtl");
		lago = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/lago/lago.obj", "modelos/lago/lago.mtl", "modelos/lago/aguaDisplacement.jpg", "modelos/lago/aguaDisplacement1.jpg");
		arbusto = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/bush/bush.obj", "modelos/bush/bush.mtl");
		arbusto1 = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/bush1/bush1.obj", "modelos/bush1/bush1.mtl");
		monstera = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/monstera/monstera.obj", "modelos/monstera/monstera.mtl");
		esfinge = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/esfinge/esfinge.obj", "modelos/esfinge/esfinge.mtl");
		puerta = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/puerta/puerta.obj", "modelos/puerta/puerta.mtl");
		gato = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/gato/gato.obj", "modelos/gato/gato.mtl");

		
		piramidesMP = std::make_unique<BillboardRR>(L"mP1.png", 0, 0, d3dDevice, d3dContext, 925.f/2.f, 500.f/2.f);
		piramidesMP2 = std::make_unique<BillboardRR>(L"mP2.png", 0, 0, d3dDevice, d3dContext, 925.f/2.f, 500.f/2.f);
		arenaMP = std::make_unique<BillboardRR>(L"arena.png", 0, 0, d3dDevice, d3dContext, 925.f/2.f, 500.f/2.f);
		oasis = std::make_unique<BillboardRR>(L"oasis.png", 0, 0, d3dDevice, d3dContext, 523.f/40.f, 65.f/40.f,0);
		palmera = std::make_unique<BillboardRR>(L"palm.png", 0, 0, d3dDevice, d3dContext, 5.f, 7.f);
	
		wPressed = false;
		aPressed = false;
		sPressed = false;
		dPressed = false;

		

	}

	~DXRR()
	{
		LiberaD3D();
		
		
	
		gestorDeLuz->release();
	}

	bool IniciaD3D(HWND hWnd)
	{
		this->hInstance = hInstance;
		this->hWnd = hWnd;

		//obtiene el ancho y alto de la ventana donde se dibuja
		RECT dimensions;
		GetClientRect(hWnd, &dimensions);
		unsigned int width = dimensions.right - dimensions.left;
		unsigned int heigth = dimensions.bottom - dimensions.top;

		//Las formas en como la pc puede ejecutar el DX11, la mas rapida es D3D_DRIVER_TYPE_HARDWARE pero solo se puede usar cuando lo soporte el hardware
		//otra opcion es D3D_DRIVER_TYPE_WARP que emula el DX11 en los equipos que no lo soportan
		//la opcion menos recomendada es D3D_DRIVER_TYPE_SOFTWARE, es la mas lenta y solo es util cuando se libera una version de DX que no sea soportada por hardware
		D3D_DRIVER_TYPE driverTypes[]=
		{
			D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
		};
		unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

		//La version de DX que utilizara, en este caso el DX11
		D3D_FEATURE_LEVEL featureLevels[]=
		{
			D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
		};
		unsigned int totalFeaturesLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = heigth;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		HRESULT result;
		unsigned int driver = 0, creationFlags = 0;
		for(driver = 0; driver<totalDriverTypes; driver++)
		{
			result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
				creationFlags, featureLevels, totalFeaturesLevels, 
				D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
				&d3dDevice, &featureLevel, &d3dContext);

			if(SUCCEEDED(result))
			{
				driverType = driverTypes[driver];
				break;
			}
		}

		if(FAILED(result))
		{

			//Error al crear el Direct3D device
			return false;
		}
		
		ID3D11Texture2D* backBufferTexture;
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
		if(FAILED(result))
		{
			//"Error al crear el swapChainBuffer
			return false;
		}

		result = d3dDevice->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget);
		if(backBufferTexture)
			backBufferTexture->Release();

		if(FAILED(result))
		{
			//Error al crear el renderTargetView
			return false;
		}


		D3D11_VIEWPORT viewport;
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)heigth;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		d3dContext->RSSetViewports(1, &viewport);

		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width;
		depthTexDesc.Height = heigth;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		
		result = d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &depthTexture);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear la DepthTexture", MB_OK);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		result = d3dDevice->CreateDepthStencilView(depthTexture, &descDSV, &depthStencilView);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el depth stencil target view", MB_OK);
			return false;
		}

		d3dContext->OMSetRenderTargets(1, &backBufferTarget, depthStencilView);

		return true;			
		
	}

	void LiberaD3D(void)
	{
		if(depthTexture)
			depthTexture->Release();
		if(depthStencilView)
			depthStencilView->Release();
		if(backBufferTarget)
			backBufferTarget->Release();
		if(swapChain)
			swapChain->Release();
		if(d3dContext)
			d3dContext->Release();
		if(d3dDevice)
			d3dDevice->Release();

		depthTexture = 0;
		depthStencilView = 0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
	}
	
	void Render(void)
	{
		if( d3dContext == 0 )
			return;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView( backBufferTarget, clearColor );
		d3dContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		camara->posCam.y = terreno->Superficie(camara->posCam.x, camara->posCam.z) + 15.f ;
		if (!gamePad) {
			izqder = (mouseX - anchoF / 2) / (anchoF * 50);
			arriaba = -(mouseY - altoF / 2) / (altoF * 50);
		}
		camara->UpdateCam(vel, arriaba, izqder);
		if (wPressed) {
			camara->posCam += camara->refFront * .05f;
		}
		if (sPressed) {
			camara->posCam += camara->refFront * -.05f;
		}
		if (aPressed) {
			camara->posCam += camara->refRight * 0.05f;
		}
		if (dPressed) {
			camara->posCam +=camara->refRight * -0.05f;
		}
		
		

		gestorDeLuz->Update(camara->getPosCam());
		skydome->Update(camara->vista, camara->proyeccion, gestorDeLuz);

		TurnOffDepth();
		skydome->Render(camara->posCam);
		TurnOnDepth();
		terreno->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
	
	//MATTE PAINTING	
		
	
		int posX, posZ;


		//colisiones limitrofes
		CollisionDetection(camara->getPosCam().x, -162.24f);
		CollisionDetection(camara->getPosCam().x, 152.25f);
		CollisionDetection(camara->getPosCam().z, -125.34f);
		CollisionDetection(camara->getPosCam().z, 145.4f);


		
		//elefante
		posX = camara->getPosCam().x; posZ = camara->getPosCam().z;
		static float rott = 0.f;
		rott += izqder;
		D3DXMATRIX* aux, * aux1; 
		aux = D3DXMatrixRotationX(&elefante->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&elefante->getMatrizMundo(), D3DXToRadian(16.f)+ rott);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 3.f, 3.f, 3.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX + camara->refFront.x*4.f, 5.f, posZ  + camara->refFront.z * 4.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		elefante->setMatrizMundo(*aux, terreno.get());
		elefante->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
	
		
		////gato

		posX = 25.56f; posZ = -36.67f;

		aux = D3DXMatrixRotationX(&puerta->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&puerta->getMatrizMundo(), D3DXToRadian(270.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 2.f, 2.f, 2.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, 0.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);


		gato->setMatrizMundo(*aux, terreno.get());
		gato->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, 4.f, 4.f);



		posX = 25.56f; posZ = -3.f;

		aux = D3DXMatrixRotationX(&puerta->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&puerta->getMatrizMundo(), D3DXToRadian(270.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 2.f, 2.f, 2.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, 0.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);


		gato->setMatrizMundo(*aux, terreno.get());
		gato->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, 4.f, 4.f);


		//// puerta

		posX =  36.7f; posZ = -18.f;

		aux = D3DXMatrixRotationX(&puerta->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&puerta->getMatrizMundo(), D3DXToRadian(90.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 15.f, 15.f, 15.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, 18.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);


		puerta->setMatrizMundo(*aux, terreno.get());
		puerta->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ-16.f, 20.f, 20.f);
		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ+16.f, 20.f, 20.f);


		//// obelisco
		
		posX = -162.24f; posZ = 45.58f;
		
		aux = D3DXMatrixRotationX(&obelisco->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1= D3DXMatrixRotationY(&obelisco->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);		
		D3DXMatrixScaling(aux1, 1.6f,1.6f,1.6f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1,posX,3.9511f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
	
		
		obelisco->setMatrizMundo(*aux, terreno.get());
		obelisco->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
	

		posX = -162.24f; posZ = -49.47f;
		aux = D3DXMatrixRotationX(&obelisco->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&obelisco->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 1.6f, 1.6f, 1.6f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, 0.f , posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		obelisco->setMatrizMundo(*aux, terreno.get());
		obelisco->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		//esfinge
		posX = 102.38f; posZ = 154.22f;

		aux = D3DXMatrixRotationX(&esfinge->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&esfinge->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 1.2f, 1.2f, 1.2f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, -6.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);


		esfinge->setMatrizMundo(*aux, terreno.get());
		esfinge->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, 45.f, 160.f);

		////casa
		posX = -74.37f; posZ = -154.72f;
		aux = D3DXMatrixRotationX(&box->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&box->getMatrizMundo(), D3DXToRadian(90.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 3.f, 3.f, 3.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) -10.3f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		box->setMatrizMundo(*aux, terreno.get());
		box->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
		
		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, 18.f, 18.f);


		posX = -74.37f; posZ = 107.22f;
		aux = D3DXMatrixRotationX(&box->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&box->getMatrizMundo(), D3DXToRadian(90.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 3.f, 3.f, 3.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.3f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		box->setMatrizMundo(*aux, terreno.get());
		box->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, 18.f, 18.f);


		posX = -78.37f; posZ = 28.9f;
		aux = D3DXMatrixRotationX(&box->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&box->getMatrizMundo(), D3DXToRadian(0.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 2.f, 2.f, 2.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.3f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		box1->setMatrizMundo(*aux, terreno.get());
		box1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, 18.f, 18.f);


		////arbol

		const int ANCHOARBOL = 6.f;

		posX = 174.34f; posZ = 10.75f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ)-18.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());
		
		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x+ camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, ANCHOARBOL, ANCHOARBOL);

		posX =81.22f; posZ = 2.8f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());

		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x+ camara->refFront.x * 8.f, camara->getPosCam().z+ camara->refFront.z * 8.f, posX, posZ, ANCHOARBOL, ANCHOARBOL);

		posX = -70.36f; posZ = -118.96f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 17.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());

		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, ANCHOARBOL, ANCHOARBOL);

		posX = -85.52f; posZ = 96.75f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 11.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());

		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, ANCHOARBOL, ANCHOARBOL);
			

		posX = 28.81f; posZ = 49.93f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());

		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, ANCHOARBOL, ANCHOARBOL);


		posX = 14.8f; posZ =-189.81f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());

		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		CollisionDetection(camara->getPosCam().x + camara->refFront.x * 8.f, camara->getPosCam().z + camara->refFront.z * 8.f, posX, posZ, ANCHOARBOL, ANCHOARBOL);
		////monstera

		posX = 165.62f; posZ = -19.53f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 4.f, 4.f, 4.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		monstera->setMatrizMundo(*aux, terreno.get());

		monstera->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		


		posX = 116.f; posZ = 30.29f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 4.f, 4.f, 4.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		monstera->setMatrizMundo(*aux, terreno.get());

		monstera->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 108.69f; posZ = -67.23f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 4.f, 4.f, 4.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		monstera->setMatrizMundo(*aux, terreno.get());

		monstera->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 95.76f; posZ = 11.72f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 4.f, 4.f, 4.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		monstera->setMatrizMundo(*aux, terreno.get());

		monstera->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		////arbusto

		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.6f, 0.6f, 0.6f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, 174.47f, terreno->Superficie(174.47f, -3.96f)-10.f, -3.96f);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, 152.08f, terreno->Superficie(152.08f, 20.48f) - 12.f, 20.48f);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		posX = 151.01f; posZ = -62.89f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 123.62f; posZ = -68.35f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);



		posX = 99.55f; posZ = -60.55f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);



		posX = 78.94f; posZ = -27.69f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);



		posX = 85.02f; posZ = -3.89f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		posX = 105.6f; posZ = 15.57f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 108.02f; posZ = -48.99f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 177.9f; posZ = -26.71f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 137.6f; posZ = -67.38f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		posX = 78.73f; posZ = -54.93f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);



		posX = 167.33f; posZ = 7.89f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		posX = 113.5f; posZ =20.5f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 83.19f; posZ = -51.14f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX =163.82f; posZ = -47.16f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 141.59f; posZ = 18.64f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 66.44f; posZ = -26.24f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 154.42f; posZ = 6.74f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 167.44f; posZ = -42.41f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());


		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 109.44f; posZ = -71.6f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 90.88f; posZ = -53.66f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 177.27f; posZ = -10.68f;
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 128.9f; posZ = 25.12f;
		
		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 8.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto->setMatrizMundo(*aux, terreno.get());

		arbusto->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		////arbusto1
		posX = 170.79f; posZ = -37.53f;
		aux = D3DXMatrixRotationX(&arbusto1->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.03f, 0.03f, 0.03f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto1->setMatrizMundo(*aux, terreno.get());

		arbusto1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 129.15f; posZ = 22.87f;
		aux = D3DXMatrixRotationX(&arbusto1->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.03f, 0.03f, 0.03f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto1->setMatrizMundo(*aux, terreno.get());

		arbusto1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 84.51f; posZ = -47.89f;
		aux = D3DXMatrixRotationX(&arbusto1->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.03f, 0.03f, 0.03f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto1->setMatrizMundo(*aux, terreno.get());

		arbusto1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 137.6f; posZ = -67.38f;
		aux = D3DXMatrixRotationX(&arbusto1->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.03f, 0.03f, 0.03f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto1->setMatrizMundo(*aux, terreno.get());

		arbusto1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 68.12f; posZ = -19.17f;
		aux = D3DXMatrixRotationX(&arbusto1->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.03f, 0.03f, 0.03f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 15.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto1->setMatrizMundo(*aux, terreno.get());

		arbusto1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		posX = 155.19f; posZ = -54.67f;
		aux = D3DXMatrixRotationX(&arbusto1->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 0.03f, 0.03f, 0.03f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 10.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbusto1->setMatrizMundo(*aux, terreno.get());

		arbusto1->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		////lago
		TurnOnAlphaBlending();
		aux = D3DXMatrixRotationX(&lago->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 1.2f, 1.2f, 1.2f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, 132.03f,3.111f, -15.37f);
		D3DXMatrixMultiply(aux, aux, aux1);
		lago->setMatrizMundo(*aux);
		lago->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		TurnOffAlphaBlending();


		piramidesMP->Draw(camara->vista, camara->proyeccion, camara->posCam, -242.47f * 3.5f, -199.f, 0.f, gestorDeLuz);
		arenaMP->Draw(camara->vista, camara->proyeccion, camara->posCam, -233.58f * 3.f, -200.f, -237.97f * 3.0f, gestorDeLuz);
		arenaMP->Draw(camara->vista, camara->proyeccion, camara->posCam, 0.f, -198.f, -242.47f * 3.5f, gestorDeLuz);
		piramidesMP2->Draw(camara->vista, camara->proyeccion, camara->posCam, 227.f * 3.f, -200.f, -222.65f * 3.0f, gestorDeLuz);
		arenaMP->Draw(camara->vista, camara->proyeccion, camara->posCam, 242.47f * 3.5f, -199.f, 0.f, gestorDeLuz);
		arenaMP->Draw(camara->vista, camara->proyeccion, camara->posCam, 233.58f * 3.f, -200.f, 237.97f * 3.0f, gestorDeLuz);
		arenaMP->Draw(camara->vista, camara->proyeccion, camara->posCam, 0.f, -199.f, 242.47f * 3.5f, gestorDeLuz);
		arenaMP->Draw(camara->vista, camara->proyeccion, camara->posCam, -227.f * 3.f, -200.f, 222.65f * 3.0f, gestorDeLuz);
		static unsigned int contOasis = 0;
		contOasis++;
		if (contOasis > 50 && contOasis < 1000) {
			contOasis++;
			oasis->Draw(camara->vista, camara->proyeccion, camara->posCam, camara->refFront, 30.f, gestorDeLuz);
		}
	
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, -237.f * 1.5f, -10.f,0.f,gestorDeLuz,0);
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, -237.f * 2.f, -10.f, 20.f, gestorDeLuz,0);
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, -237.f * 1.8f, -10.f, 100.f, gestorDeLuz,0);
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, -337.f * 1.0f, -10.f, -100.f, gestorDeLuz,0);


		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, -373.58f * 1.0f, -10.f, -258.f, gestorDeLuz,0);
		
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, 225.f * 1.0f, -10.f, -400.2f, gestorDeLuz,0);
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, 230.58f * 1.3f, -10.f, -412.f, gestorDeLuz,0);

		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, 242.47f * 1.7f, -10.f, -10.f, gestorDeLuz,0);
		palmera->Draw(camara->vista, camara->proyeccion, camara->posCam, 222.47f * 2.8f, -10.f, -20.f, gestorDeLuz,0);
		swapChain->Present( 1, 0 );
	}


	//Activa el alpha blend para dibujar con transparencias
	void TurnOnAlphaBlending()
	{
		float blendFactor[4];
		blendFactor[0] = 0.0f;
		blendFactor[1] = 0.0f;
		blendFactor[2] = 0.0f;
		blendFactor[3] = 0.0f;
		HRESULT result;

		D3D11_BLEND_DESC descABSD;
		ZeroMemory(&descABSD, sizeof(D3D11_BLEND_DESC));
		descABSD.RenderTarget[0].BlendEnable = TRUE;
		descABSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descABSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descABSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		result = d3dDevice->CreateBlendState(&descABSD, &alphaBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(alphaBlendState, blendFactor, 0xffffffff);
	}

	//Regresa al blend normal(solido)
	void TurnOffAlphaBlending()
	{
		D3D11_BLEND_DESC descCBSD;
		ZeroMemory(&descCBSD, sizeof(D3D11_BLEND_DESC));
		descCBSD.RenderTarget[0].BlendEnable = FALSE;
		descCBSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descCBSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descCBSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		HRESULT result;

		result = d3dDevice->CreateBlendState(&descCBSD, &commonBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(commonBlendState, NULL, 0xffffffff);
	}

	void TurnOnDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDSD;
		ZeroMemory(&descDSD, sizeof(descDSD));
		descDSD.DepthEnable = true;
		descDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDSD.StencilEnable=true;
		descDSD.StencilReadMask = 0xFF;
		descDSD.StencilWriteMask = 0xFF;
		descDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDSD, &depthStencilState);
		
		d3dContext->OMSetDepthStencilState(depthStencilState, 1);
	}

	void TurnOffDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDDSD;
		ZeroMemory(&descDDSD, sizeof(descDDSD));
		descDDSD.DepthEnable = false;
		descDDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDDSD.StencilEnable=true;
		descDDSD.StencilReadMask = 0xFF;
		descDDSD.StencilWriteMask = 0xFF;
		descDDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDDSD, &depthStencilDisabledState);
		d3dContext->OMSetDepthStencilState(depthStencilDisabledState, 1);
	}

	std::string getPosCam() {
		std::string sPosicion;
		D3DXVECTOR3 vPosicion = camara->getPosCam();
		sPosicion = "X: " + std::to_string(vPosicion.x) + " Y: " + std::to_string(vPosicion.y) + " Z: " + std::to_string(vPosicion.z);

		return sPosicion;
	}

	void CollisionDetection(float xP, float zP, float x0,  float z0, float ancho, float profundidad) {
		if ((xP <= x0 + ancho / 2 && xP >= x0 - ancho / 2) && (zP <= z0 + profundidad / 2 && zP >= z0 - profundidad / 2)) {
			vel = -1;
		}
	}

	void CollisionDetection(float ejeCam, float ejeWorld) {
		if (ejeWorld > 0) {
			if (ejeCam >= ejeWorld) {
				vel = -1;
			}
		}
		else {
			if (ejeCam <= ejeWorld){
				vel = -1;
			}
		}
	}
	
};
#endif