//
//  csocrypto.cpp
//  csobox
//
//  Created by Daniel Grigg on 18/07/13.
//  Copyright (c) 2013 Daniel Grigg. All rights reserved.
//
#include <cassert>
#include <string>
#include <iostream>
#include "SecureObjectsFactory.h"
#include <vector>
#include "csocrypto.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

using namespace SecureObjects;
using namespace std;

typedef std::auto_ptr<ILogin> LoginPtr;
typedef std::auto_ptr<ISecureStore> StorePtr;
typedef std::auto_ptr<ISecretMessage> SecretMessagePtr;
typedef std::auto_ptr<ISecureEnvelopeReader> EnvelopeReaderPtr;

const string SERVER = "https://utp20out1.sec.covata.com";
const string EMAIL = "daniel.grigg@qa.covata.com";
const string PASSWORD = "Password01";
const string AUTH_METHOD = "ldapuserpwd";
const string APP_NAME = "testapp";
const string APP_VERSION = "1.0";

void info(const string& message) {
  cout << message << "\n";
}

void warning(const string& warning) {
  cerr << "[WARNING] " << warning << "\n";
}

namespace cs {
  LoginPtr login() {
    try {
      
      SecureObjectsFactory::Initialise(APP_NAME, APP_VERSION);
      SecureObjectsFactory::SetConnectionAddress(SERVER);
      return SecureObjectsFactory::GetInstance_Login();
    } catch (...) {
      warning("Error logging in");
      return LoginPtr();
    }
  }
  
  AuthenticationStatus authenticate(auto_ptr<ILogin>& login) {
    try {
      auto response = login->SubmitBasicAuthenticationByEmail(EMAIL, PASSWORD, AUTH_METHOD);
      auto status = response->GetAuthenticationStatus();
      //cout << "Auth OK? " << boolalpha << (status == AuthStatus_Ok) << "\n";
      return status;
    } catch (...) {
      warning("Error authenticating");
      throw;
    }
    return AuthStatus_Denied;
  }
  
  StorePtr store_session(LoginPtr& login_session) {
    info("::store_session...");
    auto store = SecureObjectsFactory::GetInstance_SecureStore();
    login_session->TransferAuthentication(store.get(), Transfer_KeepCurrentSessionOpen);
    store->CheckAuthorisation();
    
    if (!store->IsAuthenticated()) {
      warning("Store not authenticated");
    }
    return store;
  }
  
  EnvelopeReaderPtr envelope_reader(LoginPtr& login_session,
                                    const string& in_file) {
    try {
      info("::envelope_reader...");
      auto reader = SecureObjectsFactory::GetInstance_SecureEnvelopeReader(in_file);
      login_session->TransferAuthentication(reader.get(), Transfer_KeepCurrentSessionOpen);
      reader->CheckAuthorisation();
      
      if (!reader->IsAuthenticated()) {
        warning("Reader not authenticated");
      }
      
      return reader;
      
    } catch (...)    {
      warning("authenticate error");
    }
    return EnvelopeReaderPtr();
  }
  
  void decrypt_file(EnvelopeReaderPtr& reader, const string& out_dir) {
    info("::decrypt_file...");
    reader->ExtractToDirectory(out_dir);
  }
}

void socket_test() {
  printf( "Flushing errno: %s\n", strerror( errno ) );
  sockaddr_in serv_addr;
  int retval = 0;
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("192.168.2.54");
  serv_addr.sin_port = htons(5900);
  
  printf("Connecting\n");
  
  retval = connect(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
  printf( "Connect result: %s\n", strerror( errno ) );
  if (retval < 0)    return;
  
  vector<unsigned char> data(1024);
  
  int bytes_read = read(sockfd, &data[0], 1024);
  if (bytes_read > 0) {
    info("Read data");
    
    //    for (int i = 0; i < 1024; ++i) {
    //      cout << std::hex << "0x" << data[i] << " ";
    //    }
    
  }
  
}

int render_envelope(const char* filename, const char* out_dir)
{
  // std::vector<int> bar;
  // bar.push_back(42);
  // return bar.back();
  //  socket_test();
  
  info(string("::render_envelope") + ": " + filename + "\n");
  auto login_session = cs::login();
  cs::authenticate(login_session);
  auto store_session = cs::store_session(login_session);  
  auto reader = cs::envelope_reader(login_session, filename);
  
  cs::decrypt_file(reader, out_dir);
  
  return 42;
}


