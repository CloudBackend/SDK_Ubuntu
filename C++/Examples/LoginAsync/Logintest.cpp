#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>

#include "cbe/Account.h"
#include "cbe/CloudBackend.h"
#include "cbe/Types.h"

#include "../user_credentials.cpp"

class LogInDelegate :  public cbe::delegate::LogInDelegate {
  std::mutex              mutex{};
  std::condition_variable conditionVariable{};
  bool                    called = false;

  void onLogInSuccess(cbe::CloudBackend&& cloudBackend) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      this->cloudBackend = std::move(cloudBackend); 
      called = true;
    }
    conditionVariable.notify_one();
  }

  void onLogInError(cbe::delegate::Error&& error, cbe::util::Context&& context) final {
    {
      std::lock_guard<std::mutex> lock(mutex);
      errorInfo = ErrorInfo{std::move(context), std::move(error)};
      called = true;
    }
    conditionVariable.notify_one();
  }

public:
  cbe::CloudBackend cloudBackend{cbe::DefaultCtor{}};
  ErrorInfo errorInfo{};

  void waitForRsp() {
    std::unique_lock<std::mutex> lock(mutex);
    conditionVariable.wait(lock, [this] { return called; });
    called = false;
  }
};  // class LogInDelegate

int main (int argc, char *argv[]) {
  cbe::CloudBackend cloudBackend{cbe::DefaultCtor{}};
  std::shared_ptr<LogInDelegate> logInDelegate = std::make_shared<LogInDelegate>();
  
  cloudBackend = cbe::CloudBackend::logIn(username, password, tenant, client, logInDelegate);
  logInDelegate->waitForRsp();
  if (!logInDelegate->errorInfo) {
    // Yes, no error, success!  
    std::cout << "Logged in:" << std::endl;
    std::cout << "SDK version = "
              << cloudBackend.version() << std::endl;
    std::cout << "   username = "
              << cloudBackend.account().username() << std::endl;
    std::cout << "     userId = "
              << cloudBackend.account().userId() << std::endl;
    std::cout << "     tenant = "
              << cloudBackend.account().source() << std::endl;
    std::cout << "      first = "
              << cloudBackend.account().firstName() << std::endl;
    std::cout << "       last = "
              << cloudBackend.account().lastName() << std::endl;
  } else {
    std::cout << "Warning: Login failed!" << std::endl;
    std::cout << "errorInfo = " << logInDelegate->errorInfo << std::endl;
    std::cout << "Exiting." << std::endl;
    cloudBackend.terminate();
    return 1; // Bail out - Due to failed login
  }

  std::cout << "End of program!" << std::endl;
  cloudBackend.terminate();
  return 0;
}  // main

