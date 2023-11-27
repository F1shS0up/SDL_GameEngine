#pragma once
#include <string>
#include "SDL.h"

namespace Engine
{
	struct Tileset
	{
		std::string tilesetPath;
		int tileW, tileH;
		int setW, setH;

		SDL_Texture* texture;
	};
	struct Tilemap_Component
	{
		std::string path;
		Tileset tileset;

		int gridCellW, gridCellH;
		int gridCellsX, gridCellsY;
		int gridW, gridH;
		std::pair<int, int>** grid;
	};

	class Tilemap_System
	{
	public:
		void Init(class Registry* reg, SDL_Renderer* renderer);
		void Draw(class Registry* reg, SDL_Renderer* renderer);
	};
};