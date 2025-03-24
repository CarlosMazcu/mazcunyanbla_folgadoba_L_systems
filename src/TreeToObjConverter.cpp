#include "TreeToObjConverter.hpp"


bool TreeToObjConverter::convertLSystemToObj(const std::string& lSystemString, float initialAngle, float initialLength, float initialWidth) {
    segments.clear();
    leafPoints.clear();

    std::stack<Point3D> positionStack;
    std::stack<Point3D> headingStack;    
    std::stack<Point3D> leftStack;       
    std::stack<Point3D> upStack;         
    std::stack<float> lineWidthStack;
    std::stack<float> lengthStack;
    std::stack<float> angleIncrementStack;

    const float PI = 3.1416f;
    const float degToRad = PI / 180.0f;

    Point3D currentPosition(0.0f, 0.0f, 0.0f);

   
    Point3D heading(0.0f, 1.0f, 0.0f);
    Point3D left(-1.0f, 0.0f, 0.0f);  
    Point3D up(0.0f, 0.0f, 1.0f);     

    float lineWidth = initialWidth * branchMultiplier;
    float currentLength = initialLength;
    float angleIncrement = initialAngle;
    bool invertTurns = false;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));


    for (char c : lSystemString) {

        switch (c) {
        case 'F': { 

            if (randomnessFactor > 0.0f && ((float)rand() / RAND_MAX) < 0.3f * randomnessFactor) {
                float smallRandomPitch = (((float)rand() / RAND_MAX) * 10.0f - 5.0f) * randomnessFactor * degToRad;
                float smallRandomYaw = (((float)rand() / RAND_MAX) * 10.0f - 5.0f) * randomnessFactor * degToRad;

                float cosPitch = cos(smallRandomPitch);
                float sinPitch = sin(smallRandomPitch);

                Point3D tempHeading(
                    heading.x * cosPitch + up.x * sinPitch,
                    heading.y * cosPitch + up.y * sinPitch,
                    heading.z * cosPitch + up.z * sinPitch
                );

                Point3D tempUp(
                    up.x * cosPitch - heading.x * sinPitch,
                    up.y * cosPitch - heading.y * sinPitch,
                    up.z * cosPitch - heading.z * sinPitch
                );

                heading = normalizeVector(tempHeading);
                up = normalizeVector(tempUp);

                float cosYaw = cos(smallRandomYaw);
                float sinYaw = sin(smallRandomYaw);

                tempHeading = Point3D(
                    heading.x * cosYaw + left.x * sinYaw,
                    heading.y * cosYaw + left.y * sinYaw,
                    heading.z * cosYaw + left.z * sinYaw
                );

                Point3D tempLeft(
                    left.x * cosYaw - heading.x * sinYaw,
                    left.y * cosYaw - heading.y * sinYaw,
                    left.z * cosYaw - heading.z * sinYaw
                );

                heading = normalizeVector(tempHeading);
                left = normalizeVector(tempLeft);

                up = normalizeVector(Point3D(
                    left.y * heading.z - left.z * heading.y,
                    left.z * heading.x - left.x * heading.z,
                    left.x * heading.y - left.y * heading.x
                ));
            }

            Point3D newPosition(
                currentPosition.x + heading.x * currentLength,
                currentPosition.y + heading.y * currentLength,
                currentPosition.z + heading.z * currentLength
            );

            segments.push_back(Segment3D(currentPosition, newPosition, lineWidth));

            currentPosition = newPosition;
            break;
        }
        case 'f': { 
            Point3D newPosition(
                currentPosition.x + heading.x * currentLength,
                currentPosition.y + heading.y * currentLength,
                currentPosition.z + heading.z * currentLength
            );
            currentPosition = newPosition;
            break;
        }
        case '+': { 
            float angle = (invertTurns ? -angleIncrement : angleIncrement) * degToRad;

            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;


            float yawAngle = angle * (1.0f - 0.5f * randomnessFactor);
            float cosYaw = cos(yawAngle);
            float sinYaw = sin(yawAngle);

            Point3D newHeading(
                heading.x* cosYaw + left.x * sinYaw,
                heading.y* cosYaw + left.y * sinYaw,
                heading.z* cosYaw + left.z * sinYaw
            );

            Point3D newLeft(
                left.x* cosYaw - heading.x * sinYaw,
                left.y* cosYaw - heading.y * sinYaw,
                left.z* cosYaw - heading.z * sinYaw
            );

          
            if (randomnessFactor > 0.0f) {
                float pitchIntensity = 0.7f * randomnessFactor;
                float pitchAngle = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * angle * pitchIntensity;

                float cosPitch = cos(pitchAngle);
                float sinPitch = sin(pitchAngle);

                Point3D tempHeading(
                    newHeading.x * cosPitch + up.x * sinPitch,
                    newHeading.y * cosPitch + up.y * sinPitch,
                    newHeading.z * cosPitch + up.z * sinPitch
                );

                Point3D tempUp(
                    up.x * cosPitch - newHeading.x * sinPitch,
                    up.y * cosPitch - newHeading.y * sinPitch,
                    up.z * cosPitch - newHeading.z * sinPitch
                );

                newHeading = tempHeading;
                up = tempUp;
            }

            if (randomnessFactor > 0.0f && ((float)rand() / RAND_MAX) < 0.3f * randomnessFactor) {
                float rollIntensity = 0.4f * randomnessFactor;
                float rollAngle = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * angle * rollIntensity;

                float cosRoll = cos(rollAngle);
                float sinRoll = sin(rollAngle);

                Point3D tempLeft(
                    newLeft.x * cosRoll + up.x * sinRoll,
                    newLeft.y * cosRoll + up.y * sinRoll,
                    newLeft.z * cosRoll + up.z * sinRoll
                );

                Point3D tempUp(
                    up.x * cosRoll - newLeft.x * sinRoll,
                    up.y * cosRoll - newLeft.y * sinRoll,
                    up.z * cosRoll - newLeft.z * sinRoll
                );

                newLeft = tempLeft;
                up = tempUp;
            }

            heading = normalizeVector(newHeading);
            left = normalizeVector(newLeft);
            up = normalizeVector(up);

            up = normalizeVector(Point3D(
                left.y * heading.z - left.z * heading.y,
                left.z * heading.x - left.x * heading.z,
                left.x * heading.y - left.y * heading.x
            ));
            left = normalizeVector(Point3D(
                up.y * heading.z - up.z * heading.y,
                up.z * heading.x - up.x * heading.z,
                up.x * heading.y - up.y * heading.x
            ));

            break;
        }
        case '-': {
            float angle = (invertTurns ? angleIncrement : -angleIncrement) * degToRad;
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            float yawAngle = angle * (1.0f - 0.5f * randomnessFactor);
            float cosYaw = cos(yawAngle);
            float sinYaw = sin(yawAngle);

            Point3D newHeading(
                heading.x * cosYaw + left.x * sinYaw,
                heading.y * cosYaw + left.y * sinYaw,
                heading.z * cosYaw + left.z * sinYaw
            );

            Point3D newLeft(
                left.x * cosYaw - heading.x * sinYaw,
                left.y * cosYaw - heading.y * sinYaw,
                left.z * cosYaw - heading.z * sinYaw
            );

            if (randomnessFactor > 0.0f) {
                float pitchIntensity = 0.7f * randomnessFactor;
                float pitchAngle = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * -angle * pitchIntensity;

                float cosPitch = cos(pitchAngle);
                float sinPitch = sin(pitchAngle);

                Point3D tempHeading(
                    newHeading.x * cosPitch + up.x * sinPitch,
                    newHeading.y * cosPitch + up.y * sinPitch,
                    newHeading.z * cosPitch + up.z * sinPitch
                );

                Point3D tempUp(
                    up.x * cosPitch - newHeading.x * sinPitch,
                    up.y * cosPitch - newHeading.y * sinPitch,
                    up.z * cosPitch - newHeading.z * sinPitch
                );

                newHeading = tempHeading;
                up = tempUp;
            }

            if (randomnessFactor > 0.0f && ((float)rand() / RAND_MAX) < 0.3f * randomnessFactor) {
                float rollIntensity = 0.4f * randomnessFactor;
                float rollAngle = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * -angle * rollIntensity;

                float cosRoll = cos(rollAngle);
                float sinRoll = sin(rollAngle);

                Point3D tempLeft(
                    newLeft.x * cosRoll + up.x * sinRoll,
                    newLeft.y * cosRoll + up.y * sinRoll,
                    newLeft.z * cosRoll + up.z * sinRoll
                );

                Point3D tempUp(
                    up.x * cosRoll - newLeft.x * sinRoll,
                    up.y * cosRoll - newLeft.y * sinRoll,
                    up.z * cosRoll - newLeft.z * sinRoll
                );

                newLeft = tempLeft;
                up = tempUp;
            }

            heading = normalizeVector(newHeading);
            left = normalizeVector(newLeft);
            up = normalizeVector(up);

            up = normalizeVector(Point3D(
                left.y * heading.z - left.z * heading.y,
                left.z * heading.x - left.x * heading.z,
                left.x * heading.y - left.y * heading.x
            ));
            left = normalizeVector(Point3D(
                up.y * heading.z - up.z * heading.y,
                up.z * heading.x - up.x * heading.z,
                up.x * heading.y - up.y * heading.x
            ));

            break;
        }
        case '^': { 
            float angle = angleIncrement * 0.8f * degToRad;
            // Añadir variación aleatoria
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            float cosA = cos(angle);
            float sinA = sin(angle);

            Point3D newHeading(
                heading.x * cosA - up.x * sinA,
                heading.y * cosA - up.y * sinA,
                heading.z * cosA - up.z * sinA
            );

            Point3D newUp(
                up.x * cosA + heading.x * sinA,
                up.y * cosA + heading.y * sinA,
                up.z * cosA + heading.z * sinA
            );

            heading = normalizeVector(newHeading);
            up = normalizeVector(newUp);
            break;
        }
        case 'v': { 
            float angle = -angleIncrement * 0.8f * degToRad;
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            float cosA = cos(angle);
            float sinA = sin(angle);

            Point3D newHeading(
                heading.x * cosA - up.x * sinA,
                heading.y * cosA - up.y * sinA,
                heading.z * cosA - up.z * sinA
            );

            Point3D newUp(
                up.x * cosA + heading.x * sinA,
                up.y * cosA + heading.y * sinA,
                up.z * cosA + heading.z * sinA
            );

            heading = normalizeVector(newHeading);
            up = normalizeVector(newUp);
            break;
        }
        case '\\': { 
            float angle = angleIncrement * 0.7f * degToRad;
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            float cosA = cos(angle);
            float sinA = sin(angle);

            Point3D newLeft(
                left.x * cosA - up.x * sinA,
                left.y * cosA - up.y * sinA,
                left.z * cosA - up.z * sinA
            );

            Point3D newUp(
                up.x * cosA + left.x * sinA,
                up.y * cosA + left.y * sinA,
                up.z * cosA + left.z * sinA
            );

            left = normalizeVector(newLeft);
            up = normalizeVector(newUp);
            break;
        }
        case '/': {
            float angle = -angleIncrement * 0.7f * degToRad;
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            float cosA = cos(angle);
            float sinA = sin(angle);

            Point3D newLeft(
                left.x * cosA - up.x * sinA,
                left.y * cosA - up.y * sinA,
                left.z * cosA - up.z * sinA
            );

            Point3D newUp(
                up.x * cosA + left.x * sinA,
                up.y * cosA + left.y * sinA,
                up.z * cosA + left.z * sinA
            );

            left = normalizeVector(newLeft);
            up = normalizeVector(newUp);
            break;
        }
        case '|': { 
            heading.x = -heading.x;
            heading.y = -heading.y;
            heading.z = -heading.z;

            left.x = -left.x;
            left.y = -left.y;
            left.z = -left.z;
            break;
        }
        case '[': { 
            positionStack.push(currentPosition);
            headingStack.push(heading);
            leftStack.push(left);
            upStack.push(up);
            lineWidthStack.push(lineWidth);
            lengthStack.push(currentLength);
            angleIncrementStack.push(angleIncrement);

            if (randomnessFactor > 0.0f && ((float)rand() / RAND_MAX) < 0.2f * randomnessFactor) {
                float zAngle = (20.0f + ((float)rand() / RAND_MAX) * 20.0f) * degToRad;
                bool positiveZ = ((float)rand() / RAND_MAX > 0.5f);

                float cosZ = cos(zAngle);
                float sinZ = sin(zAngle * (positiveZ ? 1.0f : -1.0f));

                Point3D newHeading(
                    heading.x * cosZ - up.x * sinZ,
                    heading.y * cosZ - up.y * sinZ,
                    heading.z * cosZ - up.z * sinZ
                );

                Point3D newUp(
                    up.x * cosZ + heading.x * sinZ,
                    up.y * cosZ + heading.y * sinZ,
                    up.z * cosZ + heading.z * sinZ
                );

                heading = normalizeVector(newHeading);
                up = normalizeVector(newUp);

                left = normalizeVector(Point3D(
                    up.y * heading.z - up.z * heading.y,
                    up.z * heading.x - up.x * heading.z,
                    up.x * heading.y - up.y * heading.x
                ));
            }
            break;
        }
        case ']': 
            if (!positionStack.empty()) {
                currentPosition = positionStack.top();
                positionStack.pop();
                heading = headingStack.top();
                headingStack.pop();
                left = leftStack.top();
                leftStack.pop();
                up = upStack.top();
                upStack.pop();
                lineWidth = lineWidthStack.top();
                lineWidthStack.pop();
                currentLength = lengthStack.top();
                lengthStack.pop();
                angleIncrement = angleIncrementStack.top();
                angleIncrementStack.pop();
            }
            break;
        case '#':
            lineWidth += 0.5f;
            break;
        case '!': 
            lineWidth -= 0.5f;
            if (lineWidth < 0.1f) lineWidth = 0.1f;
            break;
        case '@': 
            if (generateLeaves) {
                leafPoints.push_back(currentPosition);
            }
            break;
        case '>':
            currentLength *= 1.2f;
            break;
        case '<':
            currentLength /= 1.2f;
            if (currentLength < 1.0f) currentLength = 1.0f;
            break;
        case '&': // Intercambiar el significado de + y -
            invertTurns = !invertTurns;
            break;
        case '(': // Disminuir ángulo
            angleIncrement -= 5.0f;
            if (angleIncrement < 1.0f) angleIncrement = 1.0f;
            break;
        case ')': // Incrementar ángulo
            angleIncrement += 5.0f;
            break;
        }
    }
    int leafMarkers = 0;
    for (char c : lSystemString) {
        if (c == '@') leafMarkers++;
    }
    std::cout << "Leaf markers found in L-system: " << leafMarkers << std::endl;

    return true;
}

