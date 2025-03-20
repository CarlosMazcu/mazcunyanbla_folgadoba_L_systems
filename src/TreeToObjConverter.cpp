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

    float currentAngle = initialAngle * degToRad;
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
            // Añadimos una pequeña rotación aleatoria para dar más naturalidad a las ramas
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

            // Rotación de heading y left alrededor del eje up
            float cosA = cos(angle);
            float sinA = sin(angle);

            Point3D newHeading(
                heading.x * cosA + left.x * sinA,
                heading.y * cosA + left.y * sinA,
                heading.z * cosA + left.z * sinA
            );

            Point3D newLeft(
                left.x * cosA - heading.x * sinA,
                left.y * cosA - heading.y * sinA,
                left.z * cosA - heading.z * sinA
            );

            heading = normalizeVector(newHeading);
            left = normalizeVector(newLeft);
            break;
        }
        case '-': { // Girar a la derecha (rotación opuesta a +)
            float angle = (invertTurns ? angleIncrement : -angleIncrement) * degToRad;
            // Añadir algo de variación aleatoria al ángulo (±10%)
            float randomFactor = 1.0f + (((float)rand() / RAND_MAX) * 0.2f - 0.1f);
            angle *= randomFactor;

            // Rotación de heading y left alrededor del eje up
            float cosA = cos(angle);
            float sinA = sin(angle);

            Point3D newHeading(
                heading.x * cosA + left.x * sinA,
                heading.y * cosA + left.y * sinA,
                heading.z * cosA + left.z * sinA
            );

            Point3D newLeft(
                left.x * cosA - heading.x * sinA,
                left.y * cosA - heading.y * sinA,
                left.z * cosA - heading.z * sinA
            );

            heading = normalizeVector(newHeading);
            left = normalizeVector(newLeft);
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
        case '[': // Guardar el estado actual
            positionStack.push(currentPosition);
            headingStack.push(heading);
            leftStack.push(left);
            upStack.push(up);
            lineWidthStack.push(lineWidth);
            lengthStack.push(currentLength);
            angleIncrementStack.push(angleIncrement);
            break;
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
            // Si no reconocemos el símbolo, lo ignoramos
        }

        // Cada cierto tiempo, añadir una pequeña rotación aleatoria en los ejes
        // para dar más naturalidad a las ramas (1/5 de probabilidad)
        if (rand() % 5 == 0) {
            // Pequeña rotación aleatoria en el pitch (arriba/abajo)
            float randomPitch = ((float)rand() / RAND_MAX * 10.0f - 5.0f) * degToRad;
            float cosPitch = cos(randomPitch);
            float sinPitch = sin(randomPitch);

            Point3D newHeading(
                heading.x * cosPitch - up.x * sinPitch,
                heading.y * cosPitch - up.y * sinPitch,
                heading.z * cosPitch - up.z * sinPitch
            );

            Point3D newUp(
                up.x * cosPitch + heading.x * sinPitch,
                up.y * cosPitch + heading.y * sinPitch,
                up.z * cosPitch + heading.z * sinPitch
            );

            heading = normalizeVector(newHeading);
            up = normalizeVector(newUp);

            // Pequeña rotación aleatoria en el roll (inclinación)
            float randomRoll = ((float)rand() / RAND_MAX * 10.0f - 5.0f) * degToRad;
            float cosRoll = cos(randomRoll);
            float sinRoll = sin(randomRoll);

            Point3D newLeft(
                left.x * cosRoll - up.x * sinRoll,
                left.y * cosRoll - up.y * sinRoll,
                left.z * cosRoll - up.z * sinRoll
            );

            newUp = Point3D(
                up.x * cosRoll + left.x * sinRoll,
                up.y * cosRoll + left.y * sinRoll,
                up.z * cosRoll + left.z * sinRoll
            );

            left = normalizeVector(newLeft);
            up = normalizeVector(newUp);
        }
    }

    return true;
}

bool TreeToObjConverter::generateOBJ() {
    // Asegurarse de que el directorio assets existe
    std::string assetsDir = "../assets/";

    // Crear el directorio si no existe (código específico para sistemas POSIX/Unix/Linux/Mac)
#ifdef __unix__
    system("mkdir -p ../assets");
#elif defined(_WIN32) || defined(WIN32)
    system("if not exist \"..\\assets\" mkdir \"..\\assets\"");
#endif

    std::ofstream objFile(outputFilename);
    if (!objFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << outputFilename << std::endl;
        return false;
    }

    // Escribir encabezado
    objFile << "# Árbol 3D generado desde L-System\n";
    objFile << "# Generado con Tree-to-OBJ Converter\n\n";

    std::vector<Point3D> vertices;
    std::vector<Face> faces;

    // Generar geometría para cada segmento
    for (const auto& segment : segments) {
        addCylinder(segment, vertices, faces);
    }

    // Generar hojas
    if (generateLeaves) {
        for (const auto& leafPoint : leafPoints) {
            addLeaf(leafPoint, leafSize, vertices, faces);
        }
    }

    // Escribir vértices
    for (const auto& v : vertices) {
        objFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    // Escribir normales (simplificado por ahora)
    objFile << "\n# Normales\n";

    // Escribir caras (índices base-1 para formato OBJ)
    objFile << "\n# Caras\n";
    for (const auto& face : faces) {
        objFile << "f ";
        for (int idx : face.vertexIndices) {
            objFile << (idx + 1) << " ";
        }
        objFile << "\n";
    }

    objFile.close();
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