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
	std::unique_ptr<BillboardRR> billboard;
	std::unique_ptr<Camara> camara;
	
	GestorDeLuz* gestorDeLuz;
	
	
	std::unique_ptr<GrupoModelos> obelisco;
	std::unique_ptr<GrupoModelos> box;
	std::unique_ptr<GrupoModelos> arbol;
	std::unique_ptr<GrupoModelos> arbusto;
	std::unique_ptr<GrupoModelos> arbusto1;


	std::unique_ptr<GrupoModelos> lago;


	float izqder;
	float arriaba;
	float vel;
	



    DXRR(HWND hWnd, int Ancho, int Alto)
	{
		ancho = Ancho;
		alto = Alto;
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
		camara = std::make_unique<Camara>(D3DXVECTOR3(132.f,80.f,-164.121f), D3DXVECTOR3(0,80,0), D3DXVECTOR3(0,1,0), Ancho, Alto);
		TexturaTerreno* map0, *map1,  *map2;
		map0 = new TexturaTerreno(L"piedra.jpg", L"piedraNormal.jpg");    //AZUL
		map1 = new TexturaTerreno(L"tierra.jpg", L"tierraNormal.jpg");  //ROJO
		map2 = new TexturaTerreno(L"sand1.jpg", L"sandNormal.jpg");   //VERDE
		terreno = std::make_unique<TerrenoRR>(500, 500, d3dDevice, d3dContext, map0, map1, map2, L"Heighmap211.jpg", L"blend12.jpg");
		delete map0; delete map1; delete map2;
		map0 = new TexturaTerreno(L"agua.jpg", L"aguaNormal.jpg");

		delete map0;
		
		skydome = std::make_unique<SkyDome>(32, 32, 350.0f, &d3dDevice, &d3dContext, L"Sky1.png", L"Sky2.png", L"Sky3.png");
		
		
		obelisco= std::make_unique <GrupoModelos>(d3dDevice, d3dContext, "modelos/obelisco/obe.obj", "modelos/obelisco/obe.mtl");
		box = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "casa.obj", "casa.mtl");
		arbol=std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/arbol/arbol.obj", "modelos/arbol/arbol.mtl");
		lago = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/lago/lago.obj", "modelos/lago/lago.mtl", "modelos/lago/aguaDisplacement.jpg");
		arbusto = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/bush/bush.obj", "modelos/bush/bush.mtl");
		arbusto1 = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/bush1/bush1.obj", "modelos/bush1/bush1.mtl");
		
		billboard = std::make_unique<BillboardRR>(L"mP.png", 0, 0, d3dDevice, d3dContext, 925.f/2.f, 500.f/2.f);
	
	


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
		camara->posCam.y = terreno->Superficie(camara->posCam.x, camara->posCam.z) + 5 ;
		camara->UpdateCam(vel, arriaba, izqder);
		
		gestorDeLuz->Update(camara->getPosCam());
		skydome->Update(camara->vista, camara->proyeccion, gestorDeLuz);

		TurnOffDepth();
		skydome->Render(camara->posCam);
		TurnOnDepth();
		terreno->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
	
		
		billboard->Draw(camara->vista, camara->proyeccion, camara->posCam,-242.47f*3.f, -200.f,0.f, gestorDeLuz);
	
		int posX, posZ;


		//// obelisco
		
		posX = -120.89f; posZ = 45.58f;
		D3DXMATRIX* aux, * aux1; // aux1 = new D3DXMATRIX();
		aux = D3DXMatrixRotationX(&obelisco->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1= D3DXMatrixRotationY(&obelisco->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);		
		D3DXMatrixScaling(aux1, 0.6f,1.6f, 0.6f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1,posX,3.9511f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
	
		
		obelisco->setMatrizMundo(*aux, terreno.get());
		obelisco->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
	

		posX = -120.89f; posZ = -49.47f;
		aux = D3DXMatrixRotationX(&obelisco->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&obelisco->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 0.6f, 1.6f, 0.6f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, posX, 0.f , posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		obelisco->setMatrizMundo(*aux, terreno.get());
		obelisco->Draw(camara->vista, camara->proyeccion, gestorDeLuz);





		////casa
		aux = D3DXMatrixRotationX(&box->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&box->getMatrizMundo(), D3DXToRadian(90.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, 3.f, 3.f, 3.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, -74.37f, terreno->Superficie(-74.37f, -154.72f) -10.3f, -154.72f);
		D3DXMatrixMultiply(aux, aux, aux1);
		box->setMatrizMundo(*aux, terreno.get());
		box->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		////arbol
		posX = 174.34f; posZ = 10.75f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ)-18.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());
		
		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		posX =81.22f; posZ = 2.8f;
		aux = D3DXMatrixRotationX(&arbol->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixTranslation(aux1, posX, terreno->Superficie(posX, posZ) - 14.f, posZ);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->setMatrizMundo(*aux, terreno.get());

		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
		
		
		////arbusto

		aux = D3DXMatrixRotationX(&arbusto->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 2.3f, 2.3f, 2.3f);
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



		////lago

		aux = D3DXMatrixRotationX(&lago->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		D3DXMatrixScaling(aux1, 1.5f, 1.5f, 1.5f);
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixTranslation(aux1, 125.26f,3.111f, -11.33f);
		D3DXMatrixMultiply(aux, aux, aux1);
		lago->setMatrizMundo(*aux);
		lago->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


	
	
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
};
#endif