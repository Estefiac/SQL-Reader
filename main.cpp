/*
Created by Estefanía Anaya Cárdenas
Final Project : Data Structures
*/

#include<iostream>
#include <sstream>
#include <map>
#include <regex> 
#include <fstream>

using namespace std;

//We state all the functions that will be used throughout the system
void databaseStatement(string csv[][10], string types[], ofstream& file);
string match(string csv[], int rowQuantity, string types[]);
string secondMatch(string csv[], string metadata, int rowQuantity, string types[]);
void showSql(ifstream& results);

//The main is the one in charge of opening the file and creating the table, stating the csv data and the data type list
int main()
{
  //Stream in charge of the dataBase.sql file, with it you can open and write on the file
  ofstream file;
  file.open("dataBase.sql");
  //Stream in charge of the dataBase.sql file, with it you can read the file
  ifstream results("dataBase.sql");  

  //Array that contains the table gotten from a csv file
  string csv[11][10]=
  {
    {"id","objeto","estado","celular proveedor","numeroDeSerie","disponibles","precioPorPieza","onStock","añoMarca","horaDeSurtido"},
    {"1","Acuarelas","+","4424648524","249237","1200","123.25","true","1989","17:30:00"},
    {"2","Acrílicos","+","4428737937","237483","352","245.56","true","2006","17:30:00"},
    {"3","Caballete","+","4428397457","725483","2345","2530.84","false","1859","8:30:00"},
    {"4","Godete","+","4428396863","274925","2455","40.34","true","1998","8:30:00"},
    {"5","Pasteles","+","4428569273","163925","3623","639.48","true","2000","10:00:00"},
    {"6","Lienzo estándar","+","4428496396","482164","2453","345.45","true","1978","17:30:00"},
    {"7","Sketchbook","+","4428496937","737153","0","424.34","false","2003","17:30:00"},
    {"8","Pack de Pinceles","+","4421385319","163946","2485","255.35","true","1986","17:30:00"},
    {"9","Plumones","+","4424623531","956104","0","1245.35","false","2018","8:30:00"},
    {"10","Sellador","+","4424525423","730562","342","60.74","true","2013","8:30:00"}
  };

  //Array that contains the data types the system can match
  string types[10]=
  {
    {"TINYINT"},
    {"TINYTEXT"},
    {"CHAR"},
    {"BIGINT"},
    {"MEDIUMINT"},
    {"SMALLINT"},
    {"FLOAT"},
    {"BOOL"},
    {"YEAR"},
    {"TIME"},

  }; 
  
  //Statement to create a table in MYSQL is written into the file
  file << "CREATE table 'tabla';"<< endl; 

  //The function matchFunctions is called, getting csv(the table created), types(the list of data types) and file(the stream to write on the .sql file) as parameters
  databaseStatement(csv, types, file);
  
  cout<< "The CSV entered will create the next table in MYSQL" << endl;

  //The function showSql is called, getting results (the ifstream in charge of reading the .sql file) as parameter
  showSql(results);
  
  return 0;

}

//databaseStatement is the function in charge of creating the MYSQL statements and writing them on the .sql file once the data types have been stated
void databaseStatement(string csv [][10], string types[], ofstream& file){

  //colQuantity and rowQuantity are in charge of stating the dimensions of the csv table
  int colQuantity = sizeof(csv[0])/sizeof(csv[0][0]);
  int rowQuantity = sizeof(csv[0])/sizeof(csv[0][0]);

  //transpose is in charge of storing the csv table in the correct way to use it for the match of data types
  string transpose [rowQuantity][colQuantity];

  //This cycle translates the csv into the transpose table
  for(int i = 0; i < rowQuantity; i++)
  {
    for(int j = 0; j < colQuantity; j++)
    {
      transpose[i][j] = csv[j][i];
    }
  }

  //This cycle is in charge of traveling through each table's column
  for(int i = 0; i < colQuantity; i++)
  {
    //We call the function match and store its return into a string
    string currentMetadata = match(transpose[i], rowQuantity, types);
    //We call the function match and store its return into a string
    string finalMetadata = secondMatch(transpose[i], currentMetadata, rowQuantity, types);
    //Statement to alter the table and add a column in MYSQL is written into the file
    file << "ALTER table tabla ADD COLUMN \"" << transpose[i][0] << "\" " << finalMetadata << endl;
  }
  
   //This cycle is in charge of writing the statement to insert the column's values on the table in MYSQL into the file
  for(int i = 1; i <= colQuantity; i++)
  {
    file << "INSERT INTO tabla VALUES (" ;
    for(int j = 0; j < rowQuantity; j++)
    {
      file << csv[i][j];
      if(j == (rowQuantity-1))
      {
        file << "";
      }
      else
      {
        file << ", ";
      }
    }
    file << ")," << endl;
  }

  //The stream for the file is closed to be able to read it later on
  file.close();

}