bool TreeToObjConverter::generateOBJ() {
    // Variable de progreso dummy que no se usa
    std::atomic<float> dummyProgress(0.0f);
    return generateOBJ(dummyProgress);
}
bool TreeToObjConverter::generateOBJ(std::atomic<float>& progress) {
    std::string assetsDir = "../assets/";
    
#ifdef __unix__
    system("mkdir -p ../assets");
#elif defined(_WIN32) || defined(WIN32)
    system("if not exist \"..\\assets\" mkdir \"..\\assets\"");
#endif

    progress = 0.05f;

    std::ofstream objFile(outputFilename);
    if (!objFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << outputFilename << std::endl;
        return false;
    }

    progress = 0.06f;
    objFile << "# Árbol 3D generado desde L-System\n";
    objFile << "# Generado con Tree-to-OBJ Converter\n\n";

    std::vector<Point3D> vertices;
    std::vector<Face> faces;

    size_t totalSegments = segments.size();
    size_t totalLeaves = generateLeaves ? leafPoints.size() : 0;
    size_t processedItems = 0;

    for (const auto& segment : segments) {
        addCylinder(segment, vertices, faces);
        processedItems++;
        progress = 0.05f + (0.1f * processedItems / totalSegments);
    }

    processedItems = 0;
    if (generateLeaves) {
        for (const auto& leafPoint : leafPoints) {
            addLeaf(leafPoint, leafSize, vertices, faces);
            processedItems++;
            progress = 0.15f + (0.05f * processedItems / totalLeaves);
        }
    }

    size_t totalVertices = vertices.size();
    for (size_t i = 0; i < totalVertices; i++) {
        objFile << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";

        if (i % 1000 == 0 || i == totalVertices - 1) {
            progress = 0.2f + (0.1f * (i + 1) / totalVertices);
        }
    }

    objFile << "\n# Normales\n";

    progress = 0.3f;
    objFile << "\n# Caras\n";

    size_t totalFaces = faces.size();
    for (size_t i = 0; i < totalFaces; i++) {
        objFile << "f ";
        for (int idx : faces[i].vertexIndices) {
            objFile << (idx + 1) << " ";
        }
        objFile << "\n";

        if (i % 1000 == 0 || i == totalFaces - 1) {
            progress = 0.3f + (0.7f * (i + 1) / totalFaces);
        }
    }

    objFile.close();
    progress = 1.0f;

    std::cout << "Archivo OBJ generado: " << outputFilename << std::endl;
    return true;
}

