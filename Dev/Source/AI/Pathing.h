#pragma once
//========================================================================
// Pathing.h : Pathing classes
// Author: David "Rez" Graham
//
// Part of the GameCode3 Application
//
// GameCode3 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 3rd Edition" by Mike McShaffry, published by
// Charles River Media. ISBN-10: 1-58450-680-6   ISBN-13: 978-1-58450-680-5
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the author a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: 
//    http://www.amazon.com/gp/product/1584506806?ie=UTF8&tag=gamecodecompl-20&linkCode=as2&camp=1789&creative=390957&creativeASIN=1584506806
//
// There's a companion web site at http://www.mcshaffry.com/GameCode/
// 
// The source code is managed and maintained through Google Code: http://gamecode3.googlecode.com/svn/trunk/
//
// (c) Copyright 2009 Michael L. McShaffry
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License v2
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================

//========================================================================
//  Content References in Game Coding Complete 3rd Edition
// 
//========================================================================

class PathingArc;
class PathingNode;
class PathPlanNode;
class AStar;

typedef std::list<PathingArc*> PathingArcList;
typedef std::list<PathingNode*> PathingNodeList;
typedef std::vector<PathingNode*> PathingNodeVec;
typedef std::map<PathingNode*, PathPlanNode*> PathingNodeToPathPlanNodeMap;
typedef std::list<PathPlanNode*> PathPlanNodeList;

const float PATHING_DEFAULT_NODE_TOLERANCE = 5.0f;
const float PATHING_DEFAULT_ARC_WEIGHT = 1.0f;

//--------------------------------------------------------------------------------------------------------
// class PathingNode				- Chapter 17, page 629
// This class represents a single node in the pathing graph.
//
//--------------------------------------------------------------------------------------------------------
class PathingNode
{
	//unsigned int m_id;  // this is the same as the index this node occupies in the m_nodes vector of PathingGraph
	float m_tolerance;
	Vec3 m_pos;
	PathingArcList m_arcs;
	
public:
	explicit PathingNode(const Vec3& pos, float tolerance = PATHING_DEFAULT_NODE_TOLERANCE) : m_pos(pos) { m_tolerance = tolerance; }
	//explicit PathingNode(unsigned int id, const Vec3& pos, float tolerance = PATHING_DEFAULT_NODE_TOLERANCE) : m_pos(pos) { m_id = id; m_tolerance = tolerance; }
	const Vec3& GetPos(void) const { return m_pos; }
	float GetTolerance(void) const { return m_tolerance; }
	void AddArc(PathingArc* pArc);
	void GetNeighbors(PathingNodeList& outNeighbors);
	float GetCostFromNode(PathingNode* pFromNode);
private:
	PathingArc* FindArc(PathingNode* pLinkedNode);
};


//--------------------------------------------------------------------------------------------------------
// class PathingArc					- Chatper 17, page 629
// This class represents an arc that links two nodes, allowing travel between them.
//--------------------------------------------------------------------------------------------------------
class PathingArc
{
	float m_weight;
	PathingNode* m_pNodes[2];  // an arc always connects two nodes

public:
	explicit PathingArc(float weight = PATHING_DEFAULT_ARC_WEIGHT;) { m_weight = weight; }
	float GetWeight(void) const { return m_weight; }
	void LinkNodes(PathingNode* pNodeA, PathingNode* pNodeB);
	PathingNode* GetNeighbor(PathingNode* pMe);
};


//--------------------------------------------------------------------------------------------------------
// class PathPlan						- Chapter 17, 632
// This class represents a complete path and is used by the higher-level AI to determine where to go.
//--------------------------------------------------------------------------------------------------------
class PathPlan
{
	friend class AStar;

	PathingNodeList m_path;
	PathingNodeList::iterator m_index;
	
public:
	PathPlan(void) { m_index = m_path.end(); }
	
	void ResetPath(void) { m_index = m_path.begin(); }
	const Vec3& GetCurrentNodePosition(void) const { assert(m_index != m_path.end()); return (*m_index)->GetPos(); }
	bool CheckForNextNode(const Vec3& pos);
	bool CheckForEnd(void);
	
private:
	void AddNode(PathingNode* pNode);
};


