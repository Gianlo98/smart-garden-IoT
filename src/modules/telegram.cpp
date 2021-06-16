#include "sm_telegram.h"
#include "sm_core.h"

SM_Telegram::SM_Telegram() : bot(TELEGRAM_BOT_TOKEN, TELEGRAM_BOT_NAME, TELEGRAM_BOT_USERNAME) {}

String SM_Telegram::getLastMessage() {
    if (!_lastMessage.equals("")) {
        String tmp = _lastMessage;
        _lastMessage = "";
        return tmp;
    } else {
        return ""; 
    }
}

bool SM_Telegram::newMessagePresent() {
    checkNewMessage();
    return !_lastMessage.equals("");
}

void SM_Telegram::checkNewMessage() {
    _lastMessage = "";
    if (millis() > _botLastScanTime + BOT_MTBS) {
        bot.getUpdates(bot.message[0][1]);
        for (int i = 1; i < bot.message[0][0].toInt() + 1; i++) {
            String messageRcvd = bot.message[i][5];
            Serial.println(messageRcvd);
            _lastMessage = messageRcvd;
            _lastUser = bot.message[i][4];
        }
        bot.message[0][0] = "";
        _botLastScanTime = millis();
    }
}

void SM_Telegram::sendMessage(String message) {
    bot.sendMessage(_lastUser, message, "");
}