void TreeToObjConverter::addCylinder(const Segment3D& segment, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
    Point3D direction = {
        segment.end.x - segment.start.x,
        segment.end.y - segment.start.y,
        segment.end.z - segment.start.z
    };

    float length = std::sqrt(
        direction.x * direction.x +
        direction.y * direction.y +
        direction.z * direction.z
    );

    if (length < 0.0001f) return; 

    direction = normalizeVector(direction);

    Point3D perpendicular1, perpendicular2;
    calculatePerpendicularVectors(direction, perpendicular1, perpendicular2);

    int baseIndex = vertices.size();

    for (int i = 0; i < cylinderResolution; ++i) {
        float angle = 2.0f * M_PI * i / cylinderResolution;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        Point3D radial = {
            perpendicular1.x * cosA + perpendicular2.x * sinA,
            perpendicular1.y * cosA + perpendicular2.y * sinA,
            perpendicular1.z * cosA + perpendicular2.z * sinA
        };

        vertices.push_back({
            segment.start.x + radial.x * segment.thickness,
            segment.start.y + radial.y * segment.thickness,
            segment.start.z + radial.z * segment.thickness
            });

        vertices.push_back({
            segment.end.x + radial.x * segment.thickness,
            segment.end.y + radial.y * segment.thickness,
            segment.end.z + radial.z * segment.thickness
            });
    }

    for (int i = 0; i < cylinderResolution; ++i) {
        int i0 = baseIndex + i * 2;
        int i1 = baseIndex + ((i + 1) % cylinderResolution) * 2;
        int i2 = i1 + 1;
        int i3 = i0 + 1;

        faces.push_back({ i0, i1, i3 });
        faces.push_back({ i1, i2, i3 });
    }

    // Tapa base
    int centerBaseIndex = vertices.size();
    vertices.push_back(segment.start);

    for (int i = 0; i < cylinderResolution; ++i) {
        int i0 = baseIndex + i * 2;
        int i1 = baseIndex + ((i + 1) % cylinderResolution) * 2;
        faces.push_back({ centerBaseIndex, i1, i0 });
    }

    // Tapa superior
    int centerTopIndex = vertices.size();
    vertices.push_back(segment.end);

    for (int i = 0; i < cylinderResolution; ++i) {
        int i0 = baseIndex + i * 2 + 1;
        int i1 = baseIndex + ((i + 1) % cylinderResolution) * 2 + 1;
        faces.push_back({ centerTopIndex, i0, i1 });
    }
}

