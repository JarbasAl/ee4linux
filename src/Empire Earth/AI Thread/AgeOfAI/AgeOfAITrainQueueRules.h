// The arithmetic of Grille's econ.c queue_room and train_call (39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07): the game
// hands a request round-robin to every built producer of the type and queues all of it, however deep the queues
// already are. A request is cut to what keeps each producer's one-in-the-making plus queue at train_queue units; a
// producer with no room refuses the request, which the game then brings back on a later tick.
#ifndef AGEOFAI_TRAIN_QUEUE_RULES_H
#define AGEOFAI_TRAIN_QUEUE_RULES_H

namespace AgeOfAI {
namespace TrainQueueRules {

// One producer's free slots under the depth. inBusy is the unit in the making (0 or 1) plus the queued units.
inline long ProducerRoom(long inDepth, long inBusy)
{
    return inBusy < inDepth ? inDepth - inBusy : 0;
}

enum Decision { kKeep, kCut, kRefuse };

// kKeep when no producer is known (the game decides) or the producers have room for the whole request. kCut lowers
// outCount to the room. kRefuse drops the request when no producer has room.
inline Decision Decide(long inCount, long inRoom, bool inHaveProducers, long& outCount)
{
    if (!inHaveProducers || inRoom >= inCount) {
        outCount = inCount;
        return kKeep;
    }
    if (inRoom <= 0) {
        outCount = 0;
        return kRefuse;
    }
    outCount = inRoom;
    return kCut;
}

}  // namespace TrainQueueRules
}  // namespace AgeOfAI

#endif
