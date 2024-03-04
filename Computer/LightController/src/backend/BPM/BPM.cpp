#include "BPM.hpp"

BPM::~BPM()
{
    threadsShouldJoin = true;
    joinBPMThreads();
}

void BPM::createBPMThreads(/*GLFWwindow *_window*/)
{
    //if (_window != nullptr)
    //{
        pthread_create(&bpmClickThread, nullptr, &BPMClickUpdateHandler, nullptr);
        // pthread_create(&bpmKeyThread, nullptr, &BPMKeyUpdateHandler, static_cast<void *>(_window));
        pthread_create(&buttonStateThread, nullptr, &BPMButtonStateUpdater, nullptr);
    //}
}

void BPM::joinBPMThreads()
{
    while (pthread_join(bpmClickThread, nullptr) != 0) {}
    // while (pthread_join(bpmKeyThread, nullptr) != 0) {}
    while (pthread_join(buttonStateThread, nullptr) != 0) {}
}
/*
void *BPM_::BPMKeyUpdateHandler(void *args)
{
    auto *__window = static_cast<GLFWwindow *>(args);
    while (!threadsShouldJoin)
    {
        if (glfwGetKey(__window, BEAT_BUTTON) == GLFW_PRESS) { setCurrentBeatToNow(); }
    }
    return nullptr;
}*/
void *BPM::BPMClickUpdateHandler(void *args)
{
    if (args == nullptr)
    {
        while (!threadsShouldJoin)
        {
            newBeatHandler();
            usleep(2000);
        }
    }
    return nullptr;
}

void *BPM::BPMButtonStateUpdater(void *args)
{
    if (args == nullptr)
    {
        while (!threadsShouldJoin)
        {
            buttonCurrentBeat    = std::chrono::high_resolution_clock::now();
            buttonBeatDurationUS = std::chrono::duration_cast<std::chrono::microseconds>(buttonCurrentBeat - lastBeat).count();
            // printf("buttonBeatDurationUS: %lld\n", buttonBeatDurationUS);
            // printf("averageBeatDurationUS: %lld\n", averageBeatDurationUS);
            // printf("buttonBlinkDurationUS: %lld\n", buttonBlinkDurationUS);
            if (averageBeatDurationUS < 1000)
            {
                if (buttonState)
                {
                    // printf("State OFF!\n");
                    BPM::setButtonShouldBlink(false);
                    buttonState = false;
                }
            }
            else
            {
                if (abs(buttonBeatDurationUS % averageBeatDurationUS) < buttonBlinkDurationUS)
                {
                    if (!buttonState)
                    {
                        // printf("State ON!\n");
                        BPM::setButtonShouldBlink(true);
                        buttonState = true;
                    }
                }
                else
                {
                    if (buttonState)
                    {
                        // printf("State OFF!\n");
                        BPM::setButtonShouldBlink(false);
                        buttonState = false;
                    }
                }
            }
            usleep(2000);
        }
    }
    return nullptr;
}

bool BPM::newBeatHandler()
{
    if (newBeat)
    {
        // printf("New Beat recognized!\n");
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

            if (continuousBeats >= 1)
            {
                BPMIntTimesHun = static_cast<uint16_t>(6000000000.0 / static_cast<double>(averageBeatDurationUS));

                // printf("avg. BPM: %6.2lf\n", static_cast<double>(BPMIntTimesHun) / 100.0);
                // printf("avg. Beat duration: %6.4lf\n", static_cast<double>(averageBeatDurationUS) / 1000000.0);
                // printf("curr. Beat duration: %6.4lf\n\n", static_cast<double>(durationUS) / 1000000.0);

                BPMString.clear();
                BPMString = floatToString(static_cast<float>(static_cast<double>(BPMIntTimesHun) / 100.0));
                BPM::setBPM(BPMString);
            }
            continuousBeats++;
        }
        else
        {
            // printf("Stepped out of interval\n");
            continuousBeats = 0;
            lastBeat        = currentBeat;
        }

        return true;
    }

    return false;
}
