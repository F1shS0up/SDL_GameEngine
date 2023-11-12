#pragma once
namespace Engine
{
	enum Layers
	{
		A = 1,   // 0x01
		B = 2,   // 0x02
		C = 4,   // 0x04
		D = 5,   // 0x08
		E = 6,   // 0x10
		F = 7   // 0x20A = 1<<0
	};

	inline Layers operator|(Layers a, Layers b)
	{
		return static_cast<Layers>(static_cast<int>(a) | static_cast<int>(b));
	}
}