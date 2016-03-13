/* Socket
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TCPSocket.h"
#include "Timer.h"

TCPSocket::TCPSocket()
{
}

TCPSocket::TCPSocket(NetworkInterface *iface)
{
    open(iface);
}

int TCPSocket::open(NetworkInterface *iface)
{
    return Socket::open(iface, NSAPI_TCP);
}

int TCPSocket::connect(const SocketAddress &addr)
{
    if (!_socket) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    return _iface->socket_connect(_socket, addr);
}

int TCPSocket::connect(const char *host, uint16_t port)
{
    SocketAddress addr(_iface, host, port);
    if (!addr) {
        return NSAPI_ERROR_DNS_FAILURE;
    }

    return connect(addr);
}

bool TCPSocket::is_connected()
{
    return _socket && _iface->socket_is_connected(_socket);
}

int TCPSocket::send(const void *data, unsigned size)
{
    mbed::Timer timer;
    timer.start();

    while (true) {
        if (!_socket) {
            return NSAPI_ERROR_NO_SOCKET;
        }

        int sent = _iface->socket_send(_socket, data, size);
        if (sent != NSAPI_ERROR_WOULD_BLOCK || !_blocking || 
            (_timeout && timer.read_ms() > _timeout)) {
            return sent;
        }
    }
}

int TCPSocket::recv(void *data, unsigned size)
{
    mbed::Timer timer;
    timer.start();

    while (true) {
        if (!_socket) {
            return NSAPI_ERROR_NO_SOCKET;
        }
    
        int recv = _iface->socket_recv(_socket, data, size);
        if (recv != NSAPI_ERROR_WOULD_BLOCK || !_blocking || 
            (_timeout && timer.read_ms() > _timeout)) {
            return recv;
        }
    }
}
