#pragma once
#include "DirectXMath.h"

class MathHelper {
public:
	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0, 0, 0,
			0, 1.0f, 0, 0,
			0, 0, 1.0f, 0,
			0, 0, 0, 1.0f);
		return I;
	}
};