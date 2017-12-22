//  Created by Nguyen Thai An on 10/3/17.
//  Copyright © 2017 Nguyen Thai An. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <map>

using namespace std;

/*Path string for reading html input file*/
const string htmlPath = "/Users/nguyenthaian/Desktop/EngGrammar/Steganography/CypherHTML/";
const int numberOfAttribute = 2; // The minimum number of attributes which could be used.
const map<string, string> convertTable = {
    {"00100000"," "},{"01000001","A"},{"01001100","L"},{"01010111","W"},{"01101000","h"},{"01110011","s"},
    {"00110000","0"},{"01000010","B"},{"01001101","M"},{"01011000","X"},{"01101001","i"},{"01110100","t"},
    {"00110001","1"},{"01000011","C"},{"01001110","N"},{"01011001","Y"},{"01101010","j"},{"01110101","u"},
    {"00110010","2"},{"01000100","D"},{"01001111","O"},{"01011010","Z"},{"01101011","k"},{"01110110","v"},
    {"00110011","3"},{"01000101","E"},{"01010000","P"},{"01100001","a"},{"01101100","l"},{"01110111","w"},
    {"00110100","4"},{"01000110","F"},{"01010001","Q"},{"01100010","b"},{"01101101","m"},{"01111000","x"},
    {"00110101","5"},{"01000111","G"},{"01010010","R"},{"01100011","c"},{"01101110","n"},{"01111001","y"},
    {"00110110","6"},{"01001000","H"},{"01010011","S"},{"01100100","d"},{"01101111","o"},{"01111010","z"},
    {"00110111","7"},{"01001001","I"},{"01010100","T"},{"01100101","e"},{"01110000","p"},{"01111110","~"},
    {"00111000","8"},{"01001010","J"},{"01010101","U"},{"01100110","f"},{"01110001","q"},
    {"00111001","9"},{"01001011","K"},{"01010110","V"},{"01100111","g"},{"01110010","r"},
};

/* The structe of an attribute pair*/
struct pair_Atribute {
    string priAttr; // the primary attribute
    string secAttr; // the secondary attribute
};

/*
 The html class
 */
class html {
    
public:
    html();
    
    string plainText; // the final result which needed to find
    int sizeOfMess;   // use to count the number of bit that use for testing stage
    string htmlAll;   // a big string that help scanning tag become easier
    
    vector<string> listTag; // the list of tags found from htmlAll
    vector<string> htmlInfor; // the list of lines read from the input file
    vector<vector<string>> listAttributes;
    vector<vector<string>> sorted_List_Attributes; // the list of sorted attributes
    vector<pair_Atribute> get_SortedPair; // the list of sorted attribute pairs
    vector<pair_Atribute> get_OrigPair;   // the list of unsorted attribute pairs
    
    void init_html();
    string combineAll( vector<string> var);
    string getMess (vector<pair_Atribute> origPairList, vector<pair_Atribute> sortedPairList);
    string convertHex_String (string bin_String);
    vector<string> readFileHtml ();
    vector<string> getTag (string inputHtml);
    vector<string> deleteNoAttributeTag (vector<string> listTag);
    vector<string> sortingAlphabe (vector<string> var);
    vector<vector<string>> getAttributes(vector<string> listTag);
    vector<vector<string>> sortingAttribute(vector<vector<string>> listAttributes);
    vector<pair_Atribute> builtPair (vector<vector<string>> listAttribute);
    vector<pair_Atribute> builtOri_Pair (vector<vector<string>> listAttribute);
};

/*
 The function is to read string from html file
 */
vector<string> html::readFileHtml(){
    vector<string> result;
    string line;
    ifstream inputFile;
    string fileName;
    string pathFile = htmlPath;
    
    cout<<"Please include the htlm name file with its extension:"<<endl;
    cin>>fileName;
    
    pathFile.append(fileName);
    inputFile.open(pathFile); // Read from file path
    if (inputFile.fail()) {
        cout<<"Error Opening File"<< endl;
    }
    while (std::getline(inputFile, line)) {
        result.push_back(line);
    }
    inputFile.close();
    return result;
}

/*
 Combine all lines to a big string. That process easier
 */
