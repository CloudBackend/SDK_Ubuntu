/*
  CloudBackend AB 2022 - 2024.
*/

#include <condition_variable>
#include <iostream>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <string>

#include "cbe/CloudBackend.h"
#include "cbe/Container.h"
#include "cbe/QueryResult.h"

#include "cbe/delegate/CreateContainerDelegate.h"
#include "cbe/delegate/ListGroupsDelegate.h"
#include "cbe/delegate/LogInDelegate.h"
#include "cbe/delegate/QueryDelegate.h"
#include "cbe/delegate/UploadDelegate.h"

// - - - - - - - - - - - - - - - - - DELEGATES - - - - - - - - - - - - - - - - - 

class MyLogInDelegate :  public cbe::delegate::LogInDelegate
{
  // Mutex lock and condition variable are used to verify that operation has 
  // completed.
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};

  // Used to see if callback is reached
  bool                    called = false;

  // User implemented callback, return value is cbe::CloudBackend&&  
  void onLogInSuccess(cbe::CloudBackend&& cloudBackend) final {
    {
      // Lock thread during operation 
      std::lock_guard<std::mutex> lock(mutex);
      this->cloudBackend = std::move(cloudBackend); 
      called = true;
    }
    // Notify when operation is done to unlock
    conditionVariable.notify_one();
  }

  // User implemented error callback
  void onLogInError(cbe::delegate::Error&&  error, 
                    cbe::util::Context&&    context) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      // Set member variable errorInfo with information about fail
      errorInfo = ErrorInfo{std::move(context), std::move(error)};
      called = true;
    }
    conditionVariable.notify_one();
  }
public:
  // Member variables for the delegate, will later be accessed through the 
  // delegate defined in the code
  cbe::CloudBackend cloudBackend{cbe::DefaultCtor{}}; 
  ErrorInfo errorInfo{};

  // Wait function is called on the delegate when it is being used 
  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    // std::cout << "Waiting, to be logged in" << std::endl;
    conditionVariable.wait(lock, [this] { return called; });
    // std::cout << "Now we have waited: " << called << std::endl;
    // Reset called flag, so current delegate instance can be reused
    called = false;
  }

}; // class MyLogInDelegate 

//------------------------------------------------------------------------------

class MyListenerDelegate :  public cbe::delegate::CloudBackendListenerDelegate
{
  // Mutex lock and condition variable are used to verify that operation has 
  // completed.
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};

  // Used to see if callback is reached
  bool                    called = false;

  // User implemented callback, return value is cbe::Object&&
  void onRemoteObjectAdded(cbe::Object&& object) final {
    {
      std::cout << "callback: onRemoteObjectAdded" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myObject = std::move(object); 
      called = true;
    }
    conditionVariable.notify_one();
  }
  void onRemoteObjectMoved(cbe::Object&& object) final {
    {
      std::cout << "callback: onRemoteObjectMoved" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myObject = std::move(object); 
      called = true;
    }
    conditionVariable.notify_one();
  }
  void onRemoteObjectRemoved(cbe::ItemId  objectId, 
                             std::string  name) final {
    {
      std::cout << "callback: onRemoteObjectRemoved" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myObjectId = std::move(objectId);
      this->myName     = std::move(name);
      called = true;
    }
    conditionVariable.notify_one();
  }
  void onRemoteObjectRenamed(cbe::Object&& object) final {
    {
      std::cout << "callback: onRemoteObjectRenamed" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myObject = std::move(object); 
      called = true;
    }
    conditionVariable.notify_one();
  }
  void onRemoteContainerAdded(cbe::Container&& container) final {
    {
      std::cout << "callback: onRemoteContainerAdded" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myContainer = std::move(container); 
      called = true;
    }
    conditionVariable.notify_one();
  }
  void onRemoteContainerMoved(cbe::Container&& container) final {
    {
      std::cout << "callback: onRemoteContainerMoved" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myContainer = std::move(container); 
      called = true;
    }
    conditionVariable.notify_one();
  }
  void onRemoteContainerRemoved(cbe::ItemId containerId, 
                                std::string name) final {
    {
      std::cout << "callback: onRemoteContainerRemoved" << std::endl;
      std::lock_guard<std::mutex> lock(mutex);
      this->myContainerId = std::move(containerId);
      this->myName        = std::move(name);
      called = true;
    }
    conditionVariable.notify_one();
  }

