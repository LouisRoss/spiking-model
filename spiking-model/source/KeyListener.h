#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

namespace embeddedpenguins::neuron::infrastructure
{
    class KeyListener
    {
        termios oldSettings_;

    public:
        KeyListener()
        {
            tcgetattr( fileno( stdin ), &oldSettings_);
            auto newSettings = oldSettings_;
            newSettings.c_lflag &= (~ICANON & ~ECHO);
            tcsetattr(fileno( stdin ), TCSANOW, &newSettings);

        }

        bool Listen(suseconds_t wait, char& c)
        {
            timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = wait;

            fd_set set;
            FD_ZERO(&set);
            FD_SET(fileno(stdin), &set);

            // res is >0 for a character read, <0 for error, ==0 for timeout.
            int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

            if( res > 0 )
            {
                read( fileno( stdin ), &c, 1 );
            }

            return res > 0;
        }

        ~KeyListener()
        {
            tcsetattr(fileno( stdin ), TCSANOW, &oldSettings_);
        }
    };
}

int getchar(char& c)
{
    struct termios oldSettings, newSettings;

    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr( fileno( stdin ), TCSANOW, &newSettings );    

    fd_set set;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 50'000;

    FD_ZERO( &set );
    FD_SET( fileno( stdin ), &set );

    int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

    if( res > 0 )
    {
        read( fileno( stdin ), &c, 1 );
    }
    else if( res < 0 )
    {
        //perror( "select error" );
    }
    else
    {
        //printf( "Select timeout\n" );
    }

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
    return res;
}
