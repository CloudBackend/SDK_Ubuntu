#include "Notice.h"

#include <algorithm>  // std::find_if_not
#include <cctype>     // std:isspace, std::tolower, std::toupper
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>    // std::setw
#include <ios>        // std::left
#include <iostream>
#include <sstream>    // std::ostringstream
#include <stdexcept>
#include <string>

#include "cbe/Database.h"

#include "MyDelegates.cpp"
#include "MyGenericFunctions.cpp"
#include "../user_credentials.cpp"

// - - - - - - - - - - - - - - - - - FUNCTIONS - - - - - - - - - - - - - - - - - 

// Exit function that reports how program was terminated
void Notice::exitProgram(const int exitCode) {
  std::ostringstream oss;
  oss << "Exiting program with exit code: " << exitCode << ".";
  if (myCloudBackend) {
    myCloudBackend.terminate();
  }
  throw std::invalid_argument( oss.str() );
}

// Creates a new container in the parentContainer argument that is passed in
cbe::Container Notice::createMyContainer(cbe::Container parentContainer,
                                         const std::string& name) {
  // Create a delegate that is going to be used for the createContainer method
  std::shared_ptr<MyCreateContainerDelegate> createContainerDelegate = 
                                  std::make_shared<MyCreateContainerDelegate>();
  parentContainer.createContainer(name, createContainerDelegate);
  
  // Calling wait for the delegate to finish
  createContainerDelegate->waitForRsp();

  // Check if error, can also check if a container was constructed.
  if (createContainerDelegate->errorInfo){ 
    std::cout << "Error!" << std::endl;
    std::cout << "Container creation failed." << std::endl;
    
    // Print info about error 
    std::cout << createContainerDelegate->errorInfo << std::endl;
    
    // Exit program with unique code 
    exitProgram(21);
  }

  // Get container from the delegate
  return createContainerDelegate->container;
}

// Creates a new container in the parentContainer argument that is passed in
cbe::Container Notice::createMyContainer(cbe::Container parentContainer) {
  std::cout << "Notice: Create container" << std::endl;
  // Generic function to get string from user input
  const auto name = inquireString("Set name for Container", "");

  return createMyContainer(parentContainer, name);
}

// Loads container contents to be able to see what is inside
// the container that is passed in
void Notice::loadContainerContents(cbe::Container container) {
  // Generic function to print information about container
  // std::cout << "Getting sub-container of " << containerName(container, false) 
  //           << std::endl;
  
  // Create a filter that limits the query to only return containers 
  cbe::Filter containerFilter;
  containerFilter.setDataType(cbe::ItemType::Container);
  std::shared_ptr<MyQueryDelegate> queryDelegate = 
                                            std::make_shared<MyQueryDelegate>();

  // Call query function with filter and delegate
  container.query(containerFilter, queryDelegate);

  // Wait for delegate to finish
  queryDelegate->waitForRsp();
  
  // Check if error 
  if (queryDelegate->errorInfo){ // Could also check if qResult was constructed
    std::cout << "Error!" << std::endl;
    std::cout << "Could not load container contents." << std::endl;

    // Print info about error 
    std::cout << queryDelegate->errorInfo << std::endl;

    // Exit program with unique code
    exitProgram(30);
  }
  
  // Get queryResult from the delegate
  this->qResult = queryDelegate->queryResult;
}

void Notice::loadContainerContents(cbe::ContainerId containerId) {
  // Generic function to print information about container
  // std::cout << "Getting sub-container of " << containerName(container, false) 
  //           << std::endl;
  
  // Create a filter that limits the query to only return containers 
  cbe::Filter containerFilter;
  std::shared_ptr<MyQueryDelegate> queryDelegate = 
                                            std::make_shared<MyQueryDelegate>();

  // Call query function with filter and delegate
  myCloudBackend.query(containerId, containerFilter, queryDelegate);

  // Wait for delegate to finish
  queryDelegate->waitForRsp();
  
  // Check if error 
  if (queryDelegate->errorInfo){ // Could also check if qResult was constructed
    std::cout << "Error!" << std::endl;
    std::cout << "Could not load container contents." << std::endl;

    // Print info about error 
    std::cout << queryDelegate->errorInfo << std::endl;

    // Exit program with unique code
    exitProgram(30);
  }
  
  // Get queryResult from the delegate
  this->qResult = queryDelegate->queryResult;
}