//--------------------------------------------------------------------------------------------------------
// class PathPlanNode						- Chapter 17, page 634
// This class is a helper used in PathingGraph::FindPath().  It tracks the heuristical and cost data for
// a given node when building a path.
//--------------------------------------------------------------------------------------------------------
class PathPlanNode
{
	PathPlanNode* m_pPrev;  // node we just came from
	PathingNode* m_pPathingNode;  // pointer to the pathing node from the pathing graph
	PathingNode* m_pGoalNode;  // pointer to the goal node
	bool m_closed;  // the node is closed if it's already been processed
	float m_goal;  // cost of the entire path up to this point (often called g)
	float m_heuristic;  // estimated cost of this node to the goal (often called h)
	float m_fitness;  // estimated cost from start to the goal through this node (often called f)
	
public:
	explicit PathPlanNode(PathingNode* pNode, PathPlanNode* pPrevNode, PathingNode* pGoalNode);
	PathPlanNode* GetPrev(void) const { return m_pPrev; }
	PathingNode* GetPathingNode(void) const { return m_pPathingNode; }
	bool IsClosed(void) const { return m_closed; }
	float GetGoal(void) const { return m_goal; }
	float GetHeuristic(void) const { return m_heuristic; }
	float GetFitness(void) const { return m_fitness; }
	
	void UpdatePrevNode(PathPlanNode* pPrev);
	void SetClosed(bool toClose = true) { m_closed = toClose; }
	
	bool operator<(PathPlanNode* pRight) { return (m_fitness < pRight->GetFitness()); }
	
private:
	void UpdateHeuristics(void);
};


//--------------------------------------------------------------------------------------------------------
// class AStar								- Chapter 17, page 635
// This class implements the A* algorithm.
//--------------------------------------------------------------------------------------------------------
class AStar
{
	PathingNodeToPathPlanNodeMap m_nodes;
	PathingNode* m_pStartNode;
	PathingNode* m_pGoalNode;
	PathPlanNodeList m_openSet;
	
public:
	AStar(void);
	~AStar(void);
	void Destroy(void);
	
	PathPlan* operator()(PathingNode* pStartNode, PathingNode* pGoalNode);

private:
	PathPlanNode* AddToOpenSet(PathingNode* pNode, PathPlanNode* pPrevNode);
	void AddToClosedSet(PathPlanNode* pNode);
	void InsertNode(PathPlanNode* pNode);
	void ReinsertNode(PathPlanNode* pNode);
	PathPlan* RebuildPath(PathPlanNode* pGoalNode);
};


//--------------------------------------------------------------------------------------------------------
// class PathingGraph					- Chapter 17, 630
// This class is the main interface into the pathing system.  It holds the pathing graph itself and owns
// all the PathingNode and Pathing Arc objects.  There is only one instance of PathingGraph, which lives
// in TeapotWarsGame.
//--------------------------------------------------------------------------------------------------------
class PathingGraph
{
	PathingNodeVec m_nodes;  // master list of all nodes
	PathingArcList m_arcs;  // master list of all arcs
	
public:
	PathingGraph(void) {}
	~PathingGraph(void) { DestroyGraph(); }
	//bool LoadGraph(const std::string& path);  // TODO rez
	void DestroyGraph(void);
	
	PathingNode* FindClosestNode(const Vec3& pos);
	PathingNode* FindFurthestNode(const Vec3& pos);
	PathingNode* FindRandomNode(void);
	PathPlan* FindPath(const Vec3& startPoint, const Vec3& endPoint);
	PathPlan* FindPath(const Vec3& startPoint, PathingNode* pGoalNode);
	PathPlan* FindPath(PathingNode* pStartNode, const Vec3& endPoint);
	PathPlan* FindPath(PathingNode* pStartNode, PathingNode* pGoalNode);
	
	// debug functions
	void BuildTestGraph(void);
	
private:
	// helpers
	void LinkNodes(PathingNode* pNodeA, PathingNode* pNodeB);
};

// Global function for creating & initializing the pathing graph.
PathingGraph* CreatePathingGraph(void);

