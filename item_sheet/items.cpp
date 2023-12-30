#include "items.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../json/json.hpp"


#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;

string globalFilePath = "C:/Users/valte/OneDrive/Documents/GitHub/HellTakesAllGit/datafiles";

// Constructor to initialize the item
Item::Item(std::string name, int r, std::string description, std::string sprite_name, vector<StatStruct> statstruct, int item_stack, std::string script_name)
    : itemName(std::move(name)), rarity(r), itemDescription(std::move(description)), spriteName(std::move(sprite_name)), scriptName(std::move(script_name)), stats(statstruct), itemStack(item_stack) {}

//Constructor to initialize the struct
StatStruct::StatStruct(int statid, float statvalue, string scriptname)
    : stat_id(statid), stat_value(statvalue), script_name(std::move(scriptname)) {}

// Getter methods
std::string Item::GetName() const {
    return itemName;
}

int Item::GetRarity() const {
    return rarity;
}

std::string Item::GetDescription() const {
    return itemDescription;
}

std::string Item::GetScript() const {
    return scriptName;
}

vector<StatStruct> Item::GetStats() const {
    return stats;
}

int Item::GetStack() const {
    return itemStack;
}


std::string Item::GetSprite() const {
    return spriteName;
}

float Item::GetValue() const {
    return itemValue;
}

// Setter methods (if needed)
void Item::SetName(std::string name) {
    itemName = std::move(name);
}

void Item::SetRarity(int r) {
    rarity = r;
}

void Item::SetDescription(std::string description) {
    itemDescription = std::move(description);
}

void Item::SetStats(int stats) {
    itemStats = stats;
}

void Item::SetValue(float value) {
    itemValue = value;
}

void SaveProjectLocation(string* projectFileName)
{
    // Save the default JSON to the file
    std::ofstream outputFile("project_file.txt");
    if (outputFile.is_open()) {
        outputFile << *projectFileName;
        outputFile.close();
        std::cout << "Default JSON created and saved to 'filename.json'." << std::endl;
    }
    else {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
    }
}
string LoadProjectLocation()
{
    // Open the file
    std::ifstream inputFile("project_file.txt");

    if (inputFile.is_open()) {
        // Read the first line
        std::string firstLine;
        if (std::getline(inputFile, firstLine)) {
            // Output the first line
            std::cout << "First line: " << firstLine << std::endl;

        }
        else {
            // Handle the case where the file is empty
            std::cout << "File is empty." << std::endl;
        }
        return firstLine;

        // Close the file
        inputFile.close();


    }
    else {
        // Handle the case where the file couldn't be opened
        std::cerr << "Error opening file." << std::endl;
    }

    return "";
}


// Convert an item to JSON
void to_json(json& j, const JsonItem& item) {
    json statj;
    int size = item.stats.size();
    for (int i = 0; i < size; i++)
    {
        statj.push_back({ {"stat_id", item.stats[i].stat_id} , {"stat_value", item.stats[i].stat_value}, {"script_name", item.stats[i].script_name} });
    }
    j = json{ {"item_name", item.itemName}, {"rarity", item.rarity},
        {"sprite_name", item.spriteName}, 
        {"stat_array", statj},
        {"item_description", item.itemDescription},
        {"item_stack", item.itemStack} ,{"item_id", item.itemID} };
}

// Convert JSON to an item
void from_json(const json& j, JsonItem& item) {
    j.at("item_name").get_to(item.itemName);
    j.at("sprite_name").get_to(item.spriteName);
    j.at("rarity").get_to(item.rarity);
    j.at("item_description").get_to(item.itemDescription);
    j.at("item_stack").get_to(item.itemStack);
    j.at("item_id").get_to(item.itemID);
    

    for (const auto& statJson : j.at("stat_array")) {
       StatStruct stat_s(0, 0, "");
       statJson.at("stat_id").get_to(stat_s.stat_id);
       statJson.at("stat_value").get_to(stat_s.stat_value);
       statJson.at("script_name").get_to(stat_s.script_name);
       // add other assignments as needed
       item.stats.push_back(stat_s);
    }

}

//The default saving function that saves the item if there are no entries in the json
void DefaultSave(Item selectedItem) {
    JsonItem item;
    item.itemName = selectedItem.GetName();
    item.itemDescription = selectedItem.GetDescription();
    item.itemValue = selectedItem.GetValue();
    item.rarity = selectedItem.GetRarity();
    item.spriteName = selectedItem.GetSprite();
    item.itemStack = selectedItem.GetStack();
    item.scriptName = selectedItem.GetScript();
    item.stats = selectedItem.GetStats();
    item.itemID = 0;

    json j;
    j["items"] = {};

    vector<JsonItem> itemArray;
    itemArray.push_back(item);

    j["items"] = itemArray;

    // Save the default JSON to the file
    std::ofstream outputFile(globalFilePath+"/" + "sample_file.json");
    if (outputFile.is_open()) {
        outputFile << j.dump(2);
        outputFile.close();
        std::cout << "Default JSON created and saved to 'filename.json'." << std::endl;
    }
    else {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
    }
}


