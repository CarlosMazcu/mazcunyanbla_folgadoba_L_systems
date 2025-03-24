#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <ctime>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"
#include "../include/TreeToObjConverter.hpp" // Incluir el nuevo conversor

#include "ImGuiFileDialog.h"
#include <imgui.h>
#include <imgui-SFML.h>

#define INITIAL_ANGLE (25.0f)
#define INITIAL_LENGTH (10.0f)
#define INITIAL_WIDTH (2.0f)

#include <thread>
#include <atomic>
#include <future>

std::future<bool> exportFuture;
std::atomic<bool> isExporting(false);
std::atomic<float> exportProgress(0.0f);
std::string exportStatusMessage = "Preparando exportación...";
char exportingFilePath[512] = { 0 };

bool exportObjThreaded(const char* fullPath, int resolution, bool includeLeaves,
	float leafSize, float randomnessFactor, float branchRadiusMultiplier, int leaftype, const std::string& lSystemString,
	float initialAngle, float initialLength, float initialWidth, std::atomic<float>& progress) {
	// Actualizar estado inicial
	progress = 0.05f;
	exportStatusMessage = "Convirtiendo L-System a geometría 3D...";

	// Configurar el conversor 3D
	TreeToObjConverter converter(fullPath, resolution);
	converter.setGenerateLeaves(includeLeaves);
	converter.setLeafSize(leafSize * 5.0f);
	converter.setRandomnessFactor(randomnessFactor);
	converter.setBranchRadiusMultiplier(branchRadiusMultiplier);
	LeafType selectedLeafType;
	switch (leaftype) {
	case 1:
		selectedLeafType = LeafType::NEEDLE;
		break;
	case 2:
		selectedLeafType = LeafType::BROAD;
		break;
	case 3:
		selectedLeafType = LeafType::PALM;
		break;
	case 4:
		selectedLeafType = LeafType::COMPOUND;
		break;
	case 0:
		selectedLeafType = LeafType::SIMPLE;
		break;
	default:
		selectedLeafType = LeafType::SIMPLE;
		break;
	}
	converter.setLeafType(selectedLeafType);

	// Fase 1: Convertir el L-System a OBJ
	if (!converter.convertLSystemToObj(lSystemString, initialAngle, initialLength, initialWidth)) {
		exportStatusMessage = "Error en la conversión a geometría 3D";
		return false;
	}

	// Actualizar progreso

	exportStatusMessage = "Generando archivo OBJ...";

	// Pequeña pausa para asegurar que la UI se actualice
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// Fase 2: Generar el archivo OBJ
	if (!converter.generateOBJ(progress)) {
		exportStatusMessage = "Error al generar el archivo OBJ";
		return false;
	}

	// Completar
	exportProgress = 1.0f;
	exportStatusMessage = "Exportación completada con exito!";

	// Pequeña pausa para que se vea el mensaje de éxito
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	return true;
}

void showLoadingScreen() {
	// Crear una capa semitransparente sobre toda la pantalla
	ImVec2 screenSize = ImGui::GetIO().DisplaySize;
	ImGui::GetBackgroundDrawList()->AddRectFilled(
		ImVec2(0, 0), screenSize,
		IM_COL32(0, 0, 0, 180)
	);

	// Configurar tamaño y posición de la ventana modal
	ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f),
		ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400, 150));

	// Estilo para la ventana modal
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.07f, 0.07f, 0.07f, 0.94f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Banderas de la ventana
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar;

	// Crear la ventana modal
	ImGui::Begin("##LoadingScreen", nullptr, flags);

	// Título
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Exportando Modelo 3D").x) * 0.5f);
	ImGui::Text("Exportando Modelo 3D");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Mostrar el archivo que se está exportando
	ImGui::Text("Archivo: %s", exportingFilePath);

	// Mensaje de estado
	ImGui::Text("%s", exportStatusMessage.c_str());

	// Barra de progreso
	ImGui::ProgressBar(exportProgress, ImVec2(-1, 10), "");

	// Mostrar porcentaje
	//ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("%0.1f%%", exportProgress * 100.0f);

	ImGui::End();

	// Restaurar estilos
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

