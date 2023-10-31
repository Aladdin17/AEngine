#pragma once

#include "AEngine/Render/VertexArray.h"
#include "AEngine/Math/Math.h"
#include "AEngine/Render/Shader.h"
#include "AEngine/Core/PerspectiveCamera.h"
#include "AEngine/Resource/Asset.h"
#include <vector>
#include <map>

namespace AEngine
{
    struct Node
    {
        int x, y;
        int gCost = 0; // Distance from start node
        int hCost = 0; // Distance from end node
        Node* parent = nullptr;
        bool isActive = false;

        bool operator==(const Node& other) const
        {
            return this->x == other.x && this->y == other.y;
        }
    };

    class Grid : public Asset
    {
    public:
        Grid(Math::ivec2 m_gridSize, float tileSize, Math::vec3 position);
        Grid(const std::string& ident, const std::string& path);
        ~Grid() = default;

        void GenerateGrid();
	    void ResizeGrid(Math::ivec2 m_gridSize, float tileSize, Math::vec3 position);

        Math::ivec2 GetGridSize();
        float GetTileSize();
        Math::vec3 GetPosition();


	    void ToggleObstacle(const std::string& id, bool enabled);
	    void AddObstacle(const Math::vec3& position, const std::string& id, int radius);
        void SaveToFile(const std::string& path);

        bool IsActive(int row, int coloumn);
        void SetActive(int row, int coloumn);
	    std::vector<float> GetAStarPath(Math::vec3 start, Math::vec3 end, bool checkNeighbours);
        void DebugRender(const PerspectiveCamera* camera);

        static SharedPtr<Grid> Create(Math::ivec2 m_gridSize, float tileSize, Math::vec3 position);
        static SharedPtr<Grid> Create(const std::string& ident, const std::string& path);

    private:
    	std::vector<Math::vec3> Grid::SimplifyPath(std::vector<Node>& path);
        std::vector<float> AStar(Node start, Node end);
	    std::vector<Node> GetNeighbours(Node& current);
	    int GetDistance(Node nodeA, Node nodeB);
        void LoadFromFile(const std::string& path);
        Math::ivec2 GetTile(const Math::vec3& position, bool checkNeighbours);

        float m_tileOffset = 0.1f;
        Math::ivec2 m_gridSize;
        float m_tileSize;
        Math::vec3 m_position;
        std::vector<std::vector<Node>> m_grid;
        std::map<std::string, std::vector<std::pair<int, int>>> m_obstacles;
        SharedPtr<VertexArray> m_debugGrid;
        SharedPtr<Shader> m_debugShader;

    };
}