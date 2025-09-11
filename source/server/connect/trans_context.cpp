#include <vector>
#include <string>
#include <thread>
#include <queue>

#include "server/connect/trans_context.hpp"
#include "log/manage_logger.hpp"
#include "mt_queue/mt_queue.hpp"

DaneJoe::MTQueue<uint8_t> temp(4096);
void TransContext::on_recv(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer)
{
    std::vector<uint8_t> data = buffer->try_pop(1024);
    std::string str(data.begin(), data.end());
    DANEJOE_LOG_INFO("default", "TransRecv", "TransContext recieved: {}", str);
    for (auto ch : str)
    {
        temp.push((uint8_t)ch);
    }

}

void TransContext::on_send(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer)
{
    auto data = temp.try_pop(1024);
    buffer->push(data.begin(), data.end());
}


std::shared_ptr<ISocketContext> TransContextCreator::create()
{
    return std::make_shared<TransContext>();
}