void TreeToObjConverter::addLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
    if (!generateLeaves) return;

    // Seleccionar el tipo de hoja según la configuración actual
    switch (leafType) {
    case LeafType::NEEDLE:
        addNeedleLeaf(position, size, vertices, faces);
        break;
    case LeafType::BROAD:
        addBroadLeaf(position, size, vertices, faces);
        break;
    case LeafType::PALM:
        addPalmLeaf(position, size, vertices, faces);
        break;
    case LeafType::COMPOUND:
        addCompoundLeaf(position, size, vertices, faces);
        break;
    case LeafType::SIMPLE:
    default:
        addSimpleLeaf(position, size, vertices, faces);
        break;
    }
}

void TreeToObjConverter::calculateLeafOrientation(const Point3D& position, Point3D& right, Point3D& up, Point3D& forward) {
    // Orientación aleatoria con preferencia hacia arriba
    float yaw = randomFloat(0.0f, 2.0f * M_PI);
    float pitch = randomFloat(0.0f, M_PI / 3.0f); // Máximo 60 grados desde la vertical

    // Crear vectores de orientación básicos
    right = { std::cos(yaw), 0.0f, std::sin(yaw) };
    up = { 0.0f, 1.0f, 0.0f };
    forward = { -std::sin(yaw), 0.0f, std::cos(yaw) };

    // Aplicar inclinación (pitch)
    float cp = std::cos(pitch);
    float sp = std::sin(pitch);

    Point3D tempForward = {
        forward.x * cp + up.x * sp,
        forward.y * cp + up.y * sp,
        forward.z * cp + up.z * sp
    };

    Point3D tempUp = {
        up.x * cp - forward.x * sp,
        up.y * cp - forward.y * sp,
        up.z * cp - forward.z * sp
    };

    forward = normalizeVector(tempForward);
    up = normalizeVector(tempUp);

    right = {
        up.y * forward.z - up.z * forward.y,
        up.z * forward.x - up.x * forward.z,
        up.x * forward.y - up.y * forward.x
    };
    right = normalizeVector(right);
}

