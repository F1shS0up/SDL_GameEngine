#include "Tilemap.h"
#include "Registry.h"
#include "Entity.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include "SDL_image.h"
#include "Vector2DInt.h"
#include <algorithm>
namespace Engine
{
	void Tilemap_System::Init(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->tilemaps.count(e))
			{
				Json::Value tilemap;
				std::ifstream tilemapFile(reg->tilemaps[e].path, std::ifstream::binary);

				tilemapFile >> tilemap;

				reg->tilemaps[e].gridW = tilemap["width"].asInt();
				reg->tilemaps[e].gridH = tilemap["height"].asInt();
				reg->tilemaps[e].gridCellsX = tilemap["layers"][0]["gridCellsX"].asInt();
				reg->tilemaps[e].gridCellsY = tilemap["layers"][0]["gridCellsY"].asInt();
				reg->tilemaps[e].gridCellW = tilemap["layers"][0]["gridCellWidth"].asInt();
				reg->tilemaps[e].gridCellH = tilemap["layers"][0]["gridCellHeight"].asInt();

				reg->tilemaps[e].grid = (std::pair<int, int>**)malloc(reg->tilemaps[e].gridCellsY * sizeof(std::pair<int, int>*));

				int** colCheck = (int**)malloc(reg->tilemaps[e].gridCellsY * sizeof(int*));
				for (int y = 0; y < reg->tilemaps[e].gridCellsY; y++)
				{
					colCheck[y] = (int*)malloc(reg->tilemaps[e].gridCellsX * sizeof(int));
				}
				for (int y = 0; y < reg->tilemaps[e].gridCellsY; y++)
				{
					reg->tilemaps[e].grid[y] = (std::pair<int, int>*)malloc(reg->tilemaps[e].gridCellsX * sizeof(std::pair<int, int>));
					for (int x = 0; x < reg->tilemaps[e].gridCellsX; x++)
					{
						for (int i = 0; i < tilemap["layers"].size(); i++)
						{
							if (tilemap["layers"][i]["grid2D"][y][x] == "1")
							{
								Entity l = EntityManager::Instance()->CreateEntity();

								reg->AABBColliders[l] = AABBCollider_Component{ (double)reg->tilemaps[e].gridCellW, (double)reg->tilemaps[e].gridCellH, 0, 0 };
								reg->transforms[l] = Transform_Component{ Vector2D(x * reg->tilemaps[e].gridCellW, y * reg->tilemaps[e].gridCellH) };
								reg->rigidbodies[l] = Rigidbody_Component{ Vector2D(0, 0), new double(0), 10.f, 0, 0, A, Layers(), true};
							}
							if (tilemap["layers"][i]["name"].asCString()[0] == 't' && tilemap["layers"][i]["name"].asCString()[1] == '_')
							{
								if (tilemap["layers"][i]["dataCoords2D"][y][x].size() < 2)
								{
									reg->tilemaps[e].grid[y][x].first = -1;
									reg->tilemaps[e].grid[y][x].second = -1;
								}
								else
								{
									reg->tilemaps[e].grid[y][x].first = tilemap["layers"][i]["dataCoords2D"][y][x][0].asInt();
									reg->tilemaps[e].grid[y][x].second = tilemap["layers"][i]["dataCoords2D"][y][x][1].asInt();
								}
							}
							
						}
					}
				}

				std::cout << reg->lineColliders.size() << std::endl;

				reg->tilemaps[e].tileset.texture = IMG_LoadTexture(renderer, reg->tilemaps[e].tileset.tilesetPath.c_str());
			}
		}
	}
	void Tilemap_System::Draw(Registry* reg, SDL_Renderer* renderer)
	{
		for (int e = 1; e <= EntityManager::Instance()->num_entities; e++)
		{
			if (reg->tilemaps.count(e))
			{
				for (int y = 0; y < reg->tilemaps[e].gridCellsY; y++)
				{
					for (int x = 0; x < reg->tilemaps[e].gridCellsX; x++)
					{
						if (reg->tilemaps[e].grid[y][x].first != -1)
						{
							if (reg->transforms.count(e))
							{
								SDL_RenderCopy(renderer, reg->tilemaps[e].tileset.texture,
									new SDL_Rect{ reg->tilemaps[e].grid[y][x].first * reg->tilemaps[e].tileset.tileW, reg->tilemaps[e].grid[y][x].second * reg->tilemaps[e].tileset.tileH, reg->tilemaps[e].tileset.tileW, reg->tilemaps[e].tileset.tileH },
									new SDL_Rect{ (int)reg->transforms[e].position.x + x * reg->tilemaps[e].gridCellW, (int)reg->transforms[e].position.y + y * reg->tilemaps[e].gridCellH, reg->tilemaps[e].gridCellW, reg->tilemaps[e].gridCellH });
							}
							else
							{
								SDL_RenderCopy(renderer, reg->tilemaps[e].tileset.texture,
									new SDL_Rect{ reg->tilemaps[e].grid[y][x].first * reg->tilemaps[e].tileset.tileW, reg->tilemaps[e].grid[y][x].second * reg->tilemaps[e].tileset.tileH, reg->tilemaps[e].tileset.tileW, reg->tilemaps[e].tileset.tileH },
									new SDL_Rect{ (int)reg->transforms[e].position.x + x * reg->tilemaps[e].gridCellW, (int)reg->transforms[e].position.y + y * reg->tilemaps[e].gridCellH, reg->tilemaps[e].gridCellW, reg->tilemaps[e].gridCellH });
							}
						}
					}
				}
			}
		}
	}
}