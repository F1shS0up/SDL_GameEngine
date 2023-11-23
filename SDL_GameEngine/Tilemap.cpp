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
						if (tilemap["layers"][0]["dataCoords2D"][y][x].size() < 2)
						{
							reg->tilemaps[e].grid[y][x].first = -1;
							reg->tilemaps[e].grid[y][x].second = -1;
						}
						else
						{
							reg->tilemaps[e].grid[y][x].first = tilemap["layers"][0]["dataCoords2D"][y][x][0].asInt();
							reg->tilemaps[e].grid[y][x].second = tilemap["layers"][0]["dataCoords2D"][y][x][1].asInt();
						}
						if (tilemap["layers"][1]["grid2D"][y][x] == "1" && colCheck[y][x] == 0)
						{
							Vector2DInt dir = Vector2DInt(1, 0);
							Vector2D A = Vector2D(x * reg->tilemaps[e].gridCellW,
								y * reg->tilemaps[e].gridCellH);
							Vector2D start = A;

							Vector2D B = Vector2D(x * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW,
								y * reg->tilemaps[e].gridCellH);

							std::cout << "Pos: " << A.x << " " << A.y << std::endl;

							int newX = x;
							int newY = y;

							while (true)
							{
								int increment = 1;
								std::cout << "UPDATE" << std::endl;
								while (tilemap["layers"][1]["grid2D"][newY + dir.y * increment][newX + dir.x * increment] == "1")
								{
									colCheck[newY + dir.y * increment][newX + dir.x * increment] = 1;

									std::cout << "Dir check" << std::endl;
									std::cout << newY + dir.y * increment << "    " << newX + dir.x * increment << std::endl;
									std::cout << newY << "    " << newX << std::endl;
									std::cout << dir.y << "    " << dir.x << std::endl;
									std::cout << increment << std::endl;

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
									std::cout << "Dir check End" << std::endl;
									
									increment++;

								}

								Entity l = EntityManager::Instance()->CreateEntity();
								reg->lineColliders[l] = LineCollider_Component{ A, B };
								A = B;

								if (Vector2D(B.x, B.y) == start) break;

								std::cout << "Pos: " << B.x << " " << B.y << std::endl;

								newX = newX + dir.x * (increment - 1);
								newY = newY + dir.y * (increment - 1);
								if (newY != 0 && tilemap["layers"][1]["grid2D"][newY - 1][newX] == "1" && dir.y == 0)
								{
									// top
									std::cout << "Top" << std::endl;
									dir = Vector2DInt(0, -1);
									
									
								}
								else if (newY != reg->tilemaps[e].gridCellsY - 1 && tilemap["layers"][1]["grid2D"][newY + 1][newX] == "1" && dir.y == 0)
								{
									// down
									std::cout << "Down" << std::endl;
									dir = Vector2DInt(0, 1);

								}
								else if (newX != 0 && tilemap["layers"][1]["grid2D"][newY][newX - 1] == "1" && dir.x == 0)
								{
									// leftB
									std::cout << "Left" << std::endl;
									dir = Vector2DInt(-1, 0);

								}
								else if (newX != reg->tilemaps[e].gridCellsX - 1 && tilemap["layers"][1]["grid2D"][newY][newX + 1] == "1" && dir.x == 0)
								{
									// right
									std::cout << "Right" << std::endl;
									dir = Vector2DInt(1, 0);

								}


								std::cout << "Dir update" << std::endl;

							}


							////TODO Create more optimized way by linking line collider next to each other...
							//if (y == 0 || tilemap["layers"][1]["grid2D"][y - 1][x] == "0")
							//{
							//	// top
							//	Vector2D A = Vector2D(x * reg->tilemaps[e].gridCellW, 
							//		y * reg->tilemaps[e].gridCellH);
							//	Vector2D B = Vector2D(x * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW, 
							//		y * reg->tilemaps[e].gridCellH);

							//	Entity l = EntityManager::Instance()->CreateEntity();
							//	reg->lineColliders[l] = LineCollider_Component{A, B};
							//}
							//if (y == reg->tilemaps[e].gridCellsY - 1 || tilemap["layers"][1]["grid2D"][y + 1][x] == "0")
							//{
							//	// down
							//	Vector2D A = Vector2D(x * reg->tilemaps[e].gridCellW, 
							//		y * reg->tilemaps[e].gridCellH + reg->tilemaps[e].gridCellH);
							//	Vector2D B = Vector2D(x * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW, 
							//		y * reg->tilemaps[e].gridCellH + reg->tilemaps[e].gridCellH);

							//	Entity l = EntityManager::Instance()->CreateEntity();
							//	reg->lineColliders[l] = LineCollider_Component{ A, B };
							//}

							//if (x == 0 || tilemap["layers"][1]["grid2D"][y][x - 1] == "0")
							//{
							//	// left
							//	Vector2D A = Vector2D(x * reg->tilemaps[e].gridCellW,
							//		y * reg->tilemaps[e].gridCellH);
							//	Vector2D B = Vector2D(x * reg->tilemaps[e].gridCellW,
							//		y * reg->tilemaps[e].gridCellH + reg->tilemaps[e].gridCellH);

							//	Entity l = EntityManager::Instance()->CreateEntity();
							//	reg->lineColliders[l] = LineCollider_Component{ A, B };
							//}
							//if (x == reg->tilemaps[e].gridCellsX - 1 || tilemap["layers"][1]["grid2D"][y][x + 1] == "0")
							//{
							//	// right
							//	Vector2D A = Vector2D(x * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW,
							//		y * reg->tilemaps[e].gridCellH);
							//	Vector2D B = Vector2D(x * reg->tilemaps[e].gridCellW + reg->tilemaps[e].gridCellW,
							//		y * reg->tilemaps[e].gridCellH + reg->tilemaps[e].gridCellH);

							//	Entity l = EntityManager::Instance()->CreateEntity();
							//	reg->lineColliders[l] = LineCollider_Component{ A, B };
							//}
						}
						
					}
				}

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
