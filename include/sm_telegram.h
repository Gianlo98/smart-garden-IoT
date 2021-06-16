#include <Arduino.h>
#include <ESP8266TelegramBOT.h>

#ifndef SM_TELEGRAM_H
    #define SM_TELEGRAM_H
    #define BOT_MTBS 1000 

    class SM_Telegram {
        public:
            SM_Telegram();
            String getLastMessage();
            bool newMessagePresent();
            void sendMessage(String message);
        private:
            String _lastMessage;
            String _lastUser;
            long _botLastScanTime;
            TelegramBOT bot;
            void checkNewMessage();

            bool newIncomingMessage();
    };


#endif

