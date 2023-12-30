#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include <filesystem>

#include "items.h"
#include "fileDialogue.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) {
		return true;
	}

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) //Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); //Set click points, or where you are clicking within the app
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}
	}

	}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(
	const char* windowName,
	const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = reinterpret_cast<WNDPROC>(WindowProcess);
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

		
	window = CreateWindowA(
		className,
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

}


bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d) {
		return false;
	}

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL) {
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}


void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

vector<JsonItem> item_array;
string gameMakerFileLocation = "";
string fileLocation_str = "Select GML Project File Path";

bool result = FALSE;
string statFileLocation = "";
string spriteFileLocation = "";
string scriptFileLocation = "";

string spriteName = "";
string scriptName = "";

bool current_item = FALSE;
vector<string> stats;
vector<string> sprites;
vector<string> scripts;

//Create the tab section for a struct that stores the items
StatStruct statDefaults(0,0,"");
vector<StatStruct> active_tabs = { statDefaults };

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ImGui::CreateContext();
	// Set ImGui font size
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Users/valte/AppData/Local/Microsoft/Windows/Fonts/RobotoSlab-Bold.ttf", 30.f);
	//io.FontGlobalScale = 1.5;

	//Get the current items from the memory
	item_array = GetSaveFile();
	gameMakerFileLocation = LoadProjectLocation();
	if (gameMakerFileLocation != "") 
	{
		string tempString = gameMakerFileLocation.substr(0, 20);
		tempString += "...";
		fileLocation_str = tempString;

		statFileLocation = gameMakerFileLocation + "\\" + "scripts" + "\\" + "stat_macros" + "\\" + "stat_macros.gml";
		stats = GetItemStats(statFileLocation);

		spriteFileLocation = gameMakerFileLocation + "\\" + "sprites";
		sprites = GetSprites(spriteFileLocation);

		scriptFileLocation = gameMakerFileLocation + "\\" + "scripts" + "\\" + "item_scripts" + "\\" + "item_scripts.gml";
		scripts = GetScripts(scriptFileLocation);

		

	}
	//Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	//Handle loss of d3d9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

int itemStats = 0;
float itemValue = 0.0f;

//Stats for each item
int rarity_current = 0;
char item_name[32] = "New Item";
char item_description[100] = "New Item";
char item_stack[3] = "8";
int stat_current = 0;
char stat_value[256] = "0";
int sprite_current = 0;
int script_current = 0;

void gui::Render() noexcept
{

	ImGui::SetNextWindowPos({ WIDTH / 2 + 40,0 });
	ImGui::SetNextWindowSize({ WIDTH / 2 - 100,HEIGHT });
	ImGui::Begin(
		"Items List",
		&exit,
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse
	);
	if (item_array.size() > 0) {
		for (int i = 0; i < item_array.size(); i++)
		{
			if (ImGui::Button(item_array[i].itemName.c_str())) 
			{
				current_item = TRUE;
				strcpy_s(item_name, item_array[i].itemName.c_str());
				strcpy_s(item_description, item_array[i].itemDescription.c_str());
				rarity_current = item_array[i].rarity;
				int stack = item_array[i].itemStack;
				strcpy_s(item_stack, to_string(stack).c_str());

				if (stats.size() > 0) {
					/*stat_current = item_array[i].itemStats;
					float value = item_array[i].itemValue;
					string valueStr = to_string(value);
					strcpy_s(stat_value, valueStr.c_str());*/

					active_tabs = item_array[i].stats;

				}
				if (scripts.size() > 0) 
				{
					if (stat_current == 21) {
						for (int j = 0; j < scripts.size(); j++)
						{

							if (scripts[j] == item_array[i].scriptName)
							{
								script_current = j;
							}
						}

					}
				}
				if (sprites.size() > 0) {
					for (int j = 0; j < sprites.size(); j++) 
					{

						if (sprites[j] == item_array[i].spriteName) 
						{
							sprite_current = j;
						}
					}

				}
			}
		}
	}

	ImGui::End();

	//Start the main window
	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({ WIDTH/2,HEIGHT });
	ImGui::Begin(
		"Hell Takes All Item Sheet",
		&exit,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	if (ImGui::Button(fileLocation_str.c_str())) {
		result = openFile();

		if (result == TRUE) {


			gameMakerFileLocation = sFilePath;
			string tempString = gameMakerFileLocation.substr(0, 20);
			tempString += "...";
			fileLocation_str = tempString;

			statFileLocation = gameMakerFileLocation + "\\" + "scripts" + "\\" + "stat_macros" + "\\" + "stat_macros.gml";
			stats = GetItemStats(statFileLocation);

			spriteFileLocation = gameMakerFileLocation + "\\" + "sprites";
			sprites = GetSprites(spriteFileLocation);

			scriptFileLocation = gameMakerFileLocation + "\\" + "scripts" + "\\" + "item_scripts.gml";
			scripts = GetScripts(scriptFileLocation);

			SaveProjectLocation(&gameMakerFileLocation);
		}
	}
	std::filesystem::path currentPath = std::filesystem::current_path();
	std::cout << "Current Working Directory: " << currentPath << std::endl;
	ImGui::Text(currentPath.string().c_str());
	ImGui::Spacing();

	if (ImGui::BeginTabBar("Item options"))
	{
		if (ImGui::BeginTabItem("Selection")) {

			//The button for creating a new item
			if (ImGui::Button("Create Item")) {

				current_item = TRUE;
			}

			//Showing all the items contents

			if (current_item == TRUE) {
				ImGui::Spacing();
				if (ImGui::BeginChild(1)) {
					if (ImGui::CollapsingHeader("Item Options")) {

						//Show the Item Name
						ImGui::PushItemWidth(150);
						ImGui::InputText("Item Name", item_name, 32);

						//Show rarity
						ImGui::PushItemWidth(160);
						const char* items[] = { "BASIC", "RARE", "MAGIC", "UNIQUE" };
						ImGui::Combo("Rarity", &rarity_current, items, IM_ARRAYSIZE(items));

						//Show the Item Name
						ImGui::PushItemWidth(WIDTH/2-150);
						ImGui::InputText("Item Description", item_description, 100);

						//Show the Item Name
						ImGui::PushItemWidth(40);
						ImGui::InputText("Maximum Stack", item_stack, 3);

						if (stats.size() > 0) {

							int tab_size = active_tabs.size();

							for (int g = 0; g < tab_size; g++) 
							{
								//Show item stats
								ImGui::Spacing();
								ImGui::PushItemWidth(280);
								//Get the size of the vector and create an array with a minimum size of 50
								int vec_size = stats.size();
								const char* stat_array[50];

								// Copy each string to the array
								for (size_t i = 0; i < vec_size; ++i) {
									stat_array[i] = stats[i].c_str();
								}
								string stat_string = "Item Stats" + to_string(g);
								ImGui::Combo(stat_string.c_str(), &active_tabs[g].stat_id, stat_array, vec_size, 15);

								//Show the stat value
								ImGui::PushItemWidth(50);
								string value_string = "Stat Value" + to_string(g);
								string value_s = to_string(active_tabs[g].stat_value);
								// Create a dynamic char array and copy the string
								size_t size = value_s.length() + 1;
								char* value_char = new char[size];

								// Use strcpy_s with the size parameter
								strcpy_s(value_char, size, value_s.c_str());
								if (active_tabs[g].stat_id != 21) {
									ImGui::InputText(value_string.c_str(), value_char, 5);
								}

								active_tabs[g].stat_value = stof(value_char);

								if (active_tabs[g].stat_id == 21)
								{
									if (scripts.size() > 0)
									{
										//Show item stats
										ImGui::PushItemWidth(280);
										//Get the size of the vector and create an array with a minimum size of 50
										int script_size = scripts.size();
										const char* script_array[250];

										string script_string = "Scripts" + to_string(g);
										string script_s = active_tabs[g].script_name;

										int current_script = 0;
										// Copy each string to the array
										for (size_t i = 0; i < script_size; ++i) {
											script_array[i] = scripts[i].c_str();
											if (script_s == script_array[i])
											{
												current_script = i;
											}
										}

										ImGui::Combo(script_string.c_str(), &current_script, script_array, script_size, 15);
										
										active_tabs[g].script_name = script_array[current_script];
										scriptName = script_array[script_current];
									}
								}

							}

						}

						//Draw the add stat button
						if (ImGui::Button("Add Stat"))
						{
							active_tabs.push_back(statDefaults);
						}

						//Remove the current stat
						int tab_size = active_tabs.size();
						if (tab_size > 1) {
							ImGui::SameLine();
							//Draw the add stat button

							if (ImGui::Button("Remove Stat"))
							{
								active_tabs.pop_back();
							}
						}



						if (sprites.size() > 0) {
							//Show item stats
							ImGui::PushItemWidth(280);
							//Get the size of the vector and create an array with a minimum size of 50
							int vec_size = sprites.size();
							const char* sprite_array[300];

							// Copy each string to the array
							for (size_t i = 0; i < vec_size; ++i) {
								sprite_array[i] = sprites[i].c_str();
							}
							ImGui::Combo("Select Sprite", &sprite_current, sprite_array, vec_size, 15);

							spriteName = sprites[sprite_current];
						}

						ImGui::Spacing();

						if (ImGui::Button("Save Item")) {
							Item selectedItem(item_name, rarity_current, item_description, spriteName, active_tabs, stoi(item_stack),scriptName);

							SaveItemToFile(selectedItem);

							current_item = FALSE;
						}
						ImGui::SameLine(ImGui::GetWindowWidth() - 100.0f);
						if (ImGui::Button("Delete Item"))
						{
							Item selectedItem(item_name, rarity_current, item_description, spriteName, active_tabs, stoi(item_stack), scriptName);

							DeleteItemFromFile(selectedItem);

							current_item = FALSE;
						}
					}
					ImGui::EndChild();

				}

			}


			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}

	ImGui::End();

	//Show the demo window
	//ImGui::ShowDemoWindow();

}