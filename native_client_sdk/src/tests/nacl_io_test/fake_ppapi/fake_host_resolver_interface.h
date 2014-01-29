// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTS_NACL_IO_TEST_FAKE_HOST_RESOLVER_INTERFACE_H_
#define TESTS_NACL_IO_TEST_FAKE_HOST_RESOLVER_INTERFACE_H_

#include <ppapi/c/ppb_host_resolver.h>

#include "nacl_io/pepper_interface.h"
#include "sdk_util/macros.h"

// This fake resolver only know how to resolve this one
// host to this one IP address.
#define FAKE_HOSTNAME "example.com"
#define FAKE_IP 0x01020304

class FakePepperInterface;
class FakeVarManager;

class FakeHostResolverInterface : public nacl_io::HostResolverInterface {
 public:
  explicit FakeHostResolverInterface(FakePepperInterface* ppapi);

  virtual PP_Resource Create(PP_Instance);

  virtual int32_t Resolve(PP_Resource,
                          const char*,
                          uint16_t,
                          const PP_HostResolver_Hint*,
                          PP_CompletionCallback);

  virtual PP_Var GetCanonicalName(PP_Resource);
  virtual uint32_t GetNetAddressCount(PP_Resource);
  virtual PP_Resource GetNetAddress(PP_Resource, uint32_t);

 private:
  FakePepperInterface* ppapi_;

  DISALLOW_COPY_AND_ASSIGN(FakeHostResolverInterface);
};

#endif  // TESTS_NACL_IO_TEST_FAKE_HOST_RESOLVER_INTERFACE_H_