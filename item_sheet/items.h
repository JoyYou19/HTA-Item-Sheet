#pragma once

#include <iostream>
#include <vector>
#include "../json/json.hpp"

using namespace std;

/* Everything related to information about items,
 * retrieving, storing, and modifying it to fit the Json File format*/

//The struct that stores the stats and values
struct StatStruct {
    int stat_id;
    float stat_value;
    string script_name;

    StatStruct(int statid, float statvalue, string scriptname);
};

// Define a struct to represent an item
struct JsonItem {
    string itemName;
    int rarity;
    string itemDescription;
    string spriteName;
    std::string scriptName;
    int itemStats;
    float itemValue;
    vector<StatStruct> stats;
    int itemStack;
    int itemID;
};



extern string gameMakerFileLocation;//Defines the string which responds to where the gamemaker project file

extern vector<string> item_stats;

extern vector<string> GetItemStats(string file_path);

extern vector<string> GetSprites(string file_path);

extern vector<string> GetScripts(string file_path);

extern string globalFilePath;

class Item {
private:
    std::string itemName;
    int rarity;
    std::string itemDescription;
    std::string spriteName;
    std::string scriptName;
    int itemStats;
    float itemValue;
    int itemStack;
    int itemID;
    vector<StatStruct> stats;

public:
    // Constructor to initialize the item
    Item(std::string name, int r, std::string description, std::string sprite_name, vector<StatStruct> statstruct, int itemStack, string scriptName);

    // Getter methods
    std::string GetName() const;
    int GetRarity() const;
    std::string GetDescription() const;
    std::string GetSprite() const;
    std::string GetScript() const;
    vector<StatStruct> GetStats() const;
    float GetValue() const;
    int GetStack() const;

    // Setter methods (if needed)
    void SetName(std::string name);
    void SetRarity(int r);
    void SetDescription(std::string description);
    void SetStats(int stats);
    void SetValue(float value);
};

//Creates the function that saves the class to a file (preferably json)
extern void SaveItemToFile(Item selectedItem);

//Deletes the item from the file
extern void DeleteItemFromFile(Item selectedItem);


//Saves and loads the location of the project so you wouldn't have to do it manually every time
extern void SaveProjectLocation(string* projectFileName);
extern string LoadProjectLocation();


extern std::vector<JsonItem> GetSaveFile();

