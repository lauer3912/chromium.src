// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mojo/shell/service_connector.h"

#include "base/logging.h"
#include "mojo/public/bindings/remote_ptr.h"
#include "mojom/shell.h"

namespace mojo {
namespace shell {

class ServiceConnector::ServiceFactory : public Shell {
 public:
  ServiceFactory(ServiceConnector* connector, const GURL& url)
      : connector_(connector),
        url_(url) {
    MessagePipe pipe;
    shell_client_.reset(pipe.handle0.Pass(), this);
    connector_->GetLoaderForURL(url)->Load(url, pipe.handle1.Pass());
  }
  virtual ~ServiceFactory() {}

  void ConnectToClient(ScopedMessagePipeHandle handle) {
    if (handle.is_valid())
      shell_client_->AcceptConnection(handle.Pass());
  }

  virtual void Connect(const String& url,
                       ScopedMessagePipeHandle client_pipe) MOJO_OVERRIDE {
    connector_->Connect(GURL(url.To<std::string>()), client_pipe.Pass());
  }

 private:
  ServiceConnector* connector_;
  GURL url_;
  RemotePtr<ShellClient> shell_client_;
  DISALLOW_COPY_AND_ASSIGN(ServiceFactory);
};

ServiceConnector::Loader::Loader() {}
ServiceConnector::Loader::~Loader() {}

ServiceConnector::ServiceConnector() : default_loader_(NULL) {
}

ServiceConnector::~ServiceConnector() {
  for (ServiceFactoryMap::iterator it = url_to_service_factory_.begin();
       it != url_to_service_factory_.end(); ++it) {
    delete it->second;
  }
  url_to_service_factory_.clear();
}

void ServiceConnector::SetLoaderForURL(Loader* loader, const GURL& gurl) {
  DCHECK(url_to_loader_.find(gurl) == url_to_loader_.end());
  url_to_loader_[gurl] = loader;
}

ServiceConnector::Loader* ServiceConnector::GetLoaderForURL(const GURL& gurl) {
  LoaderMap::const_iterator it = url_to_loader_.find(gurl);
  if (it != url_to_loader_.end())
    return it->second;
  DCHECK(default_loader_);
  return default_loader_;
}

void ServiceConnector::Connect(const GURL& url,
                               ScopedMessagePipeHandle client_handle) {
  ServiceFactoryMap::const_iterator service_it =
      url_to_service_factory_.find(url);
  ServiceFactory* service_factory;
  if (service_it != url_to_service_factory_.end()) {
    service_factory = service_it->second;
  } else {
    service_factory = new ServiceFactory(this, url);
    url_to_service_factory_[url] = service_factory;
  }
  service_factory->ConnectToClient(client_handle.Pass());
}

}  // namespace shell
}  // namespace mojo