// Function that prints container contents
void Notice::printContainerContents(cbe::QueryResult qResult) {  
  // Put queryResult in vector of cbe::Item 
  std::vector<cbe::Item> items = qResult.getItemsSnapshot();
  std::cout << "------------------------------------------------------------" 
            << std::endl;
  // Loop through items and print them
  for (const auto& item : items) {
    // Generic print function 
    printItem(item, false);
  }
  std::cout << "------------------------------------------------------------" 
            << std::endl;
}

// The same as loadContainerContents but with different filter
void Notice::loadContainerObjects(cbe::Container container) {
  // std::cout << "Getting objects in container "
  //           << containerName(container, false) 
  //           << std::endl;
  std::shared_ptr<MyQueryDelegate> queryDelegate = 
                                            std::make_shared<MyQueryDelegate>();

  // Create a filter that limits the query to only return objects 
  cbe::Filter objectFilter;
  objectFilter.setDataType(cbe::ItemType::Object);
  // objectFilter.setItemOrder(cbe::FilterOrder::Published);
  objectFilter.setAscending(false);
  objectFilter.setByPassCache(true);
 
  // Call query function with filter and delegate
  container.query(objectFilter, queryDelegate);

  // Wait for delegate to finish
  queryDelegate->waitForRsp();
   // Check if error 
  if (queryDelegate->errorInfo){ // Can check if qResult was constructed.
    std::cout << "Error!" << std::endl;
    std::cout << "Could not load container contents." << std::endl;

    // Print info about fail
    std::cout << queryDelegate->errorInfo << std::endl;

    // Exit program with unique code 
    exitProgram(33);
  }

  // Get queryResult from the delegate
  this->qResult = queryDelegate->queryResult;
}