void TreeToObjConverter::addSimpleLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
    // Calcular vectores de orientación
    Point3D right, up, forward;
    calculateLeafOrientation(position, right, up, forward);

    // Añadir variación aleatoria al tamaño
    float actualSize = size * randomFloat(0.8f, 1.2f);

    int baseIndex = vertices.size();

    vertices.push_back(position); // Centro

    // Vértice derecho
    vertices.push_back({
        position.x + right.x * actualSize * 0.5f,
        position.y + right.y * actualSize * 0.5f,
        position.z + right.z * actualSize * 0.5f
        });

    // Vértice punta
    vertices.push_back({
        position.x + forward.x * actualSize * 1.5f,
        position.y + forward.y * actualSize * 1.5f,
        position.z + forward.z * actualSize * 1.5f
        });

    vertices.push_back({
        position.x - right.x * actualSize * 0.5f,
        position.y - right.y * actualSize * 0.5f,
        position.z - right.z * actualSize * 0.5f
        });

    vertices.push_back({
        position.x - forward.x * actualSize * 0.3f,
        position.y - forward.y * actualSize * 0.3f,
        position.z - forward.z * actualSize * 0.3f
        });

    // delante
    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 2 }); // Cara superior derecha
    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 3 }); // Cara superior izquierda
    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 4 }); // Cara inferior izquierda
    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 1 }); // Cara inferior derecha

    // detrás
    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 1 }); // Inversa superior derecha
    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 2 }); // Inversa superior izquierda
    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 3 }); // Inversa inferior izquierda
    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 4 }); // Inversa inferior derecha
}

void TreeToObjConverter::addNeedleLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
    Point3D right, up, forward;
    calculateLeafOrientation(position, right, up, forward);

    forward.y += 0.5f;
    forward = normalizeVector(forward);

    right = {
        up.y * forward.z - up.z * forward.y,
        up.z * forward.x - up.x * forward.z,
        up.x * forward.y - up.y * forward.x
    };
    right = normalizeVector(right);

    up = {
        forward.y * right.z - forward.z * right.y,
        forward.z * right.x - forward.x * right.z,
        forward.x * right.y - forward.y * right.x
    };
    up = normalizeVector(up);

    float actualSize = size * randomFloat(0.8f, 1.2f);

    float needleLength = actualSize * 3.0f;
    float needleWidth = actualSize * 0.1f;

    int baseIndex = vertices.size();

    Point3D base = position;

    Point3D tip = {
        base.x + forward.x * needleLength,
        base.y + forward.y * needleLength,
        base.z + forward.z * needleLength
    };

    vertices.push_back({
        base.x + right.x * needleWidth + up.x * needleWidth / 2,
        base.y + right.y * needleWidth + up.y * needleWidth / 2,
        base.z + right.z * needleWidth + up.z * needleWidth / 2
        });

    vertices.push_back({
        base.x - right.x * needleWidth + up.x * needleWidth / 2,
        base.y - right.y * needleWidth + up.y * needleWidth / 2,
        base.z - right.z * needleWidth + up.z * needleWidth / 2
        });

    vertices.push_back({
        base.x - right.x * needleWidth - up.x * needleWidth / 2,
        base.y - right.y * needleWidth - up.y * needleWidth / 2,
        base.z - right.z * needleWidth - up.z * needleWidth / 2
        });

    vertices.push_back({
        base.x + right.x * needleWidth - up.x * needleWidth / 2,
        base.y + right.y * needleWidth - up.y * needleWidth / 2,
        base.z + right.z * needleWidth - up.z * needleWidth / 2
        });

    vertices.push_back(tip);

    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 4 });
    faces.push_back({ baseIndex + 1, baseIndex + 2, baseIndex + 4 });
    faces.push_back({ baseIndex + 2, baseIndex + 3, baseIndex + 4 });
    faces.push_back({ baseIndex + 3, baseIndex, baseIndex + 4 });

    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 2 });
    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 1 });
}

void TreeToObjConverter::addBroadLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
    Point3D right, up, forward;
    calculateLeafOrientation(position, right, up, forward);

    float actualSize = size * randomFloat(0.8f, 1.2f);

    float leafLength = actualSize * 1.8f;
    float leafWidth = actualSize * 1.2f;

    int baseIndex = vertices.size();

    vertices.push_back(position);

    float curveFactor = actualSize * 0.2f;

    // Borde derecho (3 puntos para dar forma)
    vertices.push_back({
        position.x + right.x * leafWidth * 0.7f + forward.x * leafLength * 0.2f,
        position.y + right.y * leafWidth * 0.7f + forward.y * leafLength * 0.2f + up.y * curveFactor,
        position.z + right.z * leafWidth * 0.7f + forward.z * leafLength * 0.2f + up.z * curveFactor
        });

    vertices.push_back({
        position.x + right.x * leafWidth * 0.5f + forward.x * leafLength * 0.6f,
        position.y + right.y * leafWidth * 0.5f + forward.y * leafLength * 0.6f + up.y * curveFactor * 1.5f,
        position.z + right.z * leafWidth * 0.5f + forward.z * leafLength * 0.6f + up.z * curveFactor * 1.5f
        });

    // Punta
    vertices.push_back({
        position.x + forward.x * leafLength,
        position.y + forward.y * leafLength + up.y * curveFactor,
        position.z + forward.z * leafLength + up.z * curveFactor
        });

    // Borde izquierdo (3 puntos para dar forma)
    vertices.push_back({
        position.x - right.x * leafWidth * 0.5f + forward.x * leafLength * 0.6f,
        position.y - right.y * leafWidth * 0.5f + forward.y * leafLength * 0.6f + up.y * curveFactor * 1.5f,
        position.z - right.z * leafWidth * 0.5f + forward.z * leafLength * 0.6f + up.z * curveFactor * 1.5f
        });

    vertices.push_back({
        position.x - right.x * leafWidth * 0.7f + forward.x * leafLength * 0.2f,
        position.y - right.y * leafWidth * 0.7f + forward.y * leafLength * 0.2f + up.y * curveFactor,
        position.z - right.z * leafWidth * 0.7f + forward.z * leafLength * 0.2f + up.z * curveFactor
        });

    // Parte frontal
    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 2 });
    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 3 });
    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 4 });
    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 5 });

    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 1 });
    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 2 });
    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 3 });
    faces.push_back({ baseIndex, baseIndex + 5, baseIndex + 4 });
}