void imguiWindow(LSystem& ls, bool& readyToGenerate, bool& ruleApplied, bool& export3D) {
	static std::string axiomInput;
	static bool axiomSet = false;
	static int selectedMode = -1;
	static bool setobj = false;
	static int currentLeafType = 0;
	static std::vector<char> availableAxioms = { 'F', 'X', 'Y', 'A', 'B' };
	static std::vector<std::string> guidedActions = {
	   "Expandir ramas",
	   "Anadir hojas",
	   "Arbol mas denso",
	   "Arbol mas curvo",
	   "Ramas mas anchas",
	   "Hacerlo mas caotico"
	};

	if (isExporting) {
		// Verificar si la exportación ha terminado
		if (exportFuture.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
			// Obtener el resultado
			bool success = exportFuture.get();
			isExporting = false;

			// Mostrar diálogo de resultado
			if (success) {
				export3D = true;
				ImGui::OpenPopup("Export Success");
			}
			else {
				if (exportStatusMessage.find("conversión") != std::string::npos) {
					ImGui::OpenPopup("Conversion Error");
				}
				else {
					ImGui::OpenPopup("Export Error");
				}
			}
		}
		else {
			showLoadingScreen();
		}
	}

	ImGui::SetNextWindowPos(ImVec2(800.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(400.0f, 600.0f));
	ImGui::Begin("L-System Builder", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	if (!axiomSet) {
		ImGui::Text("Select Starting Axiom:");

		for (char axiom : availableAxioms) {
			std::string buttonLabel = std::string(1, axiom);
			if (ImGui::Button(buttonLabel.c_str(), ImVec2(40, 30))) {
				axiomInput = buttonLabel;
				ls.setAxiom(axiomInput);
				axiomSet = true;
				selectedMode = -1;
				ruleApplied = false;
			}
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	else {
		if (ImGui::Button("Return"))
		{
			axiomSet = false;
			setobj = false;
		}
		ImGui::Text("Axiom: %s", axiomInput.c_str());

		// Selección de modo
		ImGui::Text("Choose Mode:");
		if (ImGui::Button("Manual", ImVec2(80, 30))) { selectedMode = 0; }
		ImGui::SameLine();
		if (ImGui::Button("Guided", ImVec2(80, 30))) { selectedMode = 1; }
		ImGui::SameLine();
		if (ImGui::Button("Automatic", ImVec2(100, 30))) { selectedMode = 2; }

		ImGui::Separator();


		if (selectedMode == 1) {
			int step = ls.getGuidedStep();
			char axiom = axiomInput[0]; // Obtener el axioma seleccionado

			if (step == 0) {
				ImGui::Text("Step 1: What should happen first?");
				if (axiom == 'F') {
					if (ImGui::Button("Make trunk longer", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
					ImGui::SameLine();
					if (ImGui::Button("Create branches", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
				}
				else {
					if (ImGui::Button("Expand structure", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
					ImGui::SameLine();
					if (ImGui::Button("Make more compact", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
				}
			}
			else if (step == 1) {
				ImGui::Text("Step 2: How should the branches grow?");
				if (ImGui::Button("More curved", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
				ImGui::SameLine();
				if (ImGui::Button("More straight", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
			}
			else if (step == 2) {
				ImGui::Text("Step 3: Add leaves?");
				if (ImGui::Button("Yes", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
				ImGui::SameLine();
				if (ImGui::Button("No", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
			}
			else {
				ImGui::Text("L-System rule complete!");
				ImGui::Text("Generated rule: %c -> %s", axiom, ls.getCurrent().c_str());

				if (!ruleApplied) {
					if (ImGui::Button("Generate Tree")) {
						readyToGenerate = true;
						ruleApplied = true;
						setobj = true;
					}
				}
				else {
					if (ImGui::Button("Restart Instructions")) {
						ls.resetGuided();
						ruleApplied = false;
					}
				}
			}
		}

		if (selectedMode == 2) {
			if (ImGui::Button("Generate Random Tree")) {
				ls.generateRandomRules();
				readyToGenerate = true;
				setobj = true;
			}
		}

		// Sección para exportación 3D
		if (setobj) {
			ImGui::Text("3D Export Options:");

			// Variables para el manejo de archivos
			static char directory[256] = "../assets/";
			static char filename[128] = "tree3d";
			static char extension[8] = ".obj";
			static char fullPath[512];

			static int resolution = 8;
			ImGui::SliderInt("Cylinder Resolution", &resolution, 3, 32);

			static bool includeLeaves = true;
			ImGui::Checkbox("Include Leaves", &includeLeaves);

			static float leafSize = 3.0f;
			if (includeLeaves) {
				ImGui::SliderFloat("Leaf Size", &leafSize, 0.1f, 10.0f);
				static const std::vector<std::string> leafTypeNames = TreeToObjConverter::getLeafTypeNames();

				if (ImGui::BeginCombo("Leaf Type", leafTypeNames[currentLeafType].c_str())) {
					for (int i = 0; i < leafTypeNames.size(); i++) {
						bool isSelected = (currentLeafType == i);
						if (ImGui::Selectable(leafTypeNames[i].c_str(), isSelected)) {
							currentLeafType = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				switch (currentLeafType) {
				case 0:
					ImGui::TextWrapped("Simple diamond-shaped leaf (original)");
					break;
				case 1: 
					ImGui::TextWrapped("Long thin needle-like leaf, good for conifers and pines");
					break;
				case 2: 
					ImGui::TextWrapped("Wide oval leaf with curved edges, good for deciduous trees");
					break;
				case 3:
					ImGui::TextWrapped("Long segmented leaf with a central stem, ideal for palm trees");
					break;
				case 4:
					ImGui::TextWrapped("Compound leaf with multiple leaflets along a central stem, like ferns or ash trees");
					break;
				}
			}

			static float randomness3D = 1.0f;
			ImGui::SliderFloat("3D Branching Factor", &randomness3D, 0.5f, 3.0f);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("0 = Flat tree, 1 = Natural 3D, 2-3 = Very volumetric 3D branching");
			}
			static float branchRadiusMultiplier = 1.0f;
			ImGui::SliderFloat("Branch Radius", &branchRadiusMultiplier, 0.2f, 3.0f);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Multiplier for branch thickness: 1.0 = default, <1.0 = thinner, >1.0 = thicker");
			}
			ImGui::Separator();



			// Selector de directorio (mostramos el directorio actual)
			ImGui::InputText("Directory", directory, IM_ARRAYSIZE(directory));

			// Botón para abrir un selector de directorio (usando ImGuiFileDialog como ejemplo)
			if (ImGui::Button("Browse Directory...")) {
				igfd::ImGuiFileDialog::Instance()->OpenDialog(
					"ChooseDirDlgKey",      // ID único para el diálogo
					"Select Directory",      // Título de la ventana
					nullptr,                 // Sin filtros para directorios
					std::string(directory),  // Directorio inicial
					"",                      // Nombre de archivo por defecto (vacío)
					1,                       // Contador de selección (1 para directorios)
					0                      // UserDatas
				);
			}

			if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseDirDlgKey")) {
				// Si el usuario confirma la selección con OK
				if (igfd::ImGuiFileDialog::Instance()->IsOk) {
					// Obtener el directorio seleccionado
					std::string selectedDir = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();

					// Copiar el directorio seleccionado a nuestra variable
					strcpy(directory, selectedDir.c_str());

					// Asegurarse de que el directorio termina con un separador
					size_t len = strlen(directory);
					if (len > 0 && directory[len - 1] != '/' && directory[len - 1] != '\\') {
						strcat(directory, "/");
					}

					// Cerrar explícitamente el diálogo
					igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseDirDlgKey");
				}

				// Si el usuario cancela, también cerramos el diálogo
				if (!igfd::ImGuiFileDialog::Instance()->IsOk) {
					igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseDirDlgKey");
				}
			}


			// Nombre del archivo (sin extensión)
			ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));
			ImGui::SameLine();
			ImGui::Text("%s", extension); // Mostrar la extensión

			// Construir la ruta completa para mostrarla
			sprintf_s(fullPath, "%s%s%s", directory, filename, extension);
			ImGui::Text("Full path: %s", fullPath);

			// Verificar si el archivo ya existe
			bool fileExists = false;
			FILE* checkFile = fopen(fullPath, "r");
			if (checkFile) {
				fileExists = true;
				fclose(checkFile);
			}

			// Advertir si el archivo ya existe
			if (fileExists) {
				ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Warning: File already exists!");
			}

			// Mostrar botón normal de exportación o botón de sobrescritura
			bool canExport = false;

			if (isExporting) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
				ImGui::Button(fileExists ? "Exporting..." : "Exporting...", ImVec2(200, 30));
				ImGui::PopStyleVar();

				// Mostrar barra de progreso pequeña en la interfaz principal
				ImGui::Text("%s", exportStatusMessage.c_str());
				ImGui::ProgressBar(exportProgress, ImVec2(200, 10));
			}
			else {
				if (fileExists) {
					// Si el archivo existe, mostrar botón de sobrescritura con color de advertencia
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
					canExport = ImGui::Button("Overwrite Existing File", ImVec2(200, 30));
					ImGui::PopStyleColor(2);
				}
				else {
					// Botón normal de exportación
					canExport = ImGui::Button("Export to OBJ", ImVec2(200, 30));
				}
			}

			// Proceso de exportación
			if (canExport && !isExporting) {
				// Asegurarse de que el directorio termine con un separador
				size_t dirLen = strlen(directory);
				if (dirLen > 0 && directory[dirLen - 1] != '/' && directory[dirLen - 1] != '\\') {
					strcat_s(directory, "/");
				}

				// Actualizar la ruta completa por si ha cambiado
				sprintf_s(fullPath, "%s%s%s", directory, filename, extension);
				strcpy(exportingFilePath, fullPath);

				// Iniciar la exportación en un hilo separado
				isExporting = true;
				exportProgress = 0.0f;
				exportStatusMessage = "Preparando exportación...";

				// Iniciar el proceso de exportación asíncrono
				exportFuture = std::async(std::launch::async,
					exportObjThreaded, fullPath, resolution, includeLeaves,
					leafSize, randomness3D, branchRadiusMultiplier, currentLeafType, ls.getCurrent(),
					INITIAL_ANGLE, INITIAL_LENGTH, INITIAL_WIDTH, std::ref(exportProgress));
			}

			// Ventana modal para éxito
			if (ImGui::BeginPopupModal("Export Success", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "OBJ file exported successfully!");
				ImGui::Text("File saved to: %s", exportingFilePath);

				if (ImGui::Button("OK", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// Ventana modal para error de exportación
			if (ImGui::BeginPopupModal("Export Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error exporting OBJ file!");
				ImGui::Text("Could not write to file: %s", exportingFilePath);
				ImGui::Text("Please check if the directory exists and you have write permissions.");

				if (ImGui::Button("OK", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// Ventana modal para error de conversión
			if (ImGui::BeginPopupModal("Conversion Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error converting L-System to OBJ!");
				ImGui::Text("The L-System could not be converted to 3D geometry.");

				if (ImGui::Button("OK", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// Mostrar mensaje de éxito previo si corresponde
			if (export3D && !fileExists && !ImGui::IsPopupOpen("Export Success") &&
				!ImGui::IsPopupOpen("Export Error") && !ImGui::IsPopupOpen("Conversion Error")) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Last export was successful!");
			}
		}
	}

	ImGui::End();
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1200, 600), "L-System Interactive Builder");
	ImGui::SFML::Init(window);
	sf::View view = window.getDefaultView();
	sf::Clock deltaClock;

	LSystem lSystem;
	TreeRenderer treeRenderer(lSystem, window, INITIAL_ANGLE, INITIAL_LENGTH);

	bool readyToGenerate = false;
	bool ruleApplied = false;
	bool export3D = false;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (readyToGenerate) {
			lSystem.generate(5);
			readyToGenerate = false;
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		window.clear();
		treeRenderer.draw();
		imguiWindow(lSystem, readyToGenerate, ruleApplied, export3D);
		ImGui::SFML::Render(window);

		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}