//match is the function in charge of matching the elements of a column to the data type that best applies to the case (char, tinytext, int, bool or float)
string match(string csv[], int rowQuantity, string types[]) {
  
  //Here are the functions stated to match each data type depending on its characteristics
  std::regex regexBool("(true|false)");
  std::regex regexChar("(.{1})");
  std::regex regexFloat("([0-9]+[.][0-9]+)");
  std::regex regexInt("([0-9]+)");
  //metadata stores the data type for the column
  string metadata;

    //This process checks the first value and depending if it matches int, boolean or char a cycle goes through all the elements to know if it sticks to the same type or it matches better to int or float
    if (regex_match(csv[1], regexBool))
    {
      metadata = types[7];
    }
    else if (regex_match(csv[1], regexInt)|| regex_match(csv[1], regexFloat))
    {
      metadata = "INT";
      for (int i = 2; i < rowQuantity; i++)
      {
        if (regex_match(csv[i], regexInt) && metadata != "FLOAT")
        {
          metadata = "INT";
        }
        else if(regex_match(csv[i], regexFloat) || regex_match(csv[i], regexInt))
        {
          metadata = types[6];
          
        }
        else
        {
          metadata = types[1];
          break;
        }
      }
    }
    else if(regex_match(csv[1], regexChar))
    {
      metadata = types[2];
      for (int i = 2; i < rowQuantity; i++)
      {
        if(regex_match(csv[i], regexChar))
          {
            metadata = types[2];
          }
        else
         {
            metadata = types[1];
            break;
          }
      }
    }
    else {
      metadata = types[1];
    }

  //The function returns the data type for the next function to use it
  return metadata;
};

//secondMatch is the function in charge of matching the elements of a column to the data type that best applies to the case once the match function has established if it is an int or a text (time,year,tinyint,smallint,medium int or big int)
string secondMatch(string csv[],string metadata, int rowQuantity, string types[])
{
   //Here are the functions stated to match each data type depending on its characteristics
   std::regex regexTime("[0-9]{2}:[0-9]{2}:[0-9]{2}");
   std::regex regexTinyInt("^([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-4])$");
   std::regex regexSmallInt("^([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-5][0-9][0-9][0-9][0-9]|6[0-4][0-9][0-9][0-9]|65000)$");
   std::regex regexYear("(1[9][0-9][1-9]|2[0-1][0-4][0-9]|215[0-5])");
   std::regex regexMediumInt("^([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-9][0-9][0-9][0-9]|[1-9][0-9][0-9][0-9][0-9]|[1-9][0-9][0-9][0-9][0-9][0-9]|[1-9][0-9][0-9][0-9][0-9][0-9][0-9]|[1-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]|[1-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]|1000000000)$");
  
  //This process checks if the values matched to int is a small,tiny,medium or big int, if it is a year or if it is a time data type
  if (metadata != "INT")
  {
    if (metadata == types[1]){
      if (regex_match(csv[1], regexTime))
      {
        metadata = types[9];
        return metadata;
      }
      else
      {
        metadata = types[1];
        return metadata;
      }
    }
    else
    {
      return metadata;
    }
  }
  else 
  {
    for (int i = 0; i < rowQuantity; i++)
    {
      if(regex_match(csv[i], regexTinyInt) && metadata != types[5])
      {
        metadata = types[0];
      }
      else if(regex_match(csv[i], regexYear))
      {
        metadata = types[8];
      }
      else if(regex_match(csv[i], regexSmallInt) && metadata != types[8])
      {
        metadata = types[5];

      }
      else if(regex_match(csv[i], regexMediumInt))
      {
        metadata = types[4];
      }
      else
      {
        metadata = types[3];
      }
    }
    //The function returns the data type to use it on the MYSQL statments for the columns
    return metadata;
  }
};

//showSql is the function in charge of reading the file dataBase.sql after all statements have been written
void showSql(ifstream& results){
    
  if (results.is_open())
  {
    std::cout << results.rdbuf();
  }  
}