void TreeToObjConverter::addPalmLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
    Point3D right, up, forward;
    calculateLeafOrientation(position, right, up, forward);

    forward.y += 0.7f;
    forward = normalizeVector(forward);

    right = {
        up.y * forward.z - up.z * forward.y,
        up.z * forward.x - up.x * forward.z,
        up.x * forward.y - up.y * forward.x
    };
    right = normalizeVector(right);

    up = {
        forward.y * right.z - forward.z * right.y,
        forward.z * right.x - forward.x * right.z,
        forward.x * right.y - forward.y * right.x
    };
    up = normalizeVector(up);

    float actualSize = size * randomFloat(0.8f, 1.2f);

    float leafLength = actualSize * 4.0f;
    float maxWidth = actualSize * 0.8f;

    int baseIndex = vertices.size();

    Point3D base = position;

    vertices.push_back(base);

    int segments = 5;

    for (int i = 1; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;

        float segmentWidth = maxWidth * std::sin(t * M_PI);

        Point3D centerPoint = {
            base.x + forward.x * leafLength * t,
            base.y + forward.y * leafLength * t,
            base.z + forward.z * leafLength * t
        };

        float curvature = actualSize * 0.5f * std::sin(t * M_PI);

        Point3D leftPoint = {
            centerPoint.x - right.x * segmentWidth,
            centerPoint.y - right.y * segmentWidth + up.y * curvature,
            centerPoint.z - right.z * segmentWidth + up.z * curvature
        };

        Point3D rightPoint = {
            centerPoint.x + right.x * segmentWidth,
            centerPoint.y + right.y * segmentWidth + up.y * curvature,
            centerPoint.z + right.z * segmentWidth + up.z * curvature
        };

        vertices.push_back(centerPoint);
        vertices.push_back(leftPoint);
        vertices.push_back(rightPoint);

        if (i > 1) {
            int currCenterIdx = baseIndex + 1 + (i - 1) * 3;
            int currLeftIdx = currCenterIdx + 1;
            int currRightIdx = currCenterIdx + 2;

            int prevCenterIdx = baseIndex + 1 + (i - 2) * 3;
            int prevLeftIdx = prevCenterIdx + 1;
            int prevRightIdx = prevCenterIdx + 2;

            faces.push_back({ prevCenterIdx, currCenterIdx, currLeftIdx });
            faces.push_back({ prevCenterIdx, currLeftIdx, prevLeftIdx });

            faces.push_back({ prevCenterIdx, prevRightIdx, currRightIdx });
            faces.push_back({ prevCenterIdx, currRightIdx, currCenterIdx });

            faces.push_back({ prevCenterIdx, currLeftIdx, currCenterIdx });
            faces.push_back({ prevCenterIdx, prevLeftIdx, currLeftIdx });

            faces.push_back({ prevCenterIdx, currRightIdx, prevRightIdx });
            faces.push_back({ prevCenterIdx, currCenterIdx, currRightIdx });
        }

        if (i == 1) {
            faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 2 });
            faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 1 });

            faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 1 });
            faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 3 });
        }
    }
}

