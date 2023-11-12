#include "KDTree.h"
#include <algorithm>
#include <cmath>
#include <iostream>
namespace Engine
{
	KDTree::KDTree(std::vector<MassPoint*> massPoints)
	{
		for (int i = 0; i < massPoints.size();i++)
		{
			points.push_back(&massPoints[i]->position);
		}
	}
	KDTree::KDTree(std::vector<Vector2D*> points)
	{
		this->points = points;
	}

	void KDTree::BuildKDTree(int treshold)
	{
		std::vector<Vector2D*> copyOfPointsOrganizedOnX;
		std::vector<Vector2D*> copyOfPointsOrganizedOnY;

		copyOfPointsOrganizedOnX = points;
		copyOfPointsOrganizedOnY = points;

		std::sort(copyOfPointsOrganizedOnX.begin(), copyOfPointsOrganizedOnX.end(), [](Vector2D* v, Vector2D* v2) {return (v->x == v2->x || v->x < v2->x);});
		std::sort(copyOfPointsOrganizedOnY.begin(), copyOfPointsOrganizedOnY.end(), [](Vector2D* v, Vector2D* v2) {return (v->y == v2->y || v->y < v2->y);});

		Node parentNode = Node{ true, copyOfPointsOrganizedOnX , copyOfPointsOrganizedOnY };
		std::vector<Node*> activeNodes = { &parentNode };
		std::vector<Node*> finalNodes = { };

		for (Vector2D* v : copyOfPointsOrganizedOnX)
		{
			std::cout << v->x << " " << v->y << std::endl;
		}

		while (true)
		{
			std::vector<Node*> newActiveNodes;
			for (int i = 0; i < activeNodes.size(); i++)
			{

				if ((activeNodes[i]->nextDivisionOnXAxis && activeNodes[i]->pointsInNodeInYOrder.size() <= treshold) || (!activeNodes[i]->nextDivisionOnXAxis && activeNodes[i]->pointsInNodeInXOrder.size() <= treshold))
				{
					finalNodes.push_back(activeNodes[i]);
					continue;
				}
				Node one;
				Node two;

				if (activeNodes[i]->nextDivisionOnXAxis)
				{
					float median = (float)activeNodes[i]->pointsInNodeInXOrder.size() / 2.0f - 0.5f;

					int medianIndex = std::floor(median);
					float value = activeNodes[i]->pointsInNodeInXOrder[medianIndex]->x + (activeNodes[i]->pointsInNodeInXOrder[medianIndex + 1]->x - activeNodes[i]->pointsInNodeInXOrder[medianIndex]->x) * (median - medianIndex);
					std::cout << "X: " << value << std::endl;
					std::cout << "X: " << median << std::endl;
					std::vector<Vector2D*> p1X, p2X;
					std::vector<Vector2D*> p1Y, p2Y;
					for (int p = 0; p < activeNodes[i]->pointsInNodeInXOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInXOrder[p]->x >= value)
						{
							p1X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInXOrder[p]->x <= value)
						{
							p2X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
					}
					for (int p = 0; p < activeNodes[i]->pointsInNodeInYOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInYOrder[p]->x >= value)
						{
							p1Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInYOrder[p]->x <= value)
						{
							p2Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
					}
					one = Node{ !activeNodes[i]->nextDivisionOnXAxis, p1X, p1Y };
					two = Node{ !activeNodes[i]->nextDivisionOnXAxis, p2X, p2Y };
					newActiveNodes.push_back(new Node(one));
					newActiveNodes.push_back(new Node(two));
				}
				else
				{
					float median = (float)activeNodes[i]->pointsInNodeInYOrder.size() / 2.0f - 0.5f;

					int medianIndex = std::floor(median);
					float value = activeNodes[i]->pointsInNodeInYOrder[medianIndex]->y + (activeNodes[i]->pointsInNodeInYOrder[medianIndex + 1]->y - activeNodes[i]->pointsInNodeInYOrder[medianIndex]->y) * (median - medianIndex);
					std::cout << "Y: " << value << std::endl;
					std::cout << "Y index: " << median << std::endl;
					std::cout << "Y median: " << activeNodes[i]->pointsInNodeInYOrder[medianIndex]->y << std::endl;
					std::vector<Vector2D*> p1X, p2X;
					std::vector<Vector2D*> p1Y, p2Y;
					for (int p = 0; p < activeNodes[i]->pointsInNodeInXOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInXOrder[p]->y >= value)
						{
							p1X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInXOrder[p]->y <= value)
						{
							p2X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
					}
					for (int p = 0; p < activeNodes[i]->pointsInNodeInYOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInYOrder[p]->y >= value)
						{
							p1Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInYOrder[p]->y <= value)
						{
							p2Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
					}
					one = Node{ !activeNodes[i]->nextDivisionOnXAxis, p1X, p1Y };
					two = Node{ !activeNodes[i]->nextDivisionOnXAxis, p2X, p2Y };
					newActiveNodes.push_back(new Node(one));
					newActiveNodes.push_back(new Node(two));
				}

			}
			if (newActiveNodes.size() <= 0)
			{
				break;
			}
			activeNodes = newActiveNodes;
		}

		std::cout << finalNodes.size() << std::endl;
	}