void Notice::printMessages(cbe::QueryResult queryResult) {
  cbe::Object tempObject{cbe::DefaultCtor{}}; // nullptr;
  std::cout << "current notices: "
            << std::endl
            << GREY
            << "+---------------------------------------" 
            << "----------------------------------------"
            << "----------------------------------------"
            << RESET
            << std::endl;
  
  // Loop through objects from queryResult
  for (const auto& item : queryResult.getItemsSnapshot()) {
    if (item.type() == cbe::ItemType::Object) {
      // printItem(item, false);
      tempObject = cbe::CloudBackend::castObject(item);

      // Get key/value information to print
      const auto keyValues = tempObject.keyValues();
      if (!keyValues.empty()) {
          std::cout << GREY
                    << "| "
                    << CYAN
                    << std::left
                    << std::setw(15)
                    << tempObject.name()
                    << std::setw(0)
                    << ' ';
        timestamp = tempObject.updated() ;
        std::cout << GREY
                  << "| "
                  << CYAN;
        now = timestamp;
        ts = *localtime(&now);
        strftime(bufDay, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
        printf("%s", bufDay);
        // Loop through key/values for each object
        for (const auto& keyValue : keyValues) {
          const auto& key      = keyValue.first;
          const auto& sdkValue = keyValue.second;
          const auto  indexed  = std::get<bool>(sdkValue);
          if (indexed) {
            indexMark = "i";
          } else {
            indexMark = "-";
          }
          std::cout << GREY
                    << " | "
                    << key << ": "
                    << "["
                    << indexMark
                    << "] "
                    << CYAN
                    << std::left
                    << std::setw(16)
                    << std::get<std::string>(sdkValue)
                    << std::setw(0);
        }
        std::cout << std::endl;

      }  // if (!keyValues.empty())
    }   // if (item.type() == cbe::ItemType::Object)
  }
  std::cout << GREY
            << "+---------------------------------------"
            << "----------------------------------------"
            << "----------------------------------------"
            << RESET
            << std::endl;

  // If container does not contain any objects
  if (!tempObject) {
    std::cout << "Sorry, no objects found in the Container!" << std::endl;
  }
}

void Notice::printObjects(cbe::QueryResult queryResult) {
  cbe::Object tempObject{cbe::DefaultCtor{}}; // nullptr;
  std::cout << "Printing notices: " << std::endl;
  std::cout << "------------------------------------------------------------" 
            << std::endl;
  
  // Loop through objects from queryResult
  for (const auto& item : queryResult.getItemsSnapshot()) {
    if (item.type() == cbe::ItemType::Object) {
      printItem(item, false);
      tempObject = cbe::CloudBackend::castObject(item);

      // Get key/value information to print
      const auto keyValues = tempObject.keyValues();
      if (!keyValues.empty()) {

        // Loop through key/values for each object
        for (const auto& keyValue : keyValues) {
          const auto& key      = keyValue.first;
          const auto& sdkValue = keyValue.second;
          const auto  indexed  = std::get<bool>(sdkValue);
          std::cout << "    " << key << " = " << "'"
                    << std::get<std::string>(sdkValue)
                    << "'" << (indexed? " \t\t(indexed)" : "") << std::endl;
        }
      }  // if (!keyValues.empty())
    }   // if (item.type() == cbe::ItemType::Object)
  }
  std::cout << "------------------------------------------------------------" 
            << std::endl;

  // If container does not contain any objects
  if (!tempObject) {
    std::cout << "Sorry, no objects found in the Container!" << std::endl;
  }
}

cbe::Container Notice::pickContainer(const std::string& containerName) {
  // std::cout << "Pick Container" << std::endl;
  const auto items = this->qResult.getItemsSnapshot();
  // Loop through and look for container
  for (const auto& item : items) {
    // If container is found, return it 
    if (item.name() == containerName) {
      return cbe::CloudBackend::castContainer(item);
    }
  }

  // If container is not found
  std::cout << "Error: the container you asked for, \"" << containerName
            << "\", was not found." << std::endl;
  // Loops through available containers to select 
  for (const auto& item : items) {
    printItem(item, false);
  }
  return cbe::Container{cbe::DefaultCtor{}};  // empty container
}

cbe::Object Notice::pickObject(const std::string& objectName) {
  const auto items = this->qResult.getItemsSnapshot();
  for (const auto& item : items) {
    if (item.name() == objectName) {
      return cbe::CloudBackend::castObject(item);
    }
  }

  // If it is not found
  std::cout << "Error: the object you asked for, \"" << objectName
            << "\", was not found." << std::endl;
  for (const auto& item : items) {
    printItem(item, false);
  }
  return cbe::Object{cbe::DefaultCtor{}};  // empty
}

cbe::Container Notice::selectContainer(const std::string& prompt) {
  std::cout << "Select Container" << std::endl;
  const auto items = this->qResult.getItemsSnapshot();
  std::string firstName = "";
  if (qResult.containersLoaded() > 0) {
    cbe::Item firstItem = items[0];
    firstName = firstItem.name();
  }
  // Do the following until user has entered a container that exists 
  std::string containerName = "aw";
  while (true) {
    if (!useDefault) {
      // Generic function to get string from user input
      containerName = inquireString(prompt, firstName);
    }

    // Loop through and look for container that user has requested
    for (const auto& item : items) {

      // If container is found, return it 
      if (item.name() == containerName) {
        return cbe::CloudBackend::castContainer(item);
      }
    }

    // If container is not fount an error is printed
    std::cout << "Error: the container you asked for, \"" << containerName
              << "\", was not found; following are the options:" << std::endl;
    // Loops through available containers to select 
    for (const auto& item : items) {
      printItem(item, false);
    }
  }
}

cbe::Object Notice::createMyNoticeObject(cbe::Container inContainer) {
  const std::string qualFile1Name = "/tmp/" + myObjectName;

  bool indexed;
  int numOfTags;
  std::string name;
  std::string value;

  // Create variable for key/values
  cbe::KeyValues keyValues;
  std::string keyName{};
  keyName = "by";

  loadContainerObjects(inContainer);
  object = pickObject(myObjectName);
  if (object) {
    keyValues = object.keyValues();
    for (const auto& keyValue : keyValues) {
      const auto& KVkey     = keyValue.first;
      const auto& sdkValue  = keyValue.second;
      const auto  KVstring  = std::get<std::string>(sdkValue);
      const auto  KVindexed = std::get<bool>(sdkValue);
      if (KVkey == "by") {
        recent_by = KVstring;
        break;
      }
    }
  }
  value = inquireString("enter alias '" + keyName + "'", recent_by);
  recent_by = value;
  indexed = true;
  
  // Insert value as key 0 with user alias
  keyValues[keyName] = std::pair<std::string, bool>(value, indexed);

  // Next key msg
  keyName = "msg"; // may not start with a digit, has to be alpha
  value = inquireString("enter  '" + keyName + "'",
                        recent_msg);
  recent_msg = value;
  indexed = false;
  // Insert value for key 1
  keyValues[keyName] = std::pair<std::string, bool>(value, indexed);

  std::ofstream ofs{qualFile1Name};
  ofs << "<notice>\n"
      << "  <by>"
      <<   recent_by
      << "</by>\n"
      << "  <msg>"
      << recent_msg
      << "</msg>\n"
      << "</notice>\n"
      << std::flush;
  ofs.close();
  std::cout << "Uploading "
            << qualFile1Name
            << std::endl;
  std::shared_ptr<MyUploadDelegate> uploadDelegate =
                                           std::make_shared<MyUploadDelegate>();
  inContainer.upload(qualFile1Name, uploadDelegate);
  object = uploadDelegate->waitForRsp();

  // Check if error 
  if (uploadDelegate->errorInfo){ 
    std::cout << "Error!" << std::endl;
    std::cout << "Could not create object." << std::endl;
    // Print info about error
    std::cout << uploadDelegate->errorInfo << std::endl;
    // Exit with unique code
    exitProgram(31);
  }

  // Delegate for UpdateKeyValues
  std::shared_ptr<MyUpdateKeyValuesDelegate> updateKeyValuesDelegate = 
                                    std::make_shared<MyUpdateKeyValuesDelegate>();  
  object.updateKeyValues(keyValues, updateKeyValuesDelegate);
  // Wait for delegate to finish
  updateKeyValuesDelegate->waitForRsp();

  // Check if error 
  if (updateKeyValuesDelegate->errorInfo){ 
    std::cout << "Error!" << std::endl;
    std::cout << "Could not update object key/values." << std::endl;
    // Print info about error
    std::cout << updateKeyValuesDelegate->errorInfo << std::endl;
    // Exit with unique code
    exitProgram(32);
  }

  now = std::time(nullptr);
  ts = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &ts);
  std::cout << "now: ";
  printf("%s\n", buf);

  // Return object from delegate
  return updateKeyValuesDelegate->object;    
}

// - - - - - - - - - - - - - - main start - - - -  - - - - - - - - - - - - - - - 

void Notice::sendMsg() {
  username = inquireString("Type username", username);
  password = inquireString("Type password", password);
  tenant   = inquireString("Enter  tenant", tenant);

  // Create log in delegate
  std::shared_ptr<MyLogInDelegate> logInDelegate = 
                                            std::make_shared<MyLogInDelegate>();
  // Call login and set member variable
  myCloudBackend = cbe::CloudBackend::logIn(username, 
                                            password, 
                                            tenant,
                                            client,
                                            logInDelegate);
  // Wait for delegate to finish
  logInDelegate->waitForRsp();
  
  // Check if error 
  if (logInDelegate->errorInfo){
    // Print info about error
    std::cout << "Error, login failed! \nError info=" 
              << logInDelegate->errorInfo << std::endl;
    // Exit program with error code
    exitProgram(10);
  }

  myAccount = myCloudBackend.account();
  myObjectName = std::to_string(myAccount.userId()) + ".xml";
  // Confirm login and SDK version
  std::cout << "Logged in as:\t"    << myAccount.username()
            << " (" << myObjectName << ")"
            << std::endl; 
  std::cout << "SDK version: "  << myCloudBackend.version()
            << std::endl; 
  std::cout << "name:   \t"  << myAccount.firstName()
            << " "        << myAccount.lastName()
            << std::endl; 
  std::cout << "tenant: \t"  << myAccount.source()
            << std::endl; 

  rootContainer   = myAccount.rootContainer();     

  cbe::DataBases myDatabases = myAccount.databases();
  if (tenant != "cbe_githubtesters") {
    for (auto myEntry : myDatabases) {
      if ( myEntry.first == "tenant" ) {
        tenantContainer = myEntry.second.rootContainer();
      }
    }
  } else {
    std::cout << "Error: user "
              << username
              << " is not allowed to run on tenant://"
              << '\n'
              << "replace with your account login in ../user_credentials.cpp"
              << std::endl;
    exitProgram(9);
  }

  std::cout << "home://   \t("
            << rootContainer.id()
            << ")"
            << std::endl; 

  std::cout << "tenant:// \t("
            << tenantContainer.id()
            << ")"
            << std::endl; 

  if (!tenantContainer) {
    std::cout << "Warning: tenant container not found, using home://"
              << std::endl;
    topContainer = rootContainer;
  } else {
    topContainer = tenantContainer;
  }

  loadContainerContents(topContainer);

  // First level container
  firstLevelContainer = pickContainer(firstLevelContainerName);
  if (!firstLevelContainer) {
              firstLevelContainer = createMyContainer(topContainer,
                                                      firstLevelContainerName);
  }
  // Print the container selected
  std::cout << "tenant://"
            << containerName(firstLevelContainer, false)
            << std::endl;
  
  loadContainerContents(firstLevelContainer);
  // After loading the container, print its contents
  std::cout << "Boards: "
            << std::endl;
  printContainerContents(this->qResult);
  std::cout << "Count: "
            << this->qResult.getItemsSnapshot().size() 
            << std::endl;
  subContainerName = inquireString("Select board", subContainerName);

  // Pick a container notice board
  mySubContainer = pickContainer(subContainerName);
  if (!mySubContainer) {
    std::cout << "Creating board: "
              << subContainerName 
              << std::endl;
    mySubContainer = createMyContainer(firstLevelContainer, subContainerName);
  }

  // Print the container the user selected
  std::cout << "tenant://"
            << firstLevelContainerName
            <<"/"
            << containerName(mySubContainer, false)
            << std::endl;
  loadContainerObjects(mySubContainer);
  printMessages(this->qResult);
  
  do {
    // Creates a Notice object in the selected container
    object = createMyNoticeObject(mySubContainer);

    // Prints information about the object that has been created
    std::cout << "Object name: " << objectName(object, false) << std::endl;

    // Finally after the object has been created, load its contents
    loadContainerObjects(mySubContainer);

    // Print all objects and its content  
    printMessages(this->qResult);
  } while (inquireBool("new notice", true));
  
  std::cout << "Notice completed!" << std::endl;
  myCloudBackend.terminate();
} // void Notice::sendMsg()