void TreeToObjConverter::addCompoundLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces)
{

	Point3D right, up, forward;
	calculateLeafOrientation(position, right, up, forward);

	// Add slight upward tilt
	forward.y += 0.3f;
	forward = normalizeVector(forward);

	// Recalculate right and up vectors for orthogonality
	right = {
		up.y * forward.z - up.z * forward.y,
		up.z * forward.x - up.x * forward.z,
		up.x * forward.y - up.y * forward.x
	};
	right = normalizeVector(right);

	up = {
		forward.y * right.z - forward.z * right.y,
		forward.z * right.x - forward.x * right.z,
		forward.x * right.y - forward.y * right.x
	};
	up = normalizeVector(up);

	// Add random variation to size
	float actualSize = size * randomFloat(0.8f, 1.2f);

	// Parameters for the compound leaf
	float stemLength = actualSize * 3.5f;
	float stemWidth = actualSize * 0.05f;
	int numPairs = 4 + static_cast<int>(randomFloat(0, 2)); // 4-5 pairs of leaflets
	float leafletLength = actualSize * 0.7f;
	float leafletWidth = actualSize * 0.15f;

	int baseIndex = vertices.size();
	Point3D stemBase = position;

	// Create stem vertices (central vein)
	std::vector<Point3D> stemPoints;
	for (int i = 0; i <= numPairs; i++) {
		float t = static_cast<float>(i) / numPairs;
		Point3D stemPoint = {
			stemBase.x + forward.x * stemLength * t,
			stemBase.y + forward.y * stemLength * t,
			stemBase.z + forward.z * stemLength * t
		};
		stemPoints.push_back(stemPoint);
	}

	// Store first vertex index for stem base
	vertices.push_back(stemBase);
	int stemBaseIndex = baseIndex;

	// For each pair of leaflets
	for (int i = 0; i < numPairs; i++) {
		float t = static_cast<float>(i) / numPairs;

		// Leaflet size gets smaller as we approach the tip
		float scaleFactor = 1.0f - t * 0.6f;
		float currLeafletLength = leafletLength * scaleFactor;
		float currLeafletWidth = leafletWidth * scaleFactor;

		// Current point on the stem
		Point3D stemPoint = stemPoints[i];

		// Direction vectors for each pair are perpendicular to the stem
		// Add slight angle variation
		float angleVar = randomFloat(-0.1f, 0.1f);
		Point3D rightAngled = {
			right.x * std::cos(angleVar) - forward.x * std::sin(angleVar),
			right.y * std::cos(angleVar) - forward.y * std::sin(angleVar),
			right.z * std::cos(angleVar) - forward.z * std::sin(angleVar)
		};
		rightAngled = normalizeVector(rightAngled);

		// Create vertices for left leaflet
		int leftLeafletBase = vertices.size();
		vertices.push_back(stemPoint); // Attachment point

		// Add some curve to the leaflet
		float curveFactor = actualSize * 0.1f;

		// Left leaflet tip
		vertices.push_back({
			stemPoint.x - rightAngled.x * currLeafletLength,
			stemPoint.y - rightAngled.y * currLeafletLength + up.y * curveFactor,
			stemPoint.z - rightAngled.z * currLeafletLength + up.z * curveFactor
			});

		// Points for width of left leaflet
		vertices.push_back({
			stemPoint.x - rightAngled.x * currLeafletLength * 0.8f - forward.x * currLeafletWidth,
			stemPoint.y - rightAngled.y * currLeafletLength * 0.8f - forward.y * currLeafletWidth + up.y * curveFactor * 0.8f,
			stemPoint.z - rightAngled.z * currLeafletLength * 0.8f - forward.z * currLeafletWidth + up.z * curveFactor * 0.8f
			});

		vertices.push_back({
			stemPoint.x - rightAngled.x * currLeafletLength * 0.8f + forward.x * currLeafletWidth,
			stemPoint.y - rightAngled.y * currLeafletLength * 0.8f + forward.y * currLeafletWidth + up.y * curveFactor * 0.8f,
			stemPoint.z - rightAngled.z * currLeafletLength * 0.8f + forward.z * currLeafletWidth + up.z * curveFactor * 0.8f
			});

		// Create faces for left leaflet (front and back)
		faces.push_back({ leftLeafletBase, leftLeafletBase + 2, leftLeafletBase + 1 });
		faces.push_back({ leftLeafletBase, leftLeafletBase + 1, leftLeafletBase + 3 });
		faces.push_back({ leftLeafletBase, leftLeafletBase + 1, leftLeafletBase + 2 });
		faces.push_back({ leftLeafletBase, leftLeafletBase + 3, leftLeafletBase + 1 });

		// Create vertices for right leaflet
		int rightLeafletBase = vertices.size();
		vertices.push_back(stemPoint); // Attachment point

		// Right leaflet tip
		vertices.push_back({
			stemPoint.x + rightAngled.x * currLeafletLength,
			stemPoint.y + rightAngled.y * currLeafletLength + up.y * curveFactor,
			stemPoint.z + rightAngled.z * currLeafletLength + up.z * curveFactor
			});

		// Points for width of right leaflet
		vertices.push_back({
			stemPoint.x + rightAngled.x * currLeafletLength * 0.8f - forward.x * currLeafletWidth,
			stemPoint.y + rightAngled.y * currLeafletLength * 0.8f - forward.y * currLeafletWidth + up.y * curveFactor * 0.8f,
			stemPoint.z + rightAngled.z * currLeafletLength * 0.8f - forward.z * currLeafletWidth + up.z * curveFactor * 0.8f
			});

		vertices.push_back({
			stemPoint.x + rightAngled.x * currLeafletLength * 0.8f + forward.x * currLeafletWidth,
			stemPoint.y + rightAngled.y * currLeafletLength * 0.8f + forward.y * currLeafletWidth + up.y * curveFactor * 0.8f,
			stemPoint.z + rightAngled.z * currLeafletLength * 0.8f + forward.z * currLeafletWidth + up.z * curveFactor * 0.8f
			});

		// Create faces for right leaflet (front and back)
		faces.push_back({ rightLeafletBase, rightLeafletBase + 2, rightLeafletBase + 1 });
		faces.push_back({ rightLeafletBase, rightLeafletBase + 1, rightLeafletBase + 3 });
		faces.push_back({ rightLeafletBase, rightLeafletBase + 1, rightLeafletBase + 2 });
		faces.push_back({ rightLeafletBase, rightLeafletBase + 3, rightLeafletBase + 1 });
	}

	// Create an elongated tip leaflet
	Point3D tipPoint = stemPoints[numPairs];
	int tipLeafletBase = vertices.size();

	vertices.push_back(tipPoint); // Attachment point

	float tipLength = leafletLength * 0.8f;
	float tipWidth = leafletWidth * 0.5f;
	float tipCurveFactor = actualSize * 0.1f;

	// Tip leaflet end point
	vertices.push_back({
		tipPoint.x + forward.x * tipLength,
		tipPoint.y + forward.y * tipLength + up.y * tipCurveFactor,
		tipPoint.z + forward.z * tipLength + up.z * tipCurveFactor
		});

	// Points for width of tip leaflet
	vertices.push_back({
		tipPoint.x + forward.x * tipLength * 0.7f + right.x * tipWidth,
		tipPoint.y + forward.y * tipLength * 0.7f + right.y * tipWidth + up.y * tipCurveFactor * 0.8f,
		tipPoint.z + forward.z * tipLength * 0.7f + right.z * tipWidth + up.z * tipCurveFactor * 0.8f
		});

	vertices.push_back({
		tipPoint.x + forward.x * tipLength * 0.7f - right.x * tipWidth,
		tipPoint.y + forward.y * tipLength * 0.7f - right.y * tipWidth + up.y * tipCurveFactor * 0.8f,
		tipPoint.z + forward.z * tipLength * 0.7f - right.z * tipWidth + up.z * tipCurveFactor * 0.8f
		});

	// Create faces for tip leaflet (front and back)
	faces.push_back({ tipLeafletBase, tipLeafletBase + 2, tipLeafletBase + 1 });
	faces.push_back({ tipLeafletBase, tipLeafletBase + 1, tipLeafletBase + 3 });
	faces.push_back({ tipLeafletBase, tipLeafletBase + 1, tipLeafletBase + 2 });
	faces.push_back({ tipLeafletBase, tipLeafletBase + 3, tipLeafletBase + 1 });

}