	std::vector<SoftbodyNode*> KDTree::BuildKDTreeSoftbody(int treshold)
	{
		std::vector<std::pair<Softbody_Component*, Vector2D*>> copyOfPointsOrganizedOnX;
		std::vector<std::pair<Softbody_Component*, Vector2D*>> copyOfPointsOrganizedOnY;

		copyOfPointsOrganizedOnX = softbodiesAndTheirMassPointPositions;
		copyOfPointsOrganizedOnY = softbodiesAndTheirMassPointPositions;

		std::sort(copyOfPointsOrganizedOnX.begin(), copyOfPointsOrganizedOnX.end(), [](std::pair<Softbody_Component*, Vector2D*> v, std::pair<Softbody_Component*, Vector2D*> v2) {return (v.second->x == v2.second->x || v.second->x < v2.second->x);});
		std::sort(copyOfPointsOrganizedOnY.begin(), copyOfPointsOrganizedOnY.end(), [](std::pair<Softbody_Component*, Vector2D*> v, std::pair<Softbody_Component*, Vector2D*> v2) {return (v.second->y == v2.second->y || v.second->y < v2.second->y);});

		SoftbodyNode parentNode = SoftbodyNode{ true, copyOfPointsOrganizedOnX , copyOfPointsOrganizedOnY };
		std::vector<SoftbodyNode*> activeNodes = { &parentNode };
		std::vector<SoftbodyNode*> finalNodes = { };

		for (std::pair<Softbody_Component*, Vector2D*> v : copyOfPointsOrganizedOnX)
		{
			std::cout << v.second->x << " " << v.second->y << std::endl;
		}

		while (true)
		{
			std::vector<SoftbodyNode*> newActiveNodes;
			for (int i = 0; i < activeNodes.size(); i++)
			{
				if ((activeNodes[i]->nextDivisionOnXAxis && activeNodes[i]->pointsInNodeInYOrder.size() <= treshold) || (!activeNodes[i]->nextDivisionOnXAxis && activeNodes[i]->pointsInNodeInXOrder.size() <= treshold))
				{
					finalNodes.push_back(activeNodes[i]);
					continue;
				}
				SoftbodyNode one;
				SoftbodyNode two;

				if (activeNodes[i]->nextDivisionOnXAxis)
				{
					float median = (float)activeNodes[i]->pointsInNodeInXOrder.size() / 2.0f - 0.5f;

					int medianIndex = std::floor(median);
					float value = activeNodes[i]->pointsInNodeInXOrder[medianIndex].second->x + (activeNodes[i]->pointsInNodeInXOrder[medianIndex + 1].second->x - activeNodes[i]->pointsInNodeInXOrder[medianIndex].second->x) * (median - medianIndex);
					std::cout << "X: " << value << std::endl;
					std::cout << "X: " << median << std::endl;
					std::vector<std::pair<Softbody_Component*, Vector2D*>> p1X, p2X;
					std::vector<std::pair<Softbody_Component*, Vector2D*>> p1Y, p2Y;
					for (int p = 0; p < activeNodes[i]->pointsInNodeInXOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInXOrder[p].second->x >= value)
						{
							p1X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInXOrder[p].second->x <= value)
						{
							p2X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
					}
					for (int p = 0; p < activeNodes[i]->pointsInNodeInYOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInYOrder[p].second->x >= value)
						{
							p1Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInYOrder[p].second->x <= value)
						{
							p2Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
					}
					one = SoftbodyNode{ !activeNodes[i]->nextDivisionOnXAxis, p1X, p1Y };
					two = SoftbodyNode{ !activeNodes[i]->nextDivisionOnXAxis, p2X, p2Y };
					newActiveNodes.push_back(new SoftbodyNode(one));
					newActiveNodes.push_back(new SoftbodyNode(two));
				}
				else
				{
					float median = (float)activeNodes[i]->pointsInNodeInYOrder.size() / 2.0f - 0.5f;

					int medianIndex = std::floor(median);
					float value = activeNodes[i]->pointsInNodeInYOrder[medianIndex].second->y + (activeNodes[i]->pointsInNodeInYOrder[medianIndex + 1].second->y - activeNodes[i]->pointsInNodeInYOrder[medianIndex].second->y) * (median - medianIndex);
					std::cout << "Y: " << value << std::endl;
					std::cout << "Y index: " << median << std::endl;
					std::cout << "Y median: " << activeNodes[i]->pointsInNodeInYOrder[medianIndex].second->y << std::endl;
					std::vector<std::pair<Softbody_Component*, Vector2D*>> p1X, p2X;
					std::vector<std::pair<Softbody_Component*, Vector2D*>> p1Y, p2Y;
					for (int p = 0; p < activeNodes[i]->pointsInNodeInXOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInXOrder[p].second->y >= value)
						{
							p1X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInXOrder[p].second->y <= value)
						{
							p2X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
						}
					}
					for (int p = 0; p < activeNodes[i]->pointsInNodeInYOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInYOrder[p].second->y >= value)
						{
							p1Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInYOrder[p].second->y <= value)
						{
							p2Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
					}
					one = SoftbodyNode{ !activeNodes[i]->nextDivisionOnXAxis, p1X, p1Y };
					two = SoftbodyNode{ !activeNodes[i]->nextDivisionOnXAxis, p2X, p2Y };
					newActiveNodes.push_back(new SoftbodyNode(one));
					newActiveNodes.push_back(new SoftbodyNode(two));
				}

			}
			if (newActiveNodes.size() <= 0)
			{
				break;
			}
			activeNodes = newActiveNodes;
		}

