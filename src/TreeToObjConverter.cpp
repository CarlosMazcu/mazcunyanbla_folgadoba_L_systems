#include "TreeToObjConverter.hpp"


bool TreeToObjConverter::convertLSystemToObj(const std::string& lSystemString, float initialAngle, float initialLength, float initialWidth) {
    segments.clear();
    leafPoints.clear();

    std::stack<Point3D> positionStack;
    std::stack<Point3D> headingStack;    // Dirección hacia delante (heading)
    std::stack<Point3D> leftStack;       // Vector izquierda
    std::stack<Point3D> upStack;         // Vector arriba
    std::stack<float> lineWidthStack;
    std::stack<float> lengthStack;
    std::stack<float> angleIncrementStack;

    const float PI = 3.1416f;
    const float degToRad = PI / 180.0f;

    // Inicializamos con una posición en 3D
    Point3D currentPosition(0.0f, 0.0f, 0.0f);

    // Vectores de orientación iniciales (sistema de coordenadas de tortuga 3D)
    Point3D heading(0.0f, 1.0f, 0.0f);  // Dirección hacia delante (eje Y)
    Point3D left(-1.0f, 0.0f, 0.0f);    // Vector izquierda (eje -X)
    Point3D up(0.0f, 0.0f, 1.0f);       // Vector arriba (eje Z)

    float lineWidth = initialWidth;
    float currentLength = initialLength;
    float angleIncrement = initialAngle;
    bool invertTurns = false;

    // Añadir variación aleatoria a las rotaciones para más naturalidad
    std::srand(static_cast<unsigned int>(std::time(nullptr)));


    // Recorrer la cadena del L-system
    for (char c : lSystemString) {

        switch (c) {
        case 'F': { // Dibujar una línea hacia adelante

            if (randomnessFactor > 0.0f && ((float)rand() / RAND_MAX) < 0.3f * randomnessFactor) {
                // Pequeña rotación aleatoria en pitch y yaw
                float smallRandomPitch = (((float)rand() / RAND_MAX) * 10.0f - 5.0f) * randomnessFactor * degToRad;
                float smallRandomYaw = (((float)rand() / RAND_MAX) * 10.0f - 5.0f) * randomnessFactor * degToRad;

                // Aplicar pitch (rotación alrededor de left)
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

                // Aplicar yaw (rotación alrededor de up)
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

                // Asegurar ortogonalidad
                up = normalizeVector(Point3D(
                    left.y * heading.z - left.z * heading.y,
                    left.z * heading.x - left.x * heading.z,
                    left.x * heading.y - left.y * heading.x
                ));
            }

            // Calculamos el nuevo punto usando la dirección actual
            Point3D newPosition(
                currentPosition.x + heading.x * currentLength,
                currentPosition.y + heading.y * currentLength,
                currentPosition.z + heading.z * currentLength
            );

            // Añadimos el segmento a nuestra lista
            segments.push_back(Segment3D(currentPosition, newPosition, lineWidth));

            // Actualizamos la posición actual
            currentPosition = newPosition;
            break;
        }
        case 'f': { // Avanzar sin dibujar
            Point3D newPosition(
                currentPosition.x + heading.x * currentLength,
                currentPosition.y + heading.y * currentLength,
                currentPosition.z + heading.z * currentLength
            );
            currentPosition = newPosition;
            break;
        }
        case '+': { // Girar a la izquierda (rotación alrededor del eje up)
            float angle = (invertTurns ? -angleIncrement : angleIncrement) * degToRad;

            // Añadir algo de variación aleatoria al ángulo (±10%)
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            // MODIFICACIÓN CLAVE: Introducir componente 3D al ángulo
            // En lugar de rotar solo en el plano horizontal, ahora distribuimos
            // la rotación en una esfera (componentes en los tres ejes)

            // 1. Rotación alrededor del eje up (yaw) - efecto tradicional 2D
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

            // 2. Añadir una componente de rotación alrededor del eje left (pitch)
            // Solo si el factor de aleatoriedad es > 0
            if (randomnessFactor > 0.0f) {
                // El factor de pitch es proporcional al factor de aleatoriedad
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

            // 3. También podemos añadir una componente de roll si queremos
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

            // Normalizar los vectores y asegurar que forman un sistema ortogonal
            heading = normalizeVector(newHeading);
            left = normalizeVector(newLeft);
            up = normalizeVector(up);

            // Re-ortogonalizar para evitar acumulación de errores numéricos
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
        case '-': { // Girar a la derecha (rotación opuesta a +)
            // Lógica similar a '+' pero con ángulo invertido
            float angle = (invertTurns ? angleIncrement : -angleIncrement) * degToRad;
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            // Componente yaw (plano horizontal)
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

            // Componente pitch (inclinación arriba/abajo)
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

            // Componente roll (rotación sobre el eje)
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

            // Normalizar y corregir ortogonalidad
            heading = normalizeVector(newHeading);
            left = normalizeVector(newLeft);
            up = normalizeVector(up);

            // Re-ortogonalizar
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
        case '^': { // Inclinar hacia arriba (pitch up - rotación alrededor del eje left)
            float angle = angleIncrement * 0.8f * degToRad; // Usar un ángulo más pequeño para pitch
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
        case 'v': { // Inclinar hacia abajo (pitch down - rotación opuesta a ^)
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
        case '\\': { // Roll a la izquierda (rotación alrededor del eje heading)
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
        case '/': { // Roll a la derecha (rotación opuesta a \)
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
        case '|': { // Girar 180 grados (Simplemente invertir heading y left)
            heading.x = -heading.x;
            heading.y = -heading.y;
            heading.z = -heading.z;

            left.x = -left.x;
            left.y = -left.y;
            left.z = -left.z;
            break;
        }
        case '[': { // Guardar el estado actual
            positionStack.push(currentPosition);
            headingStack.push(heading);
            leftStack.push(left);
            upStack.push(up);
            lineWidthStack.push(lineWidth);
            lengthStack.push(currentLength);
            angleIncrementStack.push(angleIncrement);

            // Posibilidad de inclinar la rama en dirección Z
            if (randomnessFactor > 0.0f && ((float)rand() / RAND_MAX) < 0.2f * randomnessFactor) {
                // Rotación moderada hacia el eje Z (20-40 grados)
                float zAngle = (20.0f + ((float)rand() / RAND_MAX) * 20.0f) * degToRad;
                bool positiveZ = ((float)rand() / RAND_MAX > 0.5f);

                // Aplicar rotación para inclinar hacia el eje Z
                float cosZ = cos(zAngle);
                float sinZ = sin(zAngle * (positiveZ ? 1.0f : -1.0f));

                // Rotar alrededor de left (que apunta hacia el eje X negativo)
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

                // Recalcular left para mantener sistema ortogonal
                left = normalizeVector(Point3D(
                    up.y * heading.z - up.z * heading.y,
                    up.z * heading.x - up.x * heading.z,
                    up.x * heading.y - up.y * heading.x
                ));
            }
            break;
        }
        case ']': // Restaurar el estado anterior
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
        case '#': // Incrementar el ancho de línea
            lineWidth += 0.5f;
            break;
        case '!': // Disminuir el ancho de línea
            lineWidth -= 0.5f;
            if (lineWidth < 0.1f) lineWidth = 0.1f;
            break;
        case '@': // Marcar una hoja
            if (generateLeaves) {
                leafPoints.push_back(currentPosition);
            }
            break;
        case '>': // Aumentar longitud
            currentLength *= 1.2f;
            break;
        case '<': // Disminuir longitud
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


    return true;
}

bool TreeToObjConverter::generateOBJ() {
    // Variable de progreso dummy que no se usa
    std::atomic<float> dummyProgress(0.0f);
    return generateOBJ(dummyProgress);
}
bool TreeToObjConverter::generateOBJ(std::atomic<float>& progress) {
    // Asegurarse de que el directorio assets existe
    std::string assetsDir = "../assets/";

    // Crear el directorio si no existe (código específico para sistemas POSIX/Unix/Linux/Mac)
#ifdef __unix__
    system("mkdir -p ../assets");
#elif defined(_WIN32) || defined(WIN32)
    system("if not exist \"..\\assets\" mkdir \"..\\assets\"");
#endif

    // Iniciar progreso
    progress = 0.05f;

    std::ofstream objFile(outputFilename);
    if (!objFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << outputFilename << std::endl;
        return false;
    }

    // Escribir encabezado
    progress = 0.06f;
    objFile << "# Árbol 3D generado desde L-System\n";
    objFile << "# Generado con Tree-to-OBJ Converter\n\n";

    std::vector<Point3D> vertices;
    std::vector<Face> faces;

    // Calcular el número total de elementos a procesar
    size_t totalSegments = segments.size();
    size_t totalLeaves = generateLeaves ? leafPoints.size() : 0;
    size_t processedItems = 0;

    // Fase 1: Generar geometría para cada segmento - asignamos solo 10% (5%-15%)
    for (const auto& segment : segments) {
        addCylinder(segment, vertices, faces);
        processedItems++;
        progress = 0.05f + (0.1f * processedItems / totalSegments);
    }

    // Generar hojas - asignamos solo 5% (15%-20%)
    processedItems = 0;
    if (generateLeaves) {
        for (const auto& leafPoint : leafPoints) {
            addLeaf(leafPoint, leafSize, vertices, faces);
            processedItems++;
            progress = 0.15f + (0.05f * processedItems / totalLeaves);
        }
    }

    // Fase 2: Escribir vértices - asignamos 10% (20%-30%)
    size_t totalVertices = vertices.size();
    for (size_t i = 0; i < totalVertices; i++) {
        objFile << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";

        // Actualizar progreso cada cierto número de vértices para no ralentizar
        if (i % 1000 == 0 || i == totalVertices - 1) {
            progress = 0.2f + (0.1f * (i + 1) / totalVertices);
        }
    }

    // Marcador entre secciones
    objFile << "\n# Normales\n";

    // Fase 3: Escribir caras - asignamos 70% (30%-100%) - LA PARTE MÁS PESADA
    progress = 0.3f;
    objFile << "\n# Caras\n";

    size_t totalFaces = faces.size();
    for (size_t i = 0; i < totalFaces; i++) {
        objFile << "f ";
        for (int idx : faces[i].vertexIndices) {
            objFile << (idx + 1) << " ";
        }
        objFile << "\n";

        // Actualizar progreso cada cierto número de caras para no ralentizar
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

    if (length < 0.0001f) return; // Evitar segmentos de longitud cero

    // Normalizar dirección
    direction = normalizeVector(direction);

    // Calcular vectores perpendiculares para crear el círculo
    Point3D perpendicular1, perpendicular2;
    calculatePerpendicularVectors(direction, perpendicular1, perpendicular2);

    // Guardar índice base para las caras
    int baseIndex = vertices.size();

    // Crear vértices para los dos círculos (inicio y fin del segmento)
    for (int i = 0; i < cylinderResolution; ++i) {
        float angle = 2.0f * M_PI * i / cylinderResolution;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        // Vector radial
        Point3D radial = {
            perpendicular1.x * cosA + perpendicular2.x * sinA,
            perpendicular1.y * cosA + perpendicular2.y * sinA,
            perpendicular1.z * cosA + perpendicular2.z * sinA
        };

        // Vértices para el círculo inicial
        vertices.push_back({
            segment.start.x + radial.x * segment.thickness,
            segment.start.y + radial.y * segment.thickness,
            segment.start.z + radial.z * segment.thickness
            });

        // Vértices para el círculo final
        vertices.push_back({
            segment.end.x + radial.x * segment.thickness,
            segment.end.y + radial.y * segment.thickness,
            segment.end.z + radial.z * segment.thickness
            });
    }

    // Crear caras triangulares para el cilindro
    for (int i = 0; i < cylinderResolution; ++i) {
        int i0 = baseIndex + i * 2;
        int i1 = baseIndex + ((i + 1) % cylinderResolution) * 2;
        int i2 = i1 + 1;
        int i3 = i0 + 1;

        // Añadir cara (quad dividido en dos triángulos)
        faces.push_back({ i0, i1, i3 });
        faces.push_back({ i1, i2, i3 });
    }

    // Añadir tapas (opcional)
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
    // Índice base para esta hoja
    int baseIndex = vertices.size();

    // Crear orientación aleatoria para la hoja
    // Generar ángulos aleatorios
    float yaw = ((float)rand() / RAND_MAX) * 2.0f * M_PI;   // 0-360 grados
    float pitch = ((float)rand() / RAND_MAX) * M_PI / 2.0f; // 0-90 grados
    float roll = ((float)rand() / RAND_MAX) * M_PI / 4.0f;  // 0-45 grados

    // Calcular vectores de orientación usando los ángulos aleatorios
    float cy = cos(yaw), sy = sin(yaw);
    float cp = cos(pitch), sp = sin(pitch);
    float cr = cos(roll), sr = sin(roll);

    // Matriz de rotación 3D completa (yaw, pitch, roll)
    Point3D right, up, forward;

    // Vector derecha (right)
    right.x = cy * cr + sy * sp * sr;
    right.y = sr * cp;
    right.z = -sy * cr + cy * sp * sr;

    // Vector arriba (up)
    up.x = -cy * sr + sy * sp * cr;
    up.y = cr * cp;
    up.z = sy * sr + cy * sp * cr;

    // Vector adelante (forward)
    forward.x = sy * cp;
    forward.y = -sp;
    forward.z = cy * cp;

    // Normalizar vectores
    right = normalizeVector(right);
    up = normalizeVector(up);
    forward = normalizeVector(forward);

    // Ahora creamos una forma de hoja más compleja con vértices alrededor de la posición
    float leafLength = size * 1.5f;
    float leafWidth = size * 0.8f;

    // Vértices de la hoja (forma de diamante alargado)
    vertices.push_back(position); // Centro

    vertices.push_back({
        position.x + right.x * leafWidth * 0.5f,
        position.y + right.y * leafWidth * 0.5f,
        position.z + right.z * leafWidth * 0.5f
        }); // Derecha

    vertices.push_back({
        position.x + forward.x * leafLength,
        position.y + forward.y * leafLength,
        position.z + forward.z * leafLength
        }); // Punta

    vertices.push_back({
        position.x - right.x * leafWidth * 0.5f,
        position.y - right.y * leafWidth * 0.5f,
        position.z - right.z * leafWidth * 0.5f
        }); // Izquierda

    vertices.push_back({
        position.x - forward.x * leafLength * 0.3f,
        position.y - forward.y * leafLength * 0.3f,
        position.z - forward.z * leafLength * 0.3f
        }); // Base

    // Añadir las caras (ahora triangulares para mejor renderizado)
    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 2 }); // Cara superior derecha
    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 3 }); // Cara superior izquierda
    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 4 }); // Cara inferior izquierda
    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 1 }); // Cara inferior derecha

    // Añadir caras para el reverso de la hoja
    faces.push_back({ baseIndex, baseIndex + 2, baseIndex + 1 }); // Cara superior derecha reversa
    faces.push_back({ baseIndex, baseIndex + 3, baseIndex + 2 }); // Cara superior izquierda reversa
    faces.push_back({ baseIndex, baseIndex + 4, baseIndex + 3 }); // Cara inferior izquierda reversa
    faces.push_back({ baseIndex, baseIndex + 1, baseIndex + 4 }); // Cara inferior derecha reversa
}

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