//void TreeToObjConverter::addLeaf(const Point3D& position, float size, std::vector<Point3D>& vertices, std::vector<Face>& faces) {
//    // Índice base para esta hoja
//    int baseIndex = vertices.size();
//
//
//    float randomFactor = 1.0f - 0.75f + ((float)rand() / RAND_MAX) * 0.75f * 2.0f;
//    float scaledSize = (size * 5.0f) * randomFactor;
//
//
//
//    float yaw = ((float)rand() / RAND_MAX) * 2.0f * M_PI;   // 0-360 grados
//    float pitch = ((float)rand() / RAND_MAX) * M_PI / 2.0f; // 0-90 grados
//    float roll = ((float)rand() / RAND_MAX) * M_PI / 4.0f;  // 0-45 grados
//
//    // Calcular vectores de orientación usando los ángulos aleatorios
//    float cy = cos(yaw), sy = sin(yaw);
//    float cp = cos(pitch), sp = sin(pitch);
//    float cr = cos(roll), sr = sin(roll);
//
//    // Matriz de rotación 3D completa (yaw, pitch, roll)
//    Point3D right, up, forward;
//
//    // Vector derecha (right)
//    right.x = cy * cr + sy * sp * sr;
//    right.y = sr * cp;
//    right.z = -sy * cr + cy * sp * sr;
//
//    // Vector arriba (up)
//    up.x = -cy * sr + sy * sp * cr;
//    up.y = cr * cp;
//    up.z = sy * sr + cy * sp * cr;
//
//    // Vector adelante (forward)
//    forward.x = sy * cp;
//    forward.y = -sp;
//    forward.z = cy * cp;
//
//    // Normalizar vectores
//    right = normalizeVector(right);
//    up = normalizeVector(up);
//    forward = normalizeVector(forward);
//
//    // Ahora creamos una forma de hoja más compleja con vértices alrededor de la posición
//    float leafLength = scaledSize * 1.5f;
//    float leafWidth = scaledSize * 0.8f;
//
//    // Vértices de la hoja (forma de diamante alargado)
//    vertices.push_back(position); // Centro
//
//    vertices.push_back({
//        position.x + right.x * leafWidth * 0.5f,
//        position.y + right.y * leafWidth * 0.5f,
//        position.z + right.z * leafWidth * 0.5f
//        }); // Derecha
//
//    vertices.push_back({
//        position.x + forward.x * leafLength,
//        position.y + forward.y * leafLength,
//        position.z + forward.z * leafLength
//        }); // Punta
//
//    vertices.push_back({
//        position.x - right.x * leafWidth * 0.5f,
//        position.y - right.y * leafWidth * 0.5f,
//        position.z - right.z * leafWidth * 0.5f
//        }); // Izquierda
//
//    vertices.push_back({
//        position.x - forward.x * leafLength * 0.3f,
//        position.y - forward.y * leafLength * 0.3f,
//        position.z - forward.z * leafLength * 0.3f
//        }); // Base
//
//    // Añadir las caras (ahora triangulares para mejor renderizado)
//    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 2 }); // Cara superior derecha
//    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 3 }); // Cara superior izquierda
//    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 4 }); // Cara inferior izquierda
//    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 1 }); // Cara inferior derecha
//
//    // Añadir caras para el reverso de la hoja
//    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 1 }); // Cara superior derecha reversa
//    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 2 }); // Cara superior izquierda reversa
//    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 3 }); // Cara inferior izquierda reversa
//    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 4 }); // Cara inferior derecha reversa
//}

void TreeToObjConverter::calculatePerpendicularVectors(const Point3D& direction, Point3D& perpendicular1, Point3D& perpendicular2) {
    // Encontrar un vector perpendicular
    if (std::abs(direction.x) < 0.8f) {
        perpendicular1 = { 1.0f, 0.0f, 0.0f };
    }
    else {
        perpendicular1 = { 0.0f, 1.0f, 0.0f };
    }

    // Producto cruz para obtener segundo perpendicular
    perpendicular2 = {
        direction.y * perpendicular1.z - direction.z * perpendicular1.y,
        direction.z * perpendicular1.x - direction.x * perpendicular1.z,
        direction.x * perpendicular1.y - direction.y * perpendicular1.x
    };

    // Normalizar perpendicular2
    perpendicular2 = normalizeVector(perpendicular2);

    // Producto cruz para corregir perpendicular1
    perpendicular1 = {
        direction.y * perpendicular2.z - direction.z * perpendicular2.y,
        direction.z * perpendicular2.x - direction.x * perpendicular2.z,
        direction.x * perpendicular2.y - direction.y * perpendicular2.x
    };

    // Normalizar perpendicular1
    perpendicular1 = normalizeVector(perpendicular1);
}

Point3D TreeToObjConverter::normalizeVector(const Point3D& vector) {
    float length = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    if (length < 0.0001f) {
        return { 0.0f, 0.0f, 0.0f }; // Evitar división por cero
    }

    return {
        vector.x / length,
        vector.y / length,
        vector.z / length
    };
}