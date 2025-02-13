#ifndef INCLUDE_cbe_SOLUTION_NOTICE_BODY_H_
#define INCLUDE_cbe_SOLUTION_NOTICE_BODY_H_

#include <iostream>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "cbe/Account.h"
#include "cbe/CloudBackend.h"
#include "cbe/Group.h"
#include "cbe/GroupManager.h"
#include "cbe/QueryChain.h"
#include "cbe/QueryResult.h"
#include "cbe/Types.h"

class Notice {
public:
  // Logic is the main program 
  void sendMsg();

  // Is used when code should terminate with a unique code
  void exitProgram(int errorCode);

  // General member variables to use in the code
  // The central object owning the login session.
  cbe::CloudBackend myCloudBackend{cbe::DefaultCtor{}};
  cbe::Account      myAccount{cbe::DefaultCtor{}};
  // GroupManager to access the tenant group
  cbe::GroupManager myGroupManager{cbe::DefaultCtor{}};
  cbe::GroupId      myTenantGroup{cbe::DefaultCtor{}};
  cbe::Group        myGroup{cbe::DefaultCtor{}};
  // Top root container - home://
  cbe::Container    rootContainer{cbe::DefaultCtor{}};
  // Top tenant container - tenant://
  cbe::Container    tenantContainer{cbe::DefaultCtor{}};
  cbe::Container    topContainer{cbe::DefaultCtor{}};
  cbe::Container    firstLevelContainer{cbe::DefaultCtor{}};
  cbe::Container    mySubContainer{cbe::DefaultCtor{}};
  bool useDefault = true;
  const std::string firstLevelContainerName = "Notice";
  const std::string subContainerName        = "staff";
  std::string myObjectName{};
  std::string indexMark{};
  // timestamp conversion
  cbe::Date timestamp{};
  std::time_t now{};
  struct tm   ts{};
  char        buf[25];
  char        bufDay[10];
  char        bufTime[8];
  const std::string CYAN  = "\033[36m";
  const std::string GREY  = "\033[90m";
  const std::string RESET = "\033[0m";

  std::string recent_by  = "John";
  std::string recent_msg = "On route to Oslo.";

private:
  cbe::QueryResult  qResult{cbe::DefaultCtor{}};

  void loadContainerContents(cbe::Container container);
  void loadContainerContents(cbe::ContainerId containerId);
  void printContainerContents(cbe::QueryResult q);
  cbe::Container createMyContainer(cbe::Container parentContainer, const std::string& name);
  cbe::Container createMyContainer(cbe::Container parentContainer);
  void loadContainerObjects(cbe::Container container);
  void printMessages(cbe::QueryResult q);
  void printObjects(cbe::QueryResult q);
  cbe::Container pickContainer(const std::string& containerName);
  cbe::Object pickObject(const std::string& objectName);
  cbe::Container selectContainer(const std::string& prompt);
  cbe::Object createMyNoticeObject(cbe::Container inContainer);

  cbe::Container myContainer{cbe::DefaultCtor{}};
  cbe::Object myObject{cbe::DefaultCtor{}}; 

};  //  class Notice

#endif  // INCLUDE_cbe_SOLUTION_NOTICE_BODY_H_
