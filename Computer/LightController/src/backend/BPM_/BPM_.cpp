#include "BPM_.hpp"

BPM_::~BPM_()
{
    threadShouldJoin = true;
    joinBPMThread();
}

void BPM_::createBPMThread() { pthread_create(&bpmThread, nullptr, &BPMUpdateHandler, nullptr); }

void BPM_::joinBPMThread()
{
    while (threadShouldJoin)
    {
        if (pthread_join(bpmThread, nullptr) == 0) { threadShouldJoin = false; }
    }
}

void *BPM_::BPMUpdateHandler(void *args)
{
    if (args == nullptr)
    {
        while (!threadShouldJoin)
        {
            newBeatHandler();
            usleep(2000);
        }
    }
    return nullptr;
}

bool BPM_::newBeatHandler()
{
    if (newBeat)
    {
        printf("New Beat recognized!\n");
        pthread_mutex_lock(&mutex);
        newBeat = false;
        pthread_mutex_unlock(&mutex);

        duration = currentBeat - lastBeat;

        if (continuousBeats == 0)
        {
            continuousBeats = 1;
            lastBeat        = currentBeat;
            return true;
        }

        if (static_cast<double>(duration) / CLOCKS_PER_SEC_OWN > 5.0F)
        {
            continuousBeats = 1;
            lastBeat        = currentBeat;
            return true;
        }

        if (continuousBeats == 1) { averageBeatDuration = static_cast<float>(static_cast<double>(duration) / CLOCKS_PER_SEC_OWN); }


        if (abs((static_cast<double>(duration) / CLOCKS_PER_SEC_OWN) - averageBeatDuration) < maxBeatOffsetTolerance)
        {
            averageBeatDuration = (averageBeatDuration * static_cast<float>(continuousBeats) +
                                   static_cast<float>(static_cast<double>(duration) / CLOCKS_PER_SEC_OWN)) /
                                  (static_cast<float>(continuousBeats) + 1);

            lastBeat = currentBeat;

            if (continuousBeats >= 3)  // to average out small mistakes
            {
                BPMIntTimesTen = static_cast<uint16_t>((60.0F / averageBeatDuration) * 10.0F);

                printf("avg. BPM (x10): %5.1f\n", static_cast<float>(BPMIntTimesTen) / 10.0F);
                printf("avg. Beat duration: %6.4f\n", averageBeatDuration);
                printf("curr. Beat duration: %6.4f\n\n", static_cast<float>(static_cast<double>(duration) / CLOCKS_PER_SEC_OWN));
                /*
                                BPM.clear();
                                if (BPMIntTimesTen >= 10000) { fprintf(stderr, "BPM are to fast!\n"); }
                                else if (BPMIntTimesTen >= 1000)
                                {
                                    conversionTemp = (BPMIntTimesTen % 10) + 48;
                                    BPM.push_back(static_cast<char>(conversionTemp));
                                    BPMIntTimesTen -= conversionTemp;
                                    BPMIntTimesTen = static_cast<uint16_t>(BPMIntTimesTen / 10);

                                    BPM.push_back('.');

                                    conversionTemp = (BPMIntTimesTen % 10) + 48;
                                    BPM.push_back(static_cast<char>(conversionTemp));
                                    BPMIntTimesTen -= conversionTemp;
                                    BPMIntTimesTen = static_cast<uint16_t>(BPMIntTimesTen / 10);

                                    conversionTemp = (BPMIntTimesTen % 10) + 48;
                                    BPM.push_back(static_cast<char>(conversionTemp));
                                    BPMIntTimesTen -= conversionTemp;
                                    BPMIntTimesTen = static_cast<uint16_t>(BPMIntTimesTen / 10);

                                    BPM.push_back(static_cast<char>(BPMIntTimesTen));
                                }
                                else if (BPMIntTimesTen >= 100)
                                {
                                    conversionTemp = (BPMIntTimesTen % 10) + 48;
                                    BPM.push_back(static_cast<char>(conversionTemp));
                                    BPMIntTimesTen -= conversionTemp;
                                    BPMIntTimesTen = static_cast<uint16_t>(BPMIntTimesTen / 10);

                                    BPM.push_back('.');

                                    conversionTemp = (BPMIntTimesTen % 10) + 48;
                                    BPM.push_back(static_cast<char>(conversionTemp));
                                    BPMIntTimesTen -= conversionTemp;
                                    BPMIntTimesTen = static_cast<uint16_t>(BPMIntTimesTen / 10);

                                    BPM.push_back(static_cast<char>(BPMIntTimesTen));
                                }
                                else if (BPMIntTimesTen >= 10)
                                {
                                    BPM.push_back(static_cast<char>(BPMIntTimesTen));

                                    BPM.push_back('.');
                                    BPM.push_back('0');
                                }

                                std::reverse(BPM.begin(), BPM.end());
                                BPM::setBPM(BPM);*/
            }
            continuousBeats++;
        }
        else
        {
            printf("Stepped out of interval\n");
            continuousBeats = 0;
            lastBeat        = currentBeat;
        }

        return true;
    }

    return false;
}
