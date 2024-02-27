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

        durationUS = std::chrono::duration_cast<std::chrono::microseconds>(currentBeat - lastBeat).count();

        if (continuousBeats == 0)
        {
            continuousBeats = 1;
            lastBeat        = currentBeat;
            return true;
        }

        if (durationUS > 5000000)  // 5s
        {
            continuousBeats = 1;
            lastBeat        = currentBeat;
            return true;
        }

        if (continuousBeats == 1) { averageBeatDurationUS = durationUS; }


        if (abs(durationUS - averageBeatDurationUS) < maxBeatOffsetToleranceUS)
        {
            averageBeatDurationUS = (averageBeatDurationUS * continuousBeats + durationUS) / (continuousBeats + 1);

            lastBeat = currentBeat;

            if (continuousBeats >= 3)  // to average out small mistakes
            {
                BPMIntTimesHun = static_cast<uint16_t>(6000000000.0 / static_cast<double>(averageBeatDurationUS));

                printf("avg. BPM: %6.2lf\n", static_cast<double>(BPMIntTimesHun) / 100.0);
                printf("avg. Beat duration: %6.4lf\n", static_cast<double>(averageBeatDurationUS) / 1000000.0);
                printf("curr. Beat duration: %6.4lf\n\n", static_cast<double>(durationUS) / 1000000.0);
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

                BPMString.clear();
                BPMString = floatToString(static_cast<float>(static_cast<double>(BPMIntTimesHun) / 100.0));
                BPM::setBPM(BPMString);
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
