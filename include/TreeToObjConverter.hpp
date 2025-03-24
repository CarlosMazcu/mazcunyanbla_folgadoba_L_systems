#ifndef TREE_TO_OBJ_CONVERTER_HPP
#define TREE_TO_OBJ_CONVERTER_HPP

#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>
#include <random>
#include <ctime>
#ifndef M_PI
#   define M_PI 3.1415926535897932384626433832
#endif
// Estructura para representar un punto 3D
struct Point3D {
    float x, y, z;
    Point3D(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
};

// Estructura para representar un segmento (rama o tronco)
struct Segment3D {
    Point3D start;
    Point3D end;
    float thickness;
    Segment3D(Point3D _start, Point3D _end, float _thickness)
        : start(_start), end(_end), thickness(_thickness) {}
};

// Estructura para la superficie de un obj
struct Face {
    std::vector<int> vertexIndices;
    Face() {}
    Face(std::initializer_list<int> indices) : vertexIndices(indices) {}
};

enum class LeafType {
    SIMPLE,     // Hoja simple (diamante)
    NEEDLE,     // Acicular (coníferas)
    BROAD,      // Hoja ancha (frondosas)
    PALM        // Hoja de palmera
};

class TreeToObjConverter {
private:
    std::vector<Segment3D> segments;
    std::vector<Point3D> leafPoints;
    std::string outputFilename;
    int cylinderResolution;
    float leafSize;
    bool generateLeaves;
    float randomnessFactor;
    float branchMultiplier;
    LeafType leafType;
    std::mt19937 rng;


public:
    TreeToObjConverter(const std::string& filename = "tree3d.obj", int resolution = 8)
        : outputFilename(filename), cylinderResolution(resolution), leafSize(0.5f), generateLeaves(true), branchMultiplier(1.0f) {}

    // Métodos para configurar el conversor
    void setLeafSize(float size) { leafSize = size; }
    void setGenerateLeaves(bool generate) { generateLeaves = generate; }
    void setCylinderResolution(int resolution) { cylinderResolution = resolution; }
    void setRandomnessFactor(float factor) { randomnessFactor = factor; }
    void setBranchRadiusMultiplier(float multiplier) { branchMultiplier = multiplier; }
    void setLeafType(LeafType type) { leafType = type; }
    LeafType getLeafType() const { return leafType; }

    static std::vector<std::string> getLeafTypeNames() {
        return {
            "Simple",
            "Acicular",
            "Frondosas",
            "Palmera"
        };
    };


    // Método para convertir una cadena L-system en un modelo 3D
    bool convertLSystemToObj(const std::string& lSystemString, float initialAngle, float initialLength, float initialWidth);

    // Método para generar el archivo OBJ
    bool generateOBJ(std::atomic<float>& progress);
    bool generateOBJ();

private:
    // Métodos auxiliares para la conversión
    void addCylinder(const Segment3D& segment, std::vector<Point3D>& vertices, std::vector<Face>& faces);
    void addLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces);

    // Método para calcular vectores perpendiculares a una dirección dada
    void calculatePerpendicularVectors(const Point3D& direction, Point3D& perpendicular1, Point3D& perpendicular2);

    // Método para normalizar un vector
    Point3D normalizeVector(const Point3D& vector);

    void addSimpleLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces);
    void addNeedleLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces);
    void addBroadLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces);
    void addPalmLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces);

    // Método auxiliar para calcular orientación de hoja
    void calculateLeafOrientation(const Point3D& position, Point3D& right, Point3D& up, Point3D& forward);

    // Utilidad para números aleatorios
    float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
};

#endif // TREE_TO_OBJ_CONVERTER_HPP