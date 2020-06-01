#include <windows.h>
#include <windowsx.h>

#include "DXRR.h"
#include "GamePadRR.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool flag = true;
DXRR *dxrr;
GamePadRR *gamePad;
const int ANCHOVENTANA = 800, ALTOVENTANA = 600;


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"DXRR_E1";

    RegisterClassEx(&wc);

    RECT wr = {0, 0, ANCHOVENTANA, ALTOVENTANA};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL,
                          L"DXRR_E1",
                          L"Ejemplo 1 de DX11 RR",
                          WS_OVERLAPPEDWINDOW,
                          100,
                          100,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);
	dxrr = new DXRR(hWnd, ANCHOVENTANA, ALTOVENTANA);
	dxrr->vel=0;
    gamePad = new GamePadRR(1);

	SetTimer(hWnd, 100, 33, NULL);
    MSG msg;

    while(flag)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }
	
        dxrr->Render();
    }

    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static float dd = 25.f;
	static int mouseX = 0;
	static int mouseXPrev = 0;
	static int mouseY = 0;
	static int mouseYPrev = 0;
    switch(message)
    {

	
	case WM_DESTROY:
            {
				KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            } break;

		case WM_TIMER:
			{	ShowCursor(false);
				if (gamePad->IsConnected())
				{
					dxrr->gamePad = true;
					dxrr->izqder = 0;
					dxrr->arriaba = 0;

					if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
					{
						dxrr->izqder = 0.01;
					}
					

					if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)
					{
						dxrr->izqder = -0.01;
					}

					if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
					{
						dxrr->arriaba = 0.01;
					}
					

					if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
					{
						dxrr->arriaba = -0.01;

						
					}

					dxrr->vel = gamePad->GetState().Gamepad.bLeftTrigger/dd;
					
				}
				else {
					dxrr->gamePad = false;
				}
		

			


			
			
			
	
			} break;

		
		case WM_KEYDOWN: {
			if (!gamePad->IsConnected()) {

				switch (wParam) {
				case 'W': {
					dxrr->wPressed = true;

					break;
				}

				case 'S': {
					dxrr->sPressed = true;
					break;
				}
				case 'A': {
					dxrr->aPressed = true;
					break;
				}
				case 'D': {
					dxrr->dPressed = true;
					break;
				}

				}
			}switch (wParam) {
			case'O': {

				std::string s = dxrr->getPosCam();
				std::wstring stemp = std::wstring(s.begin(), s.end());
				LPCWSTR sw = stemp.c_str();


				MessageBox(0, sw, 0, 0);

				wParam = 0;
				break;
			}

			case 'I':
			{
				if (dd == 25.f) {
					dd = 250.f;
				}
				else {
					dd = 25.f;
				}

				break;


			}

			case 'P': {

				flag = false;

				break;
			}
			}
			break;
		}

		case WM_KEYUP:{
			switch (wParam) {
			case 'W': {
				dxrr->wPressed = false;
				break;
			}

			case 'S': {
				dxrr->sPressed = false;
				break;
			}
			case 'A': {
				dxrr->aPressed = false;
				break;
			}
			case 'D': {
				dxrr->dPressed = false;
				break;
			}
			}
			break;
					   }

		case WM_MOUSEMOVE: {
			if (!gamePad->IsConnected()) {
				dxrr->mouseX = (float)LOWORD(lParam);
				dxrr->mouseY = (float)HIWORD(lParam);
			}
			break; }
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}


