#ifndef TREE_TO_OBJ_CONVERTER_HPP
#define TREE_TO_OBJ_CONVERTER_HPP

#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>
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

class TreeToObjConverter {
private:
    std::vector<Segment3D> segments;
    std::vector<Point3D> leafPoints;
    std::string outputFilename;
    int cylinderResolution;
    float leafSize;
    bool generateLeaves;
    float randomnessFactor;

public:
    TreeToObjConverter(const std::string& filename = "tree3d.obj", int resolution = 8)
        : outputFilename(filename), cylinderResolution(resolution), leafSize(0.5f), generateLeaves(true) {}

    // Métodos para configurar el conversor
    void setLeafSize(float size) { leafSize = size; }
    void setGenerateLeaves(bool generate) { generateLeaves = generate; }
    void setCylinderResolution(int resolution) { cylinderResolution = resolution; }
    void setRandomnessFactor(float factor) { randomnessFactor = factor; }

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
};

#endif // TREE_TO_OBJ_CONVERTER_HPP