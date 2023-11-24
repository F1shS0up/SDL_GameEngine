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
						for(int i = 0; i < tilemap["layers"].size(); i++)
						{
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
							else if (tilemap["layers"][i]["name"].asCString()[0] == 'c' && tilemap["layers"][i]["name"].asCString()[1] == '_')
							{
								if (tilemap["layers"][i]["grid2D"][y][x] == "1" && colCheck[y][x] == 0)
								{
									Vector2DInt dir = Vector2DInt(1, 0);
									Vector2D A = Vector2D(x * reg->tilemaps[e].gridCellW,
										y * reg->tilemaps[e].gridCellH);
									Vector2D start = A;

									Vector2D B = Vector2D(x * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW,
										y * reg->tilemaps[e].gridCellH);

									std::cout << "Pos: " << A.x << " " << A.y << std::endl;

									int newX = int(x);
									int newY = int(y);

									while (true)
									{
										int increment = 1;

										std::cout << "X:" << newX + dir.x * increment << "Y:" << newY + dir.y * increment << std::endl;

										while (newX + dir.x * increment >= 0 && newX + dir.x * increment < reg->tilemaps[e].gridCellsX && newY + dir.y * increment >= 0 && newY + dir.y * increment < reg->tilemaps[e].gridCellsY && tilemap["layers"][i]["grid2D"][newY + dir.y * increment][newX + dir.x * increment] == "1")
										{
											std::cout << "UWU" << std::endl;
											colCheck[newY + dir.y * increment][newX + dir.x * increment] = 1;

											if (dir.x > 0)
											{
												B = Vector2D((newX + dir.x * increment) * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW,
													(newY + dir.y * increment) * reg->tilemaps[e].gridCellH);
											}
											else if (dir.x < 0)
											{
												B = Vector2D((newX + dir.x * increment) * reg->tilemaps[e].gridCellW,
													(newY + dir.y * increment) * reg->tilemaps[e].gridCellH + reg->tilemaps[e].gridCellH);
											}
											else if (dir.y > 0)
											{
												B = Vector2D((newX + dir.x * increment) * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW,
													(newY + dir.y * increment) * reg->tilemaps[e].gridCellH + reg->tilemaps[e].gridCellH);
											}
											else if (dir.y < 0)
											{
												B = Vector2D((newX + dir.x * increment) * reg->tilemaps[e].gridCellW,
													(newY + dir.y * increment) * reg->tilemaps[e].gridCellH);
											}

											increment++;
										}

										Entity l = EntityManager::Instance()->CreateEntity();
										reg->lineColliders[l] = LineCollider_Component{ A, B };
										A = B;

										if (Vector2D(B.x, B.y) == start) break;

										newX = newX + dir.x * (increment - 1);
										newY = newY + dir.y * (increment - 1);
										std::cout << "X:" << newX << "Y:" << newY << std::endl;
										if (newY > 0 && tilemap["layers"][i]["grid2D"][newY - 1][newX] == "1" && dir.y == 0)
										{
											// top
											std::cout << "Top" << std::endl;
											dir = Vector2DInt(0, -1);


										}
										else if (newY < reg->tilemaps[e].gridCellsY - 1 && tilemap["layers"][i]["grid2D"][newY + 1][newX] == "1" && dir.y == 0)
										{
											// down
											std::cout << "Down" << std::endl;
											dir = Vector2DInt(0, 1);

										}
										else if (newX > 0 && tilemap["layers"][i]["grid2D"][newY][newX - 1] == "1" && dir.x == 0)
										{
											// leftB
											std::cout << "Left" << std::endl;
											dir = Vector2DInt(-1, 0);

										}
										else if (newX < reg->tilemaps[e].gridCellsX - 1 && tilemap["layers"][i]["grid2D"][newY][newX + 1] == "1" && dir.x == 0)
										{
											// right
											std::cout << "Right" << std::endl;
											dir = Vector2DInt(1, 0);

										}
										else if (dir.x < 0)
										{
											// top
											std::cout << "Top" << std::endl;
											dir = Vector2DInt(0, -1);
											B = A + Vector2D(0, -reg->tilemaps[e].gridCellH);
										}
										else if (dir.x > 0)
										{
											// down
											std::cout << "Down" << std::endl;
											dir = Vector2DInt(0, 1);
											B = A + Vector2D(0, reg->tilemaps[e].gridCellH);
										}
										else if (dir.y < 0)
										{
											// left
											std::cout << "Left" << std::endl;
											dir = Vector2DInt(-1, 0);
											B = A + Vector2D(-reg->tilemaps[e].gridCellW, 0);
										}
										else if (dir.y > 0)
										{
											// right
											std::cout << "Right" << std::endl;
											dir = Vector2DInt(1, 0);
											B = A + Vector2D(reg->tilemaps[e].gridCellW, 0);

										}

									}
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
							SDL_RenderCopy(renderer, reg->tilemaps[e].tileset.texture, 
								new SDL_Rect{ reg->tilemaps[e].grid[y][x].first * reg->tilemaps[e].tileset.tileW, reg->tilemaps[e].grid[y][x].second * reg->tilemaps[e].tileset.tileH, reg->tilemaps[e].tileset.tileW, reg->tilemaps[e].tileset.tileH },
								new SDL_Rect{ x * reg->tilemaps[e].gridCellW, y * reg->tilemaps[e].gridCellH, reg->tilemaps[e].gridCellW, reg->tilemaps[e].gridCellH });
						}
					}
				}
			}
		}
	}
}
