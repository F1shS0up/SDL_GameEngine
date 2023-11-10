#pragma once
#include "Vector2D.h"
#include <vector>
#include "Softbody.h"

enum KDTreeType
{
	Normal, Softbody
};



struct Node
{
	bool nextDivisionOnXAxis;
	std::vector<Vector2D*> pointsInNodeInXOrder;
	std::vector<Vector2D*> pointsInNodeInYOrder;
};
struct SoftbodyNode
{
	bool nextDivisionOnXAxis;
	std::vector<std::pair<Softbody_Component*, Vector2D*>> pointsInNodeInXOrder;
	std::vector<std::pair<Softbody_Component*, Vector2D*>> pointsInNodeInYOrder;
};
class KDTree
{
public:
	KDTree(std::vector<Vector2D*> points);
	KDTree(Vector2D** points);
	KDTree(std::vector<MassPoint*> massPoints);
	KDTree(std::vector<Softbody_Component*> massPoints);

	void BuildKDTree(int treshold);
	std::vector<SoftbodyNode*> BuildKDTreeSoftbody(int treshold);


	void BuildKDTreeDebug(int treshold, SDL_Renderer* renderer);

private:
	

	std::vector<Vector2D*> points;

	std::vector<std::pair<Softbody_Component*, Vector2D*>> softbodiesAndTheirMassPointPositions;
};