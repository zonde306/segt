#include "DrawManager.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

struct D3DVertex
{
	D3DVertex(float _x, float _y, float _z, D3DCOLOR _color) : x(_x), y(_y), z(_z), color(_color)
	{}

	float x;
	float y;
	float z;
	float rhw = 0;
	D3DCOLOR color;
};

DrawManager::DrawManager(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	CreateObjects();
}

DrawManager::~DrawManager()
{
	ReleaseObjects();
}

void DrawManager::OnLostDevice()
{
	ReleaseObjects();
}

void DrawManager::OnResetDevice()
{
	CreateObjects();
}

void DrawManager::ReleaseObjects()
{
	if(m_pStateBlock)
		m_pStateBlock->Release();

	if(m_pDefaultFont)
		m_pDefaultFont->Release();

	if (m_pLine)
		m_pLine->Release();
}

void DrawManager::CreateObjects()
{
	if(FAILED(m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock)))
	{
		throw "Failed to create the state block";
	}

	if(FAILED(D3DXCreateFontA(m_pDevice, 16, 0, 0, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "Fixedsys", &m_pDefaultFont)))
	{
		throw "Failed to create the default font";
	}

	if (FAILED(D3DXCreateLine(m_pDevice, &m_pLine)))
	{
		throw "Failed to create the line";
	}
}

void DrawManager::BeginRendering()
{
	m_pStateBlock->Capture();
	m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, false);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
}

void DrawManager::EndRendering()
{
	m_pStateBlock->Apply();
}

void DrawManager::RenderLine(D3DCOLOR color, int x1, int y1, int x2, int y2)
{
	D3DVertex vertices[2] =
	{
		D3DVertex((float)x1, (float)y1, 1.0f, color),
		D3DVertex((float)x2, (float)y2, 1.0f, color)
	};

	m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertices, sizeof(D3DVertex));
}

void DrawManager::RenderRect(D3DCOLOR color, int x, int y, int w, int h)
{
	D3DVertex vertices[5] =
	{
		D3DVertex((float)x, (float)y, 1.0f, color),
		D3DVertex((float)(x + w), (float)y, 1.0f, color),
		D3DVertex((float)(x + w), (float)(y + h), 1.0f, color),
		D3DVertex((float)x, (float)(y + h), 1.0f, color),
		D3DVertex((float)x, (float)y, 1.0f, color)
	};
	m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vertices, sizeof(D3DVertex));
}

void DrawManager::RenderCircle(D3DCOLOR color, int x, int y, int r, int resolution)
{
	float curPointX;
	float curPointY;
	float oldPointX;
	float oldPointY;

	for(int i = 0; i <= resolution; ++i)
	{
		curPointX = (float)(x + r * cos(2 * M_PI * i / resolution));
		curPointY = (float)(y - r * sin(2 * M_PI * i / resolution));
		if(i > 0)
		{
			RenderLine(color, (int)curPointX, (int)curPointY, (int)oldPointX, (int)oldPointY);
		}
		oldPointX = curPointX;
		oldPointY = curPointY;
	}
}

void DrawManager::RenderText(D3DCOLOR color, int x, int y, bool centered, const char* fmt, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buffer, fmt, args);
	va_end(args);

	auto drawShadow = [&](RECT rect)
	{
		rect.left++;
		m_pDefaultFont->DrawTextA(NULL, buffer, -1, &rect, DT_TOP | DT_LEFT | DT_NOCLIP, 0xFF000000);
		rect.top++;
		m_pDefaultFont->DrawTextA(NULL, buffer, -1, &rect, DT_TOP | DT_LEFT | DT_NOCLIP, 0xFF000000);
	};

	if(centered)
	{
		RECT rec = {0, 0, 0, 0};
		m_pDefaultFont->DrawTextA(NULL, buffer, -1, &rec, DT_CALCRECT | DT_NOCLIP, color);
		rec = {x - rec.right / 2, y, 0, 0};

		// drawShadow(rec);
		// m_pDefaultFont->DrawTextA(NULL, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, color);
	}
	else
	{
		RECT rec = {x, y, 1000, 100};

		// drawShadow(rec);
		m_pDefaultFont->DrawTextA(NULL, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, color);
	}
}