string html::combineAll(vector<string> var){
    string result;
    for (size_t i = 0; i < var.size(); i ++) {
        result = result + var[i];
    }
    return result;
}

/*
 Get tags from a big string
 */
vector<string> html::getTag( string var){
    vector<string> result;
    string tag;
    size_t index1= 0;
    while (index1 < var.size()) {
        size_t temp1 = var.find('<', index1);
        size_t temp2 = var.find('>', index1 + 1);
        
        string copy = "";
        for (size_t i = temp1; i <= temp2; i ++) {
            copy = copy + var[i];
        }
        result.push_back(copy);
        index1 = temp2 + 1;
    }
    return result;
}

/*
 Delete tags which the number of attributes is less than 2
 */
vector<string> html::deleteNoAttributeTag(vector<string> var){
    int countSpace = 0;
    size_t index = var.size() - 1;
    
    while ((int)index >= 0) {
        for (size_t i = 0; i < var[index].size(); i ++) {
            if (var[index][i] == '=' && var[index][i+1] =='"' && var[index][i-1] !='=') {
                countSpace ++;
            }
        }
        if (countSpace < numberOfAttribute) {
            var.erase(var.begin() + index);
        }
        countSpace = 0;
        index --;
    }
    
    for (size_t i = var.size()-1; (int)i >= 0; i --) {
        
        size_t temp1 = var[i].find('<', 0);
        size_t temp2 = var[i].find(' ', 0);
        string copy = var[i].substr(temp1, temp2 - temp1);
        if (copy.size() > 10) {
            var.erase(var.begin() + i);
        }
    }
    
    return var;
}

/* Get attributes of each tag*/
vector<vector<string>> html::getAttributes(vector<string> var){
    vector<vector<string>> result;
    vector<size_t> location;
    vector<string> temp;
    
    for (size_t i = 0; i < var.size(); i ++) {
        //Count number of attribute of each tag
        int countAttributes = 0;
        for (size_t e = 0; e < var[i].size(); e ++) {
            if (var[i][e] =='=' && var[i][e+1] == '"' && var[i][e-1] !='=') {
                countAttributes ++;
                location.push_back(e);
            }
        }
        
        for (size_t index = 0; index < location.size(); index ++) {
            string copy;
            for (size_t j = location[index]-1; j > 0; j --) {
                if (var[i][j] == ' ' || var[i][j] ==',' || var[i][j] == ';') {
                    break;
                }
                copy = var[i][j] + copy;
            }
            temp.push_back(copy);
        }
        result.push_back(temp);
        temp.clear();
        location.clear();
    }
    return result;
}

/* Insertion Sort for each list of attributes*/
vector<string> html::sortingAlphabe(vector<string> var){
    vector<string> temp;
    temp.push_back(" ");
    for (size_t i = 0; i < var.size(); i ++) {
        temp.push_back(var[i]);
    }
    
    string key;
    for (size_t j = 2; j < temp.size(); j ++) {
        key = temp[j];
        size_t i = j - 1;
        while (i > 0 && key.compare(temp[i]) < 0) {
            temp[i + 1] = temp [i];
            i --;
        }
        temp[i + 1] = key;
    }
    
    var.clear();
    for (size_t i = 1; i < temp.size(); i ++) {
        var.push_back(temp[i]);
    }
    return  var;
}

/* sorting the list of attributes*/
vector<vector<string>> html::sortingAttribute(vector<vector<string>> var){
    for (size_t i = 0; i < var.size(); i ++) {
        var[i] = sortingAlphabe(var[i]);
    }
    return var;
}

/* Get the pair of attributes in each tag*/
vector<pair_Atribute> html::builtPair(vector<vector<string>> var) {
    vector<pair_Atribute> result;
    pair_Atribute temp;
    
    for (size_t i = 0; i < var.size(); i ++) {
        int  numberOfPair = (int)var[i].size()/2;
        size_t index = 0;
        int gotpair = 0;
        while (gotpair != numberOfPair) {
            temp.priAttr = var[i][index];
            temp.secAttr = var[i][var[i].size() - index - 1];
            result.push_back(temp);
            index ++;
            gotpair ++;
        }
    }
    return result;
}