		std::cout << finalNodes.size() << std::endl;
		return finalNodes;
	}

	void KDTree::BuildKDTreeDebug(int treshold, SDL_Renderer* renderer)
	{
		std::vector<double> xDividedInOrder, yDividedInOrder;

		std::vector<Vector2D*> copyOfPointsOrganizedOnX;
		std::vector<Vector2D*> copyOfPointsOrganizedOnY;

		copyOfPointsOrganizedOnX = points;
		copyOfPointsOrganizedOnY = points;

		std::sort(copyOfPointsOrganizedOnX.begin(), copyOfPointsOrganizedOnX.end(), [](Vector2D* v, Vector2D* v2) {return (v->x == v2->x || v->x < v2->x);});
		std::sort(copyOfPointsOrganizedOnY.begin(), copyOfPointsOrganizedOnY.end(), [](Vector2D* v, Vector2D* v2) {return (v->y == v2->y || v->y < v2->y);});

		Node parentNode = Node{ true, copyOfPointsOrganizedOnX , copyOfPointsOrganizedOnY };
		std::vector<Node*> activeNodes = { &parentNode };
		std::vector<Node*> finalNodes = { &parentNode };

		for (Vector2D* v : copyOfPointsOrganizedOnX)
		{
			std::cout << v->x << " " << v->y << std::endl;
		}

		while (true)
		{
			std::vector<Node*> newActiveNodes;
			for (int i = 0; i < activeNodes.size(); i++)
			{

				if ((activeNodes[i]->nextDivisionOnXAxis && activeNodes[i]->pointsInNodeInYOrder.size() <= treshold) || (!activeNodes[i]->nextDivisionOnXAxis && activeNodes[i]->pointsInNodeInXOrder.size() <= treshold))
				{
					finalNodes.push_back(activeNodes[i]);
					continue;
				}
				Node one;
				Node two;

				if (activeNodes[i]->nextDivisionOnXAxis)
				{
					float median = (float)activeNodes[i]->pointsInNodeInXOrder.size() / 2.0f - 0.5f;

					int medianIndex = std::floor(median);
					float value = activeNodes[i]->pointsInNodeInXOrder[medianIndex]->x + (activeNodes[i]->pointsInNodeInXOrder[medianIndex + 1]->x - activeNodes[i]->pointsInNodeInXOrder[medianIndex]->x) * (median - medianIndex);
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					SDL_RenderDrawLine(renderer, value, -10000, value, 10000);
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					std::vector<Vector2D*> p1X, p2X;
					std::vector<Vector2D*> p1Y, p2Y;
					for (int p = 0; p < activeNodes[i]->pointsInNodeInXOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInXOrder[p]->x >= value)
						{
							p1X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
							p1Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInXOrder[p]->x <= value)
						{
							p2X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
							p2Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
					}
					one = Node{ !activeNodes[i]->nextDivisionOnXAxis, p1X, p1Y };
					two = Node{ !activeNodes[i]->nextDivisionOnXAxis, p2X, p2Y };
					newActiveNodes.push_back(new Node(one));
					newActiveNodes.push_back(new Node(two));
				}
				else
				{
					float median = (float)activeNodes[i]->pointsInNodeInYOrder.size() / 2.0f - 0.5f;

					int medianIndex = std::floor(median);
					float value = activeNodes[i]->pointsInNodeInYOrder[medianIndex]->y + (activeNodes[i]->pointsInNodeInYOrder[medianIndex + 1]->y - activeNodes[i]->pointsInNodeInYOrder[medianIndex]->y) * (median - medianIndex);
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					SDL_RenderDrawLine(renderer, -10000, value, 10000, value);
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					std::vector<Vector2D*> p1X, p2X;
					std::vector<Vector2D*> p1Y, p2Y;
					for (int p = 0; p < activeNodes[i]->pointsInNodeInYOrder.size(); p++)
					{
						if (activeNodes[i]->pointsInNodeInYOrder[p]->x >= value)
						{
							p1X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
							p1Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
						if (activeNodes[i]->pointsInNodeInYOrder[p]->x <= value)
						{
							p2X.push_back(activeNodes[i]->pointsInNodeInXOrder[p]);
							p2Y.push_back(activeNodes[i]->pointsInNodeInYOrder[p]);
						}
					}
					one = Node{ !activeNodes[i]->nextDivisionOnXAxis, p1X, p1Y };
					two = Node{ !activeNodes[i]->nextDivisionOnXAxis, p2X, p2Y };
					newActiveNodes.push_back(new Node(one));
					newActiveNodes.push_back(new Node(two));
				}

			}
			if (newActiveNodes.size() <= 0)
			{
				break;
			}
			activeNodes = newActiveNodes;
		}

		std::cout << finalNodes.size() << std::endl;
	}
}