void DrawManager::FillRect(D3DCOLOR color, int x, int y, int w, int h)
{
	D3DVertex vertices[4] =
	{
		D3DVertex((float)x, (float)y, 1.0f, color),
		D3DVertex((float)(x + w), (float)y, 1.0f, color),
		D3DVertex((float)x, (float)(y + h), 1.0f, color),
		D3DVertex((float)(x + w), (float)(y + h), 1.0f, color)
	};
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vertices[0], sizeof(D3DVertex));
}

void DrawManager::DrawString(int x, int y, const std::string & text, D3DCOLOR color)
{
	RECT Position;
	Position.left = x + 1;
	Position.top = y + 1;
	m_pDefaultFont->DrawTextA(0, text.c_str(), text.length(), &Position, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
	Position.left = x;
	Position.top = y;
	m_pDefaultFont->DrawTextA(0, text.c_str(), text.length(), &Position, DT_NOCLIP, color);
}

void DrawManager::DrawString(int x, int y, D3DCOLOR color, const char * text, ...)
{
	va_list ap;
	va_start(ap, text);

	char buffer[512];
	vsprintf_s(buffer, text, ap);

	va_end(ap);

	DrawString(x, y, buffer, color);
}

void DrawManager::DrawRect(int x, int y, int width, int height, D3DCOLOR color)
{
	D3DXVECTOR2 Rect[5];
	Rect[0] = D3DXVECTOR2((float)x, (float)y);
	Rect[1] = D3DXVECTOR2((float)(x + width), (float)y);
	Rect[2] = D3DXVECTOR2((float)(x + width), (float)(y + height));
	Rect[3] = D3DXVECTOR2((float)x, (float)(y + height));
	Rect[4] = D3DXVECTOR2((float)x, (float)y);
	m_pLine->SetWidth(1);
	m_pLine->Draw(Rect, 5, color);
}

void DrawManager::DrawBorderedRect(int x, int y, int width, int height, D3DCOLOR filled, D3DCOLOR color)
{
	D3DXVECTOR2 Fill[2];
	Fill[0] = D3DXVECTOR2((float)(x + width / 2), (float)y);
	Fill[1] = D3DXVECTOR2((float)(x + width / 2), (float)(y + height));
	m_pLine->SetWidth((float)width);
	m_pLine->Draw(Fill, 2, filled);

	D3DXVECTOR2 Rect[5];
	Rect[0] = D3DXVECTOR2((float)x, (float)y);
	Rect[1] = D3DXVECTOR2((float)(x + width), (float)y);
	Rect[2] = D3DXVECTOR2((float)(x + width), (float)(y + height));
	Rect[3] = D3DXVECTOR2((float)x, (float)(y + height));
	Rect[4] = D3DXVECTOR2((float)x, (float)y);
	m_pLine->SetWidth(1);
	m_pLine->Draw(Rect, 5, color);
}

void DrawManager::DrawLine(int x, int y, int x2, int y2, D3DCOLOR color)
{
	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2((float)x, (float)y);
	Line[1] = D3DXVECTOR2((float)x2, (float)y2);
	m_pLine->SetWidth(1.0f);
	m_pLine->Draw(Line, 2, color);
}

void DrawManager::DrawFilledRect(int x, int y, int width, int height, D3DCOLOR color)
{
	D3DXVECTOR2 Rect[2];
	Rect[0] = D3DXVECTOR2((float)(x + width / 2), (float)y);
	Rect[1] = D3DXVECTOR2((float)(x + width / 2), (float)(y + height));
	m_pLine->SetWidth((float)width);
	m_pLine->Draw(Rect, 2, color);
}

void DrawManager::DrawCircle(int x, int y, int radius, D3DCOLOR color)
{
	D3DXVECTOR2 DotPoints[128];
	D3DXVECTOR2 Line[128];
	int Points = 0;
	for (float i = 0; i < M_PI * 2.0f; i += 0.1f)
	{
		float PointOriginX = x + radius * cosf(i);
		float PointOriginY = y + radius * sinf(i);
		float PointOriginX2 = radius * cosf(i + 0.1f) + x;
		float PointOriginY2 = radius * sinf(i + 0.1f) + y;
		DotPoints[Points] = D3DXVECTOR2(PointOriginX, PointOriginY);
		DotPoints[Points + 1] = D3DXVECTOR2(PointOriginX2, PointOriginY2);
		Points += 2;
	}

	m_pLine->Draw(DotPoints, Points, color);
}