/* Get the pair of attribute in each tag*/
vector<pair_Atribute> html::builtOri_Pair(vector<vector<string>> var){
    vector<pair_Atribute> result;
    pair_Atribute temp;
    for (size_t i = 0; i < var.size(); i ++) {
        int  numberOfPair = (int)var[i].size()/2;
        size_t index = 0;
        int gotPair = 0;
        while (gotPair != numberOfPair) {
            temp.priAttr = var[i][index];
            temp.secAttr = var[i][index + 1];
            result.push_back(temp);
            gotPair ++;
            index = index + 2;
        }
    }
    return result;
}

/*Compare per pair of attribute to get hidden binary*/
string html::getMess(vector<pair_Atribute> x, vector<pair_Atribute> y){
    string result;
    size_t index = 0;
    while (index < x.size() && index < y.size()) {
        if (x[index].priAttr.compare(y[index].priAttr) == 0) {
            result = result + '1';
        } else {
            result = result + '0';
        }
        index ++;
    }
    return result;
}

/* COnvert from binary string to hex string. The string is stopped converting at '~'*/
string html::convertHex_String(string binString){
    string result;
    size_t index = 0;
    int numberOfBit = 8;
    
    while (index < binString.size()) {
        string temp;
        string strfound;
        temp = binString.substr(index, numberOfBit);
        
        try {
            strfound = convertTable.at(temp);
        } catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error: The message contains a character that coun't convert to hex" << oor.what() << '\n';
        }
        
        if (strfound.compare("~") == 0) {
            break;
        }
        result = result + strfound;
        index = index + 8;
    }
    return result;
}

/* Run function */
void html:: init_html(){
    
    htmlInfor = readFileHtml();
    htmlAll = combineAll(htmlInfor);
    listTag = getTag(htmlAll);
    listTag = deleteNoAttributeTag(listTag);
    listAttributes = getAttributes(listTag);
    sorted_List_Attributes = sortingAttribute(listAttributes);
    
    cout<<"************************************List tag***************************"<< endl;
    for (size_t i = 0; i < listTag.size(); i ++) {
        printf("%d", (int)i+1);
        cout<< listTag[i]<< endl;
        cout<<"\n"<<endl;
        
        cout<<"Attributes in the tag:"<<endl;
        for (size_t j = 0; j < listAttributes[i].size(); j ++) {
            cout<<listAttributes[i][j] << endl;
        }
        cout<<"\n"<< endl;
        
        cout<<"Sorting attributes in the tag"<< endl;
        for (size_t j = 0; j < sorted_List_Attributes[i].size(); j ++) {
            cout<< sorted_List_Attributes[i][j]<<endl;
        }
        cout<<"\n"<< endl;
        
    }
    
    cout<<"************************************Pair Attribute***************************"<< endl;
    get_SortedPair = builtPair(sorted_List_Attributes);
    get_OrigPair = builtOri_Pair(listAttributes);
    for (size_t i = 0; i < get_SortedPair.size(); i ++) {
        printf("The [%d]th pair:\n", (int)i+1);
        cout<<"The sorted pair"<< endl;
        cout<<"the pri attribute ="<< get_SortedPair[i].priAttr<<endl;
        cout<<"the second attribute ="<< get_SortedPair[i].secAttr<<endl;
        cout<<"\n"<< endl;
        cout<<"The encoded pair"<<endl;
        cout<<"the pri attribute ="<< get_OrigPair[i].priAttr<<endl;
        cout<<"the second attribute ="<< get_OrigPair[i].secAttr<<endl;
        cout<<"\n"<< endl;
    }
    
    
    
    plainText = getMess(get_OrigPair, get_SortedPair);
    plainText = convertHex_String(plainText);
    cout<<"The hidden plain text is:"<< endl << plainText <<endl;
}

html::html()
{
    sizeOfMess = 0;
    listTag.clear();
    htmlInfor.clear();
    listAttributes.clear();
    sorted_List_Attributes.clear();
    get_OrigPair.clear();
    get_SortedPair.clear();
}


int main(int argc, const char * argv[]) {
    html H = *new html();
    H.init_html();
    
    return 0;
}