//Save the class to a file
void SaveItemToFile(Item selectedItem) {
    // Open the existing JSON file for reading
    std::ifstream inputFile(globalFilePath + "/" + "sample_file.json");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
    }

    // Read the existing JSON from the file
    json j = json::parse(inputFile, nullptr, false);
    if (j.is_discarded())
    {
        std::cerr << "parse error" << std::endl;

        DefaultSave(selectedItem);
    }
    else {
        
        vector<JsonItem> itemArray;
        itemArray = j["items"];

        JsonItem item;
        item.itemName = selectedItem.GetName();
        item.itemDescription = selectedItem.GetDescription();
        item.itemValue = selectedItem.GetValue();
        item.rarity = selectedItem.GetRarity();
        item.spriteName = selectedItem.GetSprite();
        item.itemStack = selectedItem.GetStack();
        item.scriptName = selectedItem.GetScript();
        item.stats = selectedItem.GetStats();

        int itemSize = itemArray.size();

        bool add_to_array = true;
        //Checks for duplicate names
        for (int i = 0; i < itemSize; i++)
        {
            if (itemArray[i].itemName == item.itemName)
            {
                item.itemID = itemArray[i].itemID;
                itemArray[i] = item;
                add_to_array = false;
            }
        }

        if (add_to_array) {
            item.itemID = itemSize;
            itemArray.push_back(item);
        }

        j["items"] = itemArray;
        
        // Save the default JSON to the file
        std::ofstream outputFile(globalFilePath + "/" + "sample_file.json");
        if (outputFile.is_open()) {
            outputFile << j.dump(2);
            outputFile.close();
            std::cout << "Default JSON created and saved to 'filename.json'." << std::endl;
        }
        else {
            std::cerr << "Error: Unable to open the file for writing." << std::endl;
        }
    }

    // Close the input file
    inputFile.close();
}

//Deletes the item from the class
void DeleteItemFromFile(Item selectedItem) {
    // Open the existing JSON file for reading
    std::ifstream inputFile(globalFilePath + "/" + "sample_file.json");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
    }

    // Read the existing JSON from the file
    json j = json::parse(inputFile, nullptr, false);
    if (j.is_discarded())
    {
        std::cerr << "parse error" << std::endl;

    }
    else {

        vector<JsonItem> itemArray;
        itemArray = j["items"];

        JsonItem item;
        item.itemName = selectedItem.GetName();
        item.itemDescription = selectedItem.GetDescription();
        item.itemValue = selectedItem.GetValue();
        item.rarity = selectedItem.GetRarity();
        item.spriteName = selectedItem.GetSprite();
        item.itemStack = selectedItem.GetStack();
        item.scriptName = selectedItem.GetScript();
        item.stats = selectedItem.GetStats();

        int itemSize = itemArray.size();

        //Checks for duplicate names
        for (int i = 0; i < itemSize; i++)
        {
            if (itemArray[i].itemName == item.itemName)
            {
                itemArray.erase(itemArray.begin() + i);

                j["items"] = itemArray;

                // Save the default JSON to the file
                std::ofstream outputFile(globalFilePath + "/" + "sample_file.json");
                if (outputFile.is_open()) {
                    outputFile << j.dump(2);
                    outputFile.close();
                    std::cout << "Default JSON created and saved to 'filename.json'." << std::endl;
                }
                else {
                    std::cerr << "Error: Unable to open the file for writing." << std::endl;
                }
            }
        }
    }

    // Close the input file
    inputFile.close();
}


//Get the saved items

vector<JsonItem> GetSaveFile() {
    // Open the existing JSON file for reading
    std::ifstream inputFile(globalFilePath + "/" + "sample_file.json");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
    }

    vector<JsonItem> output_array;

    // Read the existing JSON from the file
    json j = json::parse(inputFile, nullptr, false);
    if (j.is_discarded())
    {
        std::cerr << "parse error" << std::endl;
    }
    else {
        output_array = j["items"];
    }


    // Close the input file
    inputFile.close();

    return output_array;
}

vector<string> GetItemStats(string file_path){
    // Specify the path to your .gml file
    std::string filePath = file_path;
    string output = "false";

    vector<string> item_stats;

    // Open the file
    std::ifstream inputFile(filePath);

    // Check if the file is opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file " << filePath << std::endl;
        return item_stats; // Return an error code
    }

    // Read the contents of the file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        // Process each line as needed
        std::cout << line << std::endl;

        string tempString = line.substr(8, 20);

        // Find the position of the space
        size_t spacePosition = tempString.find(' ');

        // Extract the substring up to the space
        string result = tempString.substr(0, spacePosition);

        item_stats.push_back(result);
    }

    // Close the file
    inputFile.close();

    output = "true";

    return item_stats;
}

vector<string> GetSprites(string file_path) {
    // Check if the path exists and is a directory
    vector<string> contents;

    if (fs::exists(file_path) && fs::is_directory(file_path)) {
        // Create a vector to store file/folder names

        // Loop through the contents of the folder
        for (const auto& entry : fs::directory_iterator(file_path)) {
            // Get the filename and add it to the vector
            contents.push_back(entry.path().filename().string());
        }

        // Display the names in the array
        std::cout << "Contents of the folder:\n";
        for (const auto& name : contents) {
            std::cout << name << "\n";
        }
    }
    else {
        std::cerr << "Error: Invalid folder path or folder does not exist.\n";
    }

    return contents;

}

vector<string> GetScripts(string file_path) {
    // Specify the path to your .gml file
    std::string filePath = file_path;


    vector<string> item_stats;

    // Open the file
    std::ifstream inputFile(filePath);

    // Check if the file is opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file " << filePath << std::endl;
        return item_stats; // Return an error code
    }

    // Read the contents of the file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        // Process each line as needed
        std::cout << line << std::endl;

        string tempString = line.substr(0, 8);

        if (tempString == "function") 
        {
            // Find the position of the space
            size_t spacePosition = line.find("(");

            // Extract the substring up to the space
            string result = line.substr(9, spacePosition-9);
            item_stats.push_back(result);
        }

    }

    // Close the file
    inputFile.close();


    return item_stats;

}