public:
  // Object variable for the delegate, will later be accessed through the 
  // delegate defined in the code
  cbe::Container myContainer{cbe::DefaultCtor{}};
  cbe::Object    myObject{cbe::DefaultCtor{}};
  cbe::ItemId    myContainerId{cbe::DefaultCtor{}};
  cbe::ItemId    myObjectId{cbe::DefaultCtor{}};
  std::string    myName{cbe::DefaultCtor{}};
  // Wait function is called on the delegate when it is being used 
  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    std::cout << "Waiting, Listening." << std::endl;
    conditionVariable.wait(lock, [this] { return called; });
    std::cout << "Now we have waited: " << called << std::endl;
    // Reset called flag, so current delegate instance can be reused
    called = false;
  }

}; // class MyListenerDelegate 

//------------------------------------------------------------------------------

class MyListGroupsDelegate :  public cbe::delegate::ListGroupsDelegate 
{
  // Mutex lock and condition variable are used to verify that operation has 
  // completed.
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};

  // Used to see if callback is reached
  bool                    called = false;

  // User implemented callback, return value is cbe::CloudBackend&&  
  void onListGroupsSuccess(Groups&& theGroups) final {
    {
      // Lock thread during operation 
      std::lock_guard<std::mutex> lock(mutex);
      this->myGroups = std::move(theGroups); 
      called = true;
    }
    // Notify when operation is done to unlock
    conditionVariable.notify_one();
  }

  // User implemented error callback
  void onListGroupsError(cbe::delegate::Error&&  error, 
                         cbe::util::Context&&    context) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      // Set member variable errorInfo with information about fail
      errorInfo = ErrorInfo{std::move(context), std::move(error)};
      called = true;
    }
    conditionVariable.notify_one();
  }
public:
  // Member variables for the delegate, will later be accessed through the 
  // delegate defined in the code
  Groups    myGroups{cbe::DefaultCtor{}};
  ErrorInfo errorInfo{};

  // Wait function is called on the delegate when it is being used 
  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    // std::cout << "Waiting, to be logged in" << std::endl;
    conditionVariable.wait(lock, [this] { return called; });
    // std::cout << "Now we have waited: " << called << std::endl;
    // Reset called flag, so current delegate instance can be reused
    called = false;
  }

}; // class MyListGroupsDelegate 

//------------------------------------------------------------------------------

class MyQueryDelegate :  public cbe::delegate::QueryDelegate
{
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};
  bool                    called = false;

  /**
   * Called upon successful query.
   * @param queryResult Instance of a QueryResult containing the result set.
   */
  void onQuerySuccess(cbe::QueryResult&& queryResult) final {
    {      
      std::lock_guard<std::mutex> lock(mutex);
      this->queryResult = std::move(queryResult); 
      called = true;
    }           
    conditionVariable.notify_one();
  }

  /**
   * Called upon a failed query() or join() call.
   * @param error   Error information passed from %CloudBackend SDK.
   * @param context Additional context information about the original service
   *                call that has failed.
   */
  void onQueryError(cbe::delegate::QueryError&&         error,
                    cbe::util::Context&&                context) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      errorInfo = ErrorInfo{std::move(context), std::move(error)};
      called = true;
    }
    conditionVariable.notify_one();
  }

public:
  /*implementation of delegates */
  cbe::QueryResult  queryResult{cbe::DefaultCtor{}};
  ErrorInfo         errorInfo{};

  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    // std::cout << "Waiting, for query" << std::endl;
    conditionVariable.wait(lock, [this] { return called; });
    // std::cout << "Now we have waited: " << called << std::endl;
    // Reset called flag, so current delegate instance can be reused
    called = false;
  }
}; // class MyQueryDelegate

//------------------------------------------------------------------------------

class MyCreateContainerDelegate :  public cbe::delegate::CreateContainerDelegate
{
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};

  bool                    called = false;

  void onCreateContainerSuccess(cbe::Container&& container) final {
    {      
      std::lock_guard<std::mutex> lock(mutex);
      this->container = std::move(container); 
      called = true;
    }           
    conditionVariable.notify_one();      
  }
  void onCreateContainerError(Error&& error, cbe::util::Context&& context) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      errorInfo = ErrorInfo{std::move(context), std::move(error)};
      called = true;
    }
    conditionVariable.notify_one();
  }

