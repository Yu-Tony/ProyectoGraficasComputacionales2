#ifndef _dxrr
#define _dxrr

#define ESFINGESXZ 0.1f
#define ESFINGESY 0.1f
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
	std::unique_ptr<TerrenoRR> lago;
	std::unique_ptr<SkyDome> skydome;
	std::unique_ptr<BillboardRR> billboard;
	std::unique_ptr<Camara> camara;
	
	GestorDeLuz* gestorDeLuz;
	
	
	std::unique_ptr<GrupoModelos> esfinge;
	std::unique_ptr<GrupoModelos> box;
	std::unique_ptr<GrupoModelos> arbol;


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
		terreno = std::make_unique<TerrenoRR>(500, 500, d3dDevice, d3dContext, map0, map1, map2, L"Heighmap23.jpg", L"blend12.jpg");
		delete map0; delete map1; delete map2;
		map0 = new TexturaTerreno(L"agua.jpg", L"aguaNormal.jpg");
		lago = std::make_unique<TerrenoRR>(500, 500, d3dDevice, d3dContext, map0, L"hAgua.jpg", L"blend12.jpg");
		delete map0;
		
		skydome = std::make_unique<SkyDome>(32, 32, 350.0f, &d3dDevice, &d3dContext, L"Sky1.png", L"Sky2.png", L"Sky3.png");
		
		
		esfinge= std::make_unique <GrupoModelos>(d3dDevice, d3dContext, "modelos/esfinge/esfinge.obj", "modelos/esfinge/esfinge.mtl");
		box = std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "casa.obj", "casa.mtl");
		arbol=std::make_unique<GrupoModelos>(d3dDevice, d3dContext, "modelos/arbol/arbol.obj", "modelos/arbol/arbol.mtl");

		
		billboard = std::make_unique<BillboardRR>(L"mP.png", 0, 0, d3dDevice, d3dContext, 1196/4.f, 100.f);
	
	


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
		lago->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
		
		billboard->Draw(camara->vista, camara->proyeccion, camara->posCam,-242.63f, -20.f, -32.69f, gestorDeLuz);
	
		

		
		D3DXMATRIX* aux, * aux1; // aux1 = new D3DXMATRIX();
		aux = D3DXMatrixRotationX(&esfinge->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1= D3DXMatrixRotationY(&esfinge->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);		
		D3DXMatrixScaling(aux1, ESFINGESXZ, ESFINGESY, ESFINGESXZ);
		D3DXMatrixMultiply(aux, aux1, aux);
		D3DXMatrixTranslation(aux1,134.25f, terreno->Superficie(134.25f,-133.28f)-10.f, -133.28f);
		D3DXMatrixMultiply(aux, aux, aux1);
	
		
		esfinge->setMatrizMundo(*aux, terreno.get());
		esfinge->Draw(camara->vista, camara->proyeccion, gestorDeLuz);
	
		aux = D3DXMatrixRotationX(&esfinge->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		aux1 = D3DXMatrixRotationY(&esfinge->getMatrizMundo(), D3DXToRadian(180.f));
		D3DXMatrixMultiply(aux, aux, aux1);
		D3DXMatrixScaling(aux1, ESFINGESXZ, ESFINGESY, ESFINGESXZ);
		D3DXMatrixMultiply(aux, aux1, aux);
		D3DXMatrixTranslation(aux1, 146.91, terreno->Superficie(146.91f, -133.28f) -7.3f, -133.28f);
		D3DXMatrixMultiply(aux, aux, aux1);
		esfinge->setMatrizMundo(*aux, terreno.get());
		esfinge->Draw(camara->vista, camara->proyeccion, gestorDeLuz);


		aux = D3DXMatrixRotationX(&esfinge->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		box->setMatrizMundo(*aux, terreno.get());
		box->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		aux = D3DXMatrixRotationX(&esfinge->getMatrizMundo(), D3DXToRadian(ESFINGEROTX));
		
		aux1 = new D3DXMATRIX();
		arbol->setMatrizMundo(*aux, terreno.get());
		D3DXMatrixScaling(aux1, 1.f, 1.f, 1.f);
		D3DXMatrixMultiply(aux, aux, aux1);
		arbol->Draw(camara->vista, camara->proyeccion, gestorDeLuz);

		delete aux1;
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