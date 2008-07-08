/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef _Connection_
#define _Connection_

#include <memory>
#include <sstream>
#include <vector>

#include <boost/ptr_container/ptr_map.hpp>

#include "qpid/framing/AMQFrame.h"
#include "qpid/framing/AMQP_ServerOperations.h"
#include "qpid/framing/AMQP_ClientProxy.h"
#include "qpid/sys/AggregateOutput.h"
#include "qpid/sys/ConnectionOutputHandler.h"
#include "qpid/sys/ConnectionInputHandler.h"
#include "qpid/sys/TimeoutHandler.h"
#include "qpid/framing/ProtocolVersion.h"
#include "Broker.h"
#include "qpid/sys/Socket.h"
#include "qpid/Exception.h"
#include "ConnectionHandler.h"
#include "ConnectionState.h"
#include "SessionHandler.h"
#include "qpid/management/Manageable.h"
#include "qpid/management/Connection.h"

#include <boost/ptr_container/ptr_map.hpp>

namespace qpid {
namespace broker {

class LinkRegistry;

class Connection : public sys::ConnectionInputHandler, 
                   public ConnectionState
{
  public:
    typedef boost::shared_ptr<Connection> shared_ptr;

    Connection(sys::ConnectionOutputHandler* out, Broker& broker, const std::string& mgmtId, bool isLink = false);
    ~Connection ();

    /** Get the SessionHandler for channel. Create if it does not already exist */
    SessionHandler& getChannel(framing::ChannelId channel);

    /** Close the connection */
    void close(framing::ReplyCode code = 403,
               const string& text = string(),
               framing::ClassId classId = 0,
               framing::MethodId methodId = 0);

    // ConnectionInputHandler methods
    void received(framing::AMQFrame& frame);
    void idleOut();
    void idleIn();
    void closed();
    bool doOutput();

    void closeChannel(framing::ChannelId channel);

    // Manageable entry points
    management::ManagementObject* GetManagementObject (void) const;
    management::Manageable::status_t
        ManagementMethod (uint32_t methodId, management::Args& args);

    void requestIOProcessing (boost::function0<void>);
    void recordFromServer (framing::AMQFrame& frame);
    void recordFromClient (framing::AMQFrame& frame);
    std::string getAuthMechanism();
    std::string getAuthCredentials();
    void notifyConnectionForced(const std::string& text);
    void setUserId(const string& uid);

    framing::FrameHandler::Chain& getInChain() { return inChain; } 

  private:
    typedef boost::ptr_map<framing::ChannelId, SessionHandler> ChannelMap;
    typedef std::vector<Queue::shared_ptr>::iterator queue_iterator;

    // End of the received handler chain.
    void receivedLast(framing::AMQFrame& frame);

    ChannelMap channels;
    framing::AMQP_ClientProxy::Connection* client;
    ConnectionHandler adapter;
    bool isLink;
    bool mgmtClosing;
    const std::string mgmtId;
    boost::function0<void> ioCallback;
    management::Connection* mgmtObject;
    LinkRegistry& links;
    framing::FrameHandler::MemFunRef<Connection, &Connection::receivedLast> lastInHandler;
    framing::FrameHandler::Chain inChain;

};

}}

#endif