public:
  /*implementation of delegates */
  cbe::Container container{cbe::DefaultCtor{}};
  ErrorInfo errorInfo{};

  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    // std::cout << "Waiting, for create container" << std::endl;
    conditionVariable.wait(lock, [this] { return called; });
    // std::cout << "Now we have waited: " << called << std::endl;
    // Reset called flag, so current delegate instance can be reused
    called = false;
  }
}; // class MyCreateContainerDelegate

//------------------------------------------------------------------------------

class MyCreateObjectDelegate :  public cbe::delegate::CreateObjectDelegate
{
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};

  bool                    called = false;

  void onCreateObjectSuccess(cbe::Object&& object) final {
    {      
      std::lock_guard<std::mutex> lock(mutex);
      this->object = std::move(object); 
      called = true;
    }           
    conditionVariable.notify_one();      
  }
  void onCreateObjectError(Error&& error, cbe::util::Context&& context) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      errorInfo = ErrorInfo{std::move(context), std::move(error)};
      called = true;
    }
    conditionVariable.notify_one();
  }

public:
  /*implementation of delegates */
  cbe::Object object{cbe::DefaultCtor{}};
  ErrorInfo errorInfo{};

  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    // std::cout << "Waiting, for create object" << std::endl;
    conditionVariable.wait(lock, [this] { return called; });
    // std::cout << "Now we have waited: " << called << std::endl;
    // Reset called flag, so current delegate instance can be reused
    called = false;
  }
}; // class MyCreateObjectDelegate

//------------------------------------------------------------------------------

  struct MyUploadDelegate : cbe::delegate::UploadDelegate {
    std::mutex              mutex{};
    std::condition_variable conditionVariable{};
    bool                    called{};
    cbe::Object object{ cbe::DefaultCtor{} };

    void onUploadSuccess(cbe::Object&& object) final {
      {
        std::lock_guard<std::mutex> lock{mutex};
        this->object = std::move(object);
        called = true;
      }
      conditionVariable.notify_one();
    }

    void onUploadError(cbe::delegate::TransferError&& error,
                       cbe::util::Context&&           context) final {
      {
        std::lock_guard<std::mutex> lock{mutex};
        object = cbe::Object{ cbe::DefaultCtor{} };
        errorInfo = ErrorInfo{std::move(context), std::move(error)};
        called = true;
      }
      conditionVariable.notify_one();
    }

    ErrorInfo errorInfo{};

    cbe::Object waitForRsp() {
      std::unique_lock<std::mutex> lock{mutex};
      conditionVariable.wait(lock, [this]{ return called; } );
      called = false;
      return std::move(object);
    }
  }; // struct MyUploadDelegate

//------------------------------------------------------------------------------

  struct MyUpdateKeyValuesDelegate : cbe::delegate::UpdateKeyValuesDelegate {
    std::mutex              mutex{};
    std::condition_variable conditionVariable{};
    bool                    called{};
    cbe::Object object{ cbe::DefaultCtor{} };

    void onUpdateKeyValuesSuccess(cbe::Object&& object) final {
      {
        std::lock_guard<std::mutex> lock{mutex};
        this->object = std::move(object);
        called = true;
      }
      conditionVariable.notify_one();
    }

    void onUpdateKeyValuesError(cbe::delegate::Error&& error,
                                cbe::util::Context&&           context) final {
      {
        std::lock_guard<std::mutex> lock{mutex};
        object = cbe::Object{ cbe::DefaultCtor{} };
        errorInfo = ErrorInfo{std::move(context), std::move(error)};
        called = true;
      }
      conditionVariable.notify_one();
    }

    ErrorInfo errorInfo{};

    cbe::Object waitForRsp() {
      std::unique_lock<std::mutex> lock{mutex};
      conditionVariable.wait(lock, [this]{ return called; } );
      called = false;
      return std::move(object);
    }
  }; // struct MyUpdateKeyValuesDelegate

//------------------------------------------------